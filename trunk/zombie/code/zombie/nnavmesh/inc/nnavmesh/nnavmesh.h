#ifndef N_NAVMESH_H
#define N_NAVMESH_H

//------------------------------------------------------------------------------
/**
    @class nNavMesh
    @ingroup NebulaNavmeshSystem

    Navigation mesh pathfinding. Stores the navegation mesh and navegation graph

    (C) 2005 Conjurer Services, S.A.
*/

#include "util/narray.h"
#include "util/narray2.h"
#include "mathlib/vector.h"
#include "resource/nresource.h"

#include "mathlib/polygon.h"

class ncNavNode;
class ncNavMeshNode;
class nGfxServer2;
class polygon;
class nFloatMap;

class nNavMesh : public nResource
{
public:
    /// Constructor
    nNavMesh();
    /// Destructor
    ~nNavMesh();

    /// Destroy all nodes
    void Destroy();

    /// Insert a new node
    void InsertNode (ncNavMeshNode* node);

    /// Gets the number of nodes contained in the navigation mesh
    int GetNumNodes() const;

    // Set obstacles
    void SetObstacles (const nArray<polygon*>& obstacles);

    // Insert a new obstacle
    void InsertObstacle (polygon* poly);

    // Get number of obstacles
    int GetNumObstacles() const;

    // Get the polygon of an obstacle by its index
    polygon* GetObstaclePolygon (int index) const;

    /// Finds the navigation mesh node in a point
    ncNavMeshNode* GetNode (const vector3& point) const;
    ncNavMeshNode* GetNode (float x, float z) const;

    /// Get the navigation node containing the given point
    ncNavNode* GetNavNode( const vector3& point ) const;

    /// Get the navigation waypoint node containing the given point
    ncNavNode* GetWaypointNode( const vector3& point ) const;

    /// Get the nearest "visible" node to point
    ncNavNode* GetClosestNode( const vector3& point ) const;

    // Get a node by its index
    ncNavMeshNode* GetNode (int index) const;

    /// Says the index of a node
    int GetIndexNode (ncNavMeshNode* node) const;

    /// Gets the distance between two nodes
    int GetDistance (ncNavMeshNode* source, ncNavMeshNode* goal) const;
    int GetDistance (int source, int goal) const;

    /// Sets the distance between two nodes
    void SetDistance (int source, int goal, int distance);

    /// Gets the number of nodes contained in the navigation graph
    int GetNumGraphNodes() const;

    /// Adds nodes from other mesh
    void AddMesh (nNavMesh* mesh);
    /// Removes all mesh nodes and obstacles, but without releasing them
    void ClearMesh();

    /// Builds the navigation graph for the current navigation mesh
    void BuildNavGraph();

    /// Creates the navigation graph with the given number of nodes and with undefined distances between them
    void SetNumGraphNodes(int numNodes);

    /// Adjust a given position into a valid position
    bool AdjustPosition (vector3* position, float radius = 0.f) ;// const;

    /// Get an arbitrary point of the mesh
    vector3 GetStartPosition() const;
    /// Get the center of the mesh
    vector3 GetCenterPosition() const;

    /// Says if a point is inside of an obstacle
    polygon* IsInsideObstacle (const vector3& point, float radius, vector3* axis1 = 0, vector3* axis2 = 0) ; //const;

    /// Get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();

    /// Space where the nav mesh is inserted
    enum SpaceType
    {
        INDOOR,
        OUTDOOR,
        BRUSH
    };

    /// Insert all the mesh nodes in the spatial server for a fast look up
    void InsertIntoSpace( SpaceType spaceType );

    /// Get the cross point where a ray exits the navegable space
    bool GetBoundaryPoint( const line3& ray, vector3& crossPoint, vector3& crossNormal ) const;

#ifndef NGAME
    // Draw obstacles
    void DrawObstacles (nGfxServer2* server, float high);
#endif

    friend class nNavMeshBuilder;

protected:
    /// Load navmesh resource
    virtual bool LoadResource();

    /// Unload navmesh resource
    virtual void UnloadResource();

private:
    /// Releases a navMeshNode 
    void ReleaseNode (ncNavMeshNode* node);

    // Destroy obstacles
    void DestroyObstacles();

    /// Initializes the navigation graph
    void SetupNavGraph();

    /// Get the outdoor's terrain height map
    nFloatMap* GetHeightMap() const;

    nArray<ncNavMeshNode*> mesh;         /// Stores the nodes of the navigation mesh
    nArray2<int> navGraph;              /// Stores the navigation graph
    nArray<polygon*> obstacles;         /// Stores the obstacles of the mesh
    polygon* collide;
    polygon check;
    vector3 distances[4];
};

#endif