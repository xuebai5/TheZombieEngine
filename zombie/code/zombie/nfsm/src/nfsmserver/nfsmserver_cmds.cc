//-----------------------------------------------------------------------------
//  nfsmserver_cmds.cc
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

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nFSMServer )
    NSCRIPT_ADDCMD('EGSP', const char*, GetStoragePath , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESSP', void, SetStoragePath, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ELAL', bool, LoadAll , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGSM', nFSM*, GetFSM, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGEC', nEventCondition*, GetEventCondition, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGSC', nScriptCondition*, GetScriptCondition, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGFS', nFSMSelector*, GetFSMSelector, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ESSM', bool, SaveFSM, 1, (nFSM*), 0, ());
    NSCRIPT_ADDCMD('EESM', void, EraseFSM, 1, (nFSM*), 0, ());
    NSCRIPT_ADDCMD('EGST', nState*, GetState, 3, (const char*, const char*, int), 0, ());
    NSCRIPT_ADDCMD('EFOS', nFSM*, FindFSMOfState, 1, (nState*), 0, ());
    NSCRIPT_ADDCMD('ESOT', nState*, FindStateOfTransition, 1, (nTransition*), 0, ());
    NSCRIPT_ADDCMD('EFTR', nTransition*, FindTransition, 2, (nState*, const char*), 0, ());
    NSCRIPT_ADDCMD('EFLC', nCondition*, FindLocalCondition, 2, (nTransition*, const char*), 0, ());
    NSCRIPT_ADDCMD('EFBA', nActionDesc*, FindBehaviouralAction, 2, (nState*, const char*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('ESEE', void, SetEnqueueEvents, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGEE', bool, GetEnqueueEvents, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
