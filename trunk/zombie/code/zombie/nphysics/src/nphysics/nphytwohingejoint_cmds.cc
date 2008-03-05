//-----------------------------------------------------------------------------
//  nphytwogingejoint_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphytwogingejoint

    @cppclass nPhyTwoHingeJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Ancestor to all the physics geometries
*/
void
n_initcmds_nPhyTwoHingeJoint(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
     
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------