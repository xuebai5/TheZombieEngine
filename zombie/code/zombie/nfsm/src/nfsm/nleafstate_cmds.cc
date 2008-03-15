//-----------------------------------------------------------------------------
//  nleafstate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nleafstate.h"
#include "nfsm/nactiondesc.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nLeafState )
    NSCRIPT_ADDCMD('SETB', void, SetBehaviouralAction, 1, (nActionDesc*), 0, ());
    NSCRIPT_ADDCMD('GETB', nActionDesc*, GetBehaviouralAction , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESBA', void, SetBehaviouralActionByName, 1, (const char*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nLeafState::SaveCmds( nPersistServer* ps )
{
    if ( !this->behaviouralAction )
    {
        return false;
    }
    ps->Put( this, 'ESBA', this->behaviouralAction->GetName() );

    return nState::SaveCmds(ps);
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
