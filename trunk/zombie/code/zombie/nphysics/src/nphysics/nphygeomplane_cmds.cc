//-----------------------------------------------------------------------------
//  nphygeomplane_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomplane.h"
#include "kernel/npersistserver.h"

static void n_setequation(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomplane

    @cppclass nPhyGeomPlane

    @superclass
    nPhysicsGeom

    @classinfo
    Abstract representation of a plane geometry.
*/
void
n_initcmds_nPhyGeomPlane(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setequation_ffff",                      'DTEQ', n_setequation);
     
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setequation
    @input
    ffff
    @output
    v
    @info
    Sets the equation of a plane.
*/
static void
n_setequation(void* slf, nCmd* cmd)
{
    nPhyGeomPlane* self( static_cast<nPhyGeomPlane*>(slf));

    vector4 equation;

    equation.x = cmd->In()->GetF();
    equation.y = cmd->In()->GetF();
    equation.z = cmd->In()->GetF();
    equation.w = cmd->In()->GetF();
    
    self->SetEquation(equation);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
