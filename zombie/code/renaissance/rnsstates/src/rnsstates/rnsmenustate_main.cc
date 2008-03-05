//------------------------------------------------------------------------------
//  rnsmenustate_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsmenustate.h"

#include "nnetworkmanager/nnetworkmanager.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(RnsMenuState);
    NSCRIPT_ADDCMD('LSES', void, SetExitState, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('LEXI', void, Exit, 0, (), 0, ());
NSCRIPT_INITCMDS_END();

//------------------------------------------------------------------------------
nNebulaScriptClass( RnsMenuState, "ncommonstate" );

//------------------------------------------------------------------------------
/**
*/
RnsMenuState::RnsMenuState():
    useExitName( false )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RnsMenuState::~RnsMenuState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
RnsMenuState::OnCreate(nApplication* application)
{
    nCommonState::OnCreate(application);

    this->loadEnabled = false;
}

//------------------------------------------------------------------------------
/**
    @param prevState previous state
*/
void 
RnsMenuState::OnStateEnter( const nString & prevState )
{
    if( ! this->useExitName )
    {
        this->exitStateName = prevState;
    }

    nCommonState::OnStateEnter( prevState );
}

//------------------------------------------------------------------------------
/**
*/
void
RnsMenuState::OnFrame()
{
    nNetworkManager * network = static_cast<nNetworkManager*>( 
        nKernelServer::Instance()->Lookup( "/sys/servers/network" ) );

    if( network )
    {
        network->Trigger( this->app->GetTime() );
    }

    nCommonState::OnFrame();
}

//------------------------------------------------------------------------------
/**
    @param stateName the next state before load
*/
void
RnsMenuState::SetExitState( const char * stateName )
{
    this->exitStateName = stateName;
    this->useExitName = true;
}

//------------------------------------------------------------------------------
/**
*/
void
RnsMenuState::Exit()
{
    if( this->exitStateName.IsEmpty() )
    {
        this->app->SetQuitRequested(true);
    }
    else
    {
        this->app->SetState( this->exitStateName );
    }
}

//------------------------------------------------------------------------------
