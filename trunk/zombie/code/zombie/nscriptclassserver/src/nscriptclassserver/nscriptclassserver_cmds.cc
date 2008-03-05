//------------------------------------------------------------------------------
//  nscriptclassserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnscriptclassserver.h"

#include "nscriptclassserver/nscriptclassserver.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nScriptClassServer)
    NSCRIPT_ADDCMD('LHSC', bool, HasScript, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('LADC', void, AddClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('LALL', void, AddClasses , 0, (), 0, ());
    NSCRIPT_ADDCMD('LDLC', void, DeleteClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('LDLL', void, DeleteClasses , 0, (), 0, ());
    NSCRIPT_ADDCMD('LRFC', void, RefreshClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('LRLL', void, RefreshClasses , 0, (), 0, ());
NSCRIPT_INITCMDS_END()
