//------------------------------------------------------------------------------
//  ncgameplay_net.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"
#include "rnsgameplay/ncgameplay.h"
#include "nnetworkmanager/nnetworkmanager.h"

//------------------------------------------------------------------------------
namespace
{
    // Info about an action to be sent with BeginAction/EndAction
    // No simultaneous action building is supported
    nString netActionName;
    nArray<nArg> netActionArgs;
}

//------------------------------------------------------------------------------
/**
	BeginAction
*/
void
ncGameplay::BeginAction( const char* actionName ) const
{
    // Store action name
    netActionName = actionName;

    // Clear old args list
    netActionArgs.Clear();

    // First argument of all basic actions is the entity onto which the action is going to be applied
    nArg self;
    self.SetO( this->GetEntityObject() );
    netActionArgs.PushBack( self );
}

//------------------------------------------------------------------------------
/**
	AddIntArg
*/
void
ncGameplay::AddIntArg( int value ) const
{
    nArg arg;
    arg.SetI( value );
    netActionArgs.PushBack( arg );
}

//------------------------------------------------------------------------------
/**
	AddFloatArg
*/
void
ncGameplay::AddFloatArg( float value ) const
{
    nArg arg;
    arg.SetF( value );
    netActionArgs.PushBack( arg );
}

//------------------------------------------------------------------------------
/**
	AddStringArg
*/
void
ncGameplay::AddStringArg( const char* value ) const
{
    nArg arg;
    arg.SetS( value );
    netActionArgs.PushBack( arg );
}

//------------------------------------------------------------------------------
/**
	AddBoolArg
*/
void
ncGameplay::AddBoolArg( bool value ) const
{
    nArg arg;
    arg.SetB( value );
    netActionArgs.PushBack( arg );
}

//------------------------------------------------------------------------------
/**
	EndAction
*/
void
ncGameplay::EndAction() const
{
    // Send the action to the net, as foreground by default
    if ( nNetworkManager::Instance() )
    {
        nNetworkManager::Instance()->SendAction( netActionName.Get(), netActionArgs.Size(), netActionArgs.Begin(), true );
    }

    // Clear args list
    netActionArgs.Clear();
}

//------------------------------------------------------------------------------
