//------------------------------------------------------------------------------
//  ncnavnode.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnavmesh.h"
#include "ncnavnode/ncnavnode.h"

//------------------------------------------------------------------------------
nNebulaComponentObjectAbstract(ncNavNode,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNavNode)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Default constructor
*/
ncNavNode::ncNavNode() :
    externalLinksNumber( 0 )
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncNavNode::~ncNavNode()
{
    if ( nEntityObjectServer::Instance()->CanBeUnremoved( this->GetEntityObject()->GetId() ) )
    {
        this->ClearLocalLinks( REMOVE_SELF_FROM_TARGET );
        this->ClearExternalLinks( REMOVE_SELF_FROM_TARGET );
    }
}

//------------------------------------------------------------------------------
/**
    Add a local link
*/
void
ncNavNode::AddLocalLink( ncNavNode* node )
{
    n_assert( node );

    if ( node )
    {
        nEntityObjectId entityId( node->GetEntityObject()->GetId() );
        if ( this->links.FindIndex( entityId ) == -1 )
        {
            this->links.Append( entityId );
        }
    }
}

//------------------------------------------------------------------------------
/**
    Add an external link
*/
void
ncNavNode::AddExternalLink( ncNavNode* node )
{
    n_assert( node );

    if ( node )
    {
        nEntityObjectId entityId( node->GetEntityObject()->GetId() );
        if ( this->links.FindIndex( entityId ) == -1 )
        {
            this->links.Insert( 0, entityId );
            ++this->externalLinksNumber;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Remove a local link by target node id
*/
void
ncNavNode::RemoveLocalLink( nEntityObjectId entityId, RemoveMode removeMode )
{
    for ( int i( this->externalLinksNumber ); i < this->links.Size(); ++i )
    {
        if ( this->links[i] == entityId )
        {
            if ( removeMode == REMOVE_SELF_FROM_TARGET )
            {
                // Remove the link that the target node has to this node
                nEntityObject* targetNode( nEntityObjectServer::Instance()->GetEntityObject( entityId ) );
                if ( targetNode )
                {
                    targetNode->GetComponentSafe<ncNavNode>()->RemoveLocalLink(
                        this->GetEntityObject()->GetId(), DO_NOT_REMOVE_SELF_FROM_TARGET );
                }
            }

            this->links.Erase( i );
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Remove an external link by target node id
*/
void
ncNavNode::RemoveExternalLink( nEntityObjectId entityId, RemoveMode removeMode )
{
    for ( int i(0); i < this->externalLinksNumber; ++i )
    {
        if ( this->links[i] == entityId )
        {
            if ( removeMode == REMOVE_SELF_FROM_TARGET )
            {
                // Remove the link that the target node has to this node
                nEntityObject* targetNode( nEntityObjectServer::Instance()->GetEntityObject( entityId ) );
                if ( targetNode )
                {
                    targetNode->GetComponentSafe<ncNavNode>()->RemoveExternalLink(
                        this->GetEntityObject()->GetId(), DO_NOT_REMOVE_SELF_FROM_TARGET );
                }
            }

            this->links.Erase( i );
            --this->externalLinksNumber;
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Remove all local links
*/
void
ncNavNode::ClearLocalLinks( RemoveMode removeMode )
{
    while ( this->links.Size() > this->externalLinksNumber )
    {
        if ( removeMode == REMOVE_SELF_FROM_TARGET )
        {
            // Remove the link that the target node has to this node
            nEntityObject* targetNode( nEntityObjectServer::Instance()->GetEntityObject( this->links.Back() ) );
            if ( targetNode )
            {
                targetNode->GetComponentSafe<ncNavNode>()->RemoveLocalLink(
                    this->GetEntityObject()->GetId(), DO_NOT_REMOVE_SELF_FROM_TARGET );
            }
        }

        // Remove the link
        this->links.Erase( this->links.Size() - 1 );
    }
}

//------------------------------------------------------------------------------
/**
    Remove all external links
*/
void
ncNavNode::ClearExternalLinks( RemoveMode removeMode )
{
    for ( ; this->externalLinksNumber > 0; --this->externalLinksNumber )
    {
        if ( removeMode == REMOVE_SELF_FROM_TARGET )
        {
            // Remove the link that the target node has to this node
            nEntityObject* targetNode( nEntityObjectServer::Instance()->GetEntityObject( this->links[0] ) );
            if ( targetNode )
            {
                targetNode->GetComponentSafe<ncNavNode>()->RemoveExternalLink(
                    this->GetEntityObject()->GetId(), DO_NOT_REMOVE_SELF_FROM_TARGET );
            }
        }

        // Remove the link
        this->links.Erase( 0 );
    }
}

//------------------------------------------------------------------------------
/**
    UpdateLinks
*/
void
ncNavNode::UpdateLinks()
{
    for ( int i=0; i<this->links.Size(); i++ )
    {
        ncNavNode* node = this->GetLink(i);

        if ( node )
        {
            node->RemoveLink (this);
        }
    }

    this->links.Reset();
    this->externalLinksNumber = 0;
}

//------------------------------------------------------------------------------
/**
    RemoveLink
*/
void
ncNavNode::RemoveLink (ncNavNode* node)
{
    // @todo Fix this function, current implementation isn't safe
    for ( int i=0; i<this->links.Size(); )
    {
        ncNavNode* link = this->GetLink(i);

        if ( link )
        {
            int index = this->links.FindIndex( node->GetEntityObject()->GetId() );

            if ( index != -1 )
            {
                if ( index < this->externalLinksNumber )
                {
                    // External links are prepended
                    --this->externalLinksNumber;
                }
                this->links.Erase (index);
            }
            else
            {
                i++;
            }
        }
        else
        {
            ++i;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Get the number of local links
*/
int
ncNavNode::GetLocalLinksNumber() const
{
    return this->links.Size() - this->externalLinksNumber;
}

//------------------------------------------------------------------------------
/**
    Get the target node a local link by index
*/
ncNavNode*
ncNavNode::GetLocalLink( int index ) const
{
    // Local links are appended
    return this->GetLink( this->externalLinksNumber + index );
}

//------------------------------------------------------------------------------
/**
    Get an approximated size in bytes of this node (actually, only the component)
*/
int
ncNavNode::GetByteSize() const
{
    // Static data
    int size( sizeof(this) );

    // Links
    size += this->links.Size() * sizeof( nEntityObjectId );

    return size;
}
