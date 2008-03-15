//------------------------------------------------------------------------------
//  ncspawnpoint_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "ncspawnpoint/ncspawnpoint.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpawnPoint)
	NSCRIPT_ADDCMD_COMPOBJECT('ISWE', nEntityObject*, SpawnEntity, 1, (const nString&), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ESWE', nEntityObject*, SpawnEntityFrozen, 1, (const nString&), 0, ());
NSCRIPT_INITCMDS_END()

