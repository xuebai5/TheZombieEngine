//------------------------------------------------------------------------------
//  nentityclassserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"

#include "entity/nentityclassserver.h"

#include "entity/nentityclassserver.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nEntityClassServer)

    NSCRIPT_ADDCMD('SCSD', void, SetEntityClassDir, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GCSD', const char *, GetEntityClassDir, 0, (), 0, ());
    NSCRIPT_ADDCMD('SECL', bool, SaveEntityClasses, 0, (), 0, ());
    NSCRIPT_ADDCMD('DDCL', bool, DiscardDeletedClasses, 0, (), 0, ());
    NSCRIPT_ADDCMD('ACTI', void, AddClassToIndex, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GEEC', nEntityClass *, GetEntityClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('REEC', bool, RemoveEntityClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('NEEC', nEntityClass *, NewEntityClass, 2, (nEntityClass *, const char *), 0, ());
    NSCRIPT_ADDCMD('SCDI', void, SetEntityClassDirty, 2, (nEntityClass *, bool), 0, ());
    NSCRIPT_ADDCMD('EGCD', bool, GetEntityClassDirty, 1, (nEntityClass *), 0, ());
    NSCRIPT_ADDCMD('CSDA', void, SetDirtyAllClasses , 0, (), 0, ());
    NSCRIPT_ADDCMD('NECL', bool, NewClass, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('RECL', bool, RemoveClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GEDI', bool, GetDirty, 0, (), 0, ());
    NSCRIPT_ADDCMD('LAEC', bool, LoadAllEntityClasses, 0, (), 0, ());
    NSCRIPT_ADDCMD('FCCN', bool, CheckClassName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('SCCK', void, SetClassKey, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SLCK', void, SetLocalClassKey , 0, (), 0, ());
    NSCRIPT_ADDCMD('LESU', bool, LoadEntitySubClasses, 1, (nEntityClass *), 0, ());
//  NSCRIPT_ADDCMD('SAEC', bool, SaveEntityClass, 1, (nEntityClass *), 0, ());
//  NSCRIPT_ADDCMD('LOEC', nEntityClass *, LoadEntityClass, 1, (const char *), 0, ());
//  NSCRIPT_ADDCMD('UNEC', bool, UnloadEntityClass, 1, (nEntityClass *), 0, ());
//  NSCRIPT_ADDCMD('FIEC', nEntityClass *, FindEntityClass, 1, (const char *), 0, ());
//  NSCRIPT_ADDCMD('UAEC', bool, UnloadAllEntityClasses, 0, (), 0, ());
//  NSCRIPT_ADDCMD('UAES', bool, UnloadEntitySubClasses, 1, (nEntityClass *), 0, ());

NSCRIPT_INITCMDS_END()

