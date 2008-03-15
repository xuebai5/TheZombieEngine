#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguilabel_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguilabel.h"

static void n_setcolor(void* slf, nCmd* cmd);
static void n_getcolor(void* slf, nCmd* cmd);
static void n_setblinkingcolor(void* slf, nCmd* cmd);
static void n_getblinkingcolor(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nGuiLabel

    @cppclass
    nGuiLabel

    @superclass
    nguiwidget

    @classinfo
    A gui label with default brushes
*/
void
n_initcmds_nGuiLabel(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcolor_ffff",       'SCLR', n_setcolor);
    cl->AddCmd("ffff_getcolor_v",       'GCLR', n_getcolor);
    cl->AddCmd("v_setblinkingcolor_ffff",       'JSBC', n_setblinkingcolor);
    cl->AddCmd("ffff_getblinkingcolor_v",       'JGBC', n_getblinkingcolor);
    cl->EndCmds();
}


//-----------------------------------------------------------------------------
/**
    @cmd
    setcolor
    @input
    f(Red), f(Green), f(Blue), f(Alpha)
    @output
    v
    @info
    Set the color.
*/
static void
n_setcolor(void* slf, nCmd* cmd)
{
    nGuiLabel* self = (nGuiLabel*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetColor(vector4(f0, f1, f2, f3));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getcolor
    @input
    v
    @output
    f(Red), f(Green), f(Blue), f(Alpha)
    @info
    Get the color.
*/
static void
n_getcolor(void* slf, nCmd* cmd)
{
    nGuiLabel* self = (nGuiLabel*) slf;
    const vector4& v = self->GetColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setblinkingcolor
    @input
    f(Red), f(Green), f(Blue), f(Alpha)
    @output
    v
    @info
    Set the text blinking color.
*/
static void
n_setblinkingcolor(void* slf, nCmd* cmd)
{
    nGuiLabel* self = (nGuiLabel*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    self->SetColor(vector4(f0, f1, f2, f3));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getblinkingcolor
    @input
    v
    @output
    f(Red), f(Green), f(Blue), f(Alpha)
    @info
    Get the text blinking color.
*/
static void
n_getblinkingcolor(void* slf, nCmd* cmd)
{
    nGuiLabel* self = (nGuiLabel*) slf;
    const vector4& v = self->GetColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

