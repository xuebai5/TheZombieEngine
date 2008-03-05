#include "precompiled/pchrnsgameplay.h"

#include "ncpopulatorarea/ncpopulatorarea.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPopulatorArea)
	NSCRIPT_ADDCMD_COMPOBJECT('IINA', bool, IsInside, 1, (const vector3&), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISSP', void, SetSpawner, 1, (nEntityObject*), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IGSP', nEntityObject*, GetSpawner, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISMI', void, SetMaxInhabitants, 1, (int), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IGMI', int, GetMaxInhabitants, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IASI', bool, AddSpawnItem, 2, (const nString&, int), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ICSI', bool, ChangeSpawnItem, 2, (const nString&, int), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IRSI', bool, RemoveSpawnItem, 1, (const nString&), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IGNS', int, GetNumberIndividuals, 1, (const nString&), 0, ());
	/*NSCRIPT_ADDCMD_COMPOBJECT('IGBC', void, GetBreeds, 1, (nArray<nSpawnItem*>&), 0, ());*/
NSCRIPT_INITCMDS_END()

