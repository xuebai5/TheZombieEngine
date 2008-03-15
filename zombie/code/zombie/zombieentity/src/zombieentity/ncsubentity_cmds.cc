#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  ncsubentity_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "zombieentity/ncsubentity.h"
#include "ndebug/nceditor.h"
#include "entity/nobjectmagicinstancer.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSubentity)
    NSCRIPT_ADDCMD_COMPOBJECT('JSRP', void, SetRelativePosition, 1, (const vector3 &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGRP', const vector3 &, GetRelativePosition, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSRT', void, SetRelativeTransform, 1, (const quaternion &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGRT', const quaternion &, GetRelativeTransform, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSPI', void, SetPersistenceOID, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGPI', nEntityObjectId, GetPersistenceOID, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncSubEntity::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- relativePos
        ps->Put (this->entityObject, 'ISRP', this->relativePos.x, this->relativePos.y, this->relativePos.z);

    }

    return true;
}