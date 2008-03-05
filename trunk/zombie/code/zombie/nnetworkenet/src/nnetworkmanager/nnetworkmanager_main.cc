//------------------------------------------------------------------------------
//  nnetworkmanager_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "ncnetwork/ncnetwork.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "entity/nentityobjectserver.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nNetworkManager, "nroot");

//------------------------------------------------------------------------------
nNetworkManager * nNetworkManager::singleton = 0;

//------------------------------------------------------------------------------
const int nNetworkManager::NLOGBASIC        = NLOG1;
const int nNetworkManager::NLOGCLIENT       = NLOG2;
const int nNetworkManager::NLOGSERVER       = NLOG3;
const int nNetworkManager::NLOGACTIONS      = NLOG4;
const int nNetworkManager::NLOGENTITIES     = NLOG5;
const int nNetworkManager::NLOGAPPLICATION  = NLOG6;

static const char * networkLogNames[] = {
    "Basic Messages",
    "Client Messages",
    "Server Messages",
    "Basic Actions",
    "Entities",
    "Application Layer",
    0
};
/// Log for Network Manager System
NCREATELOGLEVELGROUP( network, "Network Manager", false, 0, networkLogNames, NLOG_GROUP_MASK );

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nNetworkManager, OnNewClient );
NSIGNAL_DEFINE( nNetworkManager, OnConnected );
NSIGNAL_DEFINE( nNetworkManager, OnDisconnected );
NSIGNAL_DEFINE( nNetworkManager, OnClientDisconnected );
NSIGNAL_DEFINE( nNetworkManager, OnRPC );
NSIGNAL_DEFINE( nNetworkManager, OnRPCServer );
NSIGNAL_DEFINE( nNetworkManager, OnNewEntity );
NSIGNAL_DEFINE( nNetworkManager, OnDeleteEntity );

//------------------------------------------------------------------------------
/**
*/
nNetworkManager::nNetworkManager():
    clock( 0 ),
    lastTime( -1 ),
    deltaTime( 0 ),
    numActionArgs(0),
    isServer( true )
{
    n_assert2( ! nNetworkManager::singleton, "nNetworkManager already created" );

    nNetworkManager::singleton = this;

    NLOG( network, ( NLOGBASIC | 0, "Create Network Manager Basic" ) );
}

//------------------------------------------------------------------------------
/**
*/
nNetworkManager::~nNetworkManager()
{
    NLOG( network, ( NLOGBASIC | 0 , "Destroy Network Manager Basic" ) );

    nNetworkManager::singleton = 0;
}

//------------------------------------------------------------------------------
/**
    @returns the singleton instance of nNetworkManager
*/
nNetworkManager *
nNetworkManager::Instance()
{
    return nNetworkManager::singleton;
}

//------------------------------------------------------------------------------
/**
    @param time application time
*/
void
nNetworkManager::Trigger( nTime time )
{
    // update times
    if( this->lastTime <= 0 )
    {
        this->lastTime = time;
    }

    this->deltaTime = float( time - this->lastTime );
    this->lastTime = time;

    if( this->updateClock )
    {
        this->clock += this->deltaTime;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::StartClock()
{
    this->updateClock = true;
    this->clock = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::StopClock()
{
    this->updateClock = false;
}

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param numarg number of input arguments of the basic action
    @param arglist input arguments
    @param foreground if the action is foreground action or background action
*/
void
nNetworkManager::SendAction( const char * action, int numarg, nArg * arglist,  bool foreground )
{
    NLOG( network, ( NLOGACTIONS | 2, "Send Basic action \"%s\"", action ) );
    if( ! nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_IN_CLIENTS ) )
    {
        nGPActionManager::Instance()->SetAction( action, numarg, arglist, foreground );
    }
}

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param numarg number of input arguments of the basic action
    @param arglist input arguments
    @param foreground if the action is foreground action or background action
*/
void
nNetworkManager::SendQueueAction( const char * action, int numarg, nArg * arglist,  bool foreground )
{
    NLOG( network, ( NLOGACTIONS | 2, "Queue Basic action \"%s\"", action ) );
    if( ! nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_IN_CLIENTS ) )
    {
        nGPActionManager::Instance()->QueueAction( action, numarg, arglist, foreground );
    }
}

//------------------------------------------------------------------------------
/**
    @param entity modified entity
*/
void
nNetworkManager::AddModifiedEntity( ncNetwork * const entity )
{
    NLOGCOND( network, entity, ( NLOGENTITIES | 3, "Add Modified Entity %x", entity->GetEntityObject()->GetId() ) );
}

//------------------------------------------------------------------------------
/**
    @param entity new entity
*/
void
nNetworkManager::AddNetworkEntity( ncNetwork * const entity )
{
    NLOGCOND( network, entity, ( NLOGENTITIES | 0, "New Network Entity %x", entity->GetEntityObject()->GetId() ) );
}

//------------------------------------------------------------------------------
/**
    @param entity new entity
*/
bool
nNetworkManager::IsNetworkEntity( const nEntityObjectId entityId )const
{
    nEntityObjectServer * objectServer = nEntityObjectServer::Instance();

    bool valid = objectServer->GetEntityObjectType( entityId ) == nEntityObjectServer::Server;
    valid = valid || ( objectServer->GetEntityObjectType( entityId ) == nEntityObjectServer::Normal );
    return valid;
}

//------------------------------------------------------------------------------
/**
    @param value arg to push
*/
void
nNetworkManager::PushIntArg( const int value )
{
    if( this->numActionArgs < nNetworkManager::MAX_ACTIONARGS )
    {
        this->actionArgs[ this->numActionArgs ].Delete();
        this->actionArgs[ this->numActionArgs ].SetI( value );
        ++this->numActionArgs;
    }
}

//------------------------------------------------------------------------------
/**
    @param value arg to push
*/
void
nNetworkManager::PushObjectArg( nObject * value )
{
    if( this->numActionArgs < nNetworkManager::MAX_ACTIONARGS )
    {
        this->actionArgs[ this->numActionArgs ].Delete();
        this->actionArgs[ this->numActionArgs ].SetO( value );
        ++this->numActionArgs;
    }
}

//------------------------------------------------------------------------------
/**
    @param value arg to push
*/
void
nNetworkManager::PushFloatArg( const float value )
{
    if( this->numActionArgs < nNetworkManager::MAX_ACTIONARGS )
    {
        this->actionArgs[ this->numActionArgs ].Delete();
        this->actionArgs[ this->numActionArgs ].SetF( value );
        ++this->numActionArgs;
    }
}

//------------------------------------------------------------------------------
/**
    @param value arg to push
*/
void
nNetworkManager::PushStringArg( const char * value )
{
    if( this->numActionArgs < nNetworkManager::MAX_ACTIONARGS )
    {
        this->actionArgs[ this->numActionArgs ].Delete();
        this->actionArgs[ this->numActionArgs ].SetS( value );
        ++this->numActionArgs;
    }
}

//------------------------------------------------------------------------------
/**
    @param value arg to push
*/
void
nNetworkManager::PushBoolArg( const bool value )
{
    if( this->numActionArgs < nNetworkManager::MAX_ACTIONARGS )
    {
        this->actionArgs[ this->numActionArgs ].Delete();
        this->actionArgs[ this->numActionArgs ].SetB( value );
        ++this->numActionArgs;
    }
}

//------------------------------------------------------------------------------
/**
    @param value arg to push
*/
void
nNetworkManager::PushCharArg( const char value )
{
    if( this->numActionArgs < nNetworkManager::MAX_ACTIONARGS )
    {
        this->actionArgs[ this->numActionArgs ].Delete();
        this->actionArgs[ this->numActionArgs ].SetC( value );
        ++this->numActionArgs;
    }
}

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param foreground if the action is foreground action or background action
*/
void
nNetworkManager::SendActionScript( const char * name, bool foreground )
{
    bool valid = this->numActionArgs > 0;
    n_assert( valid );

    if( valid )
    {
        this->SendAction( name, this->numActionArgs, this->actionArgs, foreground );
        this->numActionArgs = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param foreground if the action is foreground action or background action
*/
void
nNetworkManager::SendQueueActionScript( const char * name, bool foreground )
{
    bool valid = this->numActionArgs > 0;
    n_assert( valid );

    if( valid )
    {
        this->SendQueueAction( name, this->numActionArgs, this->actionArgs, foreground );
        this->numActionArgs = 0;
    }
}

//------------------------------------------------------------------------------
