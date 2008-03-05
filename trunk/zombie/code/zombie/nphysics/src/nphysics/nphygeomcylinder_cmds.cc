//-----------------------------------------------------------------------------
//  nphygeomcylinder_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomcylinder.h"
#include "kernel/npersistserver.h"


static void n_setlength(void* slf, nCmd* cmd);
static void n_getlength(void* slf, nCmd* cmd);
static void n_setradius(void* slf, nCmd* cmd);
static void n_getradius(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomcylinder

    @cppclass nPhyGeomCylinder

    @superclass
    nPhysicsGeom

    @classinfo
    Abstract representation of a cylinder geometry.
*/
void
n_initcmds_nPhyGeomCylinder(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setlength_f",                       'DLEN', n_setlength);
    cl->AddCmd("f_getlengths_v",                      'DGLE', n_getlength);
    cl->AddCmd("v_setradius_f",                       'DRAD', n_setradius);
    cl->AddCmd("f_getradius_v",                       'DGRA', n_getradius);
     
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setlength
    @input
    f
    @output
    v
    @info
    Sets the length of the cylinder.
*/
static void
n_setlength(void* slf, nCmd* cmd)
{
    nPhyGeomCylinder* self( static_cast<nPhyGeomCylinder*>(slf));

    phyreal length( cmd->In()->GetF() );
   
    self->SetLength(length);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getlength
    @input
    v
    @output
    f
    @info
    Gets the length of the cylinder.
*/
static void
n_getlength(void* slf, nCmd* cmd)
{
    nPhyGeomCylinder* self( static_cast<nPhyGeomCylinder*>(slf));

    phyreal length(self->GetLength());
 
    cmd->Out()->SetF(length);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setradius
    @input
    f
    @output
    v
    @info
    Sets the radius of the cylinder.
*/
static void
n_setradius(void* slf, nCmd* cmd)
{
    nPhyGeomCylinder* self( static_cast<nPhyGeomCylinder*>(slf));

    phyreal radius( cmd->In()->GetF() );
   
    self->SetRadius(radius);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getradius
    @input
    v
    @output
    f
    @info
    Gets the radius of the cylinder.
*/
static void
n_getradius(void* slf, nCmd* cmd)
{
    nPhyGeomCylinder* self( static_cast<nPhyGeomCylinder*>(slf));

    phyreal radius(self->GetRadius());
 
    cmd->Out()->SetF(radius);
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomCylinder::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DRAD'));

    n_assert2( cmd, "Error command not found" );

    /// Setting the cylinder radius
    cmd->In()->SetF( this->GetRadius() );

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DLEN');
    
    /// Setting the cylinder length
    cmd->In()->SetF( this->GetLength() );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
