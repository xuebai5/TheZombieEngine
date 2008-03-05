//-----------------------------------------------------------------------------
//  nphyhingejoint_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyhingejoint.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    nphyhingejoint

    @cppclass
    nPhyHingeJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Represents a hinge joint.
*/
NSCRIPT_INITCMDS_BEGIN(nPhyHingeJoint)
    NSCRIPT_ADDCMD('DACN', void, SetAnchor, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('DAXS', void, SetAxis, 1, (const vector3&), 0, ());
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
