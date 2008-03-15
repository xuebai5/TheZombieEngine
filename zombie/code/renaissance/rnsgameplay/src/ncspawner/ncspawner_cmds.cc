#include "precompiled/pchrnsgameplay.h"
#include "ncspawner/ncspawner.h"
//------------------------------------------------------------------------------
//  ncspawner_cmds.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------

NSCRIPT_INITCMDS_BEGIN(ncSpawner)
	NSCRIPT_ADDCMD_COMPOBJECT('IASS', void, AddSpawner, 2, (nEntityObject*, float), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISPB', void, SetProbability, 2, (nEntityObject*, float), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IRSP', void, RemoveSpawner, 1, (nEntityObject*), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ICHS', nEntityObject*, ChooseSpawner, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IESP', bool, ExistsSpawner, 1, (nEntityObject*), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISEN', nEntityObject*, SpawnEntity, 1, (const nString&), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ESEN', nEntityObject*, SpawnEntityFrozen, 1, (const nString&), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EGSN', int, GetSpawnersNumber, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EGSI', nEntityObject*, GetSpawnerByIndex, 1, (int), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EGSP', float, GetSpawnerProbability, 1, (nEntityObject*), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EASP', void, AddSpawnerById, 2, (nEntityObjectId, float), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncSpawner::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // Spawners
        for ( int i(0); i < this->listSpawners.Size(); ++i )
        {
            ps->Put (this->entityObject, 'EASP', this->listSpawners[i]->spawnerId, this->listSpawners[i]->probability);
        }
    }

    return true;
}
