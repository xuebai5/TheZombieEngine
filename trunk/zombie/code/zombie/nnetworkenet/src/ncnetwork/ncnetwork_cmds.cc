//------------------------------------------------------------------------------
//  ncnetwork_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "ncnetwork/ncnetwork.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNetwork)
    NSCRIPT_ADDCMD_COMPOBJECT('LMIM', bool, IsModified , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LMOD', void, SetModified , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LUPD', void, SetUpdated , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
