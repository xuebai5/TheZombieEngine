//------------------------------------------------------------------------------
//  nrnsentitymanager_net.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/nrnsentitymanager.h"

#include "entity/nentityobjectserver.h"

#include "nphysics/ncphysicsobj.h"

#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"

#include "rnsgameplay/ninventorymanager.h"
#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ncgameplay.h"

#include "nnetworkmanager/nnetworkmanager.h"

//------------------------------------------------------------------------------
/**
    @param id identifier of the new entity
*/
void
nRnsEntityManager::NewNetworkEntity( int id )
{
    nEntityObject * object = nEntityObjectServer::Instance()->GetEntityObject( id );

    n_assert( object );
    
    if( object )
    {
        nLevelManager::Instance()->GetCurrentLevel()->AddEntity( object );

        // Add entity to physic space
        ncPhysicsObj* phyComp = object->GetComponent<ncPhysicsObj>();
        if ( phyComp )
        {
            phyComp->AutoInsertInSpace();
        }

        ncGameplay * gameplay = object->GetComponent<ncGameplay>();
        if( gameplay )
        {
            this->gameplayEntities.Append( object );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param client identifier of the new client connected
*/
void
nRnsEntityManager::NewNetworkClient( int client )
{
    nNetworkManager * network = nNetworkManager::Instance();
    
    if( network )
    {
        nstream data;
        int eoid;

        // update actual eliminated entities
        for( nArray<nEntityObjectId>::iterator iter = this->removedEntities.Begin() ;
            iter != this->removedEntities.End() ; ++iter )
        {
            eoid = *iter;
            data.SetWrite( true );
            data.UpdateInt( eoid );
            network->CallRPCClient( client, nRnsEntityManager::DELETE_ENTITY,
                data.GetBufferSize(), data.GetBuffer() );
        }

        // send inventory items
        if( this->inventoryManager )
        {
            data.SetWrite( true );
            this->inventoryManager->NetworkUpdate( data );

            if( data.GetBufferSize() > 0 )
            {
                network->CallRPCClient( client, 
                    nRnsEntityManager::UPDATE_INVENTORY, data.GetBufferSize(), data.GetBuffer() );  
            }

            for( int i = 0 ; i < this->inventoryManager->GetNumItems() ; ++i )
            {
                data.SetWrite( true );

                this->inventoryManager->GetNetworkItem( i, data );

                if( data.GetBufferSize() > 0 )
                {
                    network->CallRPCClient( client,
                        nRnsEntityManager::INVENTORY_ITEM, data.GetBufferSize(), data.GetBuffer() );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
