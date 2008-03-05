//-----------------------------------------------------------------------------
//  nworldinterface_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchframework.h"
#include "nworldinterface/nworldinterface.h"
#include "entity/nentity.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nWorldInterface )

    NSCRIPT_ADDCMD('EGLE', nEntityObject*, GetLevelEntity, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGGE', nEntityObject*, GetGameEntity, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ENEW', nEntityObject*, NewEntity, 2, (const char*, vector3), 0, ());
    NSCRIPT_ADDCMD('ENLE', nEntityObject*, NewLocalEntity, 4, (const char*, vector3, bool, nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('ENSE', nEntityObject*, NewServerEntity, 3, (const char*, vector3, nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('EDEL', void, DeleteEntityById, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('ADTW', void, AddToWorld, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD('REFW', void, RemoveFromWorld, 1, (nEntityObject *), 0, ());

    cl->BeginSignals(1);
    N_INITCMDS_ADDSIGNAL( EntityCreated )
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
