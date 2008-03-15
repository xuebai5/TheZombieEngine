#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplaysquad_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

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