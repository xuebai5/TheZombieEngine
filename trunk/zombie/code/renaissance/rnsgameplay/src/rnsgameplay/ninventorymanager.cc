//------------------------------------------------------------------------------
//  ninventorymanager.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ninventorymanager.h"
#include "rnsgameplay/ninventoryitem.h"
#include "kernel/nkernelserver.h"

#include "rnsgameplay/nrnsentitymanager.h"

#include "entity/nentityobject.h"

#include "rnsgameplay/ncgameplayclass.h"

#include "zombieentity/ncdictionary.h"

#include "zombieentity/nctransform.h"

#ifndef NGAME
#include "gfx2/ngfxserver2.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaClass(nInventoryManager, "nobject");

nInventoryManager * nInventoryManager::singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nInventoryManager::nInventoryManager():
    nextItemID( 0 )
{
    nInventoryManager::singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nInventoryManager::~nInventoryManager()
{
    nInventoryManager::singleton = 0;

    this->Clear();
}


//------------------------------------------------------------------------------
/**
*/
void
nInventoryManager::Clear()
{
    for( int i = 0 ; i < this->items.Size() ; ++i )
    {
        if( this->items[ i ] ){
            this->items[ i ]->Release();
        }
    }

    this->items.Clear();
}

//------------------------------------------------------------------------------
/**
    @returns the nDamageManager singleton instance
*/
nInventoryManager * 
nInventoryManager::Instance(){
    return nInventoryManager::singleton;
}

//------------------------------------------------------------------------------
/**
    @param name class of the new item
    @returns the item
*/
nInventoryItem*
nInventoryManager::NewItem( const char * name )
{
    nInventoryItem * item = static_cast<nInventoryItem*>( 
        nKernelServer::Instance()->New( "ninventoryitem" ) );
    if( item )
    {
        item->Create( name );
        if( nInventoryItem::IT_NOTHING == item->GetType() )
        {
            item->Release();
            item = 0;
        }
    }

    if( item )
    {
        this->items.PushBack( item );
        item->SetItemID( this->nextItemID );
        ++this->nextItemID;
    }

    return item;
}

//------------------------------------------------------------------------------
/**
    @param name class of the new item
    @returns the item
*/
nInventoryItem*
nInventoryManager::DuplicateItem( nInventoryItem * olditem )
{
    const char * name = olditem->GetItemName();

    nInventoryItem * item = static_cast<nInventoryItem*>( 
        nKernelServer::Instance()->New( "ninventoryitem" ) );
    if( item )
    {
        item->Create( name );
        if( nInventoryItem::IT_NOTHING == item->GetType() )
        {
            item->Release();
            item = 0;
        }
    }

    if( item )
    {
        item->CopyTypeFrom( *olditem );

        this->items.PushBack( item );
        item->SetItemID( this->nextItemID );
        ++this->nextItemID;
    }

    return item;
}

//------------------------------------------------------------------------------
/**
    @param item
*/
void
nInventoryManager::DeleteItem( nInventoryItem * item )
{
    n_assert( item );

    int index = this->items.FindIndex( item );

    // erase item from array
    if( index != -1 )
    {
        this->items.EraseQuick( index );
    }
    else
    {
        n_assert2_always( "Deleted Inventory Item not found" );
    }

    // erase item
    item->Release();
}

//------------------------------------------------------------------------------
/**
    @param itemID identifier of the item
    @returns the item
*/
nInventoryItem*
nInventoryManager::GetItem( int itemID )
{
    for( int i = 0; i < this->items.Size() ; ++i )
    {
        if (this->items[ i ]->GetItemID() == itemID )
        {
            return this->items[ i ];
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param index index of the item
    @returns the item
*/
nInventoryItem*
nInventoryManager::GetItemAt( int index )
{
    if( index >= 0 && index < this->items.Size() )
    {
        return ( this->items[ index ] );
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param item item used to created the drop
    @returns the dropped item
*/
nEntityObject*
nInventoryManager::CreateDroppedItem( nInventoryItem * const item )
{
    nEntityObject * drop = 0;

    int index = this->items.FindIndex( item );
    n_assert2( index != -1, "Dropped Inventory Item don't exist" );

    if( index != -1 )
    {
        nRnsEntityManager * entityManager = nRnsEntityManager::Instance();
        n_assert( entityManager );
        if( entityManager )
        {
            switch( item->GetType() )
            {
            case nInventoryItem::IT_WEAPON:
                drop = entityManager->CreateLocalEntity( "Weaponitembox" );
                break;

            case nInventoryItem::IT_ACCESSORY:
                drop = entityManager->CreateLocalEntity( "Accesoryitembox" );
                break;

            case nInventoryItem::IT_AMMUNITION:
            case nInventoryItem::IT_EQUIPMENT:
                drop = entityManager->CreateLocalEntity( "Accesoryitembox" );
                break;

            default:
                n_assert2_always( "Create invalid dropped item" );
            }
        }
        
        if( drop )
        {
            item->SetDropObject( drop );
        }
    }

    return drop;
}

//------------------------------------------------------------------------------
/**
    @param item item used to erase the drop
*/
void
nInventoryManager::EraseDroppedItem( nInventoryItem * const item )
{
    int index = this->items.FindIndex( item );
    n_assert2( index != -1, "Dropped Inventory Item don't exist" );

    if( index != -1 )
    {
        n_assert2( item->IsDropped(), "Inventory Item is not a dropped" );
        if( item->IsDropped() )
        {
            item->RemoveDropObject();
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param data inventory data
*/
void
nInventoryManager::NetworkUpdate( nstream & data )
{
    data.UpdateInt( this->nextItemID );
}

//------------------------------------------------------------------------------
/**
    @param data inventory data
*/
void
nInventoryManager::NewNetworkItem( nstream & data )
{
    nInventoryItem * item = 0;

    item = static_cast<nInventoryItem*>( 
        nKernelServer::Instance()->New( "ninventoryitem" ) );
    if( item )
    {
        item->NetworkUpdate( data );
        if( nInventoryItem::IT_NOTHING == item->GetType() )
        {
            item->Release();
            item = 0;
        }
    }

    if( item )
    {
        bool valid;
        // check that there is not repeated itemId
        valid = this->GetItem( item->GetItemID() ) == 0;
        n_assert2( valid, "Repeated item in the inventory" );
        if( valid )
        {
            this->items.PushBack( item );

            // if it is a dropped item, drop it
            if( item->IsDropped() )
            {
                vector3 position;
                data.UpdateVector3( position );

                nEntityObject * droppedItem = 0;
                droppedItem = this->CreateDroppedItem( item );
                n_assert( droppedItem );

                if( droppedItem )
                {
                    ncTransform * itemPosition = droppedItem->GetComponentSafe<ncTransform>();
                    if( itemPosition )
                    {
                        itemPosition->SetPosition( position );
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param index item to update
    @param data inventory data
*/
void
nInventoryManager::GetNetworkItem( int index, nstream & data )
{
    if( index >= 0 && index < this->items.Size() )
    {
        nInventoryItem * item = this->items[ index ];

        if( item )
        {
            item->NetworkUpdate( data );

            if( item->IsDropped() )
            {
                vector3 position;
                nEntityObject * droppedEntity = item->GetDropObject();

                if( droppedEntity )
                {
                    ncTransform * transform = droppedEntity->GetComponent<ncTransform>();
                    if( transform )
                    {
                        position = transform->GetPosition();
                    }
                }

                data.UpdateVector3( position );
            }
        }
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @param gfxserver graphics server
*/
void
nInventoryManager::DebugDraw( nGfxServer2 * const gfxServer ) 
{
    if( ! gfxServer )
    {
        return;
    }

    float posy = -0.8f;
    const float posx = -0.95f;
    const float POSITION_Y_INC = 0.045f;
    nString text;

    ncGameplayClass * gameplayClass = 0;
    nEntityObject * entity = 0;
    nInventoryItem * item = 0;

    // show the items 
    for( int i = 0 ; i < this->items.Size() ; ++i )
    {
        item = this->items[ i ];

        text.Clear();

        if( item->IsDropped() )
        {
            text.Append( "x " );
        }
        text.Append( "[" );
        text.AppendInt( i );
        text.Append( "] " );

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

        gfxServer->Text( text.Get(), vector4( 1, 0.8f, 1.0f, 0.8f), posx, posy );
        posy += POSITION_Y_INC;
    }
}
#endif//NGAME

//------------------------------------------------------------------------------
