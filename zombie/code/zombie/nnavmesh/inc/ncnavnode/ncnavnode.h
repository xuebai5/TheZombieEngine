#ifndef NC_NAVNODE_H
#define NC_NAVNODE_H

//------------------------------------------------------------------------------
/**
    @class ncNavNode
    @ingroup NebulaNavmeshSystem

    Base class for any node in the navigation graph

    The full navigation graph is composed of different navigation graphs
    (tipically extracted from meshes and waypoints). A node then can have links
    to other nodes of the same "local" graph to which it belongs, while it also
    may have "external" links to a node of another graph, so different graphs
    get connected. This distinction is mainly for persistence purposes, for
    pathfinding an interface that treats all the graphs as a single graph is
    provided.

   (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "entity/nentityobjectserver.h"
#include "util/narray.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class ncNavNode : public nComponentObject
{

    NCOMPONENT_DECLARE(ncNavNode,nComponentObject);

public:
    /// Default constructor
    ncNavNode();
    /// Destructor
    virtual ~ncNavNode();

    // -- Graph construction
    /// Add a local link
    void AddLocalLink( ncNavNode* targetNode );
    /// Add an external link
    void AddExternalLink( ncNavNode* targetNode );
    /// Removal modes
    enum RemoveMode
    {
        /// Tell the target node of a link to remove any link it has to this node
        REMOVE_SELF_FROM_TARGET,
        /// Don't do REMOVE_SELF_FROM_TARGET
        DO_NOT_REMOVE_SELF_FROM_TARGET
    };
    /// Remove a local link by target node id
    void RemoveLocalLink( nEntityObjectId entityId, RemoveMode removeMode );
    /// Remove an external link by target node id
    void RemoveExternalLink( nEntityObjectId entityId, RemoveMode removeMode );
    /// Remove all local links
    void ClearLocalLinks( RemoveMode removeMode );
    /// Remove all external links
    void ClearExternalLinks( RemoveMode removeMode );

    // @todo Move to nav mesh node
    /// Updates links of a deleted node
    void UpdateLinks();
    /// Remove a link to a given node
    void RemoveLink (ncNavNode* node);

    // -- Graph iteration
    /// Get the total number of links, of any kind
    int GetLinksNumber() const;
    /// Get the number of local links
    int GetLocalLinksNumber() const;
    /// Get the target node of a link (of any kind) by index
    ncNavNode* GetLink( int index ) const;
    /// Get the target node a local link by index
    ncNavNode* GetLocalLink( int index ) const;

    // -- Path search
    /// Set the parent node
    void SetParent( ncNavNode* parent );
    /// Get the parent node
    ncNavNode* GetParent() const;
    /// Set h value
    void SetH( int h );
    /// Get h value
    int GetH() const;
    /// Set g value
    void SetG( int g );
    /// Get g value
    int GetG() const;
    /// Calculate f value
    void CalculateF();
    /// Set f value
    void SetF( int f );
    /// Get f value
    int GetF() const;

    // -- Navigable region related methods
    /// Says if a point is inside the node
    virtual bool IsPointInside( const vector3& point ) const = 0;
    /// Says if a 2D point is inside the 2D projection of the node
    virtual bool IsPointInside( float x, float z ) const = 0;
    /// Get the midpoint of the node
    virtual void GetMidpoint( vector3& midpoint ) const = 0;
    /// Get the node's closest navigable point to the given point
    virtual void GetClosestNavigablePoint( const vector3& point, vector3& closestPoint ) const = 0;

    // -- Statistics
    /// Get an approximated size in bytes of this node (actually, only the component)
    virtual int GetByteSize() const;

protected:
    /// Links to others nodes (local links are appended while external links, much less frequent, are prepended)
    nArray<nEntityObjectId> links;
    /// Number of external links
    int externalLinksNumber;

    // Meta information for A* Search
    ncNavNode* parent;
    int f, h, g;

};

//------------------------------------------------------------------------------
/**
    Get the total number of links, of any kind
*/
inline
int
ncNavNode::GetLinksNumber() const
{
    return this->links.Size();
}

//------------------------------------------------------------------------------
/**
    Get the target node of a link (of any kind) by index
*/
inline
ncNavNode*
ncNavNode::GetLink( int index ) const
{
#ifndef NGAME
    n_assert( index >=0 && index < this->links.Size() );
    if ( index < 0 || index >= this->links.Size() )
    {
        return NULL;
    }
#endif

    nEntityObject* node( nEntityObjectServer::Instance()->GetEntityObject( this->links[index] ) );
    n_assert( node );
    if ( !node )
    {
        return NULL;
    }
    return node->GetComponentSafe<ncNavNode>();
}

//------------------------------------------------------------------------------
/**
    Set the parent node
*/
inline
void
ncNavNode::SetParent( ncNavNode* parent )
{
    this->parent = parent;
}

//------------------------------------------------------------------------------
/**
    Get the parent node
*/
inline
ncNavNode*
ncNavNode::GetParent() const
{
    return this->parent;
}

//------------------------------------------------------------------------------
/**
    Set h value
*/
inline
void
ncNavNode::SetH( int h )
{
    this->h = h;
}

//------------------------------------------------------------------------------
/**
    Get h value
*/
inline
int
ncNavNode::GetH() const
{
    return this->h;
}

//------------------------------------------------------------------------------
/**
    Set g value
*/
inline
void
ncNavNode::SetG( int g )
{
    this->g = g;
}

//------------------------------------------------------------------------------
/**
    Get g value
*/
inline
int
ncNavNode::GetG() const
{
    return this->g;
}

//------------------------------------------------------------------------------
/**
    Calculate f value
*/
inline
void
ncNavNode::CalculateF()
{
    this->f = this->h + this->g;
}

//------------------------------------------------------------------------------
/**
  Set f value
*/
inline
void
ncNavNode::SetF( int f )
{
  this->f = f;
}

//------------------------------------------------------------------------------
/**
    Get f value
*/
inline
int
ncNavNode::GetF() const
{
    return this->f;
}

#endif // NC_NAVNODE_H
