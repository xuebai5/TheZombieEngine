//-----------------------------------------------------------------------------
//  nphygeomhashspace_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "kernel/npersistserver.h"

static void n_setlevels(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomhashspace

    @cppclass nPhyGeomHashSpace

    @superclass
    nPhySpace

    @classinfo
    Abstract representation of a space geometry.
*/
void
n_initcmds_nPhyGeomHashSpace(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_setlevels_ii",                        'DSLV', n_setlevels);
     
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setlevels
    @input
    ii
    @output
    v
    @info
    Sets the levels of a hash space.
*/
static void
n_setlevels(void* slf, nCmd* cmd)
{

    nPhyGeomHashSpace* self( static_cast<nPhyGeomHashSpace*>(slf));

    int maxvalue( cmd->In()->GetI() );

    self->SetLevels( maxvalue, cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomHashSpace::SaveCmds(nPersistServer* ps)
{
    if( !nPhySpace::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DSLV'));
    
    n_assert2( cmd, "Error command not found" );

    // storing the hash levels
    int maxcell,mincell;

    this->GetLevels(maxcell,mincell);

    cmd->In()->SetI( maxcell );
    cmd->In()->SetI( mincell );
    
    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
