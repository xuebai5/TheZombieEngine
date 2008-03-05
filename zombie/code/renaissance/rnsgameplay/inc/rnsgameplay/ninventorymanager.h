#ifndef NINVENTORYMANAGER_H
#define NINVENTORYMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nInventoryManager

    Class in charge of inventory items in the game.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"

#include "util/nstream.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nInventoryItem;
#ifndef NGAME
class nGfxServer2;
#endif//!NGAME

//------------------------------------------------------------------------------
class nInventoryManager : public nObject
{
public:
    /// constructor
    nInventoryManager();
    /// destructor
    ~nInventoryManager();

    /// return singleton instance
    static nInventoryManager * Instance();

    /// create a new item
    nInventoryItem * NewItem( const char * name );
    /// duplicate an item
    nInventoryItem * DuplicateItem( nInventoryItem * item );
    /// delete a item
    void DeleteItem( nInventoryItem * item );

    /// get the number of items in the manager
    int GetNumItems()const;
    /// get item from id
    nInventoryItem * GetItem( int itemID );
    /// get item
    nInventoryItem * GetItemAt( int index );

    /// Create a item to drop
    nEntityObject * CreateDroppedItem( nInventoryItem * const item );
    /// delete the drop graphic of an item
    void EraseDroppedItem( nInventoryItem * const item );

    /// update the inventory  from/to network data
    void NetworkUpdate( nstream & data );
    /// create a item from network
    void NewNetworkItem( nstream & data );
    /// get the data of an item to trasmit in th network
    void GetNetworkItem( int index, nstream & data );

#ifndef NGAME
    /// debug draw of entity
    void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

private:
    static nInventoryManager * singleton;

    /// clear the inventory manager
    void Clear();

    int nextItemID;

    nArray<nInventoryItem*> items;
};

//------------------------------------------------------------------------------
/**
    @returns the number of items
*/
inline
int
nInventoryManager::GetNumItems()const
{
    return items.Size();
}

//------------------------------------------------------------------------------
#endif//NINVENTORYMANAGER_H
