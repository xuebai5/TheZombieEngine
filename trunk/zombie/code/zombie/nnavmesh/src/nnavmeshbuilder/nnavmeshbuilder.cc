#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  nnavmesbuilder.cc
//------------------------------------------------------------------------------

#include "nnavmeshbuilder/nnavmeshbuilder.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "ncnavmesh/ncnavmeshobstacle.h"
#include "nnavmeshparser/nnavmeshpersist.h"
#include "nphysics/nphygeomspace.h"
#include "nphysics/nphyspace.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/nphygeomtrans.h"
#include "nphysics/nphygeomtrimesh.h"
#include "nphysics/ncphycompositeobj.h"
#include "mathlib/polygon.h"
#include "ndelaunay/ndelaunay.h"
#include "kernel/nlogclass.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialmc.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "entity/nentityobjectserver.h"
#include "nphysics/ncphyterrain.h"

#ifndef __ZOMBIE_EXPORTER__
#ifndef NGAME
#include "conjurer/nobjecteditorstate.h"
#endif
#endif

//#define MAX_HIGH        2.0f
#define EPSILON_POINTS  0.05f
#define SMALL_SIZE      0.3f
#define FIT_TEST        0.05f
#define FIT_VALUE       0.01f
#define ALIGN_RANGE     0.05f
//#define SEA_LEVEL       0.f /*-9.8f*/
//#define MAX_SLOPE       45.f

#define DRAW_HIGH       0.4f

namespace
{
    float MIN_HIGH              = 0.2f;
    float MAX_HIGH              = 1.5f;
    bool  USE_TRIANGLE_MESHES   = true;
    float MIN_SLOPE             = N_HALFPI / 3;
    float MAX_SLOPE             = N_HALFPI / 2;
    float SEA_LEVEL             = 0.f;
    bool  USE_TERRAIN_HOLES     = false;
    bool  MERGE_POLYGONS        = true;
    bool  FIT_TO_OBSTACLES      = false;
    float MIN_OBSTACLE_SIZE     = 0.2f;
    bool  GENERATE_PORTALS      = true;

    // Used to count the amount of time used by the IsInsideGeom method
    nTime timeInsideGeom;
};

NCREATELOGLEVEL (delaunay, "Delaunay Triangulation", false, 2 ) 
NCREATELOGLEVEL_REL( navbuild, "Nav. Mesh Generation", true, 2 )

//------------------------------------------------------------------------------
/**
    SetMinObstacleHeight
*/
void nNavMeshBuilder::SetMinObstacleHeight( float height )
{
    MIN_HIGH = height;
}

//------------------------------------------------------------------------------
/**
    GetMinObstacleHeight
*/
float nNavMeshBuilder::GetMinObstacleHeight()
{
    return MIN_HIGH;
}

//------------------------------------------------------------------------------
/**
    SetCharacterHeight
*/
void nNavMeshBuilder::SetCharacterHeight( float height )
{
    MAX_HIGH = height;
}

//------------------------------------------------------------------------------
/**
    GetCharacterHeight
*/
float nNavMeshBuilder::GetCharacterHeight()
{
    return MAX_HIGH;
}

//------------------------------------------------------------------------------
/**
    SetTriangleMeshesAsObstacles
*/
void nNavMeshBuilder::SetTriangleMeshesAsObstacles( bool enable )
{
    USE_TRIANGLE_MESHES = enable;
}

//------------------------------------------------------------------------------
/**
    GetTriangleMeshesAsObstacles
*/
bool nNavMeshBuilder::GetTriangleMeshesAsObstacles()
{
    return USE_TRIANGLE_MESHES;
}

//------------------------------------------------------------------------------
/**
    SetMinHardSlope
*/
void nNavMeshBuilder::SetMinHardSlope( float angle )
{
    MIN_SLOPE = angle;
}

//------------------------------------------------------------------------------
/**
    GetMinHardSlope
*/
float nNavMeshBuilder::GetMinHardSlope()
{
    return MIN_SLOPE;
}

//------------------------------------------------------------------------------
/**
    SetMaxWalkableSlope
*/
void nNavMeshBuilder::SetMaxWalkableSlope( float angle )
{
    MAX_SLOPE = angle;
}

//------------------------------------------------------------------------------
/**
    GetMaxWalkableSlope
*/
float nNavMeshBuilder::GetMaxWalkableSlope()
{
    return MAX_SLOPE;
}

//------------------------------------------------------------------------------
/**
    SetSeaLevel
*/
void nNavMeshBuilder::SetSeaLevel( float height )
{
    SEA_LEVEL = height;
}

//------------------------------------------------------------------------------
/**
    GetSeaLevel
*/
float nNavMeshBuilder::GetSeaLevel()
{
    return SEA_LEVEL;
}

//------------------------------------------------------------------------------
/**
    Enable/disable the use of terrain holes to hole the navmesh
*/
void nNavMeshBuilder::SetTerrainHolesEnabled( bool enable )
{
    USE_TERRAIN_HOLES = enable;
}

//------------------------------------------------------------------------------
/**
    Tell if terrain holes are used to hole the navmesh
*/
bool nNavMeshBuilder::GetTerrainHolesEnabled()
{
    return USE_TERRAIN_HOLES;
}

//------------------------------------------------------------------------------
/**
    Enable/disable the merging of polygons in the generated navmesh
*/
void nNavMeshBuilder::SetMergePolygons( bool enable )
{
    MERGE_POLYGONS = enable;
}

//------------------------------------------------------------------------------
/**
    Tell if polygons in the generated navmesh will be merged
*/
bool nNavMeshBuilder::GetMergePolygons()
{
    return MERGE_POLYGONS;
}

//------------------------------------------------------------------------------
/**
    Enable/disable the fitting of polygons to obstacles' boundary
*/
void nNavMeshBuilder::SetFitToObstacles( bool enable )
{
    FIT_TO_OBSTACLES = enable;
}

//------------------------------------------------------------------------------
/**
    Tell if polygons are fit to obstacles' boundary
*/
bool nNavMeshBuilder::GetFitToObstacles()
{
    return FIT_TO_OBSTACLES;
}

//------------------------------------------------------------------------------
/**
    Set the minimum size for obstacles
*/
void nNavMeshBuilder::SetMinObstacleSize( float size )
{
    MIN_OBSTACLE_SIZE = size;
}

//------------------------------------------------------------------------------
/**
    Get the minimum size for obstacles
*/
float nNavMeshBuilder::GetMinObstacleSize()
{
    return MIN_OBSTACLE_SIZE;
}

//------------------------------------------------------------------------------
/**
    Enable/disable the generation of portals
*/
void nNavMeshBuilder::SetGeneratePortals( bool enable )
{
    GENERATE_PORTALS = enable;
}

//------------------------------------------------------------------------------
/**
    Tell if the generation of portals is enabled
*/
bool nNavMeshBuilder::GetGeneratePortals()
{
    return GENERATE_PORTALS;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMeshBuilder::nNavMeshBuilder()
{
    this->mesh = 0;
    this->floorHigh = 0;
    this->floorLow = 0;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMeshBuilder::nNavMeshBuilder (nNavMesh* mesh)
{
    this->mesh = mesh;
    this->floorHigh = 0;
    this->floorLow = 0;
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nNavMeshBuilder::~nNavMeshBuilder()
{
    this->mesh = 0;
    this->points.Reset();
    this->nodes.Reset(); 
    
    this->DestroyTriMesh(); 
    this->DestroyObstacles();    
}

//------------------------------------------------------------------------------
/**
    DestroyTriMesh
*/
void 
nNavMeshBuilder::DestroyTriMesh()
{
    while ( this->triMesh.Size() )
    {
        polygon* tri = this->triMesh[0];
        n_delete (tri);
        this->triMesh.Erase (0);
    }
}

//------------------------------------------------------------------------------
/**
    SetNavMesh
*/
void 
nNavMeshBuilder::SetNavMesh (nNavMesh* mesh)
{
    this->mesh = mesh;
}

//------------------------------------------------------------------------------
/**
    SetupFloor
*/
void    
nNavMeshBuilder::SetupFloor (NavSpace& navSpace)
{
    float high = 0.f;
    float low = 0.f;
    this->floors.Clear();

    for ( int index(0); index < navSpace.GetNumGeometries(); ++index ) // && high == 0.f; ++it )
    {
        nPhysicsGeom* obstacle (navSpace.GetGeometry(index));
        nPhysicsGeom* child( this->GetShapeGeometry(obstacle) );

        if ( obstacle && child->HasAttributes (nPhysicsGeom::ground) )
        {
            nPhysicsAABB bbox;

            obstacle->GetAABB (bbox);
            if ( this->floors.Empty() )
            {
                high = bbox.maxy;
                low = bbox.miny;
            }
            else
            {
                if ( high < bbox.maxy )
                {
                    high = bbox.maxy;
                }
                if ( low > bbox.miny )
                {
                    low = bbox.miny;
                }
            }
            this->floors.Append( obstacle );
        }
    }

    this->floorHigh = high;
    this->floorLow = low;
}

//------------------------------------------------------------------------------
/**
    SetIndoorSpace
*/
void 
nNavMeshBuilder::SetIndoorSpace( nPhySpace* phySpace )
{
    this->spaceType = nNavMesh::INDOOR;
    this->navSpace.SetRootEntity( phySpace );
    this->SetPhysicSpace( this->navSpace );
}

//------------------------------------------------------------------------------
/**
    SetOutdoorSpace
*/
void 
nNavMeshBuilder::SetOutdoorSpace( nPhySpace* phySpace )
{
    this->spaceType = nNavMesh::OUTDOOR;
    this->navSpace.SetRootEntity( phySpace );

#ifndef __ZOMBIE_EXPORTER__
#ifndef NGAME
    nObjectEditorState* selState = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    n_assert( selState );
    if ( selState && selState->GetSelectionMode() == nObjectEditorState::ModeTerrainCell )
    {
        nPhysicsAABB bbox;
        this->GetWalkableCellsAABB( bbox );
        this->navSpace.SetAABB( bbox );
    }
#endif
#endif

    this->SetPhysicSpace( this->navSpace );
}

//------------------------------------------------------------------------------
/**
    SetBrushSpace
*/
void 
nNavMeshBuilder::SetBrushSpace( ncPhyCompositeObj* phyObj )
{
    this->spaceType = nNavMesh::BRUSH;
    this->navSpace.SetRootEntity( phyObj );
    this->SetPhysicSpace( this->navSpace );
}

//------------------------------------------------------------------------------
/**
    SetPhysicSpace
*/
void 
nNavMeshBuilder::SetPhysicSpace (NavSpace& navSpace)
{
    NLOG_REL( navbuild, (NLOGUSER | 1, "[1/4] Setting physics space...") );

    // First, set up the floor 
    this->SetupFloor (navSpace);

    this->DestroyObstacles();
    this->points.Reset();

    if ( this->IsOutdoor() )
    {
        // Build the height map
        this->BuildHeightMap();
        this->BuildSlopes();
        if ( USE_TERRAIN_HOLES )
        {
            this->BuildHoles();
        }
    }
    
    // Second, build all obstacles at the stage
    this->BuildObstacles( this->navSpace );
    this->InsertObstaclesIntoSpace();

    // Third, set up points for triangulation
    this->SetupPoints();

    // Prepare the tri mesh for a new triangulation
    this->DestroyTriMesh();    
}

//------------------------------------------------------------------------------
/**
    GetWalkableCellsAABB
*/
void
nNavMeshBuilder::GetWalkableCellsAABB( nPhysicsAABB& N_IFNDEF_NGAME(bbox) )
{
#ifndef __ZOMBIE_EXPORTER__
#ifndef NGAME
    nObjectEditorState* selState( static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") ) );
    if ( selState->GetSelectionMode() != nObjectEditorState::ModeTerrainCell )
    {
        return;
    }
    else if ( selState->GetSelectionCount() == 0 )
    {
        bbox.maxx = bbox.minx;
        bbox.maxz = bbox.minz;
        return;
    }
    else
    {
/*        float minx( bbox.minx );
        float minz( bbox.minz );
        bbox.minx = bbox.maxx;
        bbox.minz = bbox.maxz;
        bbox.maxx = minx;
        bbox.maxz = minz;*/
        bbox.minx = FLT_MAX;
        bbox.minz = FLT_MAX;
        bbox.maxx = -FLT_MAX;
        bbox.maxz = -FLT_MAX;
    }

    // Get cell size
    ncTerrainGMMClass* terrain( nSpatialServer::Instance()->GetOutdoorEntity()->GetClassComponentSafe<ncTerrainGMMClass>() );
    float cellSize( float(terrain->GetBlockSize() - 1) * terrain->GetPointSeparation() );

    // Find min and max of selected cells
    for ( int c(0); c < selState->GetSelectionCount(); ++c )
    {
        ncTerrainGMMCell* cell( selState->GetSelectedEntity(c)->GetComponentSafe<ncTerrainGMMCell>() );
        float minX( cell->GetCellX() * cellSize );
        float maxX( minX + float(cellSize) );
        float minZ( cell->GetCellZ() * cellSize );
        float maxZ( minZ + float(cellSize) );
        if ( bbox.minx > minX )
        {
            bbox.minx = minX;
        }
        if ( bbox.maxx < maxX )
        {
            bbox.maxx = maxX;
        }
        if ( bbox.minz > minZ )
        {
            bbox.minz = minZ;
        }
        if ( bbox.maxz < maxZ )
        {
            bbox.maxz = maxZ;
        }
    }
#endif // !__ZOMBIE_EXPORTER__
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    InsertVertexes
*/
void
nNavMeshBuilder::InsertVertexes (polygon* poly)
{
    n_assert(poly);

    if ( poly )
    {
        for ( int i=0; i<poly->GetNumVertices(); i++ )
        {
            vector3 vertex = poly->GetVertex(i);
            this->InsertVertex (vertex);
        }

        bbox3 bbox;

        poly->GetBBox (&bbox);

        const float offset = 0.5f;
        float high = this->floorHigh;
        int numVertices = poly->GetNumVertices();

        // Insert more points por triangulation generation
        for ( int i=0; i<numVertices; i++ )
        {
            int indexStart = i%numVertices;
            int indexEnd = (i+1)%numVertices;
            vector3 start = poly->GetVertex(indexStart);
            vector3 edge = poly->GetVertex(indexEnd) - start;

            for ( float f = offset; f<edge.len(); f+= offset )
            {
                vector3 point = edge;

                point.norm();
                point *= f;

                point += start;
                point.y = high;

                this->InsertVertex (point);
            }
        } 
    }
}

//------------------------------------------------------------------------------
/**
    InsertVertexes
*/
void 
nNavMeshBuilder::InsertVertexes (NavSpace& phyGeom, bool intermediate)
{
    vector3 v1, v2, v3, v4;
    nPhysicsAABB bbox;

//    if ( phyGeom )
    {
        const float offset = 1.f;

        // First, we insert the points of the space
//        if ( phyGeom->IsSpace() || this->IsObstacle(phyGeom) )
        {
            float high = this->floorHigh;

            phyGeom.GetAABB (bbox);

            v1 = vector3 (bbox.minx, high, bbox.minz);
            v2 = vector3 (bbox.minx, high, bbox.maxz);
            v3 = vector3 (bbox.maxx, high, bbox.maxz);
            v4 = vector3 (bbox.maxx, high, bbox.minz);

            //---------------------------------------------
            // Transformations
/*            if ( !phyGeom->IsSpace() )
            {
                if ( phyGeom->Type() == nPhysicsGeom::Box )
                {
                }
            }*/

            //---------------------------------------------
            this->InsertVertex (v1);
            this->InsertVertex (v2);
            this->InsertVertex (v3);
            this->InsertVertex (v4);

            if ( intermediate )
            {
                // Insert more points por triangulation generation
                for ( float x = bbox.minx; x < bbox.maxx - offset; x+= offset )                
                {
                    vector3 dummy1 (x, high, bbox.minz);
                    vector3 dummy2 (x, high, bbox.maxz);
                    
                    this->InsertVertex (dummy1);
                    this->InsertVertex (dummy2);
                }

                for ( float z = bbox.minz; z < bbox.maxz - offset; z+= offset )                
                {
                    vector3 dummy1 (bbox.minx, high, z);
                    vector3 dummy2 (bbox.maxx, high, z);
                    
                    this->InsertVertex (dummy1);
                    this->InsertVertex (dummy2);
                } 
            } // intermediate
        }
    }
}

//------------------------------------------------------------------------------
/**
    InsertVertex
*/
void
nNavMeshBuilder::InsertVertex (const vector3& vertex)
{
    bool found = false;

    for ( int i=0; i<this->points.Size() && !found; i++ )
    {
        found = this->points[i].isequal (vertex, EPSILON_POINTS);
    }

    if ( !found )
    {
        this->points.Append (vertex);
    }
}

//------------------------------------------------------------------------------
/**
    GetWalkablePolygons
*/
void
nNavMeshBuilder::GetWalkablePolygons( nPhyGeomTriMesh* triMesh, nArray<polygon*>& polys ) const
{
    n_assert( triMesh );

    vector3 position;
    triMesh->GetAbsolutePosition( position );
    quaternion quatOrientation;
    triMesh->GetAbsoluteOrientation( quatOrientation );
    matrix33 orientation( quatOrientation );

    int numTriangles( triMesh->GetNumberIndexes() / 3 );
    for ( int i(0); i < numTriangles; ++i )
    {
        int* index( triMesh->GetTriangleIndex(i) );

        nArray<vector3> points;
        points.SetFixedSize(3);

        for ( int j(0); j < 3; ++j )
        {
            int indexNum( *(index + j) );
            phyreal* coord( triMesh->GetVertex(indexNum) );

            points[j] = vector3( coord[0], coord[1], coord[2] );
            points[j] = orientation * points[j];
            points[j] += position;
        }

        // Keep polygons within a maximum walkable slope
        vector3 normal( ( points[1] - points[0] ) * ( points[2] - points[0] ) );
        normal.norm();
        vector3 up(0,1,0);
        if ( normal % up > 0.5f )
        {
            for ( int j(0); j < 3; ++j )
            {
                points[j].y = this->floorHigh;
            }
            polygon* poly( n_new(polygon)(points) );
            polys.Append( poly );
        }
    }
}

//------------------------------------------------------------------------------
/**
    GetPerimeterEdges
*/
void
nNavMeshBuilder::GetPerimeterEdges( nArray<polygon*>& polys, nArray<line3*>& edges ) const
{
    nArray<line3*> allEdges;
    nArray<int> sharedCount;

    // Get all the edges and count how many polygons share each one of them
    for ( int i(0); i < polys.Size(); ++i )
    {
        polygon* poly( polys[i] );
        for ( int j(0); j < poly->GetNumVertices(); ++j )
        {
            vector3 start( poly->GetVertex(j) );
            vector3 end( poly->GetVertex( (j+1) % poly->GetNumVertices() ) );

            // Check if the edge has been inserted yet
            bool shared( false );
            for ( int k(0); k < allEdges.Size(); ++k )
            {
                if ( ( allEdges[k]->start().isequal( start, N_TINY ) && allEdges[k]->end().isequal( end  , N_TINY ) ) ||
                     ( allEdges[k]->start().isequal( end  , N_TINY ) && allEdges[k]->end().isequal( start, N_TINY ) ) )
                {
                    ++sharedCount[k];
                    shared = true;
                    break;
                }
            }

            if ( !shared )
            {
                allEdges.Append( n_new(line3)( start, end ) );
                sharedCount.Append( 1 );
            }
        }
    }

    // The edges belonging to only one polygon are the perimeter edges
    for ( int i(0); i < allEdges.Size(); ++i )
    {
        if ( sharedCount[i] == 1 )
        {
            edges.Append( allEdges[i] );
        }
        else
        {
            n_delete( allEdges[i] );
        }
    }
}

//------------------------------------------------------------------------------
/**
    InsertEdges
*/
void
nNavMeshBuilder::InsertEdges( nArray<line3*>& edges )
{
    for ( int i(0); i < edges.Size(); ++i )
    {
        // Inserted edge vertices
        const vector3& start( edges[i]->start() );
        const vector3& end( edges[i]->end() );
        this->InsertVertex( start );
        this->InsertVertex( end );

        // Insert intermediate points to achieve a better fit to edges on triangulation
        const float offset( 0.5f );
        float len( edges[i]->len() );
        vector3 point( start );
        vector3 inc( (end - start) );
        inc.norm();
        inc *= offset;
        for ( float alpha( offset ); alpha < len; alpha += offset )
        {
            point += inc;
            this->InsertVertex( point );
        }
    }
}

//------------------------------------------------------------------------------
/**
    ReleasePolygons
*/
void
nNavMeshBuilder::ReleasePolygons( nArray<polygon*>& polys )
{
    for ( int i(0); i < polys.Size(); ++i )
    {
        n_delete( polys[i] );
    }
    polys.Clear();
}

//------------------------------------------------------------------------------
/**
    ReleaseEdges
*/
void
nNavMeshBuilder::ReleaseEdges( nArray<line3*>& edges )
{
    for ( int i(0); i < edges.Size(); ++i )
    {
        n_delete( edges[i] );
    }
    edges.Clear();
}

//------------------------------------------------------------------------------
/**
    GetRectangle
*/
void
nNavMeshBuilder::GetRectangle( nPhyGeomBox* box, polygon& poly ) const
{
    n_assert( box );

    vector3 position;
    box->GetAbsolutePosition( position );
    quaternion quatOrientation;
    box->GetAbsoluteOrientation( quatOrientation );
    matrix33 orientation( quatOrientation );

    matrix44 model;
    vector3 lengths;
    nArray<vector3> points;
    vector3 d[8];
    nArray<vector3> v;
    
    box->GetLengths (lengths);
    
    d[0] = vector3 (-lengths.x*0.5f, -lengths.y*0.5f, -lengths.z*0.5f);
    d[1] = vector3 (-lengths.x*0.5f, -lengths.y*0.5f,  lengths.z*0.5f);
    d[2] = vector3 ( lengths.x*0.5f, -lengths.y*0.5f,  lengths.z*0.5f);
    d[3] = vector3 ( lengths.x*0.5f, -lengths.y*0.5f, -lengths.z*0.5f);
    d[4] = vector3 (-lengths.x*0.5f,  lengths.y*0.5f, -lengths.z*0.5f);
    d[5] = vector3 (-lengths.x*0.5f,  lengths.y*0.5f,  lengths.z*0.5f);
    d[6] = vector3 ( lengths.x*0.5f,  lengths.y*0.5f,  lengths.z*0.5f);
    d[7] = vector3 ( lengths.x*0.5f,  lengths.y*0.5f, -lengths.z*0.5f);

    for ( int i(0); i<8; i++ )
    {
        vector3 aux;

        orientation.mult (d[i], aux);
        aux += position;
        v.Append (aux);
    }

    float minHigh = v[0].y;

    for ( int i(1); i<v.Size(); i++ )
    {
        if ( v[i].y < minHigh )
        {
            minHigh = v[i].y;
        }
    }  

    for ( int i(0); i<v.Size(); i++ )
    {
        v[i].y = this->floorHigh;
    }
    
    poly.Set (v);
    poly.MakeRectacle();
}

//------------------------------------------------------------------------------
/**
    SetupPoints
*/
void 
nNavMeshBuilder::SetupPoints()
{
    this->InsertVertexes (this->navSpace);

    if ( !this->IsOutdoor() )
    {
        // Insert points all along the floor perimeter
        for ( int i(0); i < this->floors.Size(); ++i )
        {
            nPhysicsGeom* child( this->GetShapeGeometry( this->floors[i] ) );
            switch ( child->Type() )
            {
                case nPhysicsGeom::TriangleMesh:
                    {
                        nPhyGeomTriMesh* triMesh( static_cast<nPhyGeomTriMesh*>( child ) );

                        nArray<polygon*> polys;
                        nArray<line3*> edges;

                        this->GetWalkablePolygons( triMesh, polys );
                        this->GetPerimeterEdges( polys, edges );
                        this->InsertEdges( edges );
                        this->ReleasePolygons( polys );
                        this->ReleaseEdges( edges );
                    }
                    break;
                case nPhysicsGeom::Box:
                    {
                        nPhyGeomBox* box( static_cast<nPhyGeomBox*>( child ) );

                        polygon poly;
                        this->GetRectangle( box, poly );
                        this->InsertVertexes( &poly );
                    }
                    break;
            }
        }

/*                vector3 position;
                child->GetAbsolutePosition( position );
                quaternion quatOrientation;
                child->GetAbsoluteOrientation( quatOrientation );
                matrix33 orientation( quatOrientation );

                int numTriangles( triMesh->GetNumberIndexes() / 3 );
                for ( int i(0); i < numTriangles; ++i )
                {
                    int* index( triMesh->GetTriangleIndex(i) );

                    nArray<vector3> points;
                    points.SetFixedSize(3);

                    for ( int j(0); j < 3; ++j )
                    {
                        int indexNum( *(index + j) );
                        phyreal* coord( triMesh->GetVertex(indexNum) );

                        points[j] = vector3( coord[0], coord[1], coord[2] );
                        points[j] = orientation * points[j];
                        points[j] += position;
                    }

                    // Insert polygons within a maximum walkable slope
                    vector3 normal( ( points[1] - points[0] ) * ( points[2] - points[0] ) );
                    normal.norm();
                    vector3 up(0,1,0);
                    if ( n_abs( normal % up ) < 0.5f )
                    {
                        for ( int j(0); j < 3; ++j )
                        {
                            points[j].y = this->floorHigh;
                        }
                        polygon poly( points );
                        this->InsertVertexes( &poly );
                    }
                }*/

/*                for ( int i(0); i < triMesh->GetNumberVertexes(); ++i )
                {
                    phyreal* phyVertex( triMesh->GetVertex(i) );
                    vector3 vertex( phyVertex[0], 0, phyVertex[2] );
                    vertex = orientation * vertex;
                    vertex += position;
                    vertex.y = this->floorHigh;
                    this->InsertVertex( vertex );
                }*/
    }

    for ( int i=0; i<this->obstacles.Size(); i++ )
    {
        polygon* poly = this->obstacles[i];

        if ( MIN_OBSTACLE_SIZE > TINY )
        {
            int numVertices = poly->GetNumVertices();
            n_assert( numVertices == 4 );
            if ( numVertices == 4 )
            {
                vector3 v0 = poly->GetVertex(0);
                vector3 v1 = poly->GetVertex(1);
                vector3 v2 = poly->GetVertex(2);
                vector3 v3 = poly->GetVertex(3);
                vector3 edge0 = v1 - v0;
                vector3 edge1 = v2 - v1;
                float exceeding0 = edge0.len() - MIN_OBSTACLE_SIZE;
                float exceeding1 = edge1.len() - MIN_OBSTACLE_SIZE;
                if ( exceeding0 < 0 )
                {
                    edge0.norm();
                    edge0 *= -exceeding0 * 0.5f;
                    poly->SetVertex( 0, v0 - edge0 );
                    poly->SetVertex( 1, v1 + edge0 );
                    poly->SetVertex( 2, v2 + edge0 );
                    poly->SetVertex( 3, v3 - edge0 );
                }
                if ( exceeding1 < 0 )
                {
                    edge1.norm();
                    edge1 *= -exceeding1 * 0.5f;
                    poly->SetVertex( 0, poly->GetVertex(0) - edge1 );
                    poly->SetVertex( 1, poly->GetVertex(1) - edge1 );
                    poly->SetVertex( 2, poly->GetVertex(2) + edge1 );
                    poly->SetVertex( 3, poly->GetVertex(3) + edge1 );
                }
            }
        }

        this->InsertVertexes (poly);
    }   
}

//------------------------------------------------------------------------------
/**
    GenerateMesh
*/
bool
nNavMeshBuilder::GenerateMesh()
{
    // Creation of navigation mesh
    NLOG_REL( navbuild, (NLOGUSER | 1, "[2/4] Calculating Delaunay...") );
    this->CalculateDelaunay();
    NLOG_REL( navbuild, (NLOGUSER | 1, "[3/4] Smoothing mesh...") );
    this->SmoothMesh();
    
    // Creation of navigation graph
    NLOG_REL( navbuild, (NLOGUSER | 1, "[4/4] Building navigation graph...") );
    this->GenerateLinks();    

    // Creation of navigation portals
    if ( GENERATE_PORTALS )
    {
        NLOG_REL( navbuild, (NLOGUSER | 1, "[4/4] Building navigation portals...") );
        this->GeneratePortals();
    }

    this->CleanSmallNodes (TINY);//SMALL_SIZE);
    this->CleanOrphanNodes();
    //this->BuildNavGraph();

    NLOG_REL( navbuild, (NLOGUSER | 1, "Inserting navigation mesh into space...") );
    this->RemoveObstaclesFromSpace();
    this->InsertMeshIntoSpace();

    // is resource has become valid now, so flag it
    if (this->mesh)
    {
        this->mesh->SetState(nResource::Valid);
    }

    NLOG_REL( navbuild, (NLOGUSER | 1, "Navigation mesh generated!") );
    return true;
}

//------------------------------------------------------------------------------
/**
    GenerateLinks

    @brief Generates the links between neightbour nodes. It must be called after
    build the whole mesh. Really, here is where the real navigation mesh is created
*/
void
nNavMeshBuilder::GenerateLinks()
{
    this->CreateNavMesh();

    // Find neighbours!!!
    nTime startTime( nTimeServer::Instance()->GetTime() );
    for ( int i=0; i<this->nodes.Size(); i++ )
    {
        if ( nTimeServer::Instance()->GetTime() - startTime > 10 )
        {
            startTime = nTimeServer::Instance()->GetTime();
            NLOG_REL( navbuild, (NLOGUSER | 1, "[4/4] Generating links...%d/%d", i+1, this->nodes.Size()) );
        }

        ncNavMeshNode* node = this->nodes[i];

        if ( node )
        {
            this->LinkNeighbours (node);
        }
    }

    // Now, we can adjust the vertices to the obstacles
    if ( FIT_TO_OBSTACLES )
    {
        this->Fit2Obstacles();
    }
}

//------------------------------------------------------------------------------
/**
    Generate the needed portals between nodes

    Edges are used as the portals to neighbor nodes, but after the merging
    process some edges that connect to a neighbor doesn't match exactly with the
    neighbor's respective edge. This method splits these edges so two connected
    nodes share the same edge.
*/
void
nNavMeshBuilder::GeneratePortals()
{
    for ( int i(0); i < this->nodes.Size(); ++i )
    {
        ncNavMeshNode* node = this->nodes[i];
        for ( int j(0); j < node->GetLocalLinksNumber(); ++j )
        {
            ncNavMeshNode* neighbor = node->GetLocalLink(j)->GetComponentSafe<ncNavMeshNode>();
            polygon* poly1 = node->GetZone();
            polygon* poly2 = neighbor->GetZone();
            poly1->SplitOverlappingEdges( *poly2 );
        }
    }
}

//------------------------------------------------------------------------------
/**
    SmoothMesh
*/
void
nNavMeshBuilder::SmoothMesh()
{   
    // First, clean the mesh, deleting dummy triangles and triangles inside obstacles
    timeInsideGeom = 0;
    nTime startTime( nTimeServer::Instance()->GetTime() );
    this->CleanMesh();    
    nTime endTime( nTimeServer::Instance()->GetTime() );
    NLOG_REL( navbuild, (NLOGUSER | 0, "[3/4] Time elapsed for cleaning mesh: %.0f seconds", endTime - startTime) );
    NLOG_REL( navbuild, (NLOGUSER | 0, "[3/4] Time used for IsInsideGeom: %0.f seconds", timeInsideGeom) );

    // Second, merge the trimesh, deleting unneccesaries polygons
    if ( MERGE_POLYGONS )
    {
        this->MergeTri(); 
    }
    else
    {
        this->CleanDummyTri();
    }
}

//------------------------------------------------------------------------------
/**
    Drawx 
*/
void
nNavMeshBuilder::Draw (nGfxServer2* server)
{
    this->DrawVertexes (server);
    this->DrawEdges (server);
    this->DrawSlopes (server);
}

//------------------------------------------------------------------------------
/**
    DrawEdges
*/
void
nNavMeshBuilder::DrawEdges (nGfxServer2* server)
{
    vector4 red  (1.f, 0.f, 0.f, 1.f);
    vector4 green(0.f, 1.f, 0.f, 1.f);
    vector4 blue (0.f, 0.f, 1.f, 1.f);
    vector4 color;

    matrix44 theMatrix;
    float high = 0.4f;

    server->SetTransform(nGfxServer2::Model, theMatrix);
    server->BeginLines();

    for ( int i=0;i<this->triMesh.Size(); i++ )
    {
        polygon* poly = this->triMesh[i];        
        vector3 vertexes[20], close[2];
        vector3 center = poly->Midpoint();
        
        for ( int j=0; j<poly->GetNumVertices(); j++ )
        {
            vertexes[j] = poly->GetVertex(j);
            vertexes[j].y += DRAW_HIGH;
        }

        switch (i%3)
        {
        case 0: 
            color = red;  
            break;

        case 1: 
            color = green; 
            break;

        case 2: 
            color = blue;  
            break;
        }

        color = red;

        close[0] = vertexes[poly->GetNumVertices()-1];
        close[1] = vertexes[0];

        vector3 crossLeft[2], crossRight[2];
        const float fCross = 0.03f;

        crossLeft[0]  = center + vector3 (-fCross, high,  fCross);
        crossLeft[1]  = center + vector3 ( fCross, high, -fCross);
        crossRight[0] = center + vector3 (-fCross, high, -fCross);
        crossRight[1] = center + vector3 ( fCross, high,  fCross);

        server->DrawLines3d (vertexes, poly->GetNumVertices(), color);
        server->DrawLines3d (close, 2, color);
        server->DrawLines3d (crossLeft, 2, red);
        server->DrawLines3d (crossRight, 2, red);    
    }

    server->EndLines();
}

//------------------------------------------------------------------------------
/**
    DrawShapes
*/
void 
nNavMeshBuilder::DrawVertexes (nGfxServer2* server)
{
    vector4 red (1.f, 0.f, 0.f, 1.f);
    matrix44 theMatrix;

    server->SetTransform(nGfxServer2::Model, theMatrix);
    server->BeginShapes();

#define  DOT_RADIUS 0.03f

    for ( int i=0; i<this->triMesh.Size(); i++ )
    {
        polygon* poly = this->triMesh[i];
        vector3 vertexes[20];

        for ( int j=0; j<poly->GetNumVertices(); j++ )
        {
            vertexes[j] = poly->GetVertex(j);
            vertexes[j].y += DRAW_HIGH;
        
            matrix44 model1, model2;

            model1.scale (vector3(DOT_RADIUS, DOT_RADIUS, DOT_RADIUS));
            model1.set_translation (vertexes[j]);            
            server->DrawShape (nGfxServer2::Sphere, model1, red);
        }        
    }

    server->EndShapes();
}

//------------------------------------------------------------------------------
/**
    DrawSlopes
*/
void 
nNavMeshBuilder::DrawSlopes(nGfxServer2* server)
{
    vector4 color (0.7f, 0.3f, 1.f, 1.f);
    matrix44 theMatrix;

    server->SetTransform(nGfxServer2::Model, theMatrix);
    server->BeginShapes();

/*#undef DOT_RADIUS
#define  DOT_RADIUS 1.5f*/

    for ( int i=0; i<this->slopes.Size(); i++ )
    {        
        vector3 point = this->slopes[i];

        point.y += DRAW_HIGH;

        matrix44 model1, model2;

        model1.scale (vector3(DOT_RADIUS, DOT_RADIUS, DOT_RADIUS));
        model1.set_translation (point);            
        server->DrawShape (nGfxServer2::Sphere, model1, color);     
    }

    server->EndShapes();
}

//------------------------------------------------------------------------------
/**
    CalculateDelaunay
*/
void 
nNavMeshBuilder::CalculateDelaunay()
{
#define MAX_POINTS 4096 * 4

    int iVertices, iTriangles = 0;    
    /*vector3 vertices[MAX_POINTS];
    nDelaunay::ITRIANGLE triangles[MAX_POINTS*3];*/
    nDelaunay delaunay;

    vector3* vertices;
    nDelaunay::ITRIANGLE* triangles;
    int points = this->points.Size() + 3; 

    vertices = n_new_array (vector3, points);
    triangles = n_new_array (nDelaunay::ITRIANGLE, points*3);
        
    this->SetupPointsDelaunay (&iVertices, vertices);

    // Inicialize the buffer for store triangles    
    delaunay.Triangulate (iVertices, vertices, triangles, &iTriangles);
    this->SetupMesh (iTriangles, triangles, iVertices, vertices);

    n_delete_array (vertices);
    n_delete_array (triangles);
}

//------------------------------------------------------------------------------
/**
    SetupPointsDelaunay
*/
void
nNavMeshBuilder::SetupPointsDelaunay (int* nv, vector3 pxyz[]) const
{
    if ( nv ) //&& pxyz )
    {
        int iPoints = this->points.Size();

        *nv = iPoints;          // Necessary for delaunay algorithm
                   
        for ( int i=0; i < iPoints; i++ )
        {
            vector3 point = this->points[i];

            pxyz[i].x = point.x;
            pxyz[i].y = point.y;
            pxyz[i].z = point.z;
        }

        // Sort the points in x-axis
        qsort (pxyz, iPoints, sizeof(vector3), nNavMeshBuilder::PointSorter);        
    }
}
 
//------------------------------------------------------------------------------
/**
    SetupMesh
*/
void
nNavMeshBuilder::SetupMesh (int iNumTriangles, nDelaunay::ITRIANGLE* triangles, int iNumVertices, vector3* vertices)
{
    this->DestroyTriMesh();

    if ( iNumVertices == 0 )
    {
        return;
    }

    for ( int i=0; i<iNumTriangles; i++ )
    {
        nDelaunay::ITRIANGLE tri = triangles[i];
        float x1, x2, x3, y1, y2, y3, z1, z2, z3;
  
        nArray<vector3> points;
        
        x1 = vertices[tri.p1].x;
        x2 = vertices[tri.p2].x;
        x3 = vertices[tri.p3].x;
        y1 = vertices[tri.p1].y;
        y2 = vertices[tri.p2].y;
        y3 = vertices[tri.p3].y;
        z1 = vertices[tri.p1].z;
        z2 = vertices[tri.p2].z;
        z3 = vertices[tri.p3].z;
        
        points.Append (vector3(x1, y1, z1));
        points.Append (vector3(x2, y2, z2));
        points.Append (vector3(x3, y3, z3));
        
        polygon* poly = n_new(polygon)(points);
        poly->SortCounterClock();
        
        this->triMesh.Append (poly);
    }
}

//------------------------------------------------------------------------------
/**
    PointSorter
*/
int
__cdecl
nNavMeshBuilder::PointSorter(const void* elm0, const void* elm1)
{
    vector3 p0 = *(vector3*)elm0;
    vector3 p1 = *(vector3*)elm1;

    if ( p0.x < p1.x )
    {
        return -1;
    }
    else
    if ( p0.x == p1.x )
    {
        if ( p0.z < p1.z )
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

//------------------------------------------------------------------------------
/**
    CleanMesh

    @brief Clean the mesh, deleting unnecessaries triangles
*/
void
nNavMeshBuilder::CleanMesh()
{
    float radius = 0.05f;

    // Look for triangles inside of obstacles
    nTime startTime( nTimeServer::Instance()->GetTime() );
    for ( int i=0; i<this->triMesh.Size(); )
    {
        if ( nTimeServer::Instance()->GetTime() - startTime > 10 )
        {
            startTime = nTimeServer::Instance()->GetTime();
            NLOG_REL( navbuild, (NLOGUSER | 1, "[3/4] Cleaning mesh...%d/%d", i+1, this->triMesh.Size()) );
        }

        vector3 midpoint = this->triMesh[i]->Midpoint();

        if ( this->IsOutSpace(midpoint) ||  
             this->IsInsideGeom(midpoint, radius) ||
             (!this->IsOutdoor() && this->IsOutFloor (midpoint) ) ||
             (this->IsOutdoor() && (this->IsAtSea (midpoint) || 
                                    this->IsAtSlope(midpoint) ||
                                    this->IsInHole(midpoint)) ) )
        {
            polygon* dummy = this->triMesh[i];
            n_delete (dummy;)
            this->triMesh.Erase(i);
        }
        else
        {
            i++;
        }
    }
}

//------------------------------------------------------------------------------
/**
    IsIndoor

    Says if the current physics space belongs to an indoor
*/
bool
nNavMeshBuilder::IsIndoor() const
{
    return this->spaceType == nNavMesh::INDOOR;
}

//------------------------------------------------------------------------------
/**
    IsIndoor

    Says if the current physics space belongs to an outdoor
*/
bool
nNavMeshBuilder::IsOutdoor() const
{
    return this->spaceType == nNavMesh::OUTDOOR;
}

//------------------------------------------------------------------------------
/**
    IsBrush

    Says if the current physics space belongs to a brush
*/
bool
nNavMeshBuilder::IsBrush() const
{
    return this->spaceType == nNavMesh::BRUSH;
}

//------------------------------------------------------------------------------
/**
    IsOutSpace

    @brief Says if a given point is outside the space
*/
bool
nNavMeshBuilder::IsOutSpace (const vector3& point) const
{ 
    bool bInside = false;

    nPhysicsAABB bbox;

    this->navSpace.GetAABB (bbox);            

    // carles.ros: Do not check for height boundaries. This method is called to
    // cull delauny triangles (if called from somewhere else another method may
    // be needed) and delauny triangles seem to have 0 height (or sea/ground
    // level height?). Also, the bbox given by the phySpace seems to have both
    // min and max at the terrain minimum height defined when creating the
    // terrain class (tested with terrains with heights above the minimum
    // height).
/*    bInside = bbox.maxx >= point.x && bbox.minx <= point.x &&
        bbox.maxy >= point.y && bbox.miny <= point.y &&
        bbox.maxz >= point.z && bbox.minz <= point.z;*/
    bInside = bbox.maxx >= point.x && bbox.minx <= point.x &&
        bbox.maxz >= point.z && bbox.minz <= point.z;

    return !bInside;
}

//------------------------------------------------------------------------------
/**
    Says if the AABB of a physics geometry is outside the space
*/
bool
nNavMeshBuilder::IsOutSpace( nPhysicsGeom* geom ) const
{ 
    nPhysicsAABB geomBBox;
    nPhysicsAABB spaceBBox;
    geom->GetAABB( geomBBox );
    this->navSpace.GetAABB( spaceBBox );
    
    return geomBBox.maxx < spaceBBox.minx || geomBBox.maxz < spaceBBox.minz ||
        geomBBox.minx > spaceBBox.maxx || geomBBox.minz > spaceBBox.maxz;
}

//------------------------------------------------------------------------------
/**
    IsOutFloor

    @brief Says if a given point is outside the floor
*/
bool nNavMeshBuilder::IsOutFloor (const vector3& point) const
{
    vector3 origin( point );
    origin.y = this->floorHigh + 0.1f;
    float len( 0.2f + this->floorHigh - this->floorLow );
    const int MaxContacts( 50 );
    nPhyCollide::nContact contact[ MaxContacts ];
    int contactsNumber( nPhysicsServer::Collide( origin, vector3(0,-1,0), len,
        MaxContacts, contact, nPhysicsGeom::Static | nPhysicsGeom::Ramp ) );

    bool floorFound( false );
    for ( int i(0); i < contactsNumber && !floorFound; ++i )
    {
        floorFound = contact[i].GetGeometryA()->GetAttributes() & nPhysicsGeom::ground ||
                     contact[i].GetGeometryB()->GetAttributes() & nPhysicsGeom::ground;
    }

    return !floorFound;
}

//------------------------------------------------------------------------------
/**
    IsInsideGeom

    @brief Says if a given point is inside a physic geometry
*/
bool 
nNavMeshBuilder::IsInsideGeom (const vector3& point, float radius) const
{
    nTime startTime( nTimeServer::Instance()->GetTime() );

    bool bInside = false;
    
    for ( float f=0; f<N_TWOPI && !bInside; f+= N_HALFPI*0.25f )
    {
        vector3 test = point + vector3 (radius*n_cos(f), 0.f, radius*n_sin(f));
        bInside = this->GetObstacle( test ) != NULL;
    }

    nTime endTime( nTimeServer::Instance()->GetTime() );
    timeInsideGeom += endTime - startTime;

    return bInside;
}

//------------------------------------------------------------------------------
/**
    IsAtSea

    @brief Says if a given point is at sea, using the height map of an outdoor space
*/
bool
nNavMeshBuilder::IsAtSea (const vector3& point) const
{
    nSpatialServer* spatialServer = nSpatialServer::Instance();
    n_assert(spatialServer);
    ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
    n_assert(terrainGMM);
    nFloatMap* heightMap = terrainGMM->GetHeightMap();
    n_assert(heightMap);

    float height;

    heightMap->GetHeight (point.x, point.z, height);

    return height <= SEA_LEVEL;
}

//------------------------------------------------------------------------------
/**
    IsAtSlope
*/
bool
nNavMeshBuilder::IsAtSlope (const vector3& point) const
{
    nSpatialServer* spatialServer = nSpatialServer::Instance();
    n_assert(spatialServer);
    ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
    n_assert(terrainGMM);
    nFloatMap* heightMap = terrainGMM->GetHeightMap();
    n_assert(heightMap);
    float scale = heightMap->GetGridScale();
    n_assert (scale!=0.f);

    int i = int (point.x / scale);
    int j = int (point.z / scale);

    //static const float minSlope = N_HALFPI / 3;
//    static const float minSlope = N_PI * MAX_SLOPE / 180.f;
    const float minSlope( MAX_SLOPE );
    
    return this->slopeTable.At (i, j) > minSlope; // != 0;
}

//------------------------------------------------------------------------------
/**
    IsInHole
*/
bool
nNavMeshBuilder::IsInHole( const vector3& N_IFNDEF_NGAME(point) ) const
{
#ifndef NGAME
    ncPhyTerrain* phyTerrain( this->GetPhysicsTerrain() );
    n_assert( phyTerrain );

    for ( int holeIndex(0); holeIndex < phyTerrain->GetNumHoles(); ++holeIndex )
    {
        if ( phyTerrain->GetHole(holeIndex)->IsPointInside( vector2(point.x, point.z) ) )
        {
            return true;
        }
    }
#endif // !NGAME

    return false;
}

//------------------------------------------------------------------------------
/**
    IsFloor    
*/
bool
nNavMeshBuilder::IsFloor (nPhysicsGeom* geom) const
{
    n_assert(geom);
    
    bool floor = false;

    if ( geom )
    {
        nPhysicsGeom* child( this->GetShapeGeometry(geom) );
        floor = bool ( /*child->Type() == nPhysicsGeom::TriangleMesh ||*/
                       child->HasAttributes (nPhysicsGeom::ground) );
    }

    return floor;
}

//------------------------------------------------------------------------------
/**
    IsCeiling
*/
bool
nNavMeshBuilder::IsCeiling (nPhysicsGeom* geom) const
{
    n_assert(geom);

    bool ceiling = false;

    if ( geom )
    {
        nPhysicsGeom* child( this->GetShapeGeometry(geom) );
        ceiling = bool (child->HasAttributes (nPhysicsGeom::ceiling));
    }

    return ceiling;
}

//------------------------------------------------------------------------------
/**
    IsWall
*/
bool
nNavMeshBuilder::IsWall (nPhysicsGeom* geom) const
{
    n_assert(geom);

    bool wall = false;

    if ( geom )
    {
        nPhysicsGeom* child( this->GetShapeGeometry(geom) );
        wall = bool (child->HasAttributes (nPhysicsGeom::wall));
    }

    return wall;
}

//------------------------------------------------------------------------------
/**
    IsWalkable
*/
bool
nNavMeshBuilder::IsWalkable( nPhysicsGeom* geom ) const
{
    n_assert( geom );
    nPhysicsGeom* child( this->GetShapeGeometry(geom) );
    return child->HasAttributes( nPhysicsGeom::walkable );
}

//------------------------------------------------------------------------------
/**
    IsStatic
*/
bool
nNavMeshBuilder::IsStatic( nPhysicsGeom* geom ) const
{
    n_assert( geom );
    nPhysicsGeom* child( this->GetShapeGeometry(geom) );
    return (child->GetCategories() & ( nPhysicsGeom::Static | nPhysicsGeom::Stairs )) != 0;
}

//------------------------------------------------------------------------------
/**
    IsNearFloor    
*/
bool
nNavMeshBuilder::IsNearFloor( nPhysicsGeom* geom ) const
{
    n_assert( geom );

    nPhysicsAABB bbox;
    geom->GetAABB( bbox );

    float floorHeight;

    if ( this->IsOutdoor() )
    {
        vector3 pos;
        geom->GetAbsolutePosition( pos );
        this->GetHeightMap()->GetHeight( pos.x, pos.z, floorHeight );
    }
    else
    {
        floorHeight = this->floorHigh;
    }

    return ( bbox.maxy >= floorHeight + MIN_HIGH ) &&
           ( bbox.miny <  floorHeight + MAX_HIGH );
}

//------------------------------------------------------------------------------
/**
    CleanDummyTri

    @brief Delete the polygons that has less points that a given value in the mesh
*/
void 
nNavMeshBuilder::CleanDummyTri (int vertices)
{
    for ( int i=0; i<this->triMesh.Size(); )
    {
        polygon* poly = this->triMesh[i];

        if ( poly->GetNumVertices() <= vertices )
        {
            polygon* dummy = this->triMesh[i];
            n_delete (dummy);
            this->triMesh.Erase (i);
        }
        else
        {
            i++;
        }
    }
}

//------------------------------------------------------------------------------
/**
    MergeTri
*/
void
nNavMeshBuilder::MergeTri()
{
    bool bDone = false;
    int iterationCount( 0 );

    while ( !bDone )
    {
        int i=0; 
        bDone = true;
        int mergeCount( 0 );
        int initialMeshSize( this->triMesh.Size() );
        nTime startTime( nTimeServer::Instance()->GetTime() );
        ++iterationCount;

        //for ( int i=0; i<this->triMesh.Size(); )
        while ( i<this->triMesh.Size() )
        {
            if ( nTimeServer::Instance()->GetTime() - startTime > 10 )
            {
                startTime = nTimeServer::Instance()->GetTime();
                NLOG_REL( navbuild, (NLOGUSER | 1, "[3/4] Merging mesh...%d/%d (iteration %d, merged %d polygons)",
                    i+1, this->triMesh.Size(), iterationCount, mergeCount) );
            }

            polygon* poly = this->triMesh[i];
            n_assert(poly);

            if ( poly )
            {
                polygon* neighbour = this->GetBestNeighbour (poly);

                if ( neighbour )
                {
                    poly->Join2Polygon (neighbour);
                    poly->CleanVertices();
                    this->RemovePoly (neighbour);
                    bDone = false;
                    ++mergeCount;
                }
                else
                {
                    i++;
                }
            }
            // else, infinite loop!!
        }

        NLOG_REL( navbuild, (NLOGUSER | 1, "[3/4] Merge iteration %d: %d/%d polygons merged",
            iterationCount, mergeCount, initialMeshSize) );
    } // while

    // Delete polygons with no points
    this->CleanDummyTri();

    if ( this->CleanVertices() )
    {        
        this->MergeTri();
    } 
}

//------------------------------------------------------------------------------
/**
    GetBestNeighbour
*/
polygon*
nNavMeshBuilder::GetBestNeighbour (const polygon* poly) const
{
    polygon* neighbour = 0;
    float maxArea = 0.f;

    for ( int i=0; i<this->triMesh.Size(); i++ )
    {
        polygon* element = this->triMesh[i];
        n_assert(element);

        if ( element && element != poly )
        {
            if ( poly->CanMerge(element) )
            {
                polygon test1(*poly), test2(*element);

                test1.Join2Polygon (&test2);

                if ( test1.GetArea() > maxArea )
                {
                    neighbour = element;
                }
            }
        }        
    }

    return neighbour;
}

//------------------------------------------------------------------------------
/**
    CleanSmallNodes
*/
void
nNavMeshBuilder::CleanSmallNodes (float threshold)
{
    if ( this->mesh )
    {
        for ( int i=0; i<this->mesh->mesh.Size(); )
        {
            ncNavMeshNode* node = this->mesh->mesh[i];
            
            if ( node->zone && 
                (node->zone->GetWidth() < threshold && node->zone->GetHeight() < threshold ) && 
                 node->GetLocalLinksNumber() < 2 )
            {
                this->mesh->ReleaseNode (node);
            }
            else
            {
                i++;
            }
        } // for
    }
}

//------------------------------------------------------------------------------
/**
    CleanOrphanNodes
*/
void
nNavMeshBuilder::CleanOrphanNodes()
{
    if ( this->mesh && this->mesh->mesh.Size() > 1 )
    {
        for ( int i=0; i<this->mesh->mesh.Size(); )
        {
            ncNavMeshNode* node = this->mesh->mesh[i];

            if ( node && node->GetLocalLinksNumber() == 0 )
            {
                this->mesh->ReleaseNode (node);
            }
            else
            {
                i++;
            }
        } // for
    }
}

//------------------------------------------------------------------------------
/**
    CreateNavMesh
*/
void
nNavMeshBuilder::CreateNavMesh()
{
    n_assert(mesh);

    if ( mesh )
    {
        this->mesh->Destroy();
        this->nodes.Reset();

        int size = this->triMesh.Size();

        nEntityObject* newNode = 0;
        ncNavMeshNode* meshNodeComp = 0;
        //for ( int i=0; i<this->triMesh.Size(); i++ )
        for ( int i=0; i<size; i++ )
        {
            const polygon* poly = const_cast<polygon*>(this->triMesh[i]);
            newNode = nEntityObjectServer::Instance()->NewLocalEntityObject("nenavmeshnode");
            meshNodeComp = newNode->GetComponentSafe<ncNavMeshNode>();
            meshNodeComp->CreateNode(poly);
            //ncNavMeshNode* node = n_new (nNavMeshNode)(poly);

            this->mesh->InsertNode (meshNodeComp);
            this->nodes.Append (meshNodeComp);
        }

        this->mesh->SetObstacles (this->obstacles);
    }
}

//------------------------------------------------------------------------------
/**
    Fit2Obstacles
*/
void
nNavMeshBuilder::Fit2Obstacles()
{
    for ( int i=0; i<this->mesh->mesh.Size(); i++ )
    {
        ncNavMeshNode* node = this->mesh->mesh[i];        
        n_assert(node);

        //if ( poly )
        if ( node && node->GetZone() )
        {
            vector3 midpoint;
            node->GetMidpoint( midpoint );
            polygon* poly = node->GetZone();

            const int MaxFitsPerPoint = 100;
            for ( int j=0, fitsCount=0; j<poly->GetNumVertices(); )
            {
                vector3 vertex = poly->GetVertex(j);
                vector3 test = vertex - midpoint;

                test.norm();
                test *= FIT_TEST;
                test = vertex - test;

                if ( this->IsInsideGeom (test, -0.01f) )
                {
                    vector3 fit = test;
                    poly->SetVertex (j, fit);
                    if ( ++fitsCount >= MaxFitsPerPoint )
                    {
                        NLOG_REL( navbuild, (NLOGUSER | 2, "*** Warning: Couldn't finish obstacles fit for the point %d (polygon %d)", j, i) );
                        j++;
                        fitsCount = 0;
                    }
                }
                else
                {
                    j++;
                    fitsCount = 0;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    LinkNeighbours

    @brief Find the neighbours of a give navigation node and links them to it
*/
void 
nNavMeshBuilder::LinkNeighbours (ncNavMeshNode* node) const
{
    n_assert(node);

    if ( node )
    {
        for ( int i=0; i<this->nodes.Size(); i++ )
        {
            ncNavMeshNode* element = this->nodes[i];

            if ( element && element != node )
            {
                if ( this->AreNeighbours (node, element) )
                {
                    // Update the nodes
                    node->AddLocalLink (element);
                    element->AddLocalLink (node);
                }
            }
        } // for
    }
}


//------------------------------------------------------------------------------
/**
    AreNeighbours

    @brief Says if two nodes must be neighbours wihin the navigation graph
*/
bool
nNavMeshBuilder::AreNeighbours (ncNavMeshNode* node1, ncNavMeshNode* node2) const
{
    n_assert (node1&&node2);

    bool neighbours = false;

    if ( node1 && node2 )
    {
        polygon* poly1 = node1->GetZone();
        polygon* poly2 = node2->GetZone();

        if ( poly1 && poly2 )
        {
            neighbours = poly1->ShareEdge (poly2) || poly1->PartialEdge (poly2);
        }       
    }

    return neighbours;
}

//------------------------------------------------------------------------------
/**
    BuildObstacles
*/
void
nNavMeshBuilder::BuildObstacles( NavSpace& space )
{
    for ( int index(0); index < space.GetNumGeometries(); ++index )
    {
        nPhysicsGeom* parentGeom( space.GetGeometry(index) );
        n_assert(parentGeom);

        if ( !parentGeom )
        {
            continue;
        }

        if ( parentGeom->IsSpace() )
        {
            NavSpace childSpace;
            childSpace.SetRootEntity( static_cast<nPhySpace*>( parentGeom ) );
            this->BuildObstacles( childSpace );
//            this->BuildObstacles( static_cast<nPhySpace*>( parentGeom ) );
        }
        else
        {
            nPhysicsGeom* phyGeom( this->GetShapeGeometry(parentGeom) );

            switch ( phyGeom->Type() )
            {
            case nPhysicsGeom::Box:
                {
                    if ( this->IsObstacle (parentGeom) )
                    {
                        quaternion orientationQuaternion;
                        matrix33 orientation;
                        vector3 position;

                        phyGeom->GetAbsoluteOrientation (orientationQuaternion);
                        phyGeom->GetAbsolutePosition (position);
                        orientation = orientationQuaternion;
                        
                        matrix44 model;
                        vector3 lengths;
                        polygon* obstacle = n_new(polygon);
                        nArray<vector3> points;
                        vector3 d[8];
                        nArray<vector3> v;
                        
                        nPhyGeomBox* phyBox = (nPhyGeomBox*) phyGeom;
                        phyBox->GetLengths (lengths);
                      
                        d[0] = vector3 (-lengths.x*0.5f, -lengths.y*0.5f, -lengths.z*0.5f);
                        d[1] = vector3 (-lengths.x*0.5f, -lengths.y*0.5f,  lengths.z*0.5f);
                        d[2] = vector3 ( lengths.x*0.5f, -lengths.y*0.5f,  lengths.z*0.5f);
                        d[3] = vector3 ( lengths.x*0.5f, -lengths.y*0.5f, -lengths.z*0.5f);
                        d[4] = vector3 (-lengths.x*0.5f,  lengths.y*0.5f, -lengths.z*0.5f);
                        d[5] = vector3 (-lengths.x*0.5f,  lengths.y*0.5f,  lengths.z*0.5f);
                        d[6] = vector3 ( lengths.x*0.5f,  lengths.y*0.5f,  lengths.z*0.5f);
                        d[7] = vector3 ( lengths.x*0.5f,  lengths.y*0.5f, -lengths.z*0.5f);

                        int i;
                        for ( i=0; i<8; i++ )
                        {
                            vector3 aux;

                            orientation.mult (d[i], aux);
                            aux += position;
                            v.Append (aux);
                        }

                        float minHigh = v[0].y;

                        for ( i=1; i<v.Size(); i++ )
                        {
                            if ( v[i].y < minHigh )
                            {
                                minHigh = v[i].y;
                            }
                        }  

                        for ( i=0; i<v.Size(); i++ )
                        {
                            v[i].y = this->floorHigh;
                        }
                        
                        obstacle->Set (v);
                        obstacle->MakeRectacle();

                        this->obstacles.Append (obstacle);                            
                    }
                }
                break;

            case nPhysicsGeom::TriangleMesh:
                if ( USE_TRIANGLE_MESHES && this->IsObstacle (parentGeom) )
                {
                    nPhyGeomTriMesh* triMesh( static_cast<nPhyGeomTriMesh*>( phyGeom ) );
                    nArray<vector3> points;

                    // Get all points, projected on floor
                    vector3 position;
                    triMesh->GetAbsolutePosition( position );
                    quaternion quatOrientation;
                    triMesh->GetAbsoluteOrientation( quatOrientation );
                    matrix33 orientation( quatOrientation );

                    int numVertexes( triMesh->GetNumberVertexes() );
                    for ( int i(0); i < numVertexes; ++i )
                    {
                        phyreal* coord( triMesh->GetVertex(i) );
                        vector3 point( coord[0], coord[1], coord[2] );
                        point = orientation * point;
                        point += position;
                        point.y = this->floorHigh;
                        points.Append( point );
                    }

                    // Build a rectangle around the projected points
                    polygon* obstacle( n_new(polygon) );
                    obstacle->Set( points );
                    obstacle->MakeRectacle();

                    // Use the rectangle to approximate this physics mesh as an obstacle
                    this->obstacles.Append( obstacle );
                }
                break;

            case nPhysicsGeom::Sphere:
            case nPhysicsGeom::RegularFlatEndedCylinder:            
                {
                    nPhysicsAABB bbox;
                    parentGeom->GetAABB (bbox);

                    if ( this->IsObstacle (parentGeom) )
                    {
                        polygon* obstacle = n_new(polygon);
                        nArray<vector3> points;

                        vector3 v1(bbox.minx, this->floorHigh, bbox.minz);
                        vector3 v2(bbox.minx, this->floorHigh, bbox.maxz);
                        vector3 v3(bbox.maxx, this->floorHigh, bbox.maxz);
                        vector3 v4(bbox.maxx, this->floorHigh, bbox.minz);

                        points.Append (v1);
                        points.Append (v2);
                        points.Append (v3);
                        points.Append (v4);

                        obstacle->Set(points);
                        this->obstacles.Append (obstacle);
                    }
                }
                break;
            }                
        }
    }
}

//------------------------------------------------------------------------------
/**
    IsObstacle
*/
bool
nNavMeshBuilder::IsObstacle (nPhysicsGeom* geom) const
{
    n_assert(geom);
    bool obstacles = false;

    if ( geom )
    {
        obstacles = this->IsStatic (geom) &&
                    !this->IsOutSpace (geom) &&
                    ( this->IsWall (geom) ||
                      (!this->IsFloor (geom) &&
                       !this->IsCeiling (geom) &&
                       !this->IsWalkable (geom) &&
                        this->IsNearFloor (geom) ) );
    }

    return obstacles;
}

//------------------------------------------------------------------------------
/**
    DestroyObstaclces
*/
void 
nNavMeshBuilder::DestroyObstacles()
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
   GetObstacle 
*/
polygon*
nNavMeshBuilder::GetObstacle (const vector3& point) const
{
    nArray< nEntityObject* > entities;
    if ( this->IsIndoor() )
    {
        nSpatialServer::Instance()->GetEntitiesCategory( point, nSpatialTypes::CAT_NAVMESHOBSTACLES,
            nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_ALL_CELLS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_BBOX, entities );
    }
    else
    {
        nSpatialServer::Instance()->GetEntitiesCategory( point, nSpatialTypes::CAT_NAVMESHOBSTACLES,
            nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_ALL_CELLS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_BBOX, entities );
    }

    for ( int i(0); i < entities.Size(); ++i )
    {
        polygon* obstacle = entities[i]->GetComponentSafe<ncNavMeshObstacle>()->GetPolygonalRepresentation();
        n_assert( obstacle );
        if ( obstacle->IsPointInside( point.x, point.z ) )
        {
            return obstacle;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
    CleanVertices
*/
bool 
nNavMeshBuilder::CleanVertices()
{
    bool changes = false;

    for ( int i=0; i<this->triMesh.Size(); )
    {
        polygon* poly = this->triMesh[i];

        if ( poly )
        {
            changes |= poly->CleanVertices();

            // Verify if some vertices are inside obstacles ande remove them
            if ( poly->GetNumVertices() < 3 )
            {
                triMesh.Erase (i);
                n_delete (poly);
            }
            else
            {
                i++;
            }
        }
    }   

    return changes;
}

//------------------------------------------------------------------------------
/**
    RemoveVerticesInObtacles
*/
bool 
nNavMeshBuilder::RemoveVerticesInObstacles (polygon* poly)
{
    bool changes = false;

    for ( int i=0; i<poly->GetNumVertices(); )
    {
        vector3 vertex = poly->GetVertex (i);

        if ( this->IsInsideGeom(vertex) || this->IsOutSpace(vertex) )
        {
            poly->RemoveVertex (i);
        }
        else
        {
            i++;
        }
    }

    return changes;
}

//------------------------------------------------------------------------------
/**
    BuildNavGraph
*/
void
nNavMeshBuilder::BuildNavGraph()
{
    this->mesh->BuildNavGraph();
}

//------------------------------------------------------------------------------
/**
    SaveNavMesh

    @brief Save the navigation mesh to disk.
    
    The file format is chosen from the file name extension.
*/
bool
nNavMeshBuilder::SaveNavMesh (const char* filename)
{
    return nNavMeshPersist::Save(filename, this->mesh);
}

//------------------------------------------------------------------------------
/**
    SetWalkableCells

    Set all the values of the walkable table to an initial value based on which
    cells are marked to be walkable or not.
*/
void
nNavMeshBuilder::SetWalkableCells( nArray2<bool>& N_IFNDEF_NGAME(walkableTable) )
{
#ifndef __ZOMBIE_EXPORTER__
#ifndef NGAME
    // If we're not in cells selection mode, set all the map as walkable and finish,
    // otherwise set it all as non walkable and continue looking for selected cells
    nObjectEditorState* selState( static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") ) );
    bool allWalkable( selState->GetSelectionMode() != nObjectEditorState::ModeTerrainCell );
    for ( unsigned int i(0); i < walkableTable.GetWidth(); ++i )
    {
        for ( unsigned int j(0); j < walkableTable.GetHeight(); ++j )
        {
            walkableTable.Set( i, j, allWalkable );
        }
    }
    if ( allWalkable )
    {
        return;
    }

    // Get points per cell
    ncTerrainGMMClass* terrain( nSpatialServer::Instance()->GetOutdoorEntity()->GetClassComponentSafe<ncTerrainGMMClass>() );
    int cellSize( terrain->GetBlockSize() - 1 );

    // Set as walkable all the points belonging to the selected cells
    for ( int c(0); c < selState->GetSelectionCount(); ++c )
    {
        ncTerrainGMMCell* cell( selState->GetSelectedEntity(c)->GetComponentSafe<ncTerrainGMMCell>() );
        unsigned int iStart( cell->GetCellX() * cellSize );
        unsigned int iEnd( iStart + cellSize );
        unsigned int jStart( cell->GetCellZ() * cellSize );
        unsigned int jEnd( jStart + cellSize );
        for ( unsigned int i( iStart ); i < iEnd; ++i )
        {
            for ( unsigned int j( jStart ); j < jEnd; ++j )
            {
                walkableTable.Set( i, j, true );
            }
        }
    }
#endif // !__ZOMBIE_EXPORTER__
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    BuildHeightMap
*/
void
nNavMeshBuilder::BuildHeightMap()
{
    nSpatialServer* spatialServer = nSpatialServer::Instance();
    n_assert(spatialServer);
    ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
    n_assert(terrainGMM);
    nFloatMap* heightMap = terrainGMM->GetHeightMap();
    n_assert(heightMap);
    
    // Show must go on
    int size = heightMap->GetSize();
    nArray2<bool> walkableTable;
    nArray2<int> minesTable;
    
    walkableTable.SetSize (size, size);
    minesTable.SetSize (size, size);
    
    this->SetWalkableCells( walkableTable );

    int i;
    for ( i=0; i<size; i++ )
    {
        for ( int j=0; j<size; j++ )
        {   
            if ( walkableTable.At(i,j) )
            {
                float height = heightMap->GetHeightLC (i, j);
                walkableTable.Set (i, j, bool (height > SEA_LEVEL));
            }
        }
    }

    // Sweepmines algorithm
    for ( i=0; i<size; i++ )
    {
        for ( int j=0; j<size; j++ )
        {
            this->CalculateEdges (walkableTable, minesTable, i, j);
        }
    }

    float gridScale = heightMap->GetGridScale();

    // Now, we must add all points with values upper than 1
    for ( i=0; i<size; i++ )
    {
        for ( int j=0; j<size; j++ )
        {
            int value = minesTable.At (i, j);

            if ( value > 1 && value != 9 )
            {
                this->InsertVertex (vector3 (float(gridScale*i), 0.f, (float)gridScale*j));
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    BuildSlopes
*/
void
nNavMeshBuilder::BuildSlopes()
{
//    static const float minSlope = N_HALFPI / 3;
//    static const float hardSlope = N_HALFPI / 2;
    const float minSlope( MIN_SLOPE );

    nFloatMap* heightMap( this->GetHeightMap() );
    n_assert(heightMap);


    int size = heightMap->GetSize();
    
    this->slopeTable.SetSize (size, size);

    nArray2<bool> walkableTable;
    walkableTable.SetSize( size, size );
    this->SetWalkableCells( walkableTable );

    // First x direction
    for ( int j=0; j<size; j++ )
    {
        float prevHeight = 0;

        for ( int i=0; i<size; i++ )
        {   
            float height = heightMap->GetHeightLC (i, j);
            float slope = 0.f;

            if ( i > 0 )
            {
                prevHeight = heightMap->GetHeightLC (i-1, j);
            }

            slope = n_abs (n_atan (height-prevHeight));
            this->slopeTable.Set (i, j, slope);
        }
    }

    // And now, y direction
    for ( int i=0; i<size; i++ )
    {
        float prevHeight = 0;

        for ( int j=0; j<size; j++ )
        {   
            float height = heightMap->GetHeightLC (i, j);
            float slope = 0.f;
            float current = this->slopeTable.At (i, j);

            if ( j > 0 )
            {
                prevHeight = heightMap->GetHeightLC (i, j-1);
            }
            
            slope = n_abs (n_atan (height-prevHeight));

            if ( slope > current )
            {
                this->slopeTable.Set (i, j, slope);
            }
        }
    }

/*
    // Sweepmines algorithm
    for ( int i=0; i<size; i++ )
    {
        for ( int j=0; j<size; j++ )
        {
            this->CalculateEdges (auxTable, this->slopeTable, i, j);
        }
    } */
/*
    // And y direction
    for ( int i=0; i<size; i++ )
    {
        int prevValue = 0;
        int finalValue;

        for ( j=0; j<size; j++ )
        {
            int value = auxTable.At (i, j);

            if ( j > 0)
            {
                prevValue = auxTable.At (i, j-1);
            }

            finalValue = abs (value - prevValue);
            this->slopeTable.Set (i, j, finalValue);
        }
    }*/

    // Now, we must add all points with values upper than 1
    float gridScale = heightMap->GetGridScale();

    this->slopes.Clear();

    for ( int i=0; i<size; i++ )
    {
        for ( int j=0; j<size; j++ )
        {
            float value = this->slopeTable.At (i, j);

            //if ( value > 0 && value < 80 )
            if ( value > minSlope && walkableTable.At(i,j) )
            {
                this->InsertVertex (vector3 (float(gridScale*i), 0.f, (float)gridScale*j));
                this->slopes.Append(vector3 (float(gridScale*i), 0.f, (float)gridScale*j));
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    BuildHoles
*/
void
nNavMeshBuilder::BuildHoles()
{
#ifndef NGAME
    ncPhyTerrain* phyTerrain( this->GetPhysicsTerrain() );
    n_assert( phyTerrain );
    nFloatMap* heightMap( this->GetHeightMap() );
    n_assert( heightMap );
    float gridScale( heightMap->GetGridScale() );

    for ( int holeIndex(0); holeIndex < phyTerrain->GetNumHoles(); ++holeIndex )
    {
        nTerrainLine* hole( phyTerrain->GetHole(holeIndex) );
        for ( int vertexIndex(0); vertexIndex < hole->GetNumVertices(); ++vertexIndex )
        {
            vector3 v1( hole->GetPointPosition( vertexIndex ) );
            vector3 v2( hole->GetPointPosition( (vertexIndex+1) % hole->GetNumVertices() ) );
            v1.y = v2.y = 0;

            vector3 edge( v2 - v1 );
            float len( edge.len() );
            vector3 offset( edge );
            offset.norm();
            offset *= gridScale;
            vector3 point( v1 );
            for ( float alpha(0); alpha < len; alpha += gridScale, point += offset )
            {
                this->InsertVertex( point );
            }
        }
    }
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    CalculateEdges
*/
void
nNavMeshBuilder::CalculateEdges (const nArray2<bool>& walkableTable, nArray2<int>& minesTable, int row, int column)
{
    nSpatialServer* spatialServer = nSpatialServer::Instance();
    n_assert(spatialServer);
    ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
    n_assert(terrainGMM);
    nFloatMap* heightMap = terrainGMM->GetHeightMap();
    n_assert(heightMap);

    int size = heightMap->GetSize();
    int currentValue = 0;

    for ( int i=-1; i<2; i++ )
    {
        for ( int j=-1; j<2; j++ )
        {
            int finalRow = row + i;
            int finalColumn = column + j;

            if ( finalRow < 0 || finalRow >= size )
            {
                currentValue++;
            }
            else
            if ( finalColumn < 0 || finalColumn >= size )
            {
                currentValue++;
            }
            else
            {
                if ( !walkableTable.At (finalRow, finalColumn) )
                {
                    currentValue++;
                }
            }
        } // internal for
    }

    minesTable.Set (row, column, currentValue);
}

//------------------------------------------------------------------------------
/**
    RemovePoly
*/
void
nNavMeshBuilder::RemovePoly (polygon* poly)
{
    for ( int i=0; i<this->triMesh.Size() && poly; i++ )
    {
        polygon* element = this->triMesh[i];

        if ( element == poly )
        {
            this->triMesh.Erase (i);
            n_delete(poly);
            poly = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    InsertMeshIntoSpace
*/
void
nNavMeshBuilder::InsertMeshIntoSpace()
{
    if ( !this->mesh )
    {
        return;
    }

    this->mesh->InsertIntoSpace( this->spaceType );
}

//------------------------------------------------------------------------------
/**
    Insert all the obstacles in the spatial server for a fast look up
*/
void
nNavMeshBuilder::InsertObstaclesIntoSpace()
{
    // Space type flags (outdoor/indoor)
    int flags = 0;
    if ( this->IsIndoor() )
    {
        flags |= nSpatialTypes::SPF_ONE_INDOOR;
        flags |= nSpatialTypes::SPF_ALL_CELLS;
        flags |= nSpatialTypes::SPF_INTERSECTING;
        flags |= nSpatialTypes::SPF_USE_BBOX;
    }
    else
    {
        flags |= nSpatialTypes::SPF_OUTDOORS;
        flags |= nSpatialTypes::SPF_ONE_CELL;
        flags |= nSpatialTypes::SPF_CONTAINING;
        flags |= nSpatialTypes::SPF_USE_BBOX;
    }

    // Create an entity object for each obstacle and insert them into the spatial server
    for ( int i(0); i < this->obstacles.Size(); ++i )
    {
        // Create the obstacle entity object
        nEntityObject* entity = nEntityObjectServer::Instance()->NewEntityObject( "nenavmeshobstacle" );
        entity->GetComponentSafe<ncNavMeshObstacle>()->SetPolygonalRepresentation( this->obstacles[i] );
        this->obstacleEntities.Append( entity );

        // Fit the spatial bounding box to the obstacle's polygon
        bbox3 bbox;
        this->obstacles[i]->GetBBox( &bbox );
        entity->GetComponentSafe<ncSpatialMC>()->SetBBox( bbox );

        // Insert obstacle in space
        n_verify( nSpatialServer::Instance()->InsertEntity( entity, flags ) );
    }
}

//------------------------------------------------------------------------------
/**
    RemoveObstacleFromSpace
*/
void
nNavMeshBuilder::RemoveObstaclesFromSpace()
{
    for ( int i(0); i < this->obstacleEntities.Size(); ++i )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->obstacleEntities[i] );
    }
    this->obstacleEntities.Clear();
}

//------------------------------------------------------------------------------
/**
    GetShapeGeometry
*/
nPhysicsGeom*
nNavMeshBuilder::GetShapeGeometry( nPhysicsGeom* geom ) const
{
    n_assert( geom );

    while ( geom->Type() == nPhysicsGeom::GeometryTransform )
    {
        nPhysicsGeom* child( static_cast<nPhyGeomTrans*>( geom )->GetGeometry() );
        if ( !child )
        {
            n_error( "Found an orphan physics geometry transform" );
            break;
        }
        geom = child;
    }

    return geom;
}

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nNavMeshBuilder::NavSpace::NavSpace() :
    overwriteAABB( false )
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Set a physics space as the root entity of the space
*/
void
nNavMeshBuilder::NavSpace::SetRootEntity( nPhySpace* phySpace )
{
    this->rootType = PHYSIC_SPACE;
    this->rootEntity.phySpace = phySpace;
}

//------------------------------------------------------------------------------
/**
    Set a composite object as the root entity of the space
*/
void
nNavMeshBuilder::NavSpace::SetRootEntity( ncPhyCompositeObj* phyObj )
{
    this->rootType = PHYSIC_OBJECT;
    this->rootEntity.phyObj = phyObj;
}

//------------------------------------------------------------------------------
/**
    Get the number of geometries contained in the space
*/
int
nNavMeshBuilder::NavSpace::GetNumGeometries() const
{
    return this->rootType == PHYSIC_SPACE ?
        this->rootEntity.phySpace->GetNumGeometries() :
        this->rootEntity.phyObj->GetNumGeometries();
}

//------------------------------------------------------------------------------
/**
    Get a geometry by index
*/
nPhysicsGeom*
nNavMeshBuilder::NavSpace::GetGeometry( int index ) const
{
    return this->rootType == PHYSIC_SPACE ?
        this->rootEntity.phySpace->GetGeometry( index ) :
        this->rootEntity.phyObj->GetGeometry( index );
}

//------------------------------------------------------------------------------
/**
    Get the aabb wrapping the space
*/
void
nNavMeshBuilder::NavSpace::GetAABB( nPhysicsAABB& bbox ) const
{
    if ( this->overwriteAABB )
    {
        bbox = this->customAABB;
    }
    else
    {
        if ( this->rootType == PHYSIC_SPACE )
        {
            this->rootEntity.phySpace->GetAABB( bbox );
        }
        else
        {
            this->rootEntity.phyObj->GetAABB( bbox );
        }
    }
}

//------------------------------------------------------------------------------
/**
    Overwrite space original aabb with custom one
*/
void
nNavMeshBuilder::NavSpace::SetAABB( const nPhysicsAABB& bbox )
{
    this->overwriteAABB = true;
    this->customAABB = bbox;
}

//------------------------------------------------------------------------------
/**
    GetHeightMap
*/
nFloatMap*
nNavMeshBuilder::GetHeightMap() const
{
    nSpatialServer* spatialServer = nSpatialServer::Instance();
    n_assert(spatialServer);
    ncTerrainGMMClass* terrainGMM = spatialServer->GetOutdoorEntity()->GetClassComponent <ncTerrainGMMClass>();
    n_assert(terrainGMM);
    nFloatMap* heightMap = terrainGMM->GetHeightMap();
    n_assert(heightMap);
    return heightMap;
}

//------------------------------------------------------------------------------
/**
    GetPhysicsTerrain
*/
ncPhyTerrain*
nNavMeshBuilder::GetPhysicsTerrain() const
{
    nEntityObject* outdoor( nSpatialServer::Instance()->GetOutdoorEntity() );
    n_assert( outdoor );
    return outdoor->GetComponentSafe<ncPhyTerrain>();
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
