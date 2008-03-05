//-----------------------------------------------------------------------------
//  nphygeomspace_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyspace.h"
#include "kernel/npersistserver.h"

static void n_addgeometry(void* slf, nCmd* cmd);
static void n_removegeometry(void* slf, nCmd* cmd);
static void n_setspacename(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphyspace

    @cppclass nPhySpace

    @superclass
    nPhysicsGeom

    @classinfo
    Abstract representation of a space geometry.
*/
void
n_initcmds_nPhySpace(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->AddCmd("v_addgeometry_o",                       'DDDG', n_addgeometry);
    cl->AddCmd("v_removegeometry_o",                    'DEMG', n_removegeometry);
    cl->AddCmd("v_setname_s",                           'DSPN', n_setspacename);
     
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addgeometry
    @input
    o
    @output
    v
    @info
    Adds a geometry to this space.
*/
static void
n_addgeometry(void* slf, nCmd* cmd)
{
    nPhySpace* self( static_cast<nPhySpace*>(slf));

    self->Add(static_cast<nPhysicsGeom*>(cmd->In()->GetO()));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    removegeometry
    @input
    o
    @output
    v
    @info
    Removes a geometry to this space.
*/
static void
n_removegeometry(void* slf, nCmd* cmd)
{
    nPhySpace* self( static_cast<nPhySpace*>(slf));

    self->Remove(static_cast<nPhysicsGeom*>(cmd->In()->GetO()));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setspacename
    @input
    s
    @output
    v
    @info
    Sets the space name.
*/
static void
n_setspacename(void* slf, nCmd* cmd)
{
    nPhySpace* self( static_cast<nPhySpace*>(slf));

    self->SetName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhySpace::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    if( this->GetName().Length() )
    {
        // persisting space's name
        nCmd* cmd(ps->GetCmd( this, 'DSPN' ));

        n_assert2( cmd, "Failed to find the command." );

        cmd->In()->SetS( this->GetName().Get() );

        ps->PutCmd(cmd);
    }

    int Size(this->GetNumGeometries());

    for( int i(0); i < Size; ++i )
    {
        nCmd* cmd(ps->GetCmd( this, 'DDDG'));

        n_assert2( cmd, "Failed to find the command." );

        cmd->In()->SetO( this->GetGeometry( i ) );

        ps->PutCmd(cmd);
    }

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
