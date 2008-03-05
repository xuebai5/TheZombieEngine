//-----------------------------------------------------------------------------
//  nphysliderjoint_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysliderjoint.h"
#include "kernel/npersistserver.h"



//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphysliderjoint

    @cppclass nPhySliderJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Represents a slider joint.
*/
void
n_initcmds_nPhySliderJoint(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.

    cl->EndCmds();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
