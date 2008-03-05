//-----------------------------------------------------------------------------
//  nphygeomquadtreespace_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "kernel/npersistserver.h"

static void n_create(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomquadtreespace

    @cppclass nPhyGeomQuadTreeSpace

    @superclass
    nPhySpace

    @classinfo
    Abstract representation of a space geometry.
*/
void
n_initcmds_nPhyGeomQuadTreeSpace(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_create_ffffffi",                      'DREA', n_create);
     
    cl->EndCmds();
}
//-----------------------------------------------------------------------------
/**
    @cmd
    create
    @input
    ffffffi
    @output
    v
    @info
    Creates the space.
*/
static void
n_create(void* slf, nCmd* cmd)
{
    nPhyGeomQuadTreeSpace* self( static_cast<nPhyGeomQuadTreeSpace*>(slf));

    vector3 center;

    center.x = cmd->In()->GetF();
    center.y = cmd->In()->GetF();
    center.z = cmd->In()->GetF();

    vector3 extens;
    
    extens.x = cmd->In()->GetF();
    extens.y = cmd->In()->GetF();
    extens.z = cmd->In()->GetF();
    
    self->SetUp(center, extens,cmd->In()->GetI());

    self->Create();
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomQuadTreeSpace::SaveCmds(nPersistServer* ps)
{
    if( !nPhySpace::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DREA'));

    n_assert2( cmd, "Error command not found" );

    if( this->Id() != NoValidID )
    {
        cmd->In()->SetF( centerSpace.x );
        cmd->In()->SetF( centerSpace.y );
        cmd->In()->SetF( centerSpace.z );

        cmd->In()->SetF( extensSpace.x );
        cmd->In()->SetF( extensSpace.y );
        cmd->In()->SetF( extensSpace.z );

        cmd->In()->SetI( depthSpace );

        ps->PutCmd(cmd);
    }

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
