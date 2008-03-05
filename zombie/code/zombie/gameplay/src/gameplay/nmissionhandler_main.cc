//-----------------------------------------------------------------------------
//  nmissionhandler_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"
#include "gameplay/nmissionhandler.h"

//-----------------------------------------------------------------------------
nNebulaScriptClass(nMissionHandler, "nroot");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nMissionHandler, ObjectiveStateChanged);

//------------------------------------------------------------------------------
nMissionHandler* nMissionHandler::instance = 0;

//-----------------------------------------------------------------------------
namespace
{
    const nString HiddenLabel( "hidden" );
    const nString AssignedLabel( "assigned" );
    const nString CompletedLabel( "completed" );
    const nString RemovedLabel( "removed" );

    const char* ObjectivesFilePath( "level:gameplay/objectives.n2" );
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nMissionHandler::nMissionHandler()
{
    if ( !nMissionHandler::instance )
    {
        // Initialize instance pointer
        nMissionHandler::instance = this;
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nMissionHandler::~nMissionHandler()
{
    if ( nMissionHandler::instance == this )
    {
        nMissionHandler::instance = 0;
    }

    this->Clear();
}

//-----------------------------------------------------------------------------
/**
    Do any required initialization

    Basically it binds itself to the loaded and saved level signals, so call
    this method after the level manager has been instantiated.
*/
void
nMissionHandler::Init()
{
    // Binds to the load and saved level signals to load and save the objectives list
    nLevelManager::Instance()->BindSignal( nLevelManager::SignalLevelLoaded,
        this, &nMissionHandler::OnLevelLoaded, 0 );
    nLevelManager::Instance()->BindSignal( nLevelManager::SignalLevelSaved,
        this, &nMissionHandler::OnLevelSaved, 0 );
}

//-----------------------------------------------------------------------------
/**
    Delete all the objectives
*/
void
nMissionHandler::Clear()
{
    this->objectives.Clear();
}

//-----------------------------------------------------------------------------
/**
    Get the index of the objective associated to the given name, or NOT_FOUND if there isn't any

    No high performance search struct or algorithm is used to look for the
    objective since its not expected that a level is going to have many
    objectives neither it will be done massive calls to this method.
*/
int
nMissionHandler::FindObjectiveIndex( const char* name ) const
{
    // Iterate over all the objectives looking for the one whose name matches with the given one
    for ( int i(0); i < this->objectives.Size(); ++i )
    {
        if ( this->objectives[i].name == name )
        {
            return i;
        }
    }

    // Objective name not found
    return NOT_FOUND;
}

//-----------------------------------------------------------------------------
/**
    Append a new objective to the end of the objectives list

    The state of new objectives is set to hidden by default.
    Nothing is done if there's another objective with the same name.
*/
void
nMissionHandler::AppendObjective( const char* name, const char* description )
{
    // Verify that there isn't another objective with the same name
    if ( this->FindObjectiveIndex(name) != NOT_FOUND )
    {
        return;
    }

    // Add the new objective
    this->objectives.Append( Objective(name, description, HIDDEN) );
}

//-----------------------------------------------------------------------------
/**
    Set an objective name by index

    Nothing is done if there's another objective with the same name.
*/
void
nMissionHandler::SetObjectiveNameByIndex( int index, const char* name )
{
    // Verify that there isn't another objective with the same name
    if ( this->FindObjectiveIndex(name) != NOT_FOUND )
    {
        return;
    }

    // Change the state of the objective
    n_assert( index >= 0 && index < this->objectives.Size() );
    this->objectives[index].name = name;
}

//-----------------------------------------------------------------------------
/**
    Set an objective description by index
*/
void
nMissionHandler::SetObjectiveDescriptionByIndex( int index, const char* description )
{
    // Change the description of the objective
    n_assert( index >= 0 && index < this->objectives.Size() );
    this->objectives[index].description = description;
}

//-----------------------------------------------------------------------------
/**
    Delete an objective by index
*/
void
nMissionHandler::DeleteObjectiveByIndex( int index )
{
    // Remove the objective from the objectives list
    n_assert( index >= 0 && index < this->objectives.Size() );
    this->objectives.Erase( index );
}

//-----------------------------------------------------------------------------
/**
    Set the state of an objective by name
*/
void
nMissionHandler::SetObjectiveState( const char* name, const char* stateLabel )
{
    // Get the objective index
    int index( this->FindObjectiveIndex(name) );
    if ( index == NOT_FOUND )
    {
        return;
    }

    // Get the new objective state
    ObjectiveState state( this->StateLabelToId( stateLabel ) );

    // Change the objective state to all clients and server
    this->SendObjectiveStateChange( index, state );
}

//-----------------------------------------------------------------------------
/**
    Set an objective state by index
*/
void
nMissionHandler::SetObjectiveStateByIndex( int index, ObjectiveState state )
{
    this->SendObjectiveStateChange( index, state );
}

//-----------------------------------------------------------------------------
/**
    Send a state change to all clients
*/
void
nMissionHandler::SendObjectiveStateChange( int index, ObjectiveState state )
{
    // Pack objective index and state
    nstream data;
    data.SetWrite( true );
    data.UpdateInt( index );
    int intState( state );
    data.UpdateInt( intState );

    // Update change locally
    this->ReceiveObjectiveStateChange( &data );

    // Send state change to all clients
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        nNetworkManager::Instance()->CallRPCAll(
            nRnsEntityManager::SET_OBJECTIVE_STATE,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//-----------------------------------------------------------------------------
/**
    Method used to update an objective state change notified from the server side
*/
void
nMissionHandler::ReceiveObjectiveStateChange( nstream* data )
{
    n_assert( data );
    if ( data )
    {
        // Unpack objective index and state
        data->SetWrite( false );
        int index;
        data->UpdateInt( index );
        int state;
        data->UpdateInt( state );

        // Update objective state locally
        if ( index >= 0 && index < this->objectives.Size() )
        {
            if ( this->objectives[index].state != state )
            {
                // Only update if it's really a new state to avoid signalling when there's no change at all
                this->objectives[index].state = ObjectiveState( state );
                this->SignalObjectiveStateChanged( this, index );
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Load the objectives list for the current level, replacing the old ones
*/
void
nMissionHandler::OnLevelLoaded()
{
    this->Clear();
    if ( nFileServer2::Instance()->FileExists( ObjectivesFilePath ) )
    {
        this->LoadState( ObjectivesFilePath );
    }
}

//-----------------------------------------------------------------------------
/**
    Save the objectives list into the current level
*/
void
nMissionHandler::OnLevelSaved()
{
    nString dirPath( nString(ObjectivesFilePath).ExtractDirName() );
    nFileServer2::Instance()->MakePath( dirPath.Get() );
    this->SaveStateAs( ObjectivesFilePath );
}

//-----------------------------------------------------------------------------
/**
    Get an objective state string by index
*/
const char *
nMissionHandler::GetObjectiveStateStringByIndex( int index ) const
{
    ObjectiveState state = this->GetObjectiveStateByIndex( index );
    return this->StateIdToLabel( state );
}

//-----------------------------------------------------------------------------
/**
    Set an objective state string by index
*/
void
nMissionHandler::SetObjectiveStateStringByIndex( int index, const char* stateLabel )
{
    ObjectiveState state( this->StateLabelToId( stateLabel ) );
    this->SetObjectiveStateByIndex( index, state );
}

//-----------------------------------------------------------------------------
/**
    Get the objective state id assigned to a state label
*/
nMissionHandler::ObjectiveState
nMissionHandler::StateLabelToId( const char* stateLabel ) const
{
    if ( stateLabel == HiddenLabel )
    {
        return HIDDEN;
    }
    else if ( stateLabel == AssignedLabel )
    {
        return ASSIGNED;
    }
    else if ( stateLabel == CompletedLabel )
    {
        return COMPLETED;
    }
    else if ( stateLabel == RemovedLabel )
    {
        return REMOVED;
    }

#ifndef NGAME
    nString msg( "Bad objective state '" );
    msg += stateLabel;
    msg += "'.";
    n_error( msg.Get() );
#endif
    return HIDDEN;
}

//-----------------------------------------------------------------------------
/**
    Get the objective state label assigned to a state id
*/
const char*
nMissionHandler::StateIdToLabel( ObjectiveState state ) const
{
    switch ( state )
    {
        case HIDDEN:
            return HiddenLabel.Get();
        case ASSIGNED:
            return AssignedLabel.Get();
        case COMPLETED:
            return CompletedLabel.Get();
        case REMOVED:
            return RemovedLabel.Get();
    }

#ifndef NGAME
    nString msg( "Bad objective state id " + nString(state) );
    n_error( msg.Get() );
#endif
    return HiddenLabel.Get();
}
