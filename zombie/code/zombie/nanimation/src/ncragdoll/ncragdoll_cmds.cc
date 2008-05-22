#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdoll_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncragdoll/ncragdoll.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncRagDoll,ncSkeleton);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncRagDoll)
    NSCRIPT_ADDCMD_COMPOBJECT('DSOR', void, SwitchOff, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AABP', void, GetJointData, 4, (int, vector3&, quaternion&, vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AACG', int, GetNumRagJoints, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Save state of the component

    history:
        - 08-Nov-2005   Zombie         created
*/
bool ncRagDoll::SaveCmds(nPersistServer* server)
{
    return ncSkeleton::SaveCmds(server);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
