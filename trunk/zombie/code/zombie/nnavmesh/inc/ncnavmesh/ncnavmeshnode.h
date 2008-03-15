#ifndef NC_NAVMESHNODE_H
#define NC_NAVMESHNODE_H

//------------------------------------------------------------------------------
/**
    @class ncNavMeshNode
    @ingroup NebulaNavmeshSystem

    Node for the navigation mesh 

   (C) 2005 Conjurer Services, S.A.
*/

#include "ncnavnode/ncnavnode.h"

class polygon;

#ifndef NGAME
class nGfxServer2;
class nLineHandler;
class nShader2;
#endif

//------------------------------------------------------------------------------
class ncNavMeshNode : public ncNavNode
{

    NCOMPONENT_DECLARE(ncNavMeshNode,ncNavNode);

public:

    ncNavMeshNode ();
    ncNavMeshNode (const polygon* zone );
    ~ncNavMeshNode();

    /// Gets the polygon
    polygon* GetZone() const;

    /// Get the midpoint of the node
    void GetMidpoint( vector3& midpoint ) const;

    /// Set if this node is within the outdoor
    void SetIsInOutdoor( bool inOutdoor );
    /// Tell if this node is within the outdoor
    bool IsInOutdoor() const;

#ifndef NGAME
    /// Draw
    void Draw( nLineHandler* lineHandler, bool wireframe, bool overlay, bool showLinks );
    /// Search and store which edges aren't shared with any neighbor
    void LookForBoundaryEdges();
    /// Get the boundary colour (indoor)
    vector4 GetIndoorBoundaryColour() const;
    /// Set the boundary colour (indoor)
    void SetIndoorBoundaryColour(vector4 colour);
    /// Get the boundary colour (outdoor)
    vector4 GetOutdoorBoundaryColour() const;
    /// Get the boundary colour (outdoor)
    void SetOutdoorBoundaryColour(vector4 colour);
#endif

    /// Says if a point is inside the node
    bool IsPointInside (const vector3& point) const;
    bool IsPointInside (float x, float z) const;

    /// Get the height of the ground belonging to the given point in the navmeshnode
    bool GetGroundHeight( float x, float z, float& groundHeight ) const;

    /// Get the node's closest navigable point to the given point
    void GetClosestNavigablePoint( const vector3& point, vector3& closestPoint ) const;

    /// Return codes about crossing the boundaries of the node
    enum CrossType
    {
        STAYING_INSIDE,
        GOING_TO_NEIGHBOR,
        GOING_OUT_OF_MESH
    };
    /// Get cross information regarding to a ray exiting this node
    CrossType GetCrossInfo( const line3& ray, ncNavMeshNode*& neighbor, vector3& crossPoint, vector3& crossNormal ) const;
    /// Get the neighbor sharing the given edge, or NULL if no one shares it
    ncNavMeshNode* GetNeighborSharingEdge( const line3& edge ) const;

    /// Get an estimated byte size of this node (for memory statistics)
    int GetByteSize() const;

    friend class nNavMeshBuilder;
    friend class nNavMesh;
    friend class NavMeshLoader;

private:
    /// Create a new node
    void CreateNode (const polygon* zone = 0);

    /// colour to use for drawing boundary lines (indoor)
    vector4 indoorBoundaryColour;
    /// colour to use for drawing boundary lines (outdoor)
    vector4 outdoorBoundaryColour;

#ifndef NGAME
    /// Draw edges of the zone
    void Draw( nLineHandler* lineHandler, bool isOutdoor, float high, bool wireframe, bool overlay, bool showLinks );
    void DrawEdges (nGfxServer2* server, bool isOutdoor, bool wireframe, bool overlay);
    void DrawBoundary (nLineHandler* lh, bool isOutdoor, bool overlay);
    void DrawLinks (nGfxServer2* server, bool isOutdoor, bool overlay);
    void DrawVertices (nGfxServer2* server, float high);
    /// Get shader for overlay or non overlay draw modes
    nShader2* GetShader( bool overlay ) const;
    /// Update the height of a point for drawing over the ground
    void UpdateDrawHeight( vector3& point, bool isOutdoor, bool overlay ) const;
#endif

	/// Update the height of a given position
	void UpdateHeight (vector3* position) const;

    polygon* zone;           /// Polygon of the node
    void*    userData;       /// Extra user data

    // Does this polygon belong to the outdoor? (walkable brushes are considered as indoors)
    bool isInOutdoor;

#ifndef NGAME
    nArray<int> boundaryEdges;
#endif
};

//------------------------------------------------------------------------------
/**
    GetZone
*/
inline
polygon*
ncNavMeshNode::GetZone() const
{
    return this->zone;
}

//------------------------------------------------------------------------------
/**
    Set if this node is within the outdoor
*/
inline
void
ncNavMeshNode::SetIsInOutdoor( bool inOutdoor )
{
    this->isInOutdoor = inOutdoor;
}

//------------------------------------------------------------------------------
/**
    Tell if this node is within the outdoor
*/
inline
bool
ncNavMeshNode::IsInOutdoor() const
{
    return this->isInOutdoor;
}

#endif // NC_NAVMESHNODE_H
