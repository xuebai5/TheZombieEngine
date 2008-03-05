#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntextureanimator_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntextureanimator.h"
#include "kernel/npersistserver.h"

static void n_addtexture(void* slf, nCmd* cmd);
static void n_getnumtextures(void* slf, nCmd* cmd);
static void n_gettextureat(void* slf, nCmd* cmd);
static void n_setshaderparam(void* slf, nCmd* cmd);
static void n_getshaderparam(void* slf, nCmd* cmd);
static void n_setfrequency(void* slf, nCmd* cmd);
static void n_getfrequency(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntextureanimator

    @cppclass
    nTextureAnimator

    @superclass
    nshaderanimator

    @classinfo
    Switch between different textures based on an nRenderContext variable.
*/
void
n_initcmds_nTextureAnimator(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addtexture_s",        'ATEX', n_addtexture);
    cl->AddCmd("i_getnumtextures_v",    'GNTX', n_getnumtextures);
    cl->AddCmd("s_gettextureat_i",      'GTXA', n_gettextureat);
    //<OBSOLETE> keep for backwards compatibility only
    cl->AddCmd("v_setshaderparam_s",    'SSHP', n_setshaderparam);
    cl->AddCmd("s_getshaderparam_v",    'GSHP', n_getshaderparam);
    //</OBSOLETE>
    cl->AddCmd("v_setfrequency_i",      'SFRQ', n_setfrequency);    
    cl->AddCmd("i_getfrequency_v",      'GFRQ', n_getfrequency);    
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addtexture
    @input
    s(Texture path)
    @output
    v
    @info
    Add a texture to the array (with index number equal to the number of textures added thus far).
*/
static void
n_addtexture(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    self->AddTexture(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumtextures
    @input
    v
    @output
    i(num textures)
    @info
    Get number of textures in the animator
*/
static void
n_getnumtextures(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    cmd->Out()->SetI(self->GetNumTextures());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettextureat
    @input
    i(texture index)
    @output
    s(texture path)
    @info
    Get texture path at specified index
*/
static void
n_gettextureat(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    cmd->Out()->SetS(self->GetTextureAt(cmd->In()->GetI()).Get());
}

//<OBSOLETE> keep for backwards compatibility only
//------------------------------------------------------------------------------
/**
    @cmd
    setshaderparam
    @input
    s(Texture path)
    @output
    v
    @info
    Sets the shader state parameter that will be passed to nAbstractShaderNode::SetTexture.
*/
static void
n_setshaderparam(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    self->SetShaderParam(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getshaderparam
    @input
    v
    @output
    s(Texture path)
    @info
    Gets the shader state parameter.
*/
static void
n_getshaderparam(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    cmd->Out()->SetS(self->GetShaderParam());
}
//</OBSOLETE>

//------------------------------------------------------------------------------
/**
    @cmd
    setfrequency
    @input
    i(frames per second)
    @output
    v
    @info
    sets the frequency of the animation
*/
static void
n_setfrequency( void* slf, nCmd* cmd )
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    self->SetFrequency(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfrequency
    @input
    i(frames per second)
    @output
    v
    @info
    gets the frequency of the animation
*/
static void
n_getfrequency( void* slf, nCmd* cmd )
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    cmd->Out()->SetI(self->GetFrequency());
}

//------------------------------------------------------------------------------
/**
*/
bool
nTextureAnimator::SaveCmds(nPersistServer* ps)
{
    if (nShaderAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- addtexture ---
        const int numKeys = this->GetNumTextures();
        for (int curKey = 0; curKey < numKeys; ++curKey)
        {
            cmd = ps->GetCmd(this, 'ATEX');
            cmd->In()->SetS(this->GetTextureAt(curKey).Get());
            ps->PutCmd(cmd);
        }

        //--- setfrequency ---
        if (this->GetFrequency() > 0.0f)
        {
            cmd = ps->GetCmd(this, 'SFRQ');
            cmd->In()->SetI(this->GetFrequency());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
