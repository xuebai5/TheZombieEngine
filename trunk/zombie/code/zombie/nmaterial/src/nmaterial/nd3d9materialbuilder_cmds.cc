#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nd3d9materialbuildernode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nscenenode.h"
#include "variable/nvariableserver.h"
#include "nmaterial/nd3d9materialbuilder.h"

static void n_setonbuildshaderlevels(void* slf, nCmd* cmd);
static void n_setonbuildshaderpasses(void* slf, nCmd* cmd);
static void n_setonbuildshadertree(void* slf, nCmd* cmd);
static void n_setonbuildshadername(void* slf, nCmd* cmd);
static void n_setonbuildshader(void* slf, nCmd* cmd);
static void n_getmaterial(void* slf, nCmd* cmd);
static void n_setnumlevels(void* slf, nCmd* cmd);
static void n_getlevel(void* slf, nCmd* cmd);
static void n_addpass(void* slf, nCmd* cmd);
static void n_getpass(void* slf, nCmd* cmd);
static void n_addattrib(void* slf, nCmd* cmd);
static void n_getattrib(void* slf, nCmd* cmd);
static void n_getcasevalue(void* slf, nCmd* cmd);
static void n_begincasevar(void* slf, nCmd* cmd);
static void n_addcasevalue(void* slf, nCmd* cmd);
static void n_endcasevar(void* slf, nCmd* cmd);
static void n_getcasepathlen(void* slf, nCmd* cmd);
static void n_getcasenodeat(void* slf, nCmd* cmd);
static void n_getshadername(void* slf, nCmd* cmd);
static void n_setshaderfile(void* slf, nCmd* cmd);
static void n_settechnique(void* slf, nCmd* cmd);
static void n_setsequence(void* slf, nCmd* cmd);
static void n_setbuildshader(void* slf, nCmd* cmd);
static void n_puts(void* slf, nCmd* cmd);
static void n_appendfile(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nd3d9materialbuilder

    @cppclass
    nD3D9MaterialBuilder

    @superclass
    nmaterialbuilder

    @classinfo
    A class for building shaders from a set of abstract attributes.
*/

void
n_initcmds_nD3D9MaterialBuilder(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setonbuildshaderlevels_s", 'SBSL', n_setonbuildshaderlevels);
    cl->AddCmd("v_setonbuildshaderpasses_s", 'SBSP', n_setonbuildshaderpasses);
    cl->AddCmd("v_setonbuildshadertree_s",   'SBST', n_setonbuildshadertree);
    cl->AddCmd("v_setonbuildshadername_s",   'SBSN', n_setonbuildshadername);
    cl->AddCmd("v_setonbuildshader_s",       'SBSH', n_setonbuildshader);
    cl->AddCmd("s_getmaterial_v",            'GETM', n_getmaterial);
    cl->AddCmd("v_setnumlevels_i",           'SETL', n_setnumlevels);
    cl->AddCmd("i_getlevel_v",               'GETL', n_getlevel);
    cl->AddCmd("v_addpass_s",                'ADDP', n_addpass);
    cl->AddCmd("s_getpass_v",                'GETP', n_getpass);
    cl->AddCmd("v_addattrib_ss",             'ADDA', n_addattrib);
    cl->AddCmd("s_getattrib_s",              'GETA', n_getattrib);
    cl->AddCmd("v_begincasevar_s",           'BEGC', n_begincasevar);
    cl->AddCmd("v_endcasevar_v",             'ENDC', n_endcasevar);
    cl->AddCmd("i_getcasepathlen_v",         'GCPL', n_getcasepathlen);
    cl->AddCmd("ss_getcasenodeat_i",         'GCNA', n_getcasenodeat);
    cl->AddCmd("v_addcasevalue_s",           'ADDC', n_addcasevalue);
    cl->AddCmd("s_getcasevalue_s",           'GETC', n_getcasevalue);
    cl->AddCmd("s_getshadername_v",          'GSHN', n_getshadername);
    cl->AddCmd("v_setshaderfile_s",          'SSHF', n_setshaderfile);
    cl->AddCmd("v_settechnique_s",           'STEC', n_settechnique);
    cl->AddCmd("v_setsequence_s",            'SSEQ', n_setsequence);
    cl->AddCmd("v_setbuildshader_b",         'SBDS', n_setbuildshader);
    cl->AddCmd("b_puts_s",                   'PUTS', n_puts);
    cl->AddCmd("b_appendfile_s",             'APFI', n_appendfile);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setonbuildshaderlevels

    @input
    s(functionName)

    @output
    v

    @info
    Assign script function for loading levels of detail.
*/
static void
n_setonbuildshaderlevels(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->SetOnBuildShaderLevelsFunction(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setonbuildshaderpasses

    @input
    s(functionName)

    @output
    v

    @info
    Assign script function for loading passes
*/
static void
n_setonbuildshaderpasses(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    self->SetOnBuildShaderPassesFunction(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaterial

    @input
    v

    @output
    s(materialpath)

    @info
    Get full path to current material.
*/
static void n_getmaterial(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    cmd->Out()->SetS(self->GetMaterial());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setonbuildshadertree

    @input
    s(functionName)

    @output
    v

    @info
    Assign script function for building shader tree
*/
static void
n_setonbuildshadertree(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    self->SetOnBuildShaderTreeFunction(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_setonbuildshadername(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    self->SetOnBuildShaderNameFunction(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setonbuildshader

    @input
    s(functionName)

    @output
    v

    @info
    Assign script function for building shader code
*/
static void
n_setonbuildshader(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    self->SetOnBuildShaderFunction(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setnumlevels

    @input
    i(numLevels)

    @output
    v

    @info
    Set number of levels of detail for current material
*/
static void
n_setnumlevels(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    self->SetNumLevels(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlevel

    @input
    v

    @output
    i(level)

    @info
    Get current level of detail
*/
static void
n_getlevel(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    cmd->Out()->SetI(self->GetLevel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addpass

    @input
    s(stringFourCC)

    @output
    v

    @info
    Add a render pass to the current material
*/
static void
n_addpass(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    self->AddPass(nVariableServer::StringToFourCC(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getpass

    @input
    v

    @output
    s(passFourcc)

    @info
    Get current pass in building process
*/
static void
n_getpass(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    char buf[5];
    cmd->Out()->SetS(nVariableServer::FourCCToString(self->GetPass(), buf, sizeof(buf)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    addattrib

    @input
    s(attributeName), s(attributeValue)

    @output
    v

    @info
    Add an attribute to the current shader description
*/
static void
n_addattrib(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    const char *name = cmd->In()->GetS();
    const char *value = cmd->In()->GetS();
    self->AddAttribute(name, value);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getattrib

    @input
    s(attributeName)

    @output
    s(attributeValue)

    @info
    Add an attribute to the current shader description
*/
static void
n_getattrib(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder * self = (nD3D9MaterialBuilder *) slf;
    cmd->Out()->SetS(self->GetAttribute(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    puts

    @input
    s(String)

    @output
    b(Success)

    @info
    Add a string into the shader file.
*/
static void
n_puts(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->PutS(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_getshadername(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    cmd->Out()->SetS(self->GetShaderName());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_setshaderfile(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->SetShaderFile(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_settechnique(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->SetTechnique(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_setsequence(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->SetSequence(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_setbuildshader(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->SetBuildShader(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    appendfile

    @input
    s(String)

    @output
    b(Success)

    @info
    append file into the shader file.
*/
static void
n_appendfile(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->AppendFile(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    begincasevar

    @input
    s(String)

    @output
    v

    @info
    Begin a new variable case.
*/
static void
n_begincasevar(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->BeginCaseVar(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    endcasevar

    @input
    v

    @output
    v

    @info
    End current variable case.
*/
static void
n_endcasevar(void* slf, nCmd* /*cmd*/)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->EndCaseVar();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_getcasepathlen(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    cmd->Out()->SetI(self->GetCasePathLen());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_getcasenodeat(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    const char *param;
    const char *value;
    self->GetCaseNodeAt(cmd->In()->GetI(), &param, &value);
    cmd->Out()->SetS(param);
    cmd->Out()->SetS(value);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addcasevalue

    @input
    s(caseValue)

    @output
    v

    @info
    Add a case value to the current variable.
*/
static void
n_addcasevalue(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    self->AddCaseValue(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcasevalue

    @input
    s(caseValue)

    @output
    v

    @info
    Add a case value to the current variable.
*/
static void
n_getcasevalue(void* slf, nCmd* cmd)
{
    nD3D9MaterialBuilder *self = (nD3D9MaterialBuilder *) slf;
    const char* val = self->GetCaseValue(cmd->In()->GetS()); 
    cmd->Out()->SetS( val ?  val : "" );
}
