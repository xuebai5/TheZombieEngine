//------------------------------------------------------------------------------
//  nentityobjectserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "precompiled/pchnentity.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nEntityObjectServer)

    NSCRIPT_ADDCMD('SECD', void, SetConfigDir, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GECD', const char *, GetConfigDir, 0, (), 0, ());
    NSCRIPT_ADDCMD('SCFG', bool, SaveConfig, 0, (), 0, ());
    NSCRIPT_ADDCMD('SOID', void, SetBaseEntityObjectId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('SBID', void, SetBaseEntityObjectBunchId, 1, (int), 0, ());
//  NSCRIPT_ADDCMD('GEDI', bool, GetDirty, 0, (), 0, ());
    NSCRIPT_ADDCMD('SEOD', void, SetEntityObjectDir, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GEOD', const char *, GetEntityObjectDir, 0, (), 0, ());
    NSCRIPT_ADDCMD('SEOS', void, SaveEntityObjects, 0, (), 0, ());
    NSCRIPT_ADDCMD('DDOB', void, DiscardDeletedObjects, 0, (), 0, ());
    NSCRIPT_ADDCMD('REEI', bool, RemoveEntityObjectById, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('REEO', bool, RemoveEntityObject, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD('UREO', bool, UnremoveEntityObjectById, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('CBUR', bool, CanBeUnremoved, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('GEEO', nEntityObject *, GetEntityObject, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('NEEO', nEntityObject *, NewEntityObject, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('NLEO', nEntityObject *, NewLocalEntityObject, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('NSEO', nEntityObject *, NewServerEntityObject, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('NSIE', nEntityObject *, NewServerEntityObjectWithId, 2, (const char *, nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('SODI', void, SetEntityObjectDirty, 2, (nEntityObject *, bool), 0, ());
    NSCRIPT_ADDCMD('UEOC', void, UnloadEntityObjectsFromClass, 1, (nEntityClass *), 0, ());
    NSCRIPT_ADDCMD('UAEO', void, UnloadAllEntityObjects, 0, (), 0, ());
    NSCRIPT_ADDCMD('UNEO', void, UnloadNormalEntityObjects, 0, (), 0, ());
    NSCRIPT_ADDCMD('GHID', nEntityObjectId, GetHighId , 0, (), 0, ());
    NSCRIPT_ADDCMD('BVEO', bool, IsValidEntityObjectId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('FGFE', nEntityObject* , GetFirstEntityObject, 0, (), 0, ());
    NSCRIPT_ADDCMD('FGNE', nEntityObject* , GetNextEntityObject, 0, (), 0, ());
    NSCRIPT_ADDCMD('FGOD', bool, GetEntityObjectDirty, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('CSDV', void, SetDataVersion, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CGDV', int, GetDataVersion, 0, (), 0, ());
    NSCRIPT_ADDCMD('CGHV', bool, ChangeDataVersion, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CSDA', void, SetDirtyAllObjects , 0, (), 0, ());
    NSCRIPT_ADDCMD('JCEC', bool, ChangeEntityClass, 2, (nEntityObject*, const char *), 0, ());
//  NSCRIPT_ADDCMD('LCFG', bool, LoadConfig, 0, (), 0, ());
//  NSCRIPT_ADDCMD('LOEO', nEntityObject *, LoadEntityObject, 1, (nEntityObjectId), 0, ());
//  NSCRIPT_ADDCMD('SAEO', bool, SaveEntityObject, 1, (nEntityObject *), 0, ());
//  NSCRIPT_ADDCMD('UNEO', void, UnloadEntityObject, 1, (nEntityObject *), 0, ());
//  NSCRIPT_ADDCMD('FIEO', nEntityObject *, FindEntityObject, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('SEOI', void, SwapEntityObjectIds, 2, (nEntityObject *, nEntityObject *), 0, ());
    NSCRIPT_ADDCMD('GEOT', nEntityObjectServer::nEntityObjectType, GetEntityObjectType, 1, (nEntityObjectId), 0, ());

    cl->BeginSignals( 1 );
    N_INITCMDS_ADDSIGNAL( EntityDeleted );
    cl->EndSignals();

NSCRIPT_INITCMDS_END()
