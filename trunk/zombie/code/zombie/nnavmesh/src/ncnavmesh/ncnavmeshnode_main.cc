#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  ncnavmeshnode.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ncnavmesh/ncnavmeshnode.h"
#include "mathlib/polygon.h"

#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatial.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nspatial/ncspatialquadtree.h"
#include "nnavmesh/nnavutils.h"

#ifndef NGAME
#include "gfx2/ngfxserver2.h"
#include "gfx2/nlineserver.h"
#include "nscene/nsceneserver.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncNavMeshNode,ncNavNode);
//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNavMeshNode)
NSCRIPT_INITCMDS_END()

namespace
{
    // Tolerance to consider two points to be in the same ground level when
    // comparing their projected height on the ground.
    const float GroundHeightTolerance( 0.05f );
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncNavMeshNode::ncNavMeshNode()
#ifndef NGAME
    : isInOutdoor( false )
#endif
{
    this->CreateNode();
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncNavMeshNode::ncNavMeshNode (const polygon* zone)
{
    this->CreateNode (zone);
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncNavMeshNode::~ncNavMeshNode()
{
    n_delete (this->zone);
    n_delete (this->userData);
    this->zone = 0;
    this->userData = 0;   
}

//------------------------------------------------------------------------------
/**
    CreateNode
*/
void 
ncNavMeshNode::CreateNode (const polygon* zone)
{
    if ( zone )
    {
        this->zone = n_new (polygon)(*zone);        // Copies the polygon to the node
    }
    else
    {
        this->zone = 0;
    }

    this->userData = 0;
    this->f = this->g = this->h = 0;
    this->parent = 0;
}

//------------------------------------------------------------------------------
/**
    GetMidPoint
*/
void
ncNavMeshNode::GetMidpoint( vector3& midpoint ) const
{
#ifndef NGAME
    n_assert( this->zone );
    if ( !this->zone )
    {
        return;
    }
#endif

    midpoint = this->zone->Midpoint();
    this->GetGroundHeight( midpoint.x, midpoint.z, midpoint.y );
}

//------------------------------------------------------------------------------
/**
    Tell if a point is inside this node

    A point is considered to be inside this node if the point's projection over
    the polygon is inside it and if both the point and the polygon are in the same
    ground level.
*/
bool
ncNavMeshNode::IsPointInside( const vector3& point ) const
{
    n_assert( this->zone );

    // Do a first fast culling by skipping those nav nodes whose 2D bounding box doesn't contain the point at all
    const bbox3& bbox( this->GetComponentSafe<ncSpatial>()->GetBBox() );
    if ( (point.x < bbox.vmin.x) || (point.x > bbox.vmax.x) ||
         (point.z < bbox.vmin.z) || (point.z > bbox.vmax.z) )
    {
        return false;
    }

    // Do a second slower culling by checking if the point projects inside the polygon
    if ( !this->zone->IsPointInside( point.x, point.z ) )
    {
        return false;
    }

    // Finally check if the nav node is at the same ground level as the given point
    // @todo Use GetGroundLevelHeight instead of GetGroundHeight to avoid the special
    //       case where there's an obstacle between the node and the real ground.
    //       For now this cannot be used because movement is allowed over that kind
    //       of obstacles through nav nodes, but currently there isn't any way to
    //       detect that "ground layer".
    float nodeHeight;
    float pointHeight;
    if ( this->GetGroundHeight( point.x, point.z, nodeHeight ) &&
         nNavUtils::GetGroundHeight( point, pointHeight ) )
    {
        if ( n_abs( pointHeight - nodeHeight ) <= GroundHeightTolerance )
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    IsPointInside
*/
bool
ncNavMeshNode::IsPointInside (float x, float z) const
{
    n_assert(this->zone);

    bool inside = false;

    if ( this->zone )
    {
        inside = this->zone->IsPointInside (x, z);
    }

    return inside;
}

//------------------------------------------------------------------------------
/**
    Get the node's closest navigable point to the given point
*/
void
ncNavMeshNode::GetClosestNavigablePoint( const vector3& point, vector3& closestPoint ) const
{
    n_assert( this->zone );
    this->zone->GetClosestPoint2d( point, closestPoint );
    this->GetGroundHeight( closestPoint.x, closestPoint.z, closestPoint.y );
}

//------------------------------------------------------------------------------
/**
    Get cross information regarding to a ray exiting this node
*/
ncNavMeshNode::CrossType
ncNavMeshNode::GetCrossInfo( const line3& ray, ncNavMeshNode*& neighbor, vector3& crossPoint, vector3& crossNormal ) const
{
    // If the ray ends inside the node then it doesn't leave it for sure
    // Need to do the most expensive 3D check due to overlayed navmeshes
    if ( this->IsPointInside( ray.end() ) )
    {
        return STAYING_INSIDE;
    }

    // Check if the ray intersects some edge, getting the one that intersects further from the ray's origin
    // (the furthest intersected edge is the one crossed when the ray exits the node)
    float crossDistanceSq( -FLT_MAX );
    int numVertices( this->zone->GetNumVertices() );
    for ( int i(0); i < numVertices; ++i )
    {
        // Build next edge and its plane orthogonal to XZ plane
        line3 edge( this->zone->GetVertex(i), this->zone->GetVertex( (i+1) % numVertices ) );
        plane edgePlane( edge.start(), edge.end(), edge.end() + vector3(0,1,0) );

        // Check if the ray crosses the edge
        float t;
        if ( edgePlane.intersect( ray, t ) )
        {
            if ( t >= -TINY )
            {
                vector3 rayCrossPoint( ray.ipol(t) );
                t = edge.closestpoint( rayCrossPoint );
                if ( t >= -TINY && t <= 1 + TINY )
                {
                    // Keep cross information if it's the furthest cross point so far
                    float distanceSq( ( rayCrossPoint - ray.start() ).lensquared() );
                    if ( distanceSq > crossDistanceSq )
                    {
                        neighbor = this->GetNeighborSharingEdge( edge );
                        crossDistanceSq = distanceSq;
                        crossPoint = rayCrossPoint;
                        // Assume mesh nodes are flat on XZ plane
                        crossPoint.y = ray.start().y;
                        crossNormal = edgePlane.normal();
                        if ( (ray.end() - ray.start()) % crossNormal > 0 )
                        {
                            crossNormal *= -1;
                        }
                    }
                }
            }
        }
    }

    // Extra safety check to protect from float errors when ray's origin is on an edge
    if ( crossDistanceSq < TINY )
    {
        return STAYING_INSIDE;
    }

    // If there's no neighbor after crossing the point, then the ray is leaving the mesh
    return neighbor ? GOING_TO_NEIGHBOR : GOING_OUT_OF_MESH;
}

//------------------------------------------------------------------------------
/**
    Get the neighbor sharing the given edge, or NULL if none shares it
*/
ncNavMeshNode*
ncNavMeshNode::GetNeighborSharingEdge( const line3& edge ) const
{
    // Iterate over all the neighbors looking for the one that shares the given edge
    for ( int i(0); i < this->GetLinksNumber(); ++i )
    {
        ncNavNode* node( this->GetLink(i) );
        if ( node )
        {
            ncNavMeshNode* meshNode( node->GetComponent<ncNavMeshNode>() );
            if ( meshNode )
            {
                // Check if the node shares the edge
                polygon* poly( meshNode->GetZone() );
                if ( poly->HasEdge( edge ) )
                {
                    return meshNode;
                }
            }
        }
    }

    // No neighbor shares the given edge
    return NULL;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Search and store which edges aren't shared with any neighbor
*/
void
ncNavMeshNode::LookForBoundaryEdges()
{
    this->boundaryEdges.Clear();

    int numVertices = this->zone->GetNumVertices();
    for ( int i(0); i < numVertices; ++i )
    {
        line3 edge( this->zone->GetVertex(i),
            this->zone->GetVertex( (i+1) % numVertices ) );
        if ( !this->GetNeighborSharingEdge(edge) )
        {
            this->boundaryEdges.Append( i );
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draw
*/
void
ncNavMeshNode::Draw( nLineHandler* lineHandler, bool wireframe, bool overlay, bool showLinks )
{
    this->Draw( lineHandler, this->isInOutdoor, 0, wireframe, overlay, showLinks );
}

//------------------------------------------------------------------------------
/**
    Draw
*/
void
ncNavMeshNode::Draw ( nLineHandler* lineHandler, bool isOutdoor, float /*high*/, bool wireframe, bool overlay, bool showLinks)
{
    nGfxServer2* server = nGfxServer2::Instance();
    this->DrawEdges (server, isOutdoor, wireframe, overlay);
    this->DrawBoundary( lineHandler, isOutdoor, overlay );

    // Uncomment line below to debug mesh generation
//    this->DrawVertices (server, high);

    if ( showLinks )
    {
        this->DrawLinks (server, isOutdoor, overlay);
    }

    // Uncomment block below to debug pathfinding
/*	// Draw 'parent'
	if ( this->parent )
	{
		vector3 center;
        this->GetMidpoint( center );
		vector3 parentCenter;
        this->parent->GetMidpoint( parentCenter );
		vector3 direction = parentCenter - center;

		direction.norm();
		direction *= 2.f;
		direction += center;		

		vector3 line[2];

		line[0] = center;
		line[1] = direction;

		line[0].y = -10.f;
		line[1].y = -10.f;

		server->BeginLines();
		server->DrawLines3d (line, 2, vector4(0.4f, 0.6f, 0.4f, 1.f));
		server->EndLines();
	}*/
}

//------------------------------------------------------------------------------
/**
    DrawBoundary
*/
void
ncNavMeshNode::DrawBoundary( nLineHandler* lineHandler, bool isOutdoor, bool overlay )
{
    n_assert( lineHandler );

    vector4& colour( isOutdoor ? this->outdoorBoundaryColour : this->indoorBoundaryColour );

    int numVertices( this->zone->GetNumVertices() );
    for ( int i(0); i < this->boundaryEdges.Size(); ++i )
    {
        int edgeIndex = this->boundaryEdges[i];
        vector3 v[2] = { this->zone->GetVertex(edgeIndex),
            this->zone->GetVertex( (edgeIndex+1) % numVertices ) };
        this->UpdateDrawHeight( v[0], isOutdoor, overlay );
        this->UpdateDrawHeight( v[1], isOutdoor, overlay );
        lineHandler->DrawLines3d( v, NULL, 2, colour );
    }
}

//------------------------------------------------------------------------------
/**
    DrawEdges
*/
void
ncNavMeshNode::DrawEdges (nGfxServer2* server, bool isOutdoor, bool wireframe, bool overlay)
{
    vector4 indoorLineColor( 0.8f, 0.2f, 0.8f, 1.0f );
    vector4 indoorFillColor( 0.3f, 0.0f, 0.3f, 0.5f );
    vector4 outdoorLineColor( 0.2f, 0.8f, 0.2f, 1.0f );
    vector4 outdoorFillColor( 0.1f, 0.4f, 0.1f, 0.5f );

    const int MaxVertices( 100 );
    vector3 vertexes[MaxVertices];
    vector3 center = this->zone->Midpoint();

    for ( int j=0; j<this->zone->GetNumVertices() && j < MaxVertices - 1; j++ )
    {
        vertexes[j] = this->zone->GetVertex(j);
        this->UpdateDrawHeight( vertexes[j], isOutdoor, overlay );
    }
    vertexes[ this->zone->GetNumVertices() ] = vertexes[0];

    matrix44 model;
    model.ident();
    nGfxServer2::Instance()->BeginShapes( this->GetShader(overlay) );
    if ( wireframe )
    {
        nGfxServer2::Instance()->DrawShapePrimitives( nGfxServer2::LineStrip,
            this->zone->GetNumVertices(), vertexes, 3, model,
            isOutdoor ? outdoorLineColor : indoorLineColor );
    }
    else
    {
        nGfxServer2::Instance()->DrawShapePrimitives( nGfxServer2::TriangleFan,
            this->zone->GetNumVertices() - 2, vertexes, 3, model,
            isOutdoor ? outdoorFillColor : indoorFillColor );
    }
    nGfxServer2::Instance()->EndShapes();

    if ( wireframe )
    {
        server->SetTransform(nGfxServer2::Model, matrix44());
        server->BeginLines();

        vector3 crossLeft[2], crossRight[2];
        const float fCross = 0.03f;
        float y = vertexes[0].y;

        crossLeft[0]  = center + vector3 (-fCross, 0,  fCross);
        crossLeft[1]  = center + vector3 ( fCross, 0, -fCross);
        crossRight[0] = center + vector3 (-fCross, 0, -fCross);
        crossRight[1] = center + vector3 ( fCross, 0,  fCross);

        crossLeft[0].y = crossLeft[1].y = y;
        crossRight[0].y = crossRight[1].y = y;

        vector4& color( isOutdoor ? outdoorLineColor : indoorLineColor );
        server->DrawLines3d (crossLeft, 2, color);
        server->DrawLines3d (crossRight, 2, color);    

        server->EndLines();
    }
}

//------------------------------------------------------------------------------
/**
    DrawLinks
*/
void
ncNavMeshNode::DrawLinks (nGfxServer2* server, bool isOutdoor, bool overlay)
{
    vector4 color (0.f, 0.8f, 0.4f, 1.f);
    matrix44 theMatrix;

    server->SetTransform(nGfxServer2::Model, theMatrix);
    server->BeginLines();

    vector3 lineLinks[2];
    lineLinks[0];
    this->GetMidpoint( lineLinks[0] );
    this->UpdateDrawHeight( lineLinks[0], isOutdoor, overlay );

    for ( int i=0; i<this->GetLocalLinksNumber(); i++ )
    {
        this->GetLocalLink(i)->GetMidpoint( lineLinks[1] );
        this->UpdateDrawHeight( lineLinks[1], isOutdoor, overlay );
        server->DrawLines3d (lineLinks, 2, color);
    }

    server->EndLines();
}

//------------------------------------------------------------------------------
/**
    DrawVetices
*/
void
ncNavMeshNode::DrawVertices (nGfxServer2* server, float high)
{
    vector4 green (0.f, 1.f, 0.f, 1.f);
    vector4 final (0.3f, 1.f, 0.8f, 1.f);
    vector4 red (1.f, 0.f, 0.f, 1.f);
    vector4 color, polyColor;
    matrix44 theMatrix;
    const float dotRadius = 0.01f;

    server->SetTransform(nGfxServer2::Model, theMatrix);
    
    const int MaxVertices( 100 );
    vector3 vertexes[ MaxVertices ];
    vector3 center[2];
    int iVertices = this->zone->GetNumVertices();

    this->GetMidpoint( center[0] );
    center[0].y += high;

    void* p = (void*)this;
    size_t i = reinterpret_cast<size_t>(p); 
    int col1 = int(i) & 0x0f;
    int col2 = (int(i) & 0xf0) >> 4;

    polyColor = vector4 (0.0f, col1/16.0f, col2/16.0f, 1.0f);
    
    for ( int j=0; j<iVertices && j < MaxVertices; j++ )
    {
        vertexes[j] = this->zone->GetVertex(j);
        vertexes[j].y += high;

        matrix44 model1;      

        color = j==0 ? green : j==iVertices-1 ? final : red;

        model1.scale (vector3(dotRadius, dotRadius, dotRadius));
        model1.set_translation (vertexes[j]);            
        
        server->BeginShapes();
        server->DrawShape (nGfxServer2::Sphere, model1, color); 
        server->EndShapes();

        // Debug lines from the center to every single vertex of the node
        /*server->BeginLines();
        center[1] = vertexes[j];
        server->DrawLines3d (center, 2, polyColor);
        server->EndLines();*/
    }
}

//------------------------------------------------------------------------------
/**
    DrawLinks
*/
nShader2*
ncNavMeshNode::GetShader( bool overlay ) const
{
    nShader2* shader( NULL );
    int shaderIndex( -1 );

    if ( overlay )
    {
        shaderIndex = nSceneServer::Instance()->FindShader( "shape_overlay" );
    }
    else
    {
        shaderIndex = nSceneServer::Instance()->FindShader( "shape_no_cull" );
    }

    if ( shaderIndex != -1 )
    {
        shader = nSceneServer::Instance()->GetShaderAt( shaderIndex ).GetShaderObject();
    }

    return shader;
}

//------------------------------------------------------------------------------
/**
	UpdateDrawHeight
*/
void
ncNavMeshNode::UpdateDrawHeight( vector3& point, bool isOutdoor, bool overlay ) const
{
    if ( isOutdoor )
    {
    	this->UpdateHeight( &point );
    }
    if ( !overlay )
    {
        point.y += isOutdoor ? 0.5f : 0.1f;
    }
}


//------------------------------------------------------------------------------
/**
*/
vector4
ncNavMeshNode::GetIndoorBoundaryColour() const
{
   return this->indoorBoundaryColour;
}

//------------------------------------------------------------------------------
/**
*/
void
ncNavMeshNode::SetIndoorBoundaryColour(vector4 colour)
{
   this->indoorBoundaryColour = colour;
}

//------------------------------------------------------------------------------
/**
*/
vector4
ncNavMeshNode::GetOutdoorBoundaryColour() const
{
   return this->outdoorBoundaryColour;
}

//------------------------------------------------------------------------------
/**
*/
void
ncNavMeshNode::SetOutdoorBoundaryColour(vector4 colour)
{
   this->outdoorBoundaryColour = colour;
}

#endif // !NGAME

//------------------------------------------------------------------------------
/**
    GetByteSize
*/
int
ncNavMeshNode::GetByteSize() const
{
    // Base data
    int size( ncNavNode::GetByteSize() );

    // Zone
    size += sizeof( this->zone );
    if ( zone )
    {
        size += sizeof(polygon) + this->zone->GetNumVertices() * sizeof(vector3);
    }

    // User data
    size += sizeof( this->userData );

    return size;
}

//------------------------------------------------------------------------------
/**
	UpdateHeight
*/
void
ncNavMeshNode::UpdateHeight (vector3* position) const
{
	n_assert(position);

	nSpatialServer* spatialServer = nSpatialServer::Instance();
	n_assert(spatialServer);

	if ( spatialServer->GetOutdoorEntity() )
	{
		ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
		n_assert(terrainGMM);
		nFloatMap* heightMap = terrainGMM->GetHeightMap();
		n_assert(heightMap);
		float height;

		if ( heightMap->GetHeight (position->x, position->z, height) )
		{
			position->y = height;
		}
	}
}

//------------------------------------------------------------------------------
/**
    Get the height of the ground belonging to the given point in the navmeshnode

    This function returns false only when it hasn't find a ground. In a well
    constructed level this should never happen and this function should always
    return true. But desynchronization between physics and navigation worlds due
    to later changes may cause to have invalid navmeshnodes in the level,
    making this function to don't find a ground and thus returning false.
*/
bool
ncNavMeshNode::GetGroundHeight( float x, float z, float& groundHeight ) const
{
    // Get the point in the nearness of the ground surface
    vector3 nodePoint( x, 0, z );
    if ( this->IsInOutdoor() )
    {
        nNavUtils::GetHeightMap()->GetHeight( x, z, nodePoint.y );
    }
    else
    {
        n_assert( this->zone );
        nodePoint.y = this->zone->GetVertex(0).y;
    }

    // Project exactly the point to the physical ground surface
    return nNavUtils::GetGroundHeight( nodePoint, groundHeight );
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------

