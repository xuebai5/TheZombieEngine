#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguilinelayout_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguilinelayout.h"

static void n_setorientation(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguilinelayout

    @cppclass
    nGuiLineLayout

    @superclass
    nguiformlayout

    @classinfo
    A line layout aligns child widgets in a vertical or horizontal row
*/
void
n_initcmds_nGuiLineLayout(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setorientation_i",        'JSOR', n_setorientation);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setorientation
    @input
    i(Orientation 0=Horizontal 1 = Vertical)
    @output
    v
    @info
    Set the orientation
*/
static void
n_setorientation(void* slf, nCmd* cmd)
{
    nGuiLineLayout* self = (nGuiLineLayout*) slf;
    self->SetOrientation(nGuiLineLayout::Orientation( cmd->In()->GetI() ));
}

//------------------------------------------------------------------------------
