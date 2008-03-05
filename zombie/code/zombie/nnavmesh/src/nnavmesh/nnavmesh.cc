#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  nnavmesh.cc
//------------------------------------------------------------------------------

#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "nnavmeshparser/nnavmeshpersist.h"

#include "mathlib/polygon.h"

#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialmc.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsgeom.h"

#include "entity/nentityobjectserver.h"
#include "nnavmesh/nnavutils.h"

#include "kernel/nlogclass.h"

#ifndef NGAME
#include "gfx2/ngfxserver2.h"
#endif

#define EPSILON_DISTANCE 8.f

#define INDOOR_QUERY_FLAGS nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_ALL_CELLS |\
                           nSpatialTypes::SPF_CONTAINING  | nSpatialTypes::SPF_USE_BBOX  |\
                           nSpatialTypes::SPF_USE_CELL_BBOX

#define OUTDOOR_QUERY_FLAGS nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_ONE_CELL  |\
                            nSpatialTypes::SPF_CONTAINING |\
                            nSpatialTypes::SPF_USE_CELL_BBOX

nNebulaClass(nNavMesh, "nresource");

//------------------------------------------------------------------------------
namespace
{
    // Tolerance relative to a node's height within which a point is considered at the same height as the node
    const float NodeHeightToleranceOver( 1.0f );
    const float NodeHeightToleranceBelow( 0.5f );
    // Max distance from a point where nodes are searched when looking for a near node
    const float MaxNodeVisibilityDistance( 10.0f );
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMesh::nNavMesh() : collide(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nNavMesh::~nNavMesh()
{
    this->UnloadResource();
}

//------------------------------------------------------------------------------
/**
    Destroy
*/
void 
nNavMesh::Destroy()
{
    for ( int i=0; this->mesh.Size(); )
    {
        this->ReleaseNode (this->mesh[i]);
    }
    this->mesh.Clear();
}

//------------------------------------------------------------------------------
/**
    ReleaseNode
*/
void
nNavMesh::ReleaseNode (ncNavMeshNode* node)
{
    int index = mesh.FindIndex (node);

    if ( index != - 1 )
    {
        mesh.Erase (index);
    }

    // Updates all links with this node
    node->UpdateLinks();

#ifndef NGAME
    node->GetEntityObject()->SendToLimbo( node->GetEntityObject() );
#endif
    nEntityObjectServer::Instance()->RemoveEntityObject(node->GetEntityObject());
    
    node = 0;
}

//------------------------------------------------------------------------------
/**
    InsertNode
*/
void
nNavMesh::InsertNode (ncNavMeshNode* node)
{   
    if ( this->mesh.FindIndex (node) == -1 )
    {
        this->mesh.Append (node);
    }
}

//------------------------------------------------------------------------------
/**
    GetNumNodes
*/
int
nNavMesh::GetNumNodes() const
{
    return this->mesh.Size();
}

//------------------------------------------------------------------------------
/**
    SetObstacles
*/
void
nNavMesh::SetObstacles (const nArray<polygon*>& obstacles)
{
    this->DestroyObstacles();

    for (int i=0; i<obstacles.Size(); i++ )
    {
        polygon* element = obstacles[i];
        polygon* obstacle = n_new (polygon)(*element);

        this->obstacles.Append (obstacle);
    }
}

//------------------------------------------------------------------------------
/**
    InsertObstacle
*/
void
nNavMesh::InsertObstacle (polygon* poly)
{
    if ( this->obstacles.FindIndex (poly) == -1 )
    {
        this->obstacles.Append (poly);
    }
}

//------------------------------------------------------------------------------
/**
    GetNumObstacles
*/
int
nNavMesh::GetNumObstacles() const
{
    return this->obstacles.Size();
}

//------------------------------------------------------------------------------
/**
    GetObstaclePolygon
*/
polygon*
nNavMesh::GetObstaclePolygon (int index) const
{
    n_assert( index >= 0 && index < this->obstacles.Size() );

    polygon* poly = 0;

    if ( index >= 0 && index < this->obstacles.Size() )
    {
        poly = this->obstacles[index];
    }

    return poly;
}

//------------------------------------------------------------------------------
/**
    DestroyObstaclces
*/
void 
nNavMesh::DestroyObstacles()
{
    while ( this->obstacles.Size() )
    {
        polygon* obstacle = this->obstacles[0];
        n_delete (obstacle);
        this->obstacles.Erase (0);
    }
}

//------------------------------------------------------------------------------
/**
    GetNode
*/
ncNavMeshNode*
nNavMesh::GetNode (float x, float z) const
{
    ncNavMeshNode* node = 0;

    for ( int i=0; i<this->mesh.Size() && !node; i++ )
    {
        ncNavMeshNode* element = this->mesh[i];

        if ( element && element->IsPointInside (x, z) )
        {
            node = element;
        }
    }
    
    return node;
}

//------------------------------------------------------------------------------
/**
    GetNode
*/
ncNavMeshNode*
nNavMesh::GetNode (const vector3& point) const
{
    // Look for a navigation node in indoors first
    nArray< nEntityObject* > nodes;
    nSpatialServer::Instance()->GetEntitiesCategory( point, nSpatialTypes::CAT_NAVMESHNODES,
        INDOOR_QUERY_FLAGS, 
        nodes );
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavMeshNode* meshNode( nodes[i]->GetComponent<ncNavMeshNode>() );
        if ( meshNode )
        {
            if ( meshNode->IsPointInside( point ) )
            {
                return meshNode;
            };
        }
    }

    // Look for a navigation node in outdoor
    float terrainHeight;
    GetHeightMap()->GetHeight( point.x, point.z, terrainHeight );
    nodes.Clear();
    nSpatialServer::Instance()->GetEntitiesCategory( point, nSpatialTypes::CAT_NAVMESHNODES,
        OUTDOOR_QUERY_FLAGS, nodes );
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavMeshNode* meshNode( nodes[i]->GetComponent<ncNavMeshNode>() );
        if ( meshNode )
        {
            if ( meshNode->IsPointInside( point ) )
            {
                return meshNode;
            };
        }
    }

    // No nav node found containing the given point
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Get the navigation node below the given point

    Any type of navigation node can be returned, giving more priority to mesh nodes
*/
ncNavNode*
nNavMesh::GetNavNode( const vector3& point ) const
{
    // Check if there's a mesh node containing the point
    ncNavNode* node( this->GetNode( point ) );
    if ( node )
    {
        return node;
    }

    // Check if there's a waypoint node containing the point
    return this->GetWaypointNode( point );
}

//------------------------------------------------------------------------------
/**
    Get the navigation waypoint node containing the given point

    A waypoint node contains a point if it's walkable from the center of the
    waypoint to the point
*/
ncNavNode*
nNavMesh::GetWaypointNode( const vector3& point ) const
{
    // Max "visibility distance" to look for near nodes
    sphere visibilitySphere( point, MaxNodeVisibilityDistance );

    // Look for waypoints in indoors and outdoor space
    float closestDistanceSq( FLT_MAX );
    ncNavNode* closestNode( NULL );
    nArray< nEntityObject* > nodes;
    nSpatialServer::Instance()->GetEntitiesCategory( visibilitySphere, nSpatialTypes::CAT_WAYPOINTS,
        nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_ALL_CELLS |
        nSpatialTypes::SPF_INTERSECTING | nSpatialTypes::SPF_USE_BBOX, nodes );
    nSpatialServer::Instance()->GetEntitiesCategory( visibilitySphere, nSpatialTypes::CAT_WAYPOINTS,
        nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_ALL_CELLS |
        nSpatialTypes::SPF_INTERSECTING | nSpatialTypes::SPF_USE_BBOX, nodes );
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavNode* node( nodes[i]->GetComponent<ncNavNode>() );
        if ( node )
        {
            // Build LoS
            // @todo When not using anymore the midpoints for pathfinding, use the nearest point
            //       in the node for the LoS check instead of the midpoint
            vector3 nodePoint;
            node->GetMidpoint( nodePoint );

            // Check for LoS to the node
            vector3 los( nodePoint - point );
            float losLengthSq( los.lensquared() );
            if ( losLengthSq < closestDistanceSq )
            {
                if ( nNavUtils::IsWalkable( point, nodePoint, NULL ) )
                {
                    closestDistanceSq = losLengthSq;
                    closestNode = node;
                }
            }
        }
    }

    return closestNode;
}

//------------------------------------------------------------------------------
/**
    GetClosestNode
*/
ncNavNode*
nNavMesh::GetClosestNode( const vector3& point ) const
{
    // Check if there's already a node containing the point
    ncNavNode* node( this->GetNavNode( point ) );
    if ( node )
    {
        return node;
    }

    // -- Look for a close mesh node not containing the point

    // Max "visibility distance" to look for near nodes
    sphere visibilitySphere( point, MaxNodeVisibilityDistance );

    // Look for mesh nodes in indoors
    float closestDistanceSq( FLT_MAX );
    ncNavNode* closestNode( NULL );
    nArray< nEntityObject* > nodes;
    nSpatialServer::Instance()->GetEntitiesCategory( visibilitySphere, nSpatialTypes::CAT_NAVMESHNODES,
        nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_ALL_CELLS |
        nSpatialTypes::SPF_INTERSECTING | nSpatialTypes::SPF_USE_BBOX, nodes );
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavNode* node( nodes[i]->GetComponent<ncNavNode>() );
        if ( node )
        {
            // Build LoS
            // @todo When not using anymore the midpoints for pathfinding, use the nearest point
            //       in the node for the LoS check instead of the midpoint
            vector3 nodePoint;
            node->GetMidpoint( nodePoint );

            // Skip those nav nodes which don't share the same floor level as the given point
            if ( point.y < nodePoint.y - NodeHeightToleranceBelow ||
                 point.y > nodePoint.y + NodeHeightToleranceOver )
            {
                continue;
            }

            // Check for LoS to the node
            vector3 los( nodePoint - point );
            float losLengthSq( los.lensquared() );
            if ( losLengthSq < closestDistanceSq )
            {
                if ( nNavUtils::IsWalkable( point, nodePoint, NULL ) )
                {
                    closestDistanceSq = losLengthSq;
                    closestNode = node;
                }
            }
        }
    }

    // Max "visibility volume" for outdoor is less approximate since outdoor nodes' height cannot be easyly known,
    // neither the spatial server supports for cylinder intersection checks.
    bbox3 visibilityBBox( point, vector3( MaxNodeVisibilityDistance, FLT_MAX * 0.5f, MaxNodeVisibilityDistance ) );

    // Look for mesh nodes in outdoor
    nFloatMap* terrainMap( this->GetHeightMap() );
    nodes.Clear();
    nSpatialServer::Instance()->GetEntitiesCategory( visibilitySphere, nSpatialTypes::CAT_NAVMESHNODES,
        nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_ALL_CELLS |
        nSpatialTypes::SPF_INTERSECTING | nSpatialTypes::SPF_USE_BBOX, nodes );
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavNode* node( nodes[i]->GetComponent<ncNavNode>() );
        if ( node )
        {
            // Project node point onto terrain to achieve a more precise LoS test
            vector3 nodePoint;
            node->GetMidpoint( nodePoint );
            float terrainHeight;
            terrainMap->GetHeight( nodePoint.x, nodePoint.z, terrainHeight );
            if ( nodePoint.y < terrainHeight )
            {
                nodePoint.y = terrainHeight;
            }

            // Skip those nav nodes which don't share the same floor level as the given point
            if ( point.y < nodePoint.y - NodeHeightToleranceBelow ||
                 point.y > nodePoint.y + NodeHeightToleranceOver )
            {
                continue;
            }

            // Check for LoS to the node
            // @todo When not using the midpoints for pathfinding anymore, use the nearest point
            //       in the node for the LoS check instead of the midpoint
            vector3 los( nodePoint - point );
            float losLengthSq( los.lensquared() );
            if ( losLengthSq < closestDistanceSq )
            {
                if ( nNavUtils::IsWalkable( point, nodePoint, NULL ) )
                {
                    closestDistanceSq = losLengthSq;
                    closestNode = node;
                }
            }
        }
    }

    return closestNode;
}

//------------------------------------------------------------------------------
/**
    GetNode
*/
ncNavMeshNode*
nNavMesh::GetNode (int index) const
{
    n_assert(index>=0&&index<this->mesh.Size());

    ncNavMeshNode* node = 0;

    if ( index >= 0 && index < this->mesh.Size() )
    {
        node = this->mesh[index];
    }

    return node;
}

//------------------------------------------------------------------------------
/**
    GetIndexNode

    @return the index of the node inside the mesh, -1 if not found
*/
int
nNavMesh::GetIndexNode (ncNavMeshNode* node) const
{
    int index = -1;

    for ( int i=0; i<this->mesh.Size() && index == -1; i++ )
    {
        ncNavMeshNode* element = this->mesh[i];

        if ( element && element == node )
        {
            index = i;
        }
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    GetDistance

    @return the distance between two nodes in the navigation graph
*/
int
nNavMesh::GetDistance (ncNavMeshNode* source, ncNavMeshNode* goal) const
{
    n_assert(source&&goal);

    int distance = -1;

    if ( source && goal )
    {
        int indexSource = this->GetIndexNode (source);
        int indexGoal   = this->GetIndexNode (goal);

        distance = this->GetDistance (indexSource, indexGoal);
    }

    return distance;
}

//------------------------------------------------------------------------------
/**
    GetDistance

    @return the distance between two nodes in the navigation graph
*/
int
nNavMesh::GetDistance (int source, int goal) const
{
    return this->navGraph.At (source, goal);
}

//------------------------------------------------------------------------------
/**
    SetDistance

    @param source Index of the source node
    @param goal Index of the goal node
    @param distance Distance from source node to goal node
*/
void
nNavMesh::SetDistance (int source, int goal, int distance)
{
    if ( source >= 0 && goal >= 0 && this->navGraph.ValidIndex(source,goal) )
    {
        this->navGraph.Set (source, goal, distance);
    }
}

//------------------------------------------------------------------------------
/**
    GetNumGraphNodes
*/
int
nNavMesh::GetNumGraphNodes() const
{
    n_assert( this->navGraph.GetHeight() == this->navGraph.GetWidth() );
    return this->navGraph.GetHeight();
}

//------------------------------------------------------------------------------
/**
    SetNumGraphNodes
*/
void
nNavMesh::SetNumGraphNodes (int numNodes)
{
    if ( numNodes > 0 )
    {
        this->navGraph.SetSize (numNodes, numNodes);
        n_assert( static_cast<int>(this->navGraph.GetHeight()) == numNodes );
        n_assert( static_cast<int>(this->navGraph.GetWidth()) == numNodes );
    }
}

//------------------------------------------------------------------------------
/**
    AddMesh

    @brief Adds nodes and all information about other mesh into this mesh
*/
void
nNavMesh::AddMesh (nNavMesh* mesh)
{
    n_assert(mesh);

    if ( mesh )
    {
        // Mesh nodes
        for ( int i=0; i<mesh->mesh.Size(); i++ )
        {
            ncNavMeshNode* node = mesh->GetNode (i);
            n_assert(node);

            if ( node )
            {
                this->InsertNode (node);
            }
        }

        // Obstacles
        for ( int i(0); i < mesh->obstacles.Size(); ++i )
        {
            this->InsertObstacle( mesh->obstacles[i] );
        }

        // @TODO It is necessary update the navigation mesh (note, now, we must setup the links between meshes) 
//        this->BuildNavGraph();

        this->SetState(Valid);
    }
}

//------------------------------------------------------------------------------
/**
    Removes all mesh nodes and obstacles, but without releasing them

    Usually this is called after the mesh has been merged with another one and
    we want to make sure that transfered nodes/obstacles aren't going to be
    destroyed along with this now useless container

    The resource is also invalidated
*/
void
nNavMesh::ClearMesh()
{
    this->mesh.Clear();
    this->obstacles.Clear();
    this->SetState( Empty );
}

//------------------------------------------------------------------------------
/**
    SetupNavGraph
*/
void
nNavMesh::SetupNavGraph()
{
    int size = this->mesh.Size();

    if ( size > 0 )
    {
        this->navGraph.SetSize (size, size);

        int i;
        for ( i=0; i<size; i++ )
        {
            for ( int j=0; j<size; j++ )
            {
                this->navGraph.Set (i, j, 0);
            }
        }

        // Find the conexions between nodes
        for ( i=0; i<this->mesh.Size(); i++ )
        {
            ncNavMeshNode* node = this->mesh[i];
            n_assert(node);

            if ( node )
            {
                // Calculates distances between links
                for ( int k=0; k<node->links.Size(); k++ )
                {
                    ncNavMeshNode* link = static_cast<ncNavMeshNode*>( node->GetLink(k) );
                    n_assert(link);

                    if ( link )
                    {
                        int j = this->GetIndexNode (link);
                        n_assert (this->navGraph.ValidIndex(i,j));

                        if ( j != -1 )
                        {
                            this->navGraph.Set (i, j, 1);
                        }
                    }
                }
            }

            this->navGraph.Set (i, i, 0);
        }
    }
}

//------------------------------------------------------------------------------
/**
    BuildNavGraph

    @brief Generates the navigation graph since the mesh
*/
void
nNavMesh::BuildNavGraph()
{
    int size = this->mesh.Size();

    this->SetupNavGraph();

    nTime startTime( nTimeServer::Instance()->GetTime() );
    for ( int turn = 1; turn < size; turn++ )
    {
        for ( int row = 0; row < size; row++ )
        {
            for ( int column = 0; column < size; column++ )
            {
                if ( nTimeServer::Instance()->GetTime() - startTime > 10 )
                {
                    startTime = nTimeServer::Instance()->GetTime();
                    float completed( 100.0f * (float(row)*float(size) + float(column)) / (float(size)*float(size)) );
                    NLOG_REL( navbuild, (NLOGUSER | 0, "[4/4] Building navigation graph...%d(%.2f%%)/%d ", turn-1, completed, size-1) );
                }

                if ( this->navGraph.At (row, column) == turn )
                {
                    // We cover the row, adding 1 to the value
                    for ( int i=0; i<size; i++ )
                    {
                        if ( this->navGraph.At (column, i) != 0 )
                        {
                            int cost = turn + this->navGraph.At (column, i);

                            if ( this->navGraph.At (row, i) == 0 || 
                                this->navGraph.At (row, i) > cost )
                            {
                                this->navGraph.Set (row, i, cost);
                                this->navGraph.Set (i, row, cost);
                            }

                            if ( row == i )
                            {
                                this->navGraph.Set (row, i, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    GetStartPosition
*/
vector3
nNavMesh::GetStartPosition() const
{
    vector3 position;

    if ( this->mesh.Size() > 0 )
    {
        position;
        this->mesh[0]->GetMidpoint( position );
    }

    return position;
}

//------------------------------------------------------------------------------
/**
    GetCenterPosition

    @brief Only used for outdoor navigation meshes
*/
vector3
nNavMesh::GetCenterPosition() const
{
    nSpatialServer* spatialServer = nSpatialServer::Instance();
    n_assert(spatialServer);
    ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
    n_assert(terrainGMM);
    nFloatMap* heightMap = terrainGMM->GetHeightMap();
    n_assert(heightMap);

    int size = heightMap->GetSize();
    float scale = heightMap->GetGridScale();
    float x, h; 

    x = size * 0.5f * scale;
    heightMap->GetHeight (x, x, h);
    
    return vector3 (x, h, x);
}

//------------------------------------------------------------------------------
/**
    AdjustPosition
*/
bool
nNavMesh::AdjustPosition (vector3* position, float radius) // const
{
    bool valid = true;

    if ( position )
    {
        vector3 axis1, axis2;
        polygon* obstacle = this->IsInsideObstacle (*position, radius, &axis1, &axis2);

        if ( obstacle )
        {
            // This is a first approximation
            //*position = obstacle->Midpoint() + axis1 + axis2;
            valid = false;
        }
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
    IsInsideObstacle

    @brief Says if a given point is inside an obstacle of the mesh
*/
polygon* 
nNavMesh::IsInsideObstacle (const vector3& point, float radius, vector3* paxis1, vector3* paxis2) //const
{
    polygon* obstacle = 0;

    for ( int i=0; i<this->obstacles.Size() && !obstacle; i++ )
    {
        polygon* element = this->obstacles[i];
        n_assert (element);

        if ( element )
        {
            vector3 midpoint = element->Midpoint();

            // Only check elements near to the position
            if ( (midpoint - point).lensquared() < pow(EPSILON_DISTANCE, 2) )
            {
                vector3 p0 = element->GetVertex(0);
                vector3 l1 = element->GetVertex(3) - p0;
                vector3 l2 = element->GetVertex(1) - p0;                
                vector3 distance = point - midpoint;
                                
                l1 *= 0.5f;
                l2 *= 0.5f;

                // Use 2D vectors
                vector2 axis1(l1.x, l1.z);
                vector2 axis2(l2.x, l2.z);
                vector2 d(distance.x, distance.z);
                vector2 n(axis1);
                                
                n.norm();

                // Project distance vector onto axis1 (n)
                // d2 is the perperdicular component magnitude of d and 
                // d1 is the parallel component magnitude of d
                vector2 d1, d2;
                float axis1len, axis2len;
                float d1len, d2len;

                d1 = n * (d%n);
                d2 = d - d1;

                axis1len = axis1.len();
                axis2len = axis2.len();
                d1len = d1.len();
                d2len = d2.len();

                // Check the bbox of the entity
                if ( d1len - radius < axis1len &&
                     d2len - radius < axis2len )
                {
                    vector2 vRadius = axis1;

                    if ( axis2len > axis1len )
                    {
                        vRadius = axis2;
                    }

                    if ( d.len() - radius < vRadius.len() )
                    {
                        obstacle = element;  

                        vector3 p1 = p0 + l1*2;
                        vector3 p2 = p1 + l2*2;
                        vector3 p3 = p0 + l2*2;
                        nArray<vector3> v;

                        v.Append (p0);
                        v.Append (p1);
                        v.Append (p2);
                        v.Append (p3);

                        this->check.Set (v);
                        this->distances[0] = vector3 (d.x, 0, d.y);
                        this->distances[1] = vector3 (d1.x, 0, d1.y);
                        this->distances[2] = vector3 (d2.x, 0, d2.y);
                    }

                    if ( paxis1 && paxis2 )
                    {
                        vector3 p1 = point - midpoint;

                        if ( axis1len - d1len > axis2len - d2len )
                        {
                            *paxis1 = vector3 (d1.x, 0.f, d1.y);

                            axis2len += radius;
                            axis2.norm();
                            axis2 *= axis2len;
                            *paxis2 = vector3 (axis2.x, 0.f, axis2.y);

                            // Check the direction
                            if ( p1.dot (*paxis2) < 0 )
                            {
                                *paxis2 *= -1.f;
                            }
                        }
                        else
                        {
                            *paxis2 = vector3 (d2.x, 0.f, d1.y);

                            axis1len += radius;
                            axis1.norm();
                            axis1 *= axis1len;
                            *paxis1 = vector3 (axis1.x, 0.f, axis1.y);

                            // Check the direction
                            if ( p1.dot (*paxis1) < 0 )
                            {
                                *paxis1 *= -1.f;
                            }
                        }
                    }

                    this->distances[3] = vector3 (vRadius.x, 0, vRadius.y);

                    float finalDist = this->distances[3].len() + radius;
                    this->distances[3].norm();
                    this->distances[3] *= finalDist;
                }
            }
        }
    }

    this->collide = obstacle;

    return obstacle;
}

//------------------------------------------------------------------------------
/**
    GetByteSize
*/
int
nNavMesh::GetByteSize()
{
    int size = 0;

    // Navigation mesh
    for ( int i = 0; i < this->mesh.Size(); ++i )
    {
        size += this->mesh[i]->GetByteSize();
    }

    // Navigation graph
    size += this->navGraph.GetHeight() * this->navGraph.GetWidth() * sizeof(int);

    // Obstacles
    for ( int i = 0; i < this->obstacles.Size(); ++i )
    {
        size += this->obstacles[i]->GetNumVertices() * sizeof(vector3);
        size += sizeof(polygon*);
    }

    return size;
}

//------------------------------------------------------------------------------
/**
    LoadResource

    This method is either called directly from the nResource::Load() method
    (in synchronous mode), or from the loader thread (in asynchronous mode).
    The method must try to validate its resources, set the valid and pending
    flags, and return a success code.
    This method may be called from a thread.
*/
bool
nNavMesh::LoadResource()
{
    n_assert( !this->IsValid() );

    // Clear any previous data
    this->UnloadResource();

    // Load the navigation mesh
    nString filename( this->GetFilename().Get() );
    bool success = false;
    if ( !filename.IsEmpty() )
    {
        success = nNavMeshPersist::Load(filename.Get(), this);
    }

    // Set an empty and invalid navigation mesh if load fails
    if ( !success )
    {
        this->UnloadResource();
    }
    else
    {
        this->SetState(Valid);
    }

    return success;
}

//------------------------------------------------------------------------------
/**
    UnloadResource
*/
void
nNavMesh::UnloadResource()
{
    this->DestroyObstacles();
    this->Destroy();
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    InsertIntoSpace
*/
void
nNavMesh::InsertIntoSpace( SpaceType spaceType )
{
    // Space type flags (outdoor/indoor)
    int flags = 0;
    if ( spaceType == INDOOR )
    {
        flags |= nSpatialTypes::SPF_ALL_INDOORS;
        flags |= nSpatialTypes::SPF_ALL_CELLS;
        flags |= nSpatialTypes::SPF_INTERSECTING;
        flags |= nSpatialTypes::SPF_USE_BBOX;
    }
    else
    {
        flags |= nSpatialTypes::SPF_OUTDOORS;
        flags |= nSpatialTypes::SPF_ALL_CELLS;
        flags |= nSpatialTypes::SPF_INTERSECTING;
        flags |= nSpatialTypes::SPF_USE_BBOX;
    }

    // Fit the bounding box of all nodes to their respective polygon and insert them into the spatial server
    for ( int i(0); i < this->mesh.Size(); ++i )
    {
        ncNavMeshNode* node( this->mesh[i] );
        ncSpatialMC* spatialComp( node->GetEntityObject()->GetComponentSafe<ncSpatialMC>() );
        bbox3 nodeBox;
        node->GetZone()->GetBBox( &nodeBox );
        if ( spaceType == OUTDOOR )
        {
            // Place bbox for terrain nodes just below the terrain, so other outdoor nodes (brush nodes)
            // are found first when looking for a nav node below a goal point
            const float BuryDepth( 2.f );
            nEntityObject* outdoor( nSpatialServer::Instance()->GetOutdoorEntity() );
            n_assert(outdoor);
            ncTerrainGMMClass* terrain( outdoor->GetClassComponent<ncTerrainGMMClass>() );
            terrain->GetHeightMap()->CalculateBoundingBoxLC(
                rectangle( vector2(nodeBox.vmin.x, nodeBox.vmin.z), vector2(nodeBox.vmax.x, nodeBox.vmax.z) ),
                nodeBox );
            nodeBox.vmin.y -= BuryDepth;
            nodeBox.vmax.y = nodeBox.vmin.y;
        }
        spatialComp->SetBBox( nodeBox );
        n_verify( nSpatialServer::Instance()->InsertEntity( node->GetEntityObject(), flags ) );

        node->SetIsInOutdoor( spaceType == OUTDOOR );

#ifndef NGAME
        node->LookForBoundaryEdges();
#endif
    }
}

//------------------------------------------------------------------------------
/**
    Get the cross point where a ray exits the navegable space

    @param ray Ray to check if it exits the navegable space
    @retval crossPoint Point in the navegable boundary the ray is crossing
    @retval crossNormal Normal to the boundary at the cross point, looking inside
    @return True if the ray exits the navegable space, false otherwise (including if the ray starts out)
*/
bool
nNavMesh::GetBoundaryPoint( const line3& ray, vector3& crossPoint, vector3& crossNormal ) const
{
    ncNavMeshNode* meshNode( this->GetNode( ray.start() ) );
    if ( !meshNode )
    {
        // Already out of the navigation mesh
        return false;
    }

    // Look for the navigation mesh boundary by following the ray across the mesh nodes
    const int MaxNeighbors( 20 );
    for ( int i(0); i < MaxNeighbors; ++i )
    {
        ncNavMeshNode* neighbor;
        switch ( meshNode->GetCrossInfo( ray, neighbor, crossPoint, crossNormal ) )
        {
            case ncNavMeshNode::STAYING_INSIDE:
                // Not leaving the current node, so not leaving the navegable space
                return false;
            case ncNavMeshNode::GOING_TO_NEIGHBOR:
                // Continue searching on neighbors
                meshNode = neighbor;
                break;
            case ncNavMeshNode::GOING_OUT_OF_MESH:
                // Leaving the current node, return the last cross point and normal,
                // unless navigation nodes are found that validate the last segment
                {
                    if ( this->GetWaypointNode( crossPoint ) && this->GetWaypointNode( ray.end() ) )
                    {
                        if ( nNavUtils::IsWalkable( crossPoint, ray.end(), NULL ) )
                        {
                            // There's a close node validating the movement
                            return false;
                        }
                    }
                    // No nodes found validating the movement out of the mesh,
                    // so report navegable boundary found
                    return true;
                }
        }
    }

    // Some rare case has been reached where lots of neighbors are being crossed
    // The last edge crossed is taken as the navigation boundary
    return true;
}

//------------------------------------------------------------------------------
/**
    GetHeightMap
*/
nFloatMap*
nNavMesh::GetHeightMap() const
{
    nEntityObject* outdoor( nSpatialServer::Instance()->GetOutdoorEntity() );
    n_assert( outdoor );
    ncTerrainGMMClass* terrainGMM( outdoor->GetClassComponentSafe<ncTerrainGMMClass>() );
    nFloatMap* heightMap( terrainGMM->GetHeightMap() );
    n_assert( heightMap );
    return heightMap;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    DrawObstacles
*/
void 
nNavMesh::DrawObstacles (nGfxServer2* server, float high)
{
    vector4 blue  (0.f, 0.f, 1.f, 1.f);
    vector4 green (0.f, 1.f, 0.f, 1.f);
    vector4 color;
    matrix44 theMatrix;

    server->SetTransform(nGfxServer2::Model, theMatrix);
    server->BeginLines();

    for ( int i=0; i<this->obstacles.Size(); i++ )
    {
        polygon* poly = this->obstacles[i];

        if ( this->obstacles[i] == this->collide )
        {
            color = green;
        }
        else
        {
            color = blue;
        }

        if ( poly )
        {
            int iVertices = poly->GetNumVertices();
            vector3 edges[30];
            vector3 close[2];
            
            for ( int j=0; j<iVertices; j++ )
            { 
                edges[j] = poly->GetVertex(j);
                nNavUtils::GetGroundHeight( edges[j], edges[j].y );
                edges[j].y += high;
            }

            if ( iVertices > 1 )
            {
                close[0] = poly->GetVertex(0);
                close[1] = poly->GetVertex(iVertices-1);
                nNavUtils::GetGroundHeight( close[0], close[0].y );
                nNavUtils::GetGroundHeight( close[1], close[1].y );
                close[0].y += high;
                close[1].y += high;
            }

            server->DrawLines3d (edges, iVertices, color);
            server->DrawLines3d (close, 2, color);
        }
    }

    if ( this->collide )
    {
        vector3 edges[4], close[2];

        int i;
        for ( i=0; i<4; i++ )
        {
            edges[i] = this->check.GetVertex(i);
            nNavUtils::GetGroundHeight( edges[i], edges[i].y );
            edges[i].y += 0.4f;
        }

        close[0] = edges[0];
        close[1] = edges[3];

        server->DrawLines3d (edges, 4, vector4 (1.f, 1.f, 0.f, 1.f));
        server->DrawLines3d (close, 2, vector4 (1.f, 0.5f, 0.2f, 1.f));

        // Draw the distance vectors
        edges[0] = this->collide->Midpoint();
        nNavUtils::GetGroundHeight( edges[0], edges[0].y );
        edges[0].y += 0.4f;
        edges[1] = edges[0] + this->distances[0];
        server->DrawLines3d (edges, 2, vector4 (0.8f, 0.8f, 1.f, 1.f));

        // Draw the circle
        vector3 points[90];
        float radius = 0.39f;
        i=0;

        for ( float f=0; f<N_TWOPI; f+=0.25f, i++ )
        {
            points[i] = edges[1] + vector3 (radius*n_cos(f), 0.f, radius*n_sin(f));
        }
        server->DrawLines3d (points, i, vector4(1.f, 1.f, 0.f, 1.f));

        edges[1] = edges[0] + this->distances[1];
        server->DrawLines3d (edges, 2, vector4 (0.8f, 0.8f, 0.2f, 1.f));
        
        edges[1] = edges[0] + this->distances[2];        
        server->DrawLines3d (edges, 2, vector4 (0.8f, 0.8f, 0.4f, 1.f));
    }

    server->EndLines();
}
#endif // !NGAME

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------

