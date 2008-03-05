#ifndef N_NASTAR_H
#define N_NASTAR_H

//------------------------------------------------------------------------------
/**
    @class nAStar

    Path finder for a given navigation mesh
*/

#include "kernel/nobject.h"
#include "util/narray.h"
#include "mathlib/vector.h"

class nNavMesh;
class ncNavNode;
class nGfxServer2;
class nEntityObject;

class nAStar : public nObject
{
    struct AStarValues
    {
        int f, g, h;
    };

public:
    nAStar();
    ~nAStar();

    /// Set the navigation mesh for pathfinding
    void SetMesh (nNavMesh* mesh);
    /// Get the mesh
    nNavMesh* GetMesh() const;

    /// Finds the path for two given points
    bool FindPath (const vector3& start, const vector3& goal, nArray<vector3>* path, nArray<bool>* smoothable); //, nEntityObject* entity = 0);

    /// Says if a given point is a valid goal point for an entity
    bool IsValidGoal (const vector3& goal, nEntityObject* entity = 0) const;

    /// Get the instance of the nAStar object
    static nAStar* Instance();

private:
    /// Reset internal structures
    void InitSearch (const vector3& start, const vector3& goal, ncNavNode* startNode, ncNavNode* goalNode);
    /// Run A* algotithm
    bool CalculatePath (ncNavNode* start, ncNavNode* goal);    
    /// Calculate values for A* node
    void CalculateValues (ncNavNode* current, ncNavNode* candidate, ncNavNode* goal, AStarValues* values);
    /// Get the best F in the open list
    ncNavNode* GetBestF() const;
    /// Move a node from the open to the closed list
    void MoveToClosedList (ncNavNode* node);
    /// Insert a node at the open list
    void InsertAtOpenList (ncNavNode* node);
    /// Say if a node is at the open or at the closed list
    bool IsConsidered (ncNavNode* node) const;
	/// Say if a node is at the closed list
	bool IsClosed (ncNavNode* node) const;
	/// Say if a node is at the open list
	bool IsOpen (ncNavNode* node) const;

    /// Says if a position is valid for the pathfinder 
    bool IsValidPosition (const vector3& position, nEntityObject* entity = 0) const;
	/// Build the final path
	void BuildFinalPath();

    static nAStar* Singleton;

    nNavMesh* mesh;                 /// The mesh where find the path :D

    // For A* search
    nArray<ncNavNode*> openList;
    nArray<ncNavNode*> closedList;
    nArray<ncNavNode*> pathNode;

    nArray<vector3> path;           /// The real path
    nArray<bool> smoothable;        /// Tell if each point in the path can be moved to smooth the path or is a fixed point
    vector3 start;
    vector3 goal;
	ncNavNode* startNode;
	ncNavNode* goalNode;
};

//------------------------------------------------------------------------------
/**
    SetMesh
*/
inline
void
nAStar::SetMesh (nNavMesh* mesh)
{
    this->mesh = mesh;
}

//------------------------------------------------------------------------------
/**
    GetMeshConst
*/ 
inline
nNavMesh*
nAStar::GetMesh() const
{
    return this->mesh;
}

//------------------------------------------------------------------------------
/**
    Instance
*/
inline
nAStar*
nAStar::Instance()
{
    n_assert(Singleton);
    return Singleton;
}
#endif