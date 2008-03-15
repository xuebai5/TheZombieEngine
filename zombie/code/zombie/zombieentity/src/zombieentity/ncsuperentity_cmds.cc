#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  ncsuperentity_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "zombieentity/ncsuperentity.h"
#include "ndebug/nceditor.h"
#include "entity/nobjectmagicinstancer.h"

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncSuperEntity::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- relativePos
        ps->Put (this->entityObject, 'ISRP', this->relativePos.x, this->relativePos.y, this->relativePos.z);

    }

    return true;
}