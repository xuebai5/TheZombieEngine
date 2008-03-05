//-----------------------------------------------------------------------------
//  nphyuniversaljoint_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyuniversaljoint.h"
#include "kernel/npersistserver.h"



//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphyuniversaljoint

    @cppclass nPhyUniversalJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Represents a universal joint.
*/
void
n_initcmds_nPhyUniversalJoint(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.

    cl->EndCmds();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
