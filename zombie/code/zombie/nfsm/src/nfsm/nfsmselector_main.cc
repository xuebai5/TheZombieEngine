//-----------------------------------------------------------------------------
//  nfsmselector_main.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nfsmselector.h"
#include "nfsm/nfsm.h"
#include "nfsmserver/nfsmserver.h"

nNebulaScriptClass(nFSMSelector, "nroot");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nFSMSelector::nFSMSelector()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Select and return a reference to a FSM for the given entity

    Call the selection script to select and return a reference to a FSM
*/
nFSM* nFSMSelector::Select( nEntityObject* entity )
{
    n_assert( entity );

    // Set command's input parameters
    nArg inArgs[1];
    inArgs[0].SetO( entity );

    // Execute command
    nArg outArgs[1];
    this->CallArgs( "SelectFSM", 1, outArgs, 1, inArgs );

#ifndef NGAME
    // Validate command's result type
    if ( outArgs[0].GetType() != nArg::String )
    {
        nString error( "carles.ros: Command 'SelectFSM' in the FSM selector script '" );
        error += this->GetName();
        error += "' must return a FSM name";
        n_message( error.Get() );
        return NULL;
    }
#endif

    // Get reference to FSM
    const char* fsmName = outArgs[0].GetS();
    nFSM* selectedFSM = nFSMServer::Instance()->GetFSM( fsmName );

#ifndef NGAME
    // Validate FSM name
    if ( !selectedFSM )
    {
        nString error( "carles.ros: FSM '" );
        error += fsmName;
        error += "' returned by the FSM selection script '";
        error += this->GetName();
        error += "' not found";
        n_message( error.Get() );
        return NULL;
    }
#endif

    // Return reference to FSM
    return selectedFSM;
}

//-----------------------------------------------------------------------------
