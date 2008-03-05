//-----------------------------------------------------------------------------
//  nfsmserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsmserver/nfsmserver.h"
#include "nfsm/nfsm.h"
#include "nfsm/nstate.h"
#include "nfsm/neventcondition.h"
#include "nfsm/nscriptcondition.h"
#include "nfsm/nfsmselector.h"
#include "nfsm/nactiondesc.h"
#include "nfsm/ntransition.h"
#include "ntrigger/ngameevent.h"

nNebulaScriptClass(nFSMServer, "nroot");

//------------------------------------------------------------------------------
#ifndef __NEBULA_NO_LOG__
const int nFSMServer::ErrorsLog = NLOG1;
const int nFSMServer::EventsLog = NLOG2;
const int nFSMServer::TransitionsLog = NLOG3;
const int nFSMServer::StatesLog = NLOG4;


namespace
{
    const char * logNames[] = {
        "Errors",
        "Events",
        "Transitions",
        "States",
        0
    };
}

NCREATELOGLEVELGROUP( fsm, "FSM", true, 0, logNames, nFSMServer::ErrorsLog );
#endif

//------------------------------------------------------------------------------
nFSMServer* nFSMServer::instance = 0;
const char* nFSMServer::rootObjPath = "/fsm";
const char* nFSMServer::fsmsSubpath = "fsms";
const char* nFSMServer::eventCondsSubpath = "eventconditions";
const char* nFSMServer::scriptCondsSubpath = "scriptconditions";
const char* nFSMServer::fsmSelectorsSubpath = "fsmselectors";
const char* nFSMServer::actionScriptsSubpath = "actionscripts";
const char* nFSMServer::transitionsSubpath = "transitions";
const char* nFSMServer::localCondsSubpath = "";
const char* nFSMServer::behactionsSubpath = "";

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nFSMServer::nFSMServer()
    : rootFilePath( "wc:libs/fsm" )
#ifndef NGAME
    , enqueueEvents( true )
#endif
{
    if ( !nFSMServer::instance )
    {
        // Initialize instance pointer
        nFSMServer::instance = this;

        // Run initialization script to add script commands interface to some FSM components
        nString initScriptPath( rootFilePath + "/init.lua" );
        this->RunScript( initScriptPath.Get() );
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nFSMServer::~nFSMServer()
{
    nFSMServer::instance = 0;
}

//-----------------------------------------------------------------------------
/**
    Run a script, doing some success checking
*/
bool nFSMServer::RunScript( const char* filename ) const
{
    n_assert( filename );

    nScriptServer* scriptServer = (nScriptServer*) nKernelServer::Instance()->Lookup( "/sys/servers/script" );
    if ( !scriptServer )
    {
        n_error( "Script server not found" );
        return false;
    }
    else
    {
        nString result;
        if ( !scriptServer->RunScript(filename, result) )
        {
            n_error( "Failed running script '%s': %s", filename, result.Get() );
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Run a script command, doing some success checking
*/
bool nFSMServer::RunCommand( const char* cmd ) const
{
    n_assert( cmd );

    nScriptServer* scriptServer = (nScriptServer*) nKernelServer::Instance()->Lookup( "/sys/servers/script" );
    if ( !scriptServer )
    {
        n_error( "Script server not found" );
        return false;
    }
    else
    {
        nString result;
        if ( !scriptServer->Run(cmd, result) )
        {
            n_error( "Failed running command '%s': %s", cmd, result.Get() );
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Get the directory where the all the FSMs, conditions, etc. are stored to/loaded from
*/
const char* nFSMServer::GetStoragePath() const
{
    return this->rootFilePath.Get();
}

//-----------------------------------------------------------------------------
/**
    Set the directory where the all the FSMs, conditions, etc. are stored to/loaded from
*/
void nFSMServer::SetStoragePath( const char* path )
{
    n_assert(path);
    this->rootFilePath = path;
}

//-----------------------------------------------------------------------------
/**
    Load all objects from the given relative path to root
*/
bool nFSMServer::LoadObjects( const char* subpath, bool haveScript ) const
{
    nDirectory* dir = nFileServer2::Instance()->NewDirectoryObject();
    nString dirPath;

    // Load objects from the given path
    this->MakeFilePath( subpath, NULL, dirPath );
    if ( dir->Open( dirPath.Get() ) )
    {
        bool moreFiles( dir->SetToFirstEntry() );
        while ( moreFiles )
        {
            nString filePath( dir->GetEntryName() );
            nString name( filePath.ExtractFileName() );
            // Load only nebula files
            if ( name.MatchPattern("*.n2") )
            {
                name.StripExtension();
                if ( !this->GetObject( subpath, name.Get(), haveScript ) )
                {
                    return false;
                }
            }
            moreFiles = dir->SetToNextEntry();
        }
        dir->Close();
    }

    n_delete( dir );
    return true;
}

//-----------------------------------------------------------------------------
/**
    Load all the FSMs, conditions, etc.
*/
bool nFSMServer::LoadAll() const
{
    bool success( true );
    success &= this->LoadObjects( this->fsmsSubpath );
    return success;
}

//-----------------------------------------------------------------------------
/**
    Return a FSM, loading it if needed
*/
nFSM* nFSMServer::GetFSM( const char* fsmName ) const
{
    return static_cast<nFSM*>( this->GetObject( this->fsmsSubpath, fsmName ) );
}

//-----------------------------------------------------------------------------
/**
    Return an object, loading it if needed

    @param subpath Relative path where the object is in
    @param name Name of the object to get
*/
nRoot* nFSMServer::GetObject( const char* subpath, const char* name, bool hasScript ) const
{
    // There's no object for empty names
    if ( !name )
    {
        return NULL;
    }
    if ( name[0] == '\0' )
    {
        return NULL;
    }

    // Look for the object in the NOH
    nString objPath;
    this->MakeObjPath( subpath, name, objPath );
    nRoot* obj = nKernelServer::Instance()->Lookup( objPath.Get() );

    // If the object isn't in the NOH load it
    if ( !obj )
    {
        nString filePath;
        this->MakeFilePath( subpath, name, filePath );
        if ( !nFileServer2::Instance()->FileExists( filePath.Get() ) )
        {
            return NULL;
        }
        obj = nKernelServer::Instance()->LoadAs( filePath.Get(), objPath.Get() );
        n_assert( obj );

        // If an object has an associated script, run it to bind it to the object's commands
        if ( hasScript )
        {
            // Select the object so the script can know which object has to bind to
            nKernelServer::Instance()->PushCwd( obj );

            // Run the script, binding it to the object
            this->MakeFilePath( subpath, name, filePath, "lua" );
            if ( !this->RunScript( filePath.Get() ) )
            {
                // The object has no meaning without the script, so release it if loading the script fails
                obj->Release();
                return NULL;
            }

            // Set the previous selected object (critical if this script is being ran inside another script)
            nKernelServer::Instance()->PopCwd();
        }
    }

    return obj;
}

//-----------------------------------------------------------------------------
/**
    Return an object, creating it if needed

    @param subpath Relative path where the object is in
    @param name Name of the object to get (matches the class name)
*/
nRoot*
nFSMServer::GetObject2( const char* subpath, const char* name ) const
{
    // Look for the object in the NOH
    nString objPath;
    this->MakeObjPath( subpath, name, objPath );
    nRoot* obj = nKernelServer::Instance()->Lookup( objPath.Get() );

    // If the object isn't in the NOH, create it
    if ( !obj )
    {
        obj = nKernelServer::Instance()->New( name, objPath.Get() );
        n_assert( obj );
    }

    return obj;
}

//-----------------------------------------------------------------------------
/**
    Return an event condition, loading it if needed
*/
nEventCondition* nFSMServer::GetEventCondition( const char* conditionName ) const
{
    n_assert( conditionName )
    nEventCondition* cond = static_cast<nEventCondition*>( this->GetObject( this->eventCondsSubpath, conditionName ) );

    // Event conditions without a filter condition are shared and not persisted, but generated here instead
    if ( !cond )
    {
        // Generate a new name (the event type unmutable id) for an old name
        // (needed for backwards compatibility when using names with the format event<transient id>)
        nString name( conditionName );
        if ( name.Length() > 5 )
        {
            name.StripLastNumbers();
            if ( name == "event" )
            {
                nGameEvent::Type eventTransientId = nGameEvent::Type( nString(&conditionName[5]).AsInt() );
                name = nGameEvent::GetEventPersistentId( eventTransientId );
            }
        }

        // Create the event condition
        nString path;
        this->MakeObjPath( this->eventCondsSubpath, name.Get(), path );
        cond = static_cast<nEventCondition*>( nKernelServer::Instance()->New( "neventcondition", path.Get() ) );

        // Set event type
        cond->SetEventByPersistentId( name.Get() );
    }

    return cond;
}

//-----------------------------------------------------------------------------
/**
    Return a script condition, loading it if needed
*/
nScriptCondition* nFSMServer::GetScriptCondition( const char* conditionName ) const
{
    return static_cast<nScriptCondition*>( this->GetObject2( this->scriptCondsSubpath, conditionName ) );
}

//-----------------------------------------------------------------------------
/**
    Return a FSM selector, loading it if needed
*/
nFSMSelector* nFSMServer::GetFSMSelector( const char* selectorName ) const
{
    return static_cast<nFSMSelector*>( this->GetObject2( this->fsmSelectorsSubpath, selectorName ) );
}

//-----------------------------------------------------------------------------
/**
    Save an object into the given relative path to root
*/
bool nFSMServer::SaveObject( nRoot* obj, const char* subpath ) const
{
    n_assert(obj);

    nString filePath;
    this->MakeFilePath( subpath, obj->GetName(), filePath );
    nFileServer2::Instance()->MakePath( filePath.ExtractDirName().Get() );
    return obj->SaveAs( filePath.Get() );
}

//-----------------------------------------------------------------------------
/**
    Save a FSM
*/
bool nFSMServer::SaveFSM( nFSM* fsm ) const
{
    n_assert(fsm);
    return this->SaveObject( fsm, this->fsmsSubpath );
}

//-----------------------------------------------------------------------------
/**
    Erase a file relative to root
*/
void nFSMServer::EraseFile( const char* subpath, const char* name, const char* extension ) const
{
    nString filePath;
    this->MakeFilePath( subpath, name, filePath, extension );
    nFileServer2::Instance()->DeleteFile( filePath.Get() );
}

//-----------------------------------------------------------------------------
/**
    Erase a FSM from disk
*/
void nFSMServer::EraseFSM( nFSM* fsm ) const
{
    n_assert(fsm);
    this->EraseFile( this->fsmsSubpath, fsm->GetName(), "n2" );
}

//-----------------------------------------------------------------------------
/**
    Build a NOH path from subpath and name (path = obj_root/subpath/name)
*/
void nFSMServer::MakeObjPath( const char* subpath, const char* name, nString& resultingPath ) const
{
    n_assert( this->rootObjPath );
    n_assert( subpath );
    n_assert( name );

    resultingPath = this->rootObjPath;
    resultingPath.StripTrailingSlash();
    resultingPath += "/";
    resultingPath += subpath;
    resultingPath.StripTrailingSlash();
    resultingPath += "/";
    resultingPath += name;
}

//-----------------------------------------------------------------------------
/**
    Build a file path from subpath and name (path = file_root/subpath/name.extension)
*/
void nFSMServer::MakeFilePath( const char* subpath, const char* name, nString& resultingPath, const char* extension ) const
{
    n_assert( this->rootFilePath.Get() );
    n_assert( subpath );

    resultingPath = this->rootFilePath.Get();
    resultingPath.StripTrailingSlash();
    resultingPath += "/";
    resultingPath += subpath;
    if ( name )
    {
        resultingPath.StripTrailingSlash();
        resultingPath += "/";
        resultingPath += name;
        if ( extension )
        {
            resultingPath += ".";
            resultingPath += extension;
        }
        else
        {
            resultingPath += ".n2";
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Return a state, creating an empty one if needed
*/
nState* nFSMServer::GetState( const char* fsmName, const char* stateName, int stateType ) const
{
    // Get the fsm that contains the state
    nFSM* fsm = this->GetFSM( fsmName );
    n_assert( fsm );
    
    // Look for the the state in the NOH
    nString statePath( fsm->GetFullName() );
    statePath.StripTrailingSlash();
    statePath += "/";
    statePath += stateName;
    nState* state = static_cast<nState*>( nKernelServer::Instance()->Lookup( statePath.Get() ) );

    // If the state isn't in the NOH create it
    if ( !state )
    {
        nString type;
        switch ( stateType )
        {
            case nState::Node:
                type = "nnodestate";
                break;
            case nState::Leaf:
                type = "nleafstate";
                break;
            case nState::End:
                type = "nendstate";
                break;
            default:
                n_error( "It's just impossible to reach this line?!?! Maybe has a new state type been added?" );
                return NULL;
        }
        state = static_cast<nState*>( nKernelServer::Instance()->New( type.Get(), statePath.Get() ) );
        n_assert( state );
        n_assert( state->GetStateType() == stateType );
    }

    return state;
}

//-----------------------------------------------------------------------------
/**
    Return the FSM which a state belongs to
*/
nFSM* nFSMServer::FindFSMOfState( nState* state ) const
{
    nRoot* obj = state->GetParent();
    n_assert( obj->IsA("nfsm") );
    return static_cast<nFSM*>( obj );
}

//-----------------------------------------------------------------------------
/**
    Return the state which a transition belongs to
*/
nState* nFSMServer::FindStateOfTransition( nTransition* transition ) const
{
    nRoot* obj = transition->GetParent();
    n_assert( obj->IsA("nroot") );
    obj = obj->GetParent();
    n_assert( obj->IsA("nstate") );
    return static_cast<nState*>( obj );
}

//-----------------------------------------------------------------------------
/**
    Return a transition of a state
*/
nTransition* nFSMServer::FindTransition( nState* state, const char* transitionName ) const
{
    n_assert( state );
    n_assert( transitionName );

    // Create the behavioural action full path
    nString transitionPath( state->GetFullName() );
    transitionPath.StripTrailingSlash();
    transitionPath += "/";
    transitionPath += this->transitionsSubpath;
    transitionPath.StripTrailingSlash();
    transitionPath += "/";
    transitionPath += transitionName;

    // Return the behavioural action (well, its description that is)
    nTransition* transition = static_cast<nTransition*>( nKernelServer::Instance()->Lookup( transitionPath.Get() ) );
    n_assert( transition );
    n_assert( transition->IsA("ntransition") );
    return transition;
}

//-----------------------------------------------------------------------------
/**
    Return the local condition of a transition
*/
nCondition* nFSMServer::FindLocalCondition( nTransition* transition, const char* conditionName ) const
{
    n_assert( transition );
    n_assert( conditionName );

    // Create the event condition full path
    nString conditionPath( transition->GetFullName() );
    conditionPath.StripTrailingSlash();
    conditionPath += "/";
    conditionPath += this->localCondsSubpath;
    conditionPath.StripTrailingSlash();
    conditionPath += "/";
    conditionPath += conditionName;

    // Return the condition
    nRoot* condition = nKernelServer::Instance()->Lookup( conditionPath.Get() );
    n_assert( condition );
    n_assert( condition->IsA("neventcondition") || condition->IsA("nscriptcondition") );
    return static_cast<nCondition*>( condition );
}

//-----------------------------------------------------------------------------
/**
    Return the behavioural action of a state
*/
nActionDesc* nFSMServer::FindBehaviouralAction( nState* state, const char* actionName ) const
{
    n_assert( state );
    n_assert( actionName );

    // Create the behavioural action full path
    nString actionPath( state->GetFullName() );
    actionPath.StripTrailingSlash();
    actionPath += "/";
    actionPath += this->behactionsSubpath;
    actionPath.StripTrailingSlash();
    actionPath += "/";
    actionPath += actionName;

    // Return the behavioural action (well, its description that is)
    nRoot* action = nKernelServer::Instance()->Lookup( actionPath.Get() );
    n_assert( action );
    n_assert( action->IsA("nactiondesc") );
    return static_cast<nActionDesc*>( action );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Set if events are enqueued or processed instantaneously
*/
void
nFSMServer::SetEnqueueEvents( bool enable )
{
    this->enqueueEvents = enable;
}

//-----------------------------------------------------------------------------
/**
    Tell if events are enqueued or processed instantaneously
*/
bool
nFSMServer::GetEnqueueEvents() const
{
    return this->enqueueEvents;
}
#endif // !NGAME
