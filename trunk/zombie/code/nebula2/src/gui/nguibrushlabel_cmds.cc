#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguibrushlabel_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguibrushlabel.h"

static void n_setalignment(void* slf, nCmd* cmd);
static void n_getalignment(void* slf, nCmd* cmd);
static void n_settext(void* slf, nCmd* cmd);
static void n_gettext(void* slf, nCmd* cmd);
static void n_gettextextent(void* slf, nCmd* cmd);
static void n_setint(void* slf, nCmd* cmd);
static void n_getint(void* slf, nCmd* cmd);
static void n_settypefacename(void* slf, nCmd* cmd);
static void n_gettypefacename(void* slf, nCmd* cmd);
static void n_setbrushsize(void* slf, nCmd* cmd);
static void n_getbrushsize(void* slf, nCmd* cmd);
static void n_addbrushdefinition(void* slf, nCmd* cmd);
static void n_setcharseparation(void* slf, nCmd* cmd);
static void n_getcharseparation(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nGuiBrushLabel

    @cppclass
    nGuiBrushLabel

    @superclass
    nguilabel

    @classinfo
    A gui label that renders a text string with character brushes
*/
void
n_initcmds_nGuiBrushLabel(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setalignment_s",      'SAGN', n_setalignment);
    cl->AddCmd("s_getalignment_v",      'GAGN', n_getalignment);
    cl->AddCmd("v_settext_s",           'STXT', n_settext);
    cl->AddCmd("s_gettext_v",           'GTXT', n_gettext);
    cl->AddCmd("ff_gettextextent_v",    'GEXT', n_gettextextent);
    cl->AddCmd("v_setint_i",            'SINT', n_setint);
    cl->AddCmd("i_getint_v",            'GINT', n_getint);
    cl->AddCmd("v_settypefacename_s",            'STFN', n_settypefacename);
    cl->AddCmd("s_gettypefacename_v",            'GTFN', n_gettypefacename);
    cl->AddCmd("v_setbrushsize_ii",            'JSBS', n_setbrushsize);
    cl->AddCmd("ii_getbrushsize_v",            'JGBS', n_getbrushsize);
    cl->AddCmd("v_addbrushdefinition_ssffffs",            'JABD', n_addbrushdefinition);
    cl->AddCmd("v_setcharseparation_f",            'JSCS', n_setcharseparation);
    cl->AddCmd("f_getcharseparation_v",            'JGCS', n_getcharseparation);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setalignment
    @input
    s(Alignment = left | center | right)
    @output
    v
    @info
    Set the (horizontal) text alignment in the text widget.
*/
static void
n_setalignment(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    const char* str = cmd->In()->GetS();
    nGuiBrushLabel::Alignment align;
    if (strcmp("left", str) == 0)        align = nGuiBrushLabel::Left;
    else if (strcmp("right", str) == 0)  align = nGuiBrushLabel::Right;
    else if (strcmp("center", str) == 0) align = nGuiBrushLabel::Center;
    else 
    {
        align = nGuiBrushLabel::Center;
        n_error("nGuiBrushLabel.setalign: Invalid align string '%s'", str);
    }
    self->SetAlignment(align);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getalignment
    @input
    v
    @output
    s(Alignment = left | center | right)
    @info
    Get the (horizontal) text alignment in the text widget.
*/
static void
n_getalignment(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    switch (self->GetAlignment())
    {
        case nGuiBrushLabel::Left:   cmd->Out()->SetS("left"); break;
        case nGuiBrushLabel::Center: cmd->Out()->SetS("center"); break;
        default:                    cmd->Out()->SetS("right"); break;
    }
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settext
    @input
    s(TextString)
    @output
    v
    @info
    Set the label text as string.
*/
static void
n_settext(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    self->SetText(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettext
    @input
    v
    @output
    s(TextString)
    @info
    Get the label text as string.
*/
static void
n_gettext(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    cmd->Out()->SetS(self->GetText());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettextextent
    @input
    v
    @output
    f(X), f(Y)
    @info
    Get the text extent for the currently set text and font.
*/
static void
n_gettextextent(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    const vector2& extent = self->GetTextExtent();
    cmd->Out()->SetF(extent.x);
    cmd->Out()->SetF(extent.y);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setint
    @input
    i(IntegerValue)
    @output
    v
    @info
    Set the label text as an integer.
*/
static void
n_setint(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    self->SetInt(cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getint
    @input
    v
    @output
    i(IntegerValue)
    @info
    Get the label text as integer.
*/
static void
n_getint(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    cmd->Out()->SetI(self->GetInt());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settypefacename
    @input
    s(TypeFaceName)
    @output
    v
    @info
    Set the label fake type face
*/
static void
n_settypefacename(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    self->SetTypeFaceName(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettypefacename
    @input
    v
    @output
    s(TypeFaceName)
    @info
    Get the label text as integer.
*/
static void
n_gettypefacename(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    cmd->Out()->SetS(self->GetTypeFaceName());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setbrushsize
    @input
    i(BrushSizeX)
    i(BrushSizeY)
    @output
    v
    @info
    Set the characters brush size in pixels
*/
static void
n_setbrushsize(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    self->SetBrushSize(cmd->In()->GetI(),cmd->In()->GetI());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getint
    @input
    v
    @output
    i(BrushSizeX)
    i(BrushSizeY)
    @info
    Get the characters brush size in pixels
*/
static void
n_getbrushsize(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    int x, y;
    self->GetBrushSize(x, y);
    cmd->Out()->SetI(x);
    cmd->Out()->SetI(y);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addbrushdefinition
    @input
    v
    @output
    s(StartCharacter)
    s(EndCharacter)
    f(X0)
    f(Y0)
    f(XS)
    f(YS)
    s(ImageName)
    @info
    Add a set of characters definitions
*/
static void
n_addbrushdefinition(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    const char *s1 = cmd->In()->GetS();
    const char *s2 = cmd->In()->GetS();
    float f1 = cmd->In()->GetF();
    float f2 = cmd->In()->GetF();
    float f3 = cmd->In()->GetF();
    float f4 = cmd->In()->GetF();
    const char *s3 = cmd->In()->GetS();

    self->AddBrushDefinition(s1,s2,
                             vector2(f1,f2),
                             vector2(f3,f4),
                             s3);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setcharseparation
    @input
    f(CharSeparation)
    @output
    v
    @info
    Set the separation between characters. Character size-relative. Can be negative.
*/
static void
n_setcharseparation(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    self->SetCharSeparation(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getcharseparation
    @input
    v
    @output
    f(CharSeparation)
    @info
    Get the separation between characters
*/
static void
n_getcharseparation(void* slf, nCmd* cmd)
{
    nGuiBrushLabel* self = (nGuiBrushLabel*) slf;
    cmd->Out()->SetF(self->GetCharSeparation());
}
