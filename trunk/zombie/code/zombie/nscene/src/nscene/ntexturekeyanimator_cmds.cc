#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntexturekeyanimator_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntexturekeyanimator.h"
#include "kernel/npersistserver.h"

//<obsolete>
static void n_settexturename(void* slf, nCmd* cmd);
static void n_gettexturename(void* slf, nCmd* cmd);
//</obsolete>
static void n_addkey(void* slf, nCmd* cmd);
static void n_getnumkeys(void* slf, nCmd* cmd);
static void n_getkeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntexturekeyanimator

    @cppclass
    nTextureKeyAnimator

    @superclass
    nanimator

    @classinfo
    Animate a texture attribute of a nabstractshadernode.
*/
void
n_initcmds_nTextureKeyAnimator(nClass* cl)
{
    cl->BeginCmds();
    //<obsolete>
    cl->AddCmd("v_settexturename_s",    'STXN', n_settexturename);
    cl->AddCmd("s_gettexturename_v",    'GTXN', n_gettexturename);
    //</obsolete>
    cl->AddCmd("v_addkey_fs",           'ADDK', n_addkey);
    cl->AddCmd("i_getnumkeys_v",        'GNKS', n_getnumkeys);
    cl->AddCmd("fs_getkeyat_i",         'GKAT', n_getkeyat);
    cl->EndCmds();
}

//<obsolete>
//------------------------------------------------------------------------------
/**
    @cmd
    settexturename
    @input
    s(TextureName)
    @output
    v
    @info
    Set name of texture variable to animate in target object.
*/
void
n_settexturename(void* slf, nCmd* cmd)
{
    nTextureKeyAnimator* self = (nTextureKeyAnimator*) slf;
    self->SetTextureName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexturename
    @input
    v
    @output
    s(TextureName)
    @info
    Get name of texture variable to animate in target object.
*/
void
n_gettexturename(void* slf, nCmd* cmd)
{
    nTextureKeyAnimator* self = (nTextureKeyAnimator*) slf;
    cmd->Out()->SetS(self->GetTextureName());
}
//</obsolete>

//------------------------------------------------------------------------------
/**
    @cmd
    addkey
    @input
    f(Time), s(TextureResource)
    @output
    v
    @info
    Add a texture key to the animation key array.
*/
void
n_addkey(void* slf, nCmd* cmd)
{
    nTextureKeyAnimator* self = (nTextureKeyAnimator*) slf;
    float f0;
    const char *s0;
    f0 = cmd->In()->GetF();
    s0 = cmd->In()->GetS();
    self->AddKey(f0, s0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumkeys
    @input
    v
    @output
    i(NumKeys)
    @info
    Returns number of key in animation array.
*/
void
n_getnumkeys(void* slf, nCmd* cmd)
{
    nTextureKeyAnimator* self = (nTextureKeyAnimator*) slf;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyat
    @input
    i(KeyIndex)
    @output
    f(Time), f(X), f(Y), f(Z), f(W)
    @info
    Returns key at given index.
*/
void
n_getkeyat(void* slf, nCmd* cmd)
{
    nTextureKeyAnimator* self = (nTextureKeyAnimator*) slf;
    float f0;
    const char *s0;
    self->GetKeyAt(cmd->In()->GetI(), f0, s0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetS(s0);
}

//------------------------------------------------------------------------------
/**
*/
bool
nTextureKeyAnimator::SaveCmds(nPersistServer* ps)
{
    if (nShaderAnimator::SaveCmds(ps))
    {
        nCmd* cmd;
        
        //--- addkey ---
        int i;
        int numKeys = this->GetNumKeys();
        for (i = 0; i < numKeys; i++)
        {
            float time;
            const char *name;
            cmd = ps->GetCmd(this, 'ADDK');
            this->GetKeyAt(i, time, name);
            cmd->In()->SetF(time);
            cmd->In()->SetS(name);
            ps->PutCmd(cmd);
        }
        
        return true;
    }
    return false;
}
