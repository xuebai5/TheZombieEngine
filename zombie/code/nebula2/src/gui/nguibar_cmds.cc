#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguibar_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguibar.h"

static void n_setfillmode(void* slf, nCmd* cmd);
static void n_getfillmode(void* slf, nCmd* cmd);
static void n_setfillstate(void* slf, nCmd* cmd);
static void n_getfillstate(void* slf, nCmd* cmd);
static void n_setfillstatesegments(void* slf, nCmd* cmd);
static void n_getfillstatesegments(void* slf, nCmd* cmd);
static void n_isfillstatesegmented(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguibar

    @cppclass
    nGuiBar

    @superclass
    nguiwidget

    @classinfo
    A graphical bar widget
*/
void
n_initcmds_nGuiBar(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setfillmode_i",      'JSFM', n_setfillmode);
    cl->AddCmd("i_getfillmode_v",      'JGFM', n_getfillmode);
    cl->AddCmd("v_setfillstate_f",      'JSFS', n_setfillstate);
    cl->AddCmd("f_getfillstate_v",      'JGFS', n_getfillstate);
    cl->AddCmd("v_setfillstatesegments_i",      'JSFG', n_setfillstatesegments);
    cl->AddCmd("i_getfillstatesegments_v",      'JGFG', n_getfillstatesegments);
    cl->AddCmd("b_isfillstatesegmented_v",      'JIFG', n_isfillstatesegmented);
    /*NSCRIPT_ADDCMD('JSFM', void, SetFillMode, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGFM', int, GetFillMode, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSFS', void, SetFillState, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JGFS', float, GetFillState, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSFG', void, SetFillStateSegments, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGFG', int, GetFillStateSegments, 0, (), 0, ());
    NSCRIPT_ADDCMD('JIFS', bool, IsFillStateSegmented, 0, (), 0, ());*/
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfillmode
    @input
    i(Mode=0:LeftToRight|1:RightToLeft|2:TopToBottom\3:BottomToTop)
    @output
    v
    @info
    Set bar orientation mode
*/
static void
n_setfillmode(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    int mode = cmd->In()->GetI();
    self->SetFillMode( static_cast<nGuiBar::FillMode>(mode) );
}


//------------------------------------------------------------------------------
/**
    @cmd
    getfillmode
    @input
    v
    @output
    i(Mode=0:LeftToRight|1:RightToLeft|2:TopToBottom\3:BottomToTop)
    @info
    Get bar orientation mode
*/
static void
n_getfillmode(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    int mode = self->GetFillMode();
    cmd->Out()->SetI( mode );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfillstate
    @input
    f(state)
    @output
    v
    @info
    Set bar fill state (0..1)
*/
static void
n_setfillstate(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    float value = cmd->In()->GetF();
    self->SetFillState( value );
}


//------------------------------------------------------------------------------
/**
    @cmd
    getfillstate
    @input
    v
    @output
    f(state)
    @info
    Get bar fill state (0..1)
*/
static void
n_getfillstate(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    float value = self->GetFillState();
    cmd->Out()->SetF( value );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfillstatesegments
    @input
    i(segments)
    @output
    v
    @info
    Set bar segments
*/
static void
n_setfillstatesegments(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    int segments = cmd->In()->GetI();
    self->SetFillStateSegments( segments );
}


//------------------------------------------------------------------------------
/**
    @cmd
    getfillstatesegments
    @input
    v
    @output
    i(segments)
    @info
    Get bar segments
*/
static void
n_getfillstatesegments(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    int segments = self->GetFillStateSegments();
    cmd->Out()->SetI( segments );
}

//------------------------------------------------------------------------------
/**
    @cmd
    isfillstatesegmented
    @input
    v
    @output
    b(segmented)
    @info
    Get wether bar is segmented
*/
static void
n_isfillstatesegmented(void* slf, nCmd* cmd)
{
    nGuiBar* self = (nGuiBar*) slf;
    bool isSegmented = self->IsFillStateSegmented();
    cmd->Out()->SetB( isSegmented );
}
