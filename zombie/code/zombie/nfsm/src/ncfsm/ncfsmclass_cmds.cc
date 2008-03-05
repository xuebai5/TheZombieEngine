//-----------------------------------------------------------------------------
//  ncfsmclass_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "ncfsm/ncfsmclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncFSMClass)
    NSCRIPT_ADDCMD_COMPCLASS('ESPF', void, SetParentFSM, 1, (nFSM*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGPF', nFSM*, GetParentFSM , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESTI', void, SetConditionPoolingFrequency, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGTI', int, GetConditionPoolingFrequency , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESFN', void, SetParentFSMByName, 1, (const char*), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
