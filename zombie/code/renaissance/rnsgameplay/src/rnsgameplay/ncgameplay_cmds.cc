#include "precompiled/pchrnsgameplay.h"
/**
   @file ncgameplay_cmds.cc
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/
//-----------------------------------------------------------------------------
#include "rnsgameplay/ncgameplay.h"
#include "entity/nentityobject.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplay)
    NSCRIPT_ADDCMD_COMPOBJECT('ISNM', void, SetName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGNM', const nString&, GetName, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISGP', void, SetGroup, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGGP', const nString&, GetGroup, 0, (), 0, ());
	/*NSCRIPT_ADDCMD_COMPOBJECT('IABA', void, AddBasicAction, 4, (const char*, int, nArg*, bool), 0, ());*/
	NSCRIPT_ADDCMD_COMPOBJECT('IPBA', void, PopBasicAction, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EBEG', void, BeginAction, 1, (const char*), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EAIA', void, AddIntArg, 1, (int), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EAFA', void, AddFloatArg, 1, (float), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EASA', void, AddStringArg, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EABA', void, AddBoolArg, 1, (bool), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('EEND', void, EndAction, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplay::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // --------------------------------------- General purpose --
        // -- name        
        ps->Put (this->entityObject, 'ISNM', this->name.Get());
        // -- group name
        ps->Put (this->entityObject, 'ISGP', this->group.Get());
    }

    return true;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
