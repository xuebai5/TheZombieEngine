#ifndef N_NAVMESHBUILDER_H
#define N_NAVMESHBUILDER_H

//------------------------------------------------------------------------------
/**
    @class nMeshBuilder
    @ingroup NebulaNavmeshSystem

    Navigation mesh builder for pathfinding.    

    (C) 2005 Conjurer Services, S.A.
*/

#include "mathlib/vector.h"
#include "util/narray.h"
#include "util/narray2.h"

#include "nnavmesh/nnavmesh.h"
#include "ndelaunay/ndelaunay.h"
#include "nphysics/nphysicsaabb.h"

class ncNavMeshNode;
class nPhySpace;
class nPhysicsGeom;
class ncPhyCompositeObj;
class polygon;
class nGfxServer2;
class nFile;
class nFloatMap;
class nPhyGeomBox;
class nPhyGeomTriMesh;
class ncPhyTerrain;

class nNavMeshBuilder
{
public:
    /// Constructor
    nNavMeshBuilder ();
    nNavMeshBuilder (nNavMesh* mesh);
    /// Destructor
    ~nNavMeshBuilder();

    /// Set the space for an indoor
    void SetIndoorSpace( nPhySpace* phySpace );
    /// Set the space for an outdoor
    void SetOutdoorSpace( nPhySpace* phySpace );
    /// Set the space for a walkable brush
    void SetBrushSpace( ncPhyCompositeObj* phyObj );
    /// Set the mesh
    void SetNavMesh (nNavMesh* mesh);
    /// Generates the mesh
    bool GenerateMesh();
    
    /// Draw debug traces
    void Draw (nGfxServer2* server);

    /// Save the navigation mesh to disk. The file format is chosen from the file name extension.
    bool SaveNavMesh (const char* filename);
    
    /// Set until which height above floor level an obstacle can rise without being considered as such
    static void SetMinObstacleHeight( float height );
    /// Get until which height above floor level an obstacle can rise without being considered as such
    static float GetMinObstacleHeight();
    /// Set until which height above floor level an obstacle is considered as such
    static void SetCharacterHeight( float height );
    /// Get until which height above floor level an obstacle is considered as such
    static float GetCharacterHeight();
    /// Enable/disable the use of triable meshes as obstacles
    static void SetTriangleMeshesAsObstacles( bool enable );
    /// Enable/disable the use of triable meshes as obstacles
    static bool GetTriangleMeshesAsObstacles();
    /// Set minimum slope angle to create another navigation node
    static void SetMinHardSlope( float angle );
    /// Get minimum slope angle to create another navigation node
    static float GetMinHardSlope();
    /// Set maximum walkable slope angle
    static void SetMaxWalkableSlope( float angle );
    /// Get maximum walkable slope angle
    static float GetMaxWalkableSlope();
    /// Set the sea level
    static void SetSeaLevel( float height );
    /// Get the sea level
    static float GetSeaLevel();
    /// Enable/disable the use of terrain holes to hole the navmesh
    static void SetTerrainHolesEnabled( bool enable );
    /// Tell if terrain holes are used to hole the navmesh
    static bool GetTerrainHolesEnabled();
    /// Enable/disable the merging of polygons in the generated navmesh
    static void SetMergePolygons( bool enable );
    /// Tell if polygons in the generated navmesh will be merged
    static bool GetMergePolygons();
    /// Enable/disable the fitting of polygons to obstacles' boundary
    static void SetFitToObstacles( bool enable );
    /// Tell if polygons are fit to obstacles' boundary
    static bool GetFitToObstacles();
    /// Set the minimum size for obstacles
    static void SetMinObstacleSize( float size );
    /// Get the minimum size for obstacles
    static float GetMinObstacleSize();
    /// Enable/disable the generation of portals
    static void SetGeneratePortals( bool enable );
    /// Tell if the generation of portals is enabled
    static bool GetGeneratePortals();

private:
    /// Class to make queries to a space or a walkable object transparently
    class NavSpace
    {
    public:
        /// Default constructor
        NavSpace();
        /// Set a physics space as the root entity of the space
        void SetRootEntity( nPhySpace* phySpace );
        /// Set a composite object as the root entity of the space
        void SetRootEntity( ncPhyCompositeObj* phyObj );
        /// Get the number of geometries contained in the space
        int GetNumGeometries() const;
        /// Get a geometry by index
        nPhysicsGeom* GetGeometry( int index ) const;
        /// Get the aabb wrapping the space
        void GetAABB( nPhysicsAABB& bbox ) const;
        /// Overwrite space original aabb with custom one
        void SetAABB( const nPhysicsAABB& bbox );

    private:
        /// The root entity is a physic space or a physic object?
        enum RootType { PHYSIC_SPACE, PHYSIC_OBJECT } rootType;
        /// The root entity of the space
        union
        {
            nPhySpace* phySpace;
            ncPhyCompositeObj* phyObj;
        } rootEntity;
        /// Use custom AABB instead of space's one?
        bool overwriteAABB;
        /// Custom AABB
        nPhysicsAABB customAABB;
    };

    /// Set the space
    void SetPhysicSpace (NavSpace& navSpace);

    /// Calculate points coordinates
    void SetupPoints();

    /// Generate the links between the nodes
    void GenerateLinks();
    /// Generate the needed portals between nodes
    void GeneratePortals();
    /// Insert the vertexes of a physic geom
    void InsertVertexes (NavSpace& phyGeom, bool intermediate = true);
    /// Insert the vertices of a treated polygon as obstacle
    void InsertVertexes (polygon* poly);
    /// Insert a vertex
    void InsertVertex (const vector3& vertex);

    /// Delete the triMesh
    void DestroyTriMesh();
    
    /// Delete the obstacles structure
    void DestroyObstacles();
    
    // Draw nodes
    void DrawEdges (nGfxServer2* server);
    void DrawVertexes (nGfxServer2* server); 
    void DrawSlopes (nGfxServer2* server);

    // Use delaunay
    void CalculateDelaunay();

    /// Builds the obstacles inside the space
    void BuildObstacles( NavSpace& space );
    /// Build the height map of an outdoor terrain
    void BuildHeightMap();
    /// Build the slopes of an outdoor terrain
    void BuildSlopes();
    /// Build the holes of an outdoor terrain
    void BuildHoles();

    /// Smooth the mesh deleting unnecesaries nodes
    void SmoothMesh ();
    
    // Initialize points for Delaunay triangulation
    void SetupPointsDelaunay (int* nv, vector3 pxyz[]) const;

    // Initialize mesh
    void SetupMesh (int iNumTriangles, nDelaunay::ITRIANGLE* triangles, int iNumVertices, vector3* vertices);

    /// a qsort() hook for generating a sorted index array
    static int __cdecl PointSorter(const void* elm0, const void* elm1);

    /// Clean the mesh
    void CleanMesh();

    /// Erases dummy triangles (with no points)
    void CleanDummyTri (int vertices = 0);

    /// Clean aligned vertices in every single polygon
    bool CleanVertices();

    /// Merge triangles to convex polygons
    void MergeTri();

    /// Fits polygons to obstacles
    void Fit2Obstacles();

    /// Says if the current physics space belongs to an indoor
    bool IsIndoor() const;
    /// Says if the current physics space belongs to an outdoor
    bool IsOutdoor() const;
    /// Says if the current physics space belongs to a brush
    bool IsBrush() const;

    /// Says if a point is inside of a physic geometry
    bool IsInsideGeom (const vector3& point, float radius = 0.f) const;

    /// Says if a point is outside of the space
    bool IsOutSpace (const vector3& point) const;
    /// Says if the AABB of a physics geometry is outside of the space
    bool IsOutSpace (nPhysicsGeom* geom) const;

    /// Says if a point is outside of the floor
    bool IsOutFloor (const vector3& point) const;

    /// Says if a point is at "sea"
    bool IsAtSea (const vector3& point) const;
    /// Says if a point is at an slope
    bool IsAtSlope (const vector3& point) const;
    /// Says if a point is in a hole
    bool IsInHole( const vector3& point ) const;

    /// Initializes the mesh with the polygons created
    void CreateNavMesh();

    /// Updates the neighbours of a navigation node
    void LinkNeighbours (ncNavMeshNode* node) const;

    /// Says if two nodes are neighbours
    bool AreNeighbours (ncNavMeshNode* node1, ncNavMeshNode* node2) const;

    /// Finds the best neighbour for a given poly
    polygon* GetBestNeighbour (const polygon* poly) const;

    /// Erases polygons smaller than a given area
    void CleanSmallNodes (float threshold);

    /// Erases orphan nodes in the mesh
    void CleanOrphanNodes();

    /// Erases vertices inside obstacles
    bool RemoveVerticesInObstacles (polygon* poly);
    /// Erases a poly from the trimesh
    void RemovePoly (polygon* poly);

    /// Says if a physic geom is considered a floor
    bool IsFloor (nPhysicsGeom* geom) const;
    /// Says if a physic geom is considered a ceiling
    bool IsCeiling (nPhysicsGeom* geom) const;
    /// Says if a physic geom is considered a wall
    bool IsWall (nPhysicsGeom* geom) const;
    /// Says if a physic geom is considered walkable
    bool IsWalkable (nPhysicsGeom* geom) const;
    /// Says if a physic geom is considered static
    bool IsStatic (nPhysicsGeom* geom) const;
    /// Says if a physic geom is considered to be near the floor
    bool IsNearFloor (nPhysicsGeom* geom) const;
    /// Says if a physic geometry is a valid obstacle for the navigator mesh
    bool IsObstacle (nPhysicsGeom* geom) const;
    /// Calculate the floor high
    void SetupFloor (NavSpace& navSpace);

    /// Build the navigation graph
    void BuildNavGraph();

    /// Get the obstacle where a given point is
    polygon* GetObstacle (const vector3& point) const;

    /// Auxiliary method for "sweepmines" algorithm
    void CalculateEdges (const nArray2<bool>& walkableTable, nArray2<int>& minesTable, int row, int column);

    /// Init walkable table from walkable cells
    void SetWalkableCells( nArray2<bool>& walkableTable );
    /// Get the bbox containing all the cells marked as walkable
    void GetWalkableCellsAABB( nPhysicsAABB& bbox );

    /// Insert all the mesh nodes in the spatial server for a fast look up
    void InsertMeshIntoSpace();
    /// Insert all the obstacles in the spatial server for a fast look up
    void InsertObstaclesIntoSpace();
    /// Remove all the obstacles from the spatial server
    void RemoveObstaclesFromSpace();

    /// Return the first non transform geometry below the given geometry
    nPhysicsGeom* GetShapeGeometry( nPhysicsGeom* geom ) const;

    /// Get the outdoor's terrain height map
    nFloatMap* GetHeightMap() const;
    /// Get the physics terrain component of the outdoor
    ncPhyTerrain* GetPhysicsTerrain() const;

    /// Get the polygons of a triangle mesh that face up within a maximum walkable slope
    void GetWalkablePolygons( nPhyGeomTriMesh* triMesh, nArray<polygon*>& polys ) const;
    /// Get the perimeter edges of a polygon soup
    void GetPerimeterEdges( nArray<polygon*>& polys, nArray<line3*>& edges ) const;
    /// Insert points for triangulation for the given edges
    void InsertEdges( nArray<line3*>& edges );
    /// Delete the given polygon
    void ReleasePolygons( nArray<polygon*>& polys );
    /// Delete the given edges
    void ReleaseEdges( nArray<line3*>& edges );
    /// Get the projected rectangle on XZ plane of a OBB
    void GetRectangle( nPhyGeomBox* box, polygon& poly ) const;

    /// References the physic world
    NavSpace navSpace;
    /// References the mesh build in progress
    nNavMesh* mesh;
    /// References to the floor of the space
    nArray<nPhysicsGeom*> floors;
    /// Auxiliary structure for store all points and triangles for triangulation
    nArray<vector3> points;
    nArray<polygon*> triMesh;
    nArray<polygon*> obstacles;
    /// Entities inserted in the space for fast look up of obstacles
    nArray<nEntityObject*> obstacleEntities;
    
    nArray<vector3> slopes;
    nArray2<float>  slopeTable;
    
    /// Stores a copy of nodes, but the owner is the mesh, not the builder
    nArray<ncNavMeshNode*> nodes;
    float floorHigh;
    float floorLow;

    /// Building the mesh for an indoor or an outdoor?
    nNavMesh::SpaceType spaceType;
};

#endif