//-----------------------------------------------------------------------------
//  nphygeombox_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeombox.h"
#include "kernel/npersistserver.h"


static void n_setlengths(void* slf, nCmd* cmd);
static void n_getlengths(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeombox

    @cppclass nPhyGeomBox

    @superclass
    nPhysicsGeom

    @classinfo
    Abstract representation of a box geometry.
*/
void
n_initcmds_nPhyGeomBox(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setlengths_fff",                       'DSLE', n_setlengths);
    cl->AddCmd("fff_getlengths_v",                       'DGLE', n_getlengths);
     
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setlengths
    @input
    fff
    @output
    v
    @info
    Sets the lengths of the box.
*/
static void
n_setlengths(void* slf, nCmd* cmd)
{
    nPhyGeomBox* self( static_cast<nPhyGeomBox*>(slf));

    vector3 lengths;

    lengths.x = cmd->In()->GetF();
    lengths.y = cmd->In()->GetF();
    lengths.z = cmd->In()->GetF();
    
    self->SetLengths(lengths);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getlengths
    @input
    v
    @output
    fff
    @info
    Gets the lengths of the box.
*/
static void
n_getlengths(void* slf, nCmd* cmd)
{
    nPhyGeomBox* self( static_cast<nPhyGeomBox*>(slf));

    vector3 lengths;

    self->GetLengths(lengths);
 
    cmd->Out()->SetF(lengths.x);
    cmd->Out()->SetF(lengths.y);
    cmd->Out()->SetF(lengths.z);
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomBox::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DSLE'));

    n_assert2( cmd, "Error command not found" );

    vector3 lengths;
    
    this->GetLengths( lengths );

    cmd->In()->SetF( lengths.x );
    cmd->In()->SetF( lengths.y );
    cmd->In()->SetF( lengths.z );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
