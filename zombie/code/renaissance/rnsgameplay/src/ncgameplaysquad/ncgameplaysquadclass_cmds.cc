#include "precompiled/pchgameplay.h"

/**
    @file ncgameplaysquadclass_cmds.cc
*/

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