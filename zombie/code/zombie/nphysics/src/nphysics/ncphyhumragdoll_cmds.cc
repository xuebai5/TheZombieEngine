//-----------------------------------------------------------------------------
//  ncphyhumragdoll_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyhumragdoll.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyHumRagDoll)
        NSCRIPT_ADDCMD_COMPOBJECT('DHED', void, SetHead, 1, (ncPhyRagDollLimb*), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DARM', void, SetArm, 2, (const ncPhyHumRagDoll::which,ncPhyRagDollLimb*), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DFRM', void, SetForeArm, 2, (const ncPhyHumRagDoll::which,ncPhyRagDollLimb*), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DLEG', void, SetLeg, 2, (const ncPhyHumRagDoll::which,ncPhyRagDollLimb*), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DFLG', void, SetForeLeg, 2, (const ncPhyHumRagDoll::which,ncPhyRagDollLimb*), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DNCK', void, SetNeckPoint, 1, (const vector3&), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DSLD', void, SetShoulderPoint, 2, (const ncPhyHumRagDoll::which,const vector3&), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DELW', void, SetElbowPoint, 2, (const ncPhyHumRagDoll::which,const vector3&), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DWRT', void, SetWristPoint, 2, (const ncPhyHumRagDoll::which,const vector3&), 0, ());
        NSCRIPT_ADDCMD_COMPOBJECT('DKNE', void, SetKneePoint, 2, (const ncPhyHumRagDoll::which,const vector3&), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
