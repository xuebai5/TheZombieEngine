//------------------------------------------------------------------------------
//  ninventorycontainer.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ninventorycontainer.h"

#include "rnsgameplay/ninventorymanager.h"

#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ncgpweaponclass.h"
#include "rnsgameplay/ncgpammunitionclass.h"

#include "entity/nentityobject.h"

#include "rnsgameplay/ncgpweapon.h"

#ifndef NGAME
#include "ndebug/ndebugserver.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaScriptClass(nInventoryContainer, "nobject");

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nInventoryContainer);
    NSCRIPT_ADDCMD('LGNI', int, GetNumItems , 0, (), 0, ());
    NSCRIPT_ADDCMD('LGWI', nInventoryItem *, GetWeaponItemAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LGIA', nInventoryItem *, GetItemAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LGID', nInventoryItem *, GetItem, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LGNB', int, GetNumBullets, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LRBL', void, RemoveBullets, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('LGNE', int, GetNumEquipment, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('LREQ', void, RemoveEquipment, 2, (const char*, int), 0, ());
    NSCRIPT_ADDCMD('LGOS', int, GetEntitySlot, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('LGES', int, GetEquipmentSlot, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('LGEI', nEntityObject*, GetInventoryItem, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END();

//------------------------------------------------------------------------------
int nInventoryContainer::slotRestrictions[] = {
        (ncGPWeaponClass::WT_MELEE),              // WST_MELEE = 0
        (ncGPWeaponClass::WT_ASSAULT |            // WST_MAIN = 1
            ncGPWeaponClass::WT_LIGHTMACHINE | 
            ncGPWeaponClass::WT_SNIPER | 
            ncGPWeaponClass::WT_PORTABLE_HEAVY), 
        (ncGPWeaponClass::WT_SUBMACHINE | ncGPWeaponClass::WT_SHOTGUN), // WST_SECONDARY = 2
        (ncGPWeaponClass::WT_HANDGUN),           // WST_HANDGUN = 3
        -1,                                      // WST_CARRIED = 4
        0,                                       // WST_LAST
    };

const char * nInventoryContainer::slotNames[] = {
        "Melee",
        "Main",
        "Secondary",
        "Handgun",
        "Carried",
        "",
    };

//------------------------------------------------------------------------------
/**
*/
nInventoryContainer::nInventoryContainer():
    container( nInventoryContainer::INITIAL_SIZE, 0 ),
    maxStackSize( 1000 ),
    isPlayerInventory( true )
{
    for( int i = 0 ; i < WST_LAST ; ++i )
    {
        this->weaponSlots[ i ] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nInventoryContainer::~nInventoryContainer()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
nInventoryContainer::Clear()
{
    nInventoryManager * manager = nInventoryManager::Instance();

    for( int i = 0 ; i < WST_LAST ; ++i )
    {
        if( this->weaponSlots[ i ] )
        {
            if( manager )
            {
                manager->DeleteItem( this->weaponSlots[ i ] );
            }

            this->weaponSlots[ i ] = 0;
        }
    }

    this->ClearSlots();
}

//------------------------------------------------------------------------------
/**
*/
void
nInventoryContainer::ClearSlots()
{
    nInventoryManager * manager = nInventoryManager::Instance();
    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        if( this->container[ i ] ){
            if( manager )
            {
                manager->DeleteItem( this->container[ i ] );
            }
        }
    }

    this->container.Clear();
}

//------------------------------------------------------------------------------
/**
    @param newSize new size of the inventory
*/
void
nInventoryContainer::SetSize( const int newSize )
{
    this->ClearSlots();

    if( INFINITE_SLOTS == newSize )
    {
        this->container.Reallocate( INITIAL_SIZE, INITIAL_SIZE );
    }
    else
    {
        if( newSize > 0 )
        {
            this->container.Reallocate( newSize, 0 );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param item new item to insert in the inventory
    @param slot if item is a weapon, expecify where put the weapon
    @returns old item if new item replace it, or new item if we can not
    insert into the inventory
*/
bool
nInventoryContainer::InsertItem( nInventoryItem * item, WeaponSlotType slotType )
{
    // if item is a weapon and this is a player inventory
    if( item->IsWeapon() && this->isPlayerInventory )
    {
        return this->InsertWeaponItem( item, slotType );
    }

    // else, if item is to the normal container
    return this->InsertNormalItem( item );
}

//------------------------------------------------------------------------------
/**
    @param item new item to insert in the inventory
    @param slot if item is a weapon, expecify where put the weapon
    @returns old item if new item replace it, or new item if we can not
    insert into the inventory
*/
bool
nInventoryContainer::InsertWeaponItem( nInventoryItem * item, WeaponSlotType slotType )
{
    nEntityObject * weapon = 0;
    ncGPWeaponClass * weaponClass = 0;

    weapon = item->GetEntity();
    if( weapon )
    {
        weaponClass = weapon->GetClassComponent<ncGPWeaponClass>();
    }

    if( weaponClass )
    {
        // if slot is any
        if( slotType == WST_ANY )
        {
            int i;

            // search an adecuate empty slot
            for( i = 0 ; i < WST_CARRIED ; ++i )
            {
                if( ( slotRestrictions[ i ] & weaponClass->GetWeaponType() ) != 0 )
                {
                    break;
                }
            }

            // if there is not empty slot
            if( this->weaponSlots[ i ] )
            {
                // we can insert weapon
                return false;
            }

            // put new weapon in the slot
            this->weaponSlots[ i ] = item;
        }
        // else, slot is something
        else
        {
            bool valid = slotType >= 0 && slotType <= WST_CARRIED;
            n_assert2( valid, "Invalid slot type in inventary" );
            if( ! valid )
            {
                slotType = WST_CARRIED;
            }

            // if the weapon can be putted in the slot
            if( ( slotRestrictions[ slotType ] & weaponClass->GetWeaponType() ) != 0 )
            {
                // if slot is not empty
                if( this->weaponSlots[ slotType ] != 0 )
                {
                    // we can insert weapon
                    return false;
                }

                // put new weapon in the slot
                this->weaponSlots[ slotType ] = item;
            }
            // else the weapon can not be putted in
            else
            {
                // we can insert weapon
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @param item new item to insert in the inventory
    @returns old item if new item replace it, or new item if we can not
    insert into the inventory
*/
bool
nInventoryContainer::InsertNormalItem( nInventoryItem * item )
{
    n_assert( item );
    if( !item )
    {
        return 0;
    }

    // if item is stackable
    if( item->IsStackable() )
    {
        n_assert2( item->StackSize() > 0 , "Stackable item without elements" );

        nArray<nInventoryItem*>::iterator it;

        // search for a stackable item to put the item
        for( it = this->container.Begin(); it != this->container.End() ; ++it )
        {
            if( (*it)->IsSameType( *item ) && 
                ( (*it)->IsInfinite() || (*it)->StackSize() < this->maxStackSize ) )
            {
                break;
            }                    
        }
        
        // calculate if we can put the stack item
        int allowedSpace = 0;
        if( it != this->container.End() )
        {
            allowedSpace += this->maxStackSize - (*it)->StackSize();
        }
        allowedSpace += ( this->container.AllocSize() - this->container.Size() ) * this->maxStackSize;
        if( allowedSpace < item->StackSize() )
        {
            return false;
        }

        // if stackable item is found
        if( it != this->container.End() )
        {
            // get elements to move 
            int space = this->maxStackSize - (*it)->StackSize();
            int elements = n_min( space, item->StackSize() );
            if( (*it)->IsInfinite() )
            {
                elements = (*it)->StackSize();
            }

            // put all items until full the stack
            (*it)->SetStack( (*it)->StackSize() + elements );
            item->SetStack( item->StackSize() - elements );
        }

        nInventoryManager * manager = nInventoryManager::Instance();

        // while there is space in the inventory and there the original item stack is not empty
        while( ( this->container.Size() < this->container.AllocSize() ) && 
            ( item->StackSize() > this->maxStackSize ) )
        {
            // create a new stack that equal original item
            nInventoryItem * newItem = manager->DuplicateItem( item );
            n_assert( newItem );

            if( newItem )
            {
                // move to the new stack items until full it
                newItem->SetStack( this->maxStackSize );
                item->SetStack( item->StackSize() - this->maxStackSize );

                // insert stack into the container
                this->container.PushBack( newItem );
            }
            else
            {
                return false;
            }
        }

        // if item is empty delete it
        if( 0 == item->StackSize() )
        {
            nInventoryManager * manager = nInventoryManager::Instance();
            if( manager )
            {
                manager->DeleteItem( item );
            }
            else
            {
                item->Release();
            }
        }
        // if item has element insert the original item
        else if( this->container.Size() < this->container.AllocSize() )
        {
            this->container.PushBack( item );
        }
        else
        {
            // there is no space to insert
            return false;
        }
    }
    // else, if item is no stackable
    else
    {
        // if there is space in the inventory
        if( this->container.Size() < this->container.AllocSize() )
        {
            // insert item in the container
            this->container.PushBack( item );
        }
        else
        {
            // can not be inserted
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @param slot the slot with the weapon
    @returns the item
*/
nInventoryItem*
nInventoryContainer::GetWeaponItemAt( int slot )
{
    bool valid = slot >= 0 && slot <= WST_CARRIED;
    if( valid )
    {
        nInventoryItem * item = this->weaponSlots[ slot ];

        return item;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param slot the slot with the item
    @returns the item
*/
nInventoryItem*
nInventoryContainer::GetItemAt( int slot )
{
    if( slot >= 0 && slot < this->container.Size() )
    {
        return ( this->container[ slot ] );
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param itemID the identifier of the item
    @returns the item
*/
nInventoryItem*
nInventoryContainer::GetItem( int itemID )
{
    if( itemID != -1 )
    {
        for( int i = 0 ; i < WST_LAST ; ++i )
        {
            if( this->weaponSlots[ i ] )
            {
                if( itemID == this->weaponSlots[ i ]->GetItemID() )
                {
                    return this->weaponSlots[ i ];
                }
            }
        }

        for( int i = 0 ; i < this->container.Size() ; ++i )
        {
            if( this->container[ i ] )
            {
                if( itemID == this->container[ i ]->GetItemID() )
                {
                    return this->container[ i ];
                }
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param slot the slot with the weapon
    @returns the item
*/
nInventoryItem*
nInventoryContainer::RemoveWeaponItemAt( WeaponSlotType slot )
{
    bool valid = slot >= 0 && slot <= WST_CARRIED;
    if( valid )
    {
        nInventoryItem * item = this->weaponSlots[ slot ];

        this->weaponSlots[ slot ] = 0;
        return item;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param slot the slot with the item
    @returns the item
*/
nInventoryItem*
nInventoryContainer::RemoveItemAt( int slot )
{
    if( slot >= 0 && slot < this->container.Size() )
    {
        nInventoryItem * item = this->container[ slot ];

        this->container.EraseQuick( slot );

        return item;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param entity entity that is searched in the inventory
    @return the inventory item
*/
nInventoryItem*
nInventoryContainer::GetItemWith( const nEntityObject * const entity )
{
    if( entity )
    {
        for( int i = 0 ; i < WST_LAST ; ++i )
        {
            if( this->weaponSlots[ i ] )
            {
                if( entity == this->weaponSlots[ i ]->GetEntity() )
                {
                    return this->weaponSlots[ i ];
                }
            }
        }

        for( int i = 0 ; i < this->container.Size() ; ++i )
        {
            if( this->container[ i ] )
            {
                if( entity == this->container[ i ]->GetEntity() )
                {
                    return this->container[ i ];
                }
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param name name of the class for entity
    @return the inventory entity
*/
nEntityObject*
nInventoryContainer::GetEntityWith( const char * name )
{
    nEntityObject * entity = 0;
    for( int i = 0 ; i < WST_LAST ; ++i )
    {
        if( this->weaponSlots[ i ] )
        {
            entity = this->weaponSlots[ i ]->GetEntity();
            if( entity && entity->IsA( name ) )
            {
                return entity;
            }
        }
    }

    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        if( this->container[ i ] )
        {
            entity = this->container[ i ]->GetEntity();
            if( entity && entity->IsA( name ) )
            {
                return entity;
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param type type of the ammunition
    @return number of bullets
*/
int
nInventoryContainer::GetNumBullets( int type )const
{
    int bullets = 0;

    nEntityObject * entity = 0;
    ncGPAmmunitionClass * ammunition = 0;

    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        if( this->container[ i ] && this->container[ i ]->IsAmmunition() )
        {
            entity = this->container[ i ]->GetEntity();
            if( entity )
            {
                ammunition = entity->GetClassComponent<ncGPAmmunitionClass>();
                if( ammunition && ( ammunition->GetAmmoType() == type ) )
                {
                    bullets += this->container[ i ]->StackSize();
                }
            }
        }
    }

    return bullets;
}

//------------------------------------------------------------------------------
/**
    @param type type of the ammunition
    @param number of bullets
*/
void
nInventoryContainer::RemoveBullets( int type, int bullets )
{
    bool hasBullets = true;
    while( bullets && hasBullets )
    {
        // search the inventory item with less bullets
        int index = -1;
        int minBullets = -1;

        nEntityObject * entity = 0;
        ncGPAmmunitionClass * ammunition = 0;

        for( int i = 0 ; i < this->container.Size() ; ++i )
        {
            if( this->container[ i ] && this->container[ i ]->IsAmmunition() )
            {
                entity = this->container[ i ]->GetEntity();
                if( entity )
                {
                    ammunition = entity->GetClassComponent<ncGPAmmunitionClass>();
                    if( ammunition && ( ammunition->GetAmmoType() == type ) )
                    {
                        int itemBullets = this->container[ i ]->StackSize();
                        if( ( minBullets < 0 ) || ( minBullets > itemBullets ) )
                        {
                            index = i;
                            minBullets = itemBullets;
                        }
                    }
                }
            }
        }

        if( index != -1 )
        {
            // remove bullets from item
            int removeBullets = n_min( bullets, minBullets );
            minBullets -= removeBullets;
            bullets -= removeBullets;

            this->container[ index ]->SetStack( minBullets );

            // if item has not bullets remove it
            if( minBullets <= 0 )
            {
                nInventoryManager * manager = nInventoryManager::Instance();
                if( manager )
                {
                    manager->DeleteItem( this->container[ index ] );
                }
                else
                {
                    this->container[ index ]->Release();
                }
                this->container.EraseQuick( index );
            }
        }
        else
        {
            hasBullets = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param name name of the equipment to remove
    @param number of equipment items
*/
void
nInventoryContainer::RemoveEquipment( const char * name, int number )
{
    bool hasEquipment = true;
    while( number && hasEquipment )
    {
        // search the inventory item with less bullets
        int index = -1;
        int minNum = -1;

        nEntityObject * entity = 0;

        for( int i = 0 ; i < this->container.Size() ; ++i )
        {
            if( this->container[ i ] && this->container[ i ]->IsEquipment() )
            {
                entity = this->container[ i ]->GetEntity();
                if( entity && entity->IsA( name ) )
                {
                    int itemNum = this->container[ i ]->StackSize();
                    if( ( minNum < 0 ) || ( minNum > itemNum ) )
                    {
                        index = i;
                        minNum = itemNum;
                    }
                }
            }
        }

        if( index != -1 )
        {
            // remove bullets from item
            int removeNum = n_min( number, minNum );
            minNum -= removeNum;
            number -= removeNum;

            this->container[ index ]->SetStack( minNum );

            // if item has not bullets remove it
            if( minNum <= 0 )
            {
                nInventoryManager * manager = nInventoryManager::Instance();
                if( manager )
                {
                    manager->DeleteItem( this->container[ index ] );
                }
                else
                {
                    this->container[ index ]->Release();
                }
                this->container.EraseQuick( index );
            }
        }
        else
        {
            hasEquipment = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param name name of the equipment
    @return number of equipment objects
*/
int
nInventoryContainer::GetNumEquipment( const char * name )const
{
    int number = 0;

    nEntityObject * entity = 0;

    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        if( this->container[ i ] && this->container[ i ]->IsEquipment() )
        {
            entity = this->container[ i ]->GetEntity();
            if( entity && entity->IsA( name ) )
            {
                number += this->container[ i ]->StackSize();
            }
        }
    }

    return number;
}

//------------------------------------------------------------------------------
/**
    @param name name of the equipment
    @return slot of last equipment
*/
int
nInventoryContainer::GetEntitySlot( nEntityObject * entity )const
{
    if( entity )
    {
        for( int i = 0 ; i < this->container.Size() ; ++i )
        {
            if( this->container[ i ] )
            {
                if( entity == this->container[ i ]->GetEntity() )
                {
                    return i;
                }
            }
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
    @param name name of the equipment
    @return slot of last equipment
*/
int
nInventoryContainer::GetEquipmentSlot( const char * name )const
{
    int slot = -1;

    nEntityObject * entity = 0;

    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        if( this->container[ i ] && this->container[ i ]->IsEquipment() )
        {
            entity = this->container[ i ]->GetEntity();
            if( entity && entity->IsA( name ) )
            {
                slot = i;
            }
        }
    }

    return slot;
}

//------------------------------------------------------------------------------
/**
    @param name name of the equipment
    @return equipment entity
*/
nEntityObject*
nInventoryContainer::GetInventoryItem( const char * name )const
{
    nEntityObject * entity = 0;

    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        if( this->container[ i ] )
        {
            entity = this->container[ i ]->GetEntity();
            if( entity && entity->IsA( name ) )
            {
                return entity;
            }
        }
    }

    return 0;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @param gfxserver graphics server
*/
void
nInventoryContainer::DebugDraw( nGfxServer2 * const gfxServer ) 
{
    if( ! gfxServer )
    {
        return;
    }

    float posy = -0.6f;
    const float posx = 0.1f;
    const float POSITION_Y_INC = 0.045f;
    nString text;

    ncGameplayClass * gameplayClass = 0;
    nEntityObject * entity = 0;

    // show the weapon bar
    for( int i = 0 ; i < WST_LAST ; ++i )
    {
        if( this->weaponSlots[ i ] )
        {
            text = nInventoryContainer::slotNames[ i ];
            text.Append( " : " );
            entity = this->weaponSlots[ i ]->GetEntity();
            if( entity )
            {
                gameplayClass = entity->GetClassComponent<ncGameplayClass>();
                if( gameplayClass )
                {
                    text.Append( gameplayClass->GetBeautyName() );
                }
            }

            gfxServer->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), posx, posy );
            posy += POSITION_Y_INC;
        }
    }

    nInventoryItem * item = 0;

    // show the items 
    for( int i = 0 ; i < this->container.Size() ; ++i )
    {
        text = "[";
        text.AppendInt( i );
        text.Append( "] " );

        item = this->container[ i ];
        if( item )
        {
            text.Append( nInventoryItem::typeNames[ item->GetType() ] );
            text.Append( " - " );

            if( item->IsStackable() )
            {
                text.Append( "(" );
                text.AppendInt( item->StackSize() );
                text.Append( ") " );
            }

            entity = item->GetEntity();
            if( entity )
            {
                gameplayClass = entity->GetClassComponent<ncGameplayClass>();
                if( gameplayClass )
                {
                    text.Append( gameplayClass->GetBeautyName() );
                }
                else
                {
                    text.Append( entity->GetEntityClass()->GetName() );
                }
            }
        }

        gfxServer->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), posx, posy );
        posy += POSITION_Y_INC;
    }
}
#endif//!NGAME

//------------------------------------------------------------------------------
