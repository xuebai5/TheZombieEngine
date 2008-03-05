#include "precompiled/pchnmaterial.h"
//--------------------------------------------------
//  nmaterial_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//--------------------------------------------------
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialserver.h"
#include "nscene/nshadertree.h"
#include "kernel/npersistserver.h"
#include "variable/nvariableserver.h"

static void n_addparam(void* slf, nCmd* cmd);
static void n_getnumparams(void* slf, nCmd* cmd);
static void n_getparamat(void* slf, nCmd* cmd);
static void n_hasparam(void* slf, nCmd* cmd);
static void n_removeparam(void* slf, nCmd* cmd);
static void n_beginlevel(void* slf, nCmd* cmd);
static void n_endlevel(void* slf, nCmd* cmd);
static void n_beginpass(void* slf, nCmd* cmd);
static void n_endpass(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmaterial

    @cppclass
    nMaterial

    @superclass
    nroot

    @classinfo
    A material describes components of a surface
*/
void
n_initcmds_nMaterial(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addparam_ss",     'APAR', n_addparam);
    cl->AddCmd("i_getnumparams_v",  'GNPR', n_getnumparams);
    cl->AddCmd("ss_getparamat_i",   'GPAR', n_getparamat);
    cl->AddCmd("b_hasparam_ss",     'HPAR', n_hasparam);
    cl->AddCmd("v_removeparam_ss",  'RPAR', n_removeparam);
    cl->AddCmd("v_beginlevel_v",    'BGNL', n_beginlevel);
    cl->AddCmd("v_endlevel_v",      'ENDL', n_endlevel);
    cl->AddCmd("v_beginpass_s",     'BGNP', n_beginpass);
    cl->AddCmd("v_endpass_v",       'ENDP', n_endpass);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addparam

    @input
    s(paramName), s(paramValue)

    @output
    v

    @info
    Add a material parameter.
*/
void
n_addparam(void* slf, nCmd* cmd)
{
    nMaterial* self = (nMaterial*) slf;
    const char *param = cmd->In()->GetS();
    const char *value = cmd->In()->GetS();
    self->AddParam(param, value);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumparams

    @input
    v

    @output
    i(numParams)

    @info
    Get number of material parameters.
*/
void
n_getnumparams(void* slf, nCmd* cmd)
{
    nMaterial* self = (nMaterial*) slf;
    cmd->Out()->SetI(self->GetNumParams());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparamat

    @input
    i

    @output
    s(paramName), s(paramValue)

    @info
    Get material parameter by index.
*/
void
n_getparamat(void* slf, nCmd* cmd)
{
    const char *paramName;
    const char *paramValue;
    nMaterial* self = (nMaterial*) slf;
    self->GetParamAt(cmd->In()->GetI(), &paramName, &paramValue);
    cmd->Out()->SetS(paramName);
    cmd->Out()->SetS(paramValue);
}

//------------------------------------------------------------------------------
/**
    @cmd
    hasparam

    @input
    s(paramName), s(paramValue)

    @output
    b(hasParam)

    @info
    Check whether a material has a param
*/
void
n_hasparam(void* slf, nCmd* cmd)
{
    nMaterial* self = (nMaterial*) slf;
    const char *name = cmd->In()->GetS();
    const char *value = cmd->In()->GetS();
    cmd->Out()->SetB(self->HasParam(name, value));
}

//------------------------------------------------------------------------------
/**
    @cmd
    removeparam

    @input
    s(paramName), s(paramValue)

    @output
    b(removeParam)

    @info
    remove a name param from a material
*/
void
n_removeparam(void* slf, nCmd* cmd)
{
    nMaterial* self = (nMaterial*) slf;
    const char *name = cmd->In()->GetS();
    const char *value = cmd->In()->GetS();
    self->RemoveParam(name, value);
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginlevel

    @input
    i(levelIndex)

    @output
    v

    @info
    Begin a new level of detail.
*/
void
n_beginlevel(void* slf, nCmd* /*cmd*/)
{
    nMaterial *self = (nMaterial *) slf;
    self->BeginLevel();
}

//------------------------------------------------------------------------------
/**
    @cmd
    endlevel

    @input
    v

    @output
    v

    @info
    End current level of detail.
*/
void
n_endlevel(void* slf, nCmd* /*cmd*/)
{
    nMaterial *self = (nMaterial *) slf;
    self->EndLevel();
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginpass

    @input
    s(passFourCC)

    @output
    v

    @info
    Begin a pass shader.
*/
void
n_beginpass(void* slf, nCmd* cmd)
{
    nMaterial *self = (nMaterial *) slf;
    self->BeginPass(nVariableServer::StringToFourCC(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    endpass

    @input
    v

    @output
    v

    @info
    End a pass shader.
*/
void
n_endpass(void* slf, nCmd* /*cmd*/)
{
    nMaterial *self = (nMaterial *) slf;
    self->EndPass();
}

//------------------------------------------------------------------------------
/**
    Object persistency.

    history:
        - 10-Nov-2005   ma.garcias  Persist only parameters or shader tree.
        - 13-Mar-2006   ma.garcias  Persist shader tree for library only
*/
bool
nMaterial::SaveCmds(nPersistServer* ps)
{
    nCmd* cmd;

    int numParams = this->GetNumParams();
    if (numParams > 0)
    {
        //--- addparam ---
        for (int i = 0; i < numParams; i++)
        {
            const char *paramName;
            const char *paramValue;
            this->GetParamAt(i, &paramName, &paramValue);
            
            cmd = ps->GetCmd(this, 'APAR');
            cmd->In()->SetS(paramName);
            cmd->In()->SetS(paramValue);
            ps->PutCmd(cmd);
        }
    }

    if (numParams == 0 || nMaterialServer::Instance()->IsSavingMaterialLibrary())
    {
        //--- beginlevel ---
        int numLevels = this->GetNumLevels();
        for (int level = 0; level < numLevels; level++)
        {
            // --- beginlevel ---
            cmd = ps->GetCmd(this, 'BGNL');
            ps->PutCmd(cmd);
            
            for (int i = 0; i < this->GetNumPasses(level); i++)
            {
                char buf[5];
                nFourCC fourcc = this->GetPassAt(level, i);
                
                nShaderTree *shaderTree = this->GetShaderTree(level, fourcc);
                if (shaderTree)
                {
                    //--- beginpass ---
                    cmd = ps->GetCmd(this, 'BGNP');
                    cmd->In()->SetS(nVariableServer::FourCCToString(fourcc, buf, sizeof(buf)));
                    
                    // save shader tree
                    if (ps->BeginObjectWithCmd(this, cmd))
                    {
                        if (!shaderTree->SaveCmds(ps))
                        {
                            return false;
                        }
                        ps->EndObject(false);
                    }
                }
            }
            
            //--- endlevel ---
            cmd = ps->GetCmd(this, 'ENDL');
            ps->PutCmd(cmd);
        }
    }
    
    return true;
}
