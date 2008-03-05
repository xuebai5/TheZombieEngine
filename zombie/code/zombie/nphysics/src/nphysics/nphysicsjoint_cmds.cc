//-----------------------------------------------------------------------------
//  nphysicsjoint_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsjoint.h"
#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphysicsworld.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nphysicsjoint

    @cppclass
    nPhysicsJoint

    @superclass
    nObject

    @classinfo
    An abstract representation of a joint.
*/
NSCRIPT_INITCMDS_BEGIN(nPhysicsJoint)
    /*NSCRIPT_ADDCMD('DTTC', void, Attach, 2, (ncPhysicsObj*, ncPhysicsObj*), 0, ());*/
    NSCRIPT_ADDCMD('DTPR', void, SetParam, 3, (const phy::jointparameter, const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD('DRET', void, Create, 1, (nPhysicsWorld*), 0, ());
NSCRIPT_INITCMDS_END()

