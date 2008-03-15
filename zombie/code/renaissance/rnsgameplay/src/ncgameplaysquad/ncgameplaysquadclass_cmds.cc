#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplaysquadclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ncgameplaysquad/ncgameplaysquadclass.h"

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplaySquadClass::SaveCmds (nPersistServer* ps)
{
    if ( nComponentClass::SaveCmds(ps) )
    {
        // -- maxMembers
        ps->Put (this->entityClass, 'ISXM', this->maxMembers);
    }

    return true;
}