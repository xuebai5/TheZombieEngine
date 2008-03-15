#include "precompiled/pchrnsgameplay.h"
//-----------------------------------------------------------------------------
//  ncgameplayclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "rnsgameplay/ncgameplayclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayClass)
    NSCRIPT_ADDCMD_COMPCLASS('ISBN', void, SetBeautyName, 1, (nString), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGBN', nString, GetBeautyName, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplayClass::SaveCmds (nPersistServer* ps)
{
    if ( nComponentClass::SaveCmds(ps) )
    {
        // -- beautyName
        ps->Put (this->entityClass, 'ISBN', this->beautyName.Get() );
    }

    return true;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
