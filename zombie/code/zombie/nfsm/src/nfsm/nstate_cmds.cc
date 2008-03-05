//-----------------------------------------------------------------------------
//  nstate_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nstate.h"
#include "nfsm/ntransition.h"
#include "nfsm/nactiondesc.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nState)
    NSCRIPT_ADDCMD('ADDT', void, AddTransition, 1, (nTransition*), 0, ());
    NSCRIPT_ADDCMD('REMT', void, RemoveTransition, 1, (nTransition*), 0, ());
    NSCRIPT_ADDCMD('DELT', void, DeleteTransition, 1, (nTransition*), 0, ());
    NSCRIPT_ADDCMD('ADDE', void, AddEmotionalAction, 1, (nActionDesc*), 0, ());
    NSCRIPT_ADDCMD('REME', void, RemoveEmotionalAction, 1, (nActionDesc*), 0, ());
    NSCRIPT_ADDCMD('DELE', void, DeleteEmotionalAction, 1, (nActionDesc*), 0, ());
    NSCRIPT_ADDCMD('EATR', void, AddTransitionByName, 1, (const char*), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nState::SaveCmds( nPersistServer* ps )
{
    // Event transitions
    for ( int i(0); i < this->eventTransitions.Size(); ++i )
    {
        ps->Put( this, 'EATR', this->eventTransitions.GetElementAt(i)->GetName() );
    }

    // Script transitions
    for ( int i(0); i < this->scriptTransitions.Size(); ++i )
    {
        ps->Put( this, 'EATR', this->scriptTransitions[i]->GetName() );
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
