//------------------------------------------------------------------------------
//  ncnetwork_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "ncnetwork/ncnetwork.h"

#include "nnetworkmanager/nnetworkmanager.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncNetwork,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncNetwork::ncNetwork():
    localRole( NET_ROLE_NONE ),
    remoteRole( NET_ROLE_NONE ),
    updated( true ),
    savedState( 0 ),
    stateSize( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncNetwork::~ncNetwork()
{
    if( this->savedState )
    {
        n_delete_array( this->savedState );
    }
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncNetwork::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        nNetworkManager * network = nNetworkManager::Instance();

        if( network )
        {
            int id = this->GetEntityObject()->GetId();

            if( network->IsNetworkEntity( id ) )
            {
                network->AddNetworkEntity( this );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncNetwork::SetModified()
{
    if( this->updated )
    {
        this->updated = false;

        nNetworkManager * network = nNetworkManager::Instance();
        if( network && network->IsServer() )
        {
            network->AddModifiedEntity( this );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param frameTime time since last update
*/
void
ncNetwork::Update( float /*frameTime*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncNetwork::UpdateState()
{
    if( this->savedState )
    {
        nstream savedBuffer;
        
        savedBuffer.SetExternBuffer( this->stateSize, this->savedState );
        savedBuffer.SetWrite( false );

        this->UpdateNetwork( savedBuffer );

        savedBuffer.SetExternBuffer( 0, 0 );

        n_delete_array( this->savedState );
        this->savedState = 0;
        this->stateSize = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncNetwork::SetSavedState( const char * buffer, int size )
{
    if( this->savedState )
    {
        n_delete_array( this->savedState );
        this->savedState = 0;
        this->stateSize = 0;
    }

    if( buffer && ( size > 0 ) )
    {
        this->stateSize = size;
        this->savedState = n_new_array( char, size );
        memcpy( this->savedState, buffer, size );
    }
}

//------------------------------------------------------------------------------
