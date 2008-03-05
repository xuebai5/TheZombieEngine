//-----------------------------------------------------------------------------
//  nphyballsocketjoint_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyballsocketjoint.h"
#include "kernel/npersistserver.h"


//-----------------------------------------------------------------------------

static void n_setanchor(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphyballsocketjoint

    @cppclass nPhyBallSocketJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Represents a ball and socket joint.
*/
void
n_initcmds_nPhyBallSocketJoint(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setanchor_fff",                   'DSAN', n_setanchor);

    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setanchor
    @input
    fff
    @output
    v
    @info
    Sets the anchor point of this joint
*/
static void
n_setanchor(void* slf, nCmd* cmd)
{
    nPhyBallSocketJoint* self( static_cast<nPhyBallSocketJoint*>(slf));

    
    vector3 anchor;

    anchor.x = cmd->In()->GetF();
    anchor.y = cmd->In()->GetF();
    anchor.z = cmd->In()->GetF();    

    self->SetAnchor( anchor );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
