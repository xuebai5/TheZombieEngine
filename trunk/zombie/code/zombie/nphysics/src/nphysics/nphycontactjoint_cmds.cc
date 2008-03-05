//-----------------------------------------------------------------------------
//  nphycontactjoint_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphycontactjoint.h"
#include "kernel/npersistserver.h"



//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphycontactjoint

    @cppclass nPhyContactJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Represents a contact joint.
*/
void
n_initcmds_nPhyContactJoint(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.

    cl->EndCmds();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
