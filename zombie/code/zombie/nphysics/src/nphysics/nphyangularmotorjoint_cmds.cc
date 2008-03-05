//-----------------------------------------------------------------------------
//  nphyangularmotorjoint_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyangularmotorjoint.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------

static void n_setaxis(void* slf, nCmd* cmd);
static void n_setnumaxis(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphyangularmotorjoint

    @cppclass nPhyAngularMotorJoint

    @superclass
    nPhysicsJoint

    @classinfo
    Represents a angular motor joint.
*/
void
n_initcmds_nPhyAngularMotorJoint(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setaxis_ifff",                   'DTAX', n_setaxis);
    cl->AddCmd("v_setnumaxis_i",                   'DMAX', n_setnumaxis);

    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setaxis
    @input
    oo
    @output
    v
    @info
    Attaches two objects
*/
static void
n_setaxis(void* slf, nCmd* cmd)
{
    nPhyAngularMotorJoint* self( static_cast<nPhyAngularMotorJoint*>(slf));

    phy::jointaxis which(phy::jointaxis(cmd->In()->GetI()));

    vector3 axis;

    axis.x = cmd->In()->GetF();
    axis.y = cmd->In()->GetF();
    axis.z = cmd->In()->GetF();

    switch( which )
    {
    case phy::axisA:
        self->SetFirstAxis( axis );
        break;
    case phy::axisB:
        self->SetSecondAxis( axis );
        break;
    case phy::axisC:
        self->SetThirdAxis( axis );
        break;
    default:
        n_assert2_always( "Bad axis" );
    }
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setnumaxos
    @input
    i
    @output
    v
    @info
    Attaches two objects
*/
static void
n_setnumaxis(void* slf, nCmd* cmd)
{
    nPhyAngularMotorJoint* self( static_cast<nPhyAngularMotorJoint*>(slf));

    self->SetNumberOfAxis( cmd->In()->GetI() );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

