//-----------------------------------------------------------------------------
//  nphygeomcone_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomcone.h"
#include "kernel/npersistserver.h"


static void n_setlength(void* slf, nCmd* cmd);
static void n_getlength(void* slf, nCmd* cmd);
static void n_setradius(void* slf, nCmd* cmd);
static void n_getradius(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomcone

    @cppclass nPhyGeomCone

    @superclass
    nPhysicsGeom

    @classinfo
    Abstract representation of a cone geometry.
*/
void
n_initcmds_nPhyGeomCone(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setlength_f",                       'SLEC', n_setlength);
    cl->AddCmd("f_getlengths_v",                      'DGLE', n_getlength);
    cl->AddCmd("v_setradius_f",                       'DRAC', n_setradius);
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
    Sets the length of the cone.
*/
static void
n_setlength(void* slf, nCmd* cmd)
{
    nPhyGeomCone* self( static_cast<nPhyGeomCone*>(slf));

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
    Gets the length of the cone.
*/
static void
n_getlength(void* slf, nCmd* cmd)
{
    nPhyGeomCone* self( static_cast<nPhyGeomCone*>(slf));

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
    Sets the radius of the cone.
*/
static void
n_setradius(void* slf, nCmd* cmd)
{
    nPhyGeomCone* self( static_cast<nPhyGeomCone*>(slf));

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
    Gets the radius of the cone.
*/
static void
n_getradius(void* slf, nCmd* cmd)
{
    nPhyGeomCone* self( static_cast<nPhyGeomCone*>(slf));

    phyreal radius(self->GetRadius());
 
    cmd->Out()->SetF(radius);
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomCone::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DRAC'));

    n_assert2( cmd, "Error command not found" );

    /// Setting the cone radius
    cmd->In()->SetF( this->GetRadius() );

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DLEC');
    
    /// Setting the cone length
    cmd->In()->SetF( this->GetLength() );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
