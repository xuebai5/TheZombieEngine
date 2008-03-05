//------------------------------------------------------------------------------
//  ncnetworkliving.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsnetwork.h"

#include "rnsnetwork/ncnetworkliving.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "ncaimovengine/ncaimovengine.h"

#include "nclogicanimator/nclogicanimator.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ninventorymanager.h"

#include "zombieentity/ncdictionaryclass.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncNetworkLiving,ncNetwork);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNetworkLiving)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
const float ncNetworkLiving::lerpFactor = 2.0f;
const float ncNetworkLiving::minimumDistance = 0.0001f;
const float ncNetworkLiving::maximumDistance = 5.0000f;
const float ncNetworkLiving::minimumAngle = 0.01f;
const float ncNetworkLiving::maximumAngle = N_HALFPI;

//------------------------------------------------------------------------------
/**
*/
ncNetworkLiving::ncNetworkLiving():
    livingComp( 0 ),
    movEngine( 0 ),
    transform( 0 ),
    playerComp( 0 ),
    updatePosition( false ),
    updateRotation( false )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncNetworkLiving::~ncNetworkLiving()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncNetworkLiving::InitInstance(nObject::InitInstanceMsg initType)
{
    this->livingComp = this->GetComponentSafe<ncGameplayLiving>();
    this->movEngine = this->GetComponent<ncAIMovEngine>();
    this->transform = this->GetComponent<ncTransform>();

    this->animator = this->GetComponent<ncLogicAnimator>();
    
    this->playerComp = this->GetComponent<ncGameplayPlayer>();

    ncDictionaryClass * dictionary = this->GetClassComponent<ncDictionaryClass>();
    if( dictionary && dictionary->GetBoolVariable( "NetworkDisableAnimationUpdate" ) )
    {
        this->animator = 0;
    }

    ncNetwork::InitInstance( initType );
}

//------------------------------------------------------------------------------
/**
    @param frameTime time since last update
*/
void
ncNetworkLiving::Update( float frameTime )
{
    if( this->transform )
    {
        float lerp = n_min( frameTime * ncNetworkLiving::lerpFactor, 1.0f );

        vector3 diff, actualValue, newValue;

        if( this->updatePosition )
        {
            if( this->livingComp && ! this->livingComp->IsDead() )
            {
                if( this->playerComp )
                {
                    this->playerComp->GetPhysicPosition( actualValue );
                }
                else
                {
                    actualValue = this->transform->GetPosition();
                }

                newValue = this->finalPosition;
                newValue.lerp( actualValue, lerp );

                diff = newValue - this->finalPosition;
                float distance = diff.lensquared();
                if( distance <= ncNetworkLiving::minimumDistance && distance >= ncNetworkLiving::maximumDistance )
                {
                    newValue = this->finalPosition;
                    this->updatePosition = false;
                }

                if( this->playerComp )
                {
                    this->playerComp->SetPhysicPosition( newValue );
                }
                else
                {
                    this->transform->SetPosition( newValue );
                }
            }
        }

        quaternion actualRotation, newRotation;
        if( this->updateRotation )
        {
            actualRotation = this->transform->GetQuat();

            newRotation.slerp( actualRotation, this->finalRotation, lerp );

            float angle = newRotation.angle_between( this->finalRotation );
            if( angle <= ncNetworkLiving::minimumAngle && angle >= ncNetworkLiving::maximumAngle )
            {
                newRotation = this->finalRotation;
                this->updateRotation = false;
            }
            this->transform->SetQuat( newRotation );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be updated
*/
void
ncNetworkLiving::UpdateNetwork( nstream & buffer )
{
    if( this->livingComp )
    {
        int health;

        if( buffer.GetWrite() )
        {
            health = this->livingComp->GetHealth();
        }

        buffer.UpdateInt( health );

        if( ! buffer.GetWrite() )
        {
            this->livingComp->SetHealth( health );
        }
    }

    if( this->transform )
    {
        vector3 position;
        quaternion rotation;

        if( buffer.GetWrite() )
        {
            if( this->playerComp )
            {
                this->playerComp->GetPhysicPosition( position );
            }
            else
            {
                position = this->transform->GetPosition();
            }

            rotation = this->transform->GetQuat();
        }

        buffer.UpdateVector3( position );
        buffer.UpdateFloat( rotation.x );
        buffer.UpdateFloat( rotation.y );
        buffer.UpdateFloat( rotation.z );
        buffer.UpdateFloat( rotation.w );

        if( ! buffer.GetWrite() )
        {
            vector3 diff;

            // update position
            this->finalPosition = position;

            if( this->playerComp )
            {
                this->playerComp->GetPhysicPosition( position );
            }
            else
            {
                position = this->transform->GetPosition();
            }

            diff = position - this->finalPosition;
            float distance = diff.lensquared();
            if( this->playerComp && distance > ncNetworkLiving::maximumDistance )
            {
                if( this->livingComp && ! this->livingComp->IsDead() )
                {
                    this->playerComp->SetPhysicPosition( this->finalPosition );
                }
                this->updatePosition = false;
            }
            else
            {
                if( distance > ncNetworkLiving::minimumDistance && distance < ncNetworkLiving::maximumDistance )
                {
                    this->updatePosition = true;
                }
                else
                {
                    this->updatePosition = false;
                    if( this->livingComp && ! this->livingComp->IsDead() )
                    {
                        this->transform->SetPosition( this->finalPosition );
                    }
                }
            }

            // update angle
            this->finalRotation = rotation;
            rotation = this->transform->GetQuat();

            float angle = rotation.angle_between( this->finalRotation );
            if( angle > ncNetworkLiving::minimumAngle && angle < ncNetworkLiving::maximumAngle )
            {
                this->updateRotation = true;
            }
            else
            {
                this->updateRotation = false;
                this->transform->SetQuat( this->finalRotation );
            }
        }
    }

    if( this->movEngine )
    {
        int movEngineState;
        vector3 goal;
        
        if( buffer.GetWrite() )
        {
            movEngineState = this->movEngine->GetMovEngineState();

            // get moving state
            if( this->movEngine->IsMoving() )
            {
                goal = this->movEngine->GetGoal();
            }
        }

        buffer.UpdateInt( movEngineState );
        buffer.UpdateVector3( goal );

        if( ! buffer.GetWrite() )
        {
            if( 1 == movEngineState && ! this->movEngine->IsPaused() )
            {
                this->movEngine->SetPause( true );
            }
            else
            {
                // restore paused state
                if( this->movEngine->IsPaused() )
                {
                    this->movEngine->SetPause( false );
                }

                this->movEngine->SetMovEngineState( movEngineState );

                // update moving state
                if( this->movEngine->IsMoving() )
                {
                    this->movEngine->MoveTo( goal );
                }
            }
        }
    }

    if( this->animator )
    {
        this->animator->UpdateAnims( buffer );
    }
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be initialized
*/
bool
ncNetworkLiving::InitFromNetwork( nstream & buffer )
{
    if( buffer.GetWrite() )
    {
        this->CopyTo( buffer );
    }
    else
    {
        this->InitializeWith( buffer );
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @param data where put the initialize data
*/
void
ncNetworkLiving::CopyTo( nstream & data )
{
    if( this->livingComp )
    {
        int health;

        // copy the actual health of player to buffer
        health = this->livingComp->GetHealth();

        data.UpdateInt( health );

        // send inventory weapon items
        nInventoryContainer * inventory = this->livingComp->GetInventory();
        n_assert( inventory );
        
        nInventoryItem * item = 0;
        for( int i = nInventoryContainer::WST_MELEE ; 
            i < nInventoryContainer::WST_LAST ; ++i )
        {
            int itemID = -1;
            if( inventory )
            {
                item = inventory->GetWeaponItemAt( i );
            }

            if( item )
            {
                itemID = item->GetItemID();
            }

            data.UpdateInt( itemID );
        }

        // send inventory normal items
        int numItems = 0; 
        if( inventory )
        {
            numItems = inventory->GetNumItems();
        }

        data.UpdateInt( numItems );

        for( int i = 0; i < numItems ; ++i )
        {
            int itemID = -1;
            item = inventory->GetItemAt( i );
            n_assert( item );
            if( item )
            {
                itemID = item->GetItemID();
            }

            data.UpdateInt( itemID );
        }

        // send actual weapon
        int weaponID = -1;
        if( this->livingComp->GetCurrentWeapon() )
        {
            item = inventory->GetItemWith( this->livingComp->GetCurrentWeapon() );
            NLOGCOND( network, !item, ( 5, "SERVER: ERROR: Weapon is not from inventory" ) );
            if( item )
            {
                weaponID = item->GetItemID();
            }
        }

        data.UpdateInt( weaponID );
    }
}

//------------------------------------------------------------------------------
/**
    @param data from where get the initialize data
*/
void
ncNetworkLiving::InitializeWith( nstream & data )
{
    if( this->livingComp )
    {
        int health;

        // copy the actual health from buffer
        data.UpdateInt( health );

        this->livingComp->SetHealth( health );

        // get inventory weapon items
        nInventoryContainer * inventory = this->livingComp->GetInventory();
        nInventoryManager * invManager = nInventoryManager::Instance();
        n_assert( inventory && invManager );

        int itemID = -1;
        nInventoryItem * item = 0;

        for( int i = nInventoryContainer::WST_MELEE ;
            i < nInventoryContainer::WST_LAST ; ++i )
        {
            data.UpdateInt( itemID );
            
            item = 0;
            if( invManager && ( itemID != -1 ) )
            {
                item = invManager->GetItem( itemID );
                NLOGCOND( network, !item, 
                    ( 5, "CLIENT: ERROR: No inventory item %d", itemID ) );
            }

            if( inventory && item )
            {
                bool valid = inventory->InsertItem( item, nInventoryContainer::WeaponSlotType( i ) );
                NLOGCOND( network, !valid, 
                    ( 5, "CLIENT: ERROR: Can't insert inventory item %d", itemID ) );
            }
        }

        // get inventory normal items
        int numItems = 0;

        data.UpdateInt( numItems );

        for( int i = 0 ; i < numItems ; ++i )
        {
            data.UpdateInt( itemID );
            item = 0;
            if( invManager )
            {
                item = invManager->GetItem( itemID );
                NLOGCOND( network, !item, 
                    ( 5, "CLIENT: ERROR: No inventory item %d", itemID ) );
            }

            if( inventory && item )
            {
                bool valid = inventory->InsertItem( item, nInventoryContainer::WeaponSlotType( i ) );
                NLOGCOND( network, !valid, 
                    ( 5, "CLIENT: ERROR: Can't insert inventory item %d", itemID ) );
            }
        }

        // get actual weapon
        data.UpdateInt( itemID );
        if( itemID != -1 )
        {
            nEntityObject * weapon = 0;
            item = 0;
            if( inventory )
            {
                item = inventory->GetItem( itemID );
                NLOGCOND( network, !item, 
                    ( 5, "CLIENT: ERROR: No inventory item %d", itemID ) );

                if( item && item->IsWeapon() )
                {
                    weapon = item->GetEntity();
                }
            }

            if( weapon )
            {
                this->livingComp->SetCurrentWeapon( weapon );
            }
        }
    }
}

//------------------------------------------------------------------------------
