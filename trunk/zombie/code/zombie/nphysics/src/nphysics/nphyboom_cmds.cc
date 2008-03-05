//-----------------------------------------------------------------------------
//  nphyboom_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyboom.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphyboom

    @cppclass nPhyBoom

    @superclass
    nroot

    @classinfo
    Represents a explosion.
*/
void
n_initcmds_nPhyBoom(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.

    cl->EndCmds();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
