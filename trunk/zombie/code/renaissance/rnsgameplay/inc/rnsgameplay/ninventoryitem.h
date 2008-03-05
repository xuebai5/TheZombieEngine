#ifndef NINVENTORYITEM_H
#define NINVENTORYITEM_H
//------------------------------------------------------------------------------
/**
    @class nInventoryItem
    
    Item in an inventory
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/nobject.h"

#include "util/nstream.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nEntityClass;

//------------------------------------------------------------------------------
class nInventoryItem : public nObject
{
public:
    enum ItemType{
        IT_NOTHING,
        IT_WEAPON,
        IT_ACCESSORY,
        IT_EQUIPMENT,
        IT_AMMUNITION,
        IT_LAST
    };

    static const char * typeNames[];

    /// constructor
    nInventoryItem();

    /// constructor with name
    void Create( const char * name );
    /// destructor
    ~nInventoryItem();

        /// returns the type of item
        int GetType ()const;
        /// get the stack size of the stackable item
        int StackSize ()const;
        /// set the stack size of stackable item
        void SetStack(int);
        /// set that item is in use
        void SetUsed(bool);
        /// get if the stackable item has infinite space
        bool IsInfinite ()const;
        /// returns true if type of item is a weapon
        bool IsWeapon ()const;
        /// returns true if type of item is an ammunition
        bool IsAmmunition ()const;
        /// returns true if type of item is an equipment
        bool IsEquipment ()const;
        /// say if item is stackable
        bool IsStackable ()const;
        /// say if item is used
        bool IsUsed ()const;
        /// say if item is in the floor
        bool IsDropped ()const;
        /// get the entity of the item
        nEntityObject * GetEntity ()const;

    /// Sets the item ID of item
    void SetItemID( int id );
    /// Gets the item ID
    int GetItemID();

    /// returns the name which item can be created
    const char * GetItemName()const;

    /// returns if one item is similar to another
    bool IsSameType( const nInventoryItem & item )const;
    /// copy the item type from another inventory item
    void CopyTypeFrom( const nInventoryItem & item );
    /// set the drop graphics object
    void SetDropObject( nEntityObject * entity );
    /// get the drop graphics object
    nEntityObject * GetDropObject()const;
    /// delete drop graphic iamge
    void RemoveDropObject();

    /// update the inventory item from/to network data
    void NetworkUpdate( nstream & data );

private:
    nEntityClass * entityClass;
    nEntityObject * entityItem;
    nEntityObject * dropObject;

    ItemType itemType;
    int itemID;

    int stackSize;

    bool isStackable;
    bool isInfinite;
    bool isUsed;
    bool isDropped;
};

//------------------------------------------------------------------------------
/**
    @param value new item ID
*/
inline
void
nInventoryItem::SetItemID( int value )
{
    this->itemID = value;
}

//------------------------------------------------------------------------------
/**
    @returns the item ID
*/
inline
int
nInventoryItem::GetItemID()
{
    return this->itemID;
}

//------------------------------------------------------------------------------
/**
    @returns the item type
*/
inline
int
nInventoryItem::GetType()const
{
    return int( this->itemType );
}

//------------------------------------------------------------------------------
/**
    @returns true if stack size is infinite
*/
inline
bool
nInventoryItem::IsInfinite()const
{
    return this->isInfinite;
}

//------------------------------------------------------------------------------
/**
    @returns the stack size
*/
inline
int
nInventoryItem::StackSize()const
{
    return this->stackSize;
}

//------------------------------------------------------------------------------
/**
    @param num new stack size
*/
inline
void
nInventoryItem::SetStack( int num )
{
    if( this->isStackable )
    {
        this->stackSize = num;
    }
}

//------------------------------------------------------------------------------
/**
    @returns true if item is a weapon
*/
inline
bool
nInventoryItem::IsWeapon()const
{
    return (this->itemType == nInventoryItem::IT_WEAPON);
}

//------------------------------------------------------------------------------
/**
    @returns true if item is an ammunition
*/
inline
bool
nInventoryItem::IsAmmunition()const
{
    return (this->itemType == nInventoryItem::IT_AMMUNITION);
}

//------------------------------------------------------------------------------
/**
    @returns true if item is a equipment
*/
inline
bool
nInventoryItem::IsEquipment()const
{
    return (this->itemType == nInventoryItem::IT_EQUIPMENT);
}

//------------------------------------------------------------------------------
/**
    @returns true if item is stackable
*/
inline
bool
nInventoryItem::IsStackable()const
{
    return this->isStackable;
}

//------------------------------------------------------------------------------
/**
    @returns the entity object
*/
inline
nEntityObject*
nInventoryItem::GetEntity()const
{
    return this->entityItem;
}

//------------------------------------------------------------------------------
/**
    @param value used state
*/
inline
void
nInventoryItem::SetUsed( bool value )
{
    this->isUsed = value;
}

//------------------------------------------------------------------------------
/**
    @returns true if item is used
*/
inline
bool
nInventoryItem::IsUsed()const
{
    return this->isUsed;
}

//------------------------------------------------------------------------------
/**
    @returns true if item is dropped in the floor
*/
inline
bool
nInventoryItem::IsDropped()const
{
    return this->isDropped;
}

//------------------------------------------------------------------------------
#endif//NINVENTORYITEM_H
