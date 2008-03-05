#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nviewportnode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nviewportnode.h"
#include "kernel/npersistserver.h"

static void n_setbgcolor(void* slf, nCmd* cmd);
static void n_getbgcolor(void* slf, nCmd* cmd);
static void n_setoverride(void* slf, nCmd* cmd);
static void n_getoverride(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nviewportnode

    @cppclass
    nViewportNode

    @superclass
    nrenderpathnode

    @classinfo
    Render the scene to a viewport.
*/
void
n_initcmds_nViewportNode(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setbgcolor_ffff",     'SBGC', n_setbgcolor);
    cl->AddCmd("ffff_getbgcolor_v",     'GBGC', n_getbgcolor);
    cl->AddCmd("v_setoverride_b",       'SOVR', n_setoverride);
    cl->AddCmd("b_getoverride_v",       'GOVR', n_getoverride);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    
    
    @input
    ffff(bgColor)
    
    @output
    v
    
    @info
    Set the channel from which to get the camera object.
*/
static void
n_setbgcolor(void* slf, nCmd* cmd)
{
    nViewportNode * self = (nViewportNode *) slf;
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetBgColor(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getbgcolor
    
    @input
    v
    
    @output
    ffff(bgColor)
    
    @info
    Set the channel from which to get the viewport object.
*/
static void
n_getbgcolor(void* slf, nCmd* cmd)
{
    nViewportNode * self = (nViewportNode *) slf;
    vector4 v = self->GetBgColor();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
*/
static void
n_setoverride(void* slf, nCmd* cmd)
{
    nViewportNode * self = (nViewportNode *) slf;
    self->SetOverride(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_getoverride(void* slf, nCmd* cmd)
{
    nViewportNode * self = (nViewportNode *) slf;
    cmd->Out()->SetB(self->GetOverride());
}

//------------------------------------------------------------------------------
/**
*/
bool
nViewportNode::SaveCmds(nPersistServer* ps)
{
    if (nRenderPathNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setbgcolor ---
        cmd = ps->GetCmd(this, 'SBGC');
        const vector4& bgColor = this->GetBgColor();
        cmd->In()->SetF(bgColor.x);
        cmd->In()->SetF(bgColor.y);
        cmd->In()->SetF(bgColor.z);
        cmd->In()->SetF(bgColor.w);
        ps->PutCmd(cmd);

        //--- setclearcolor ---
        //--- setcleardepth ---
        //--- setclearstencil ---

        //--- setoverride ---
        if (this->GetOverride())
        {
            cmd = ps->GetCmd(this, 'SOVR');
            cmd->In()->SetB(true);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
