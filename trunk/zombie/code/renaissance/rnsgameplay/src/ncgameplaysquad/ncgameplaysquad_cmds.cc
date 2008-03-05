#include "precompiled/pchgameplay.h"

/**
    @file ncgameplaysquad_cmds.cc
*/

#include "ncgameplaysquad/ncgameplaysquad.h"

//------------------------------------------------------------------------------
/**
SaveCmds
*/
bool
ncGameplaySquad::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- formationState
        ps->Put (this->entityObject, 'ISFS', this->formationState);
    }

    return true;
}