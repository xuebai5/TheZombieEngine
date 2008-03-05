//------------------------------------------------------------------------------
//  ninventoryitem.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ninventoryitem.h"

#include "entity/nentityobject.h"

#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInventoryItem, "nobject");

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nInventoryItem);
    NSCRIPT_ADDCMD('LGIT', int, GetType , 0, (), 0, ());
    NSCRIPT_ADDCMD('LGTS', int, StackSize , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSTS', void, SetStack, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LSUS', void, SetUsed, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('LINF', bool, IsInfinite , 0, (), 0, ());
    NSCRIPT_ADDCMD('LIWP', bool, IsWeapon , 0, (), 0, ());
    NSCRIPT_ADDCMD('LIAM', bool, IsAmmunition , 0, (), 0, ());
    NSCRIPT_ADDCMD('LIEQ', bool, IsEquipment , 0, (), 0, ());
    NSCRIPT_ADDCMD('LIST', bool, IsStackable , 0, (), 0, ());
    NSCRIPT_ADDCMD('LISD', bool, IsUsed , 0, (), 0, ());
    NSCRIPT_ADDCMD('LIDR', bool, IsDropped , 0, (), 0, ());
    NSCRIPT_ADDCMD('LGEN', nEntityObject *, GetEntity , 0, (), 0, ());
NSCRIPT_INITCMDS_END();

//------------------------------------------------------------------------------
const char * nInventoryItem::typeNames[] = {
        "Nothing",      //IT_NOTHING
        "Weapon",       //IT_WEAPON
        "Accesory",     //IT_ACCESSORY
        "Equipment",    //IT_EQUIPMENT
        "Ammunition",   //IT_AMMUNITION
        "Last",         //IT_LAST
    };

//------------------------------------------------------------------------------
/**
*/
nInventoryItem::nInventoryItem():
    itemType( nInventoryItem::IT_NOTHING ),
    entityItem( 0 ),
    entityClass( 0 ),
    dropObject( 0 ),
    stackSize( 0 ),
    isStackable( false ),
    isInfinite( false ),
    isUsed( false ),
    isDropped( false )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param name name of the item
*/
void
nInventoryItem::Create( const char * name )
{
    if( name == 0 )
    {
        return;
    }

    bool valid = true;
    nClass * clazz = nKernelServer::Instance()->FindClass( name );
    valid = clazz != 0;

    if( valid )
    {
        if( clazz->IsA( "neweapon" ) )
        {
            this->itemType = IT_WEAPON;
            this->entityItem = nEntityObjectServer::Instance()->NewLocalEntityObject( name );
            valid = this->entityItem != 0;
        }
        else if( clazz->IsA( "neweaponaddon" ) )
        {
            this->itemType = IT_ACCESSORY;
            this->entityItem = nEntityObjectServer::Instance()->NewLocalEntityObject( name );
            valid = this->entityItem != 0;
        }
        else if( clazz->IsA( "neequipment" ) )
        {
            this->itemType = IT_EQUIPMENT;
            this->entityItem = nEntityObjectServer::Instance()->NewLocalEntityObject( name );
            valid = this->entityItem != 0;
            this->isStackable = true;
            this->stackSize = 0;
        }
        else if( clazz->IsA( "neammunition" ) )
        {
            this->itemType = IT_AMMUNITION;
            this->entityItem = nEntityObjectServer::Instance()->NewLocalEntityObject( name );
            valid = this->entityItem != 0;
            this->isStackable = true;
            this->stackSize = 0;
        }
        else
        {
            n_assert2_always( "Item type is not adecuate" );
        }
    }

    if( this->entityItem )
    {
        this->entityClass = this->entityItem->GetEntityClass();
    }

    if( ! valid )
    {
        itemType = IT_NOTHING;
    }
}

//------------------------------------------------------------------------------
/**
*/
nInventoryItem::~nInventoryItem()
{
    if( this->entityItem )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->entityItem );
        this->entityItem = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param item item to compare with
    @returns true if item is the same that myself
*/
bool
nInventoryItem::IsSameType( const nInventoryItem & item )const
{
    return ( this->itemType == item.itemType ) && ( this->entityClass == item.entityClass );
}

//------------------------------------------------------------------------------
/**
    @returns the item name
*/
const char *
nInventoryItem::GetItemName()const
{
    if( this->entityClass )
    {
        return this->entityClass->GetName();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param item item to copy the type
*/
void
nInventoryItem::CopyTypeFrom( const nInventoryItem & item )
{
    this->itemType = item.itemType;
    this->entityClass = item.entityClass;
}

//------------------------------------------------------------------------------
/**
    @param entity graphics object
*/
void
nInventoryItem::SetDropObject( nEntityObject * entity )
{
    n_assert( entity );

    if( entity ){
        this->dropObject = entity;
        this->isDropped = true;
    }
}

//------------------------------------------------------------------------------
/**
    @returns the drop object
*/
nEntityObject*
nInventoryItem::GetDropObject()const
{
    return this->dropObject;
}

//------------------------------------------------------------------------------
/**
*/
void
nInventoryItem::RemoveDropObject()
{
    n_assert( this->dropObject );

    if( this->dropObject )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->dropObject );
        this->dropObject = 0;
        this->isDropped = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param data inventory data
*/
void
nInventoryItem::NetworkUpdate( nstream & data )
{
    char name[ N_MAXNAMELEN ];

    if( data.GetWrite() )
    {
        strncpy( name, this->GetItemName(), N_MAXNAMELEN );

        data.UpdateString( name );
    }
    else
    {
        data.UpdateString( name );
        this->Create( name );
    }

    data.UpdateInt( this->itemID );
    data.UpdateInt( this->stackSize );
    data.UpdateBool( this->isStackable );
    data.UpdateBool( this->isInfinite );
    data.UpdateBool( this->isUsed );
    data.UpdateBool( this->isDropped );
}

//------------------------------------------------------------------------------
