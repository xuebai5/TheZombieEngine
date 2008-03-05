#include "precompiled/pchnspatial.h"
//-------------------------------------------------------------------
//  nspatialsector_cmds.cc
//  (C) 2004 Gary Haussmann
//-------------------------------------------------------------------
#include "nspatial/nspatialsector.h"

static void n_createsector(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nspatialsector

    @superclass
    nroot

    @classinfo
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_createsector_s",'CRSC',n_createsector);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    createsector

    @input
    s (sectorname)

    @output
    v

    @info
    creates a spatial sector
*/

static void n_createsector(void *o, nCmd *cmd)
{
    nSpatialSector *self = (nSpatialSector *) o;

}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------


static void n_setsubdivnum(void *o, nCmd *cmd)
{
    nOctree *self = (nOctree *) o;
    self->SetSubdivNum(cmd->In()->GetI());
}