#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncsceneindexed_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncsceneindexed.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ntransformnode.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneIndexed)
    NSCRIPT_ADDCMD_COMPOBJECT('SSSP', void, SetSubscenePath, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GSSP', const char *, GetSubscenePath, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

