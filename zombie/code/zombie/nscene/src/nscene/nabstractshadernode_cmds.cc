#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nabstractshadernode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nabstractshadernode.h"
#include "kernel/npersistserver.h"

static void n_setuvpos(void* slf, nCmd* cmd);
static void n_getuvpos(void* slf, nCmd* cmd);
static void n_setuveuler(void* slf, nCmd* cmd);
static void n_getuveuler(void* slf, nCmd* cmd);
static void n_setuvscale(void* slf, nCmd* cmd);
static void n_getuvscale(void* slf, nCmd* cmd);
static void n_settexture(void* slf, nCmd* cmd);
static void n_gettexture(void* slf, nCmd* cmd);
static void n_setint(void* slf, nCmd* cmd);
static void n_getint(void* slf, nCmd* cmd);
static void n_setbool(void* slf, nCmd* cmd);
static void n_getbool(void* slf, nCmd* cmd);
static void n_setfloat(void* slf, nCmd* cmd);
static void n_getfloat(void* slf, nCmd* cmd);
static void n_setvector(void* slf, nCmd* cmd);
static void n_getvector(void* slf, nCmd* cmd);

static void n_setparamfromvar(void* slf, nCmd* cmd);
static void n_getparamfromvar(void* slf, nCmd* cmd);

static void n_setselectortype(void* slf, nCmd* cmd);
//OBSOLETE- keep for backwards compatibility
static void n_addtype(void* slf, nCmd* cmd);


static void n_setpassenabled(void* slf, nCmd* cmd);
static void n_getpassenabled(void* slf, nCmd* cmd);
static void n_setpassallflags(void* slf, nCmd* cmd);
static void n_setdefaultpassenable(void* slf, nCmd* cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    nabstractshadernode

    @cppclass
    nAbstractShaderNode
    
    @superclass
    ntransformnode

    @classinfo
    Parent class for shader related scene node classes. Don't use this
    class directly, instead use its subclasses.
*/
void
n_initcmds_nAbstractShaderNode(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setuvpos_iff",    'SUVP', n_setuvpos);
    cl->AddCmd("ff_getuvpos_i",     'GUVP', n_getuvpos);
    cl->AddCmd("v_setuveuler_iff",  'SUVE', n_setuveuler);
    cl->AddCmd("ff_getuveuler_i",   'GUVE', n_getuveuler);
    cl->AddCmd("v_setuvscale_iff",  'SUVS', n_setuvscale);
    cl->AddCmd("ff_getuvscale_i",   'GUVS', n_getuvscale);
    cl->AddCmd("v_settexture_ss",   'STXT', n_settexture);
    cl->AddCmd("s_gettexture_s",    'GTXT', n_gettexture);
    cl->AddCmd("v_setint_si",       'SINT', n_setint);
    cl->AddCmd("i_getint_s",        'GINT', n_getint);
    cl->AddCmd("v_setbool_sb",      'SBOO', n_setbool);
    cl->AddCmd("b_getbool_s",       'GBOO', n_getbool);
    cl->AddCmd("v_setfloat_sf",     'SFLT', n_setfloat);
    cl->AddCmd("f_getfloat_s",      'GFLT', n_getfloat);
    cl->AddCmd("v_setvector_sffff", 'SVEC', n_setvector);
    cl->AddCmd("ffff_getvector_s",  'GVEC', n_getvector);

    cl->AddCmd("v_setparamfromvar_ss",  'SPFV', n_setparamfromvar);
    cl->AddCmd("s_getparamfromvar_s",   'GPFV', n_getparamfromvar);

    cl->AddCmd("v_addtype_ss",          'ATYP', n_addtype);
    cl->AddCmd("v_setselectortype_s",   'SSTY', n_setselectortype);

    /// Pass enable
    cl->AddCmd("v_setpassenabled_sb",      'CSPE', n_setpassenabled);
    cl->AddCmd("b_getpassenabled_s",       'CGPE', n_getpassenabled);
    cl->AddCmd("v_setpassallflags_b",      'CSPA', n_setpassallflags);
    cl->AddCmd("v_setdefaultpassenable_b", 'CDPD', n_setdefaultpassenable);


    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuvpos
    @input
    i(Layer), fff(Pos)
    @output
    v
    @info
    Set uv coordinate translation for given uv set.
*/
static void
n_setuvpos(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    static vector2 v;
    int i = cmd->In()->GetI();
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    self->SetUvPos(i, v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuvpos
    @input
    i(Layer)
    @output
    fff(Pos)
    @info
    Get uv coordinate translation for given uv set.
*/
static void
n_getuvpos(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const vector2& v = self->GetUvPos(cmd->In()->GetI());
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuveuler
    @input
    i(Layer), fff(EulerAngles)
    @output
    v
    @info
    Set uv coordinate euler rotation for given uv set.
*/
static void
n_setuveuler(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    static vector2 v;
    int i = cmd->In()->GetI();
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    self->SetUvEuler(i, v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuveuler
    @input
    i(Layer)
    @output
    fff(EulerAngles)
    @info
    Get uv coordinate euler rotation for given uv set.
*/
static void
n_getuveuler(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const vector2& v = self->GetUvEuler(cmd->In()->GetI());
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setuvscale
    @input
    i(Layer), fff(Scale)
    @output
    v
    @info
    Set uv coordinate scale for given uv set.
*/
static void
n_setuvscale(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    static vector2 v;
    int i = cmd->In()->GetI();
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    self->SetUvScale(i, v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getuvscale
    @input
    i(Layer)
    @output
    fff(Scale)
    @info
    Get uv coordinate scale for given uv set.
*/
static void
n_getuvscale(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const vector2& v = self->GetUvScale(cmd->In()->GetI());
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexture
    @input
    s(VarName), s(TextureResource)
    @output
    v
    @info
    Set a texture resource shader variable.
*/
static void
n_settexture(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetTexture(nShaderState::StringToParam(s0), s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexture
    @input
    s(VarName)
    @output
    s(TextureResource)
    @info
    Get a texture resource shader variable.
*/
static void
n_gettexture(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetS(self->GetTexture(nShaderState::StringToParam(cmd->In()->GetS())));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setint
    @input
    s(VarName), i(IntVal)
    @output
    v
    @info
    Set a integer shader variable.
*/
static void
n_setint(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    int i0 = cmd->In()->GetI();
    self->SetInt(nShaderState::StringToParam(s0), i0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getint
    @input
    s(VarName)
    @output
    i(IntVal)
    @info
    Get a integer shader variable.
*/
static void
n_getint(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetI(self->GetInt(nShaderState::StringToParam(cmd->In()->GetS())));
}
//------------------------------------------------------------------------------
/**
    @cmd
    setbool
    @input
    s(VarName), b(BoolVal)
    @output
    v
    @info
    Set a boolean shader variable.
*/
static void
n_setbool(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    bool b0 = cmd->In()->GetB();
    self->SetBool(nShaderState::StringToParam(s0), b0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getbool
    @input
    s(VarName)
    @output
    b(BoolVal)
    @info
    Get a boolean shader variable.
*/
static void
n_getbool(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetB(self->GetBool(nShaderState::StringToParam(cmd->In()->GetS())));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setfloat
    @input
    s(VarName), f(FloatVal)
    @output
    v
    @info
    Set a float shader variable.
*/
static void
n_setfloat(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    float f0 = cmd->In()->GetF();
    self->SetFloat(nShaderState::StringToParam(s0), f0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfloat
    @input
    s(VarName)
    @output
    f(FloatVal)
    @info
    Get a float shader variable.
*/
static void
n_getfloat(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    cmd->Out()->SetF(self->GetFloat(nShaderState::StringToParam(cmd->In()->GetS())));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvector
    @input
    s(VarName), f(X), f(Y), f(Z), f(W)
    @output
    v
    @info
    Set a 4d vector shader variable.
*/
static void
n_setvector(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    vector4 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    v.w = cmd->In()->GetF();
    self->SetVector(nShaderState::StringToParam(s0), v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvector
    @input
    s(VarName)
    @output
    f(X), f(Y), f(Z), f(W)
    @info
    Get a 4d vector shader variable.
*/
static void
n_getvector(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    vector4 v = self->GetVector(nShaderState::StringToParam(cmd->In()->GetS()));
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
    cmd->Out()->SetF(v.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvectorfromvar
    @input
    s(ParamName), s(VarName)
    @output
    v
    @info
    Set variable to retrieve a vector shader override
*/
static void
n_setparamfromvar(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetParamFromVar(nShaderState::StringToParam(s0), s1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvectorfromvar
    @input
    s(ParamName)
    @output
    s(VarName)
    @info
    Get variable to retrieve a vector shader override
*/
static void
n_getparamfromvar(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    const char* s0 = cmd->In()->GetS();
    cmd->Out()->SetS(self->GetParamFromVar(nShaderState::StringToParam(s0)));
}

//------------------------------------------------------------------------------
/**
    @cmd addtype
    @output
      v
    @input
      s(typeName), s(typeValue)
    @info 
      Add a new environment type to the node.
*/
static void
n_addtype(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    /*const char *param =*/ cmd->In()->GetS();
    const char *value = cmd->In()->GetS();
    self->SetSelectorType(nVariableServer::StringToFourCC(value));
}

//------------------------------------------------------------------------------
/**
    @cmd addtype
    @output
      v
    @input
      s(typeName), s(typeValue)
    @info 
      set selector type.
*/
static void
n_setselectortype(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    self->SetSelectorType(nVariableServer::StringToFourCC(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @output
      v
    @input
      s(pass fourcc), b(pass enable)
    @info 
      set pass enable.
*/
static void
n_setpassenabled(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    nFourCC fourcc = nVariableServer::StringToFourCC(cmd->In()->GetS());
    self->SetPassEnabled(fourcc, cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @output
      b(pass enable)
    @input
      s(pass fourcc), 
    @info 
      get pass enable.
*/
static void
n_getpassenabled(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    nFourCC fourcc = nVariableServer::StringToFourCC(cmd->In()->GetS());
    cmd->Out()->SetB(self->GetPassEnabled(fourcc));
}

//------------------------------------------------------------------------------
/**
    @cmd addtype
    @output
      v
    @input
      b
    @info 
      set all pass flags.
*/
static void
n_setpassallflags(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    bool allFlags = (cmd->In()->GetB());
    self->SetPassAllFlags(allFlags);
}


//------------------------------------------------------------------------------
/**
    @cmd setdefaultpassenable
    @output
      v
    @input
      b
    @info 
      set if by deafault the pass is enable.
*/
static void
n_setdefaultpassenable(void* slf, nCmd* cmd)
{
    nAbstractShaderNode* self = (nAbstractShaderNode*) slf;
    bool defaultIsEnable = (cmd->In()->GetB());
    self->SetDefaultPassEnable(defaultIsEnable);
}



//------------------------------------------------------------------------------
/**
*/
bool
nAbstractShaderNode::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
        nCmd* cmd;
        int i;
        int num;
        static const vector2 nullVec;
        static const vector2 oneVec(1.0f, 1.0f);

        //--- setselectortype ---
        nFourCC fourcc = this->GetSelectorType();
        if (fourcc)
        {
            cmd = ps->GetCmd(this, 'SSTY');
            char buf[5];
            cmd->In()->SetS(nVariableServer::FourCCToString(fourcc, buf, sizeof(buf)));
            ps->PutCmd(cmd);
        }

        //--- setuvpos/setuveuler/setuvscale ---
        for (i = 0; i < nGfxServer2::MaxTextureStages; i++)
        {
            const vector2& p = this->textureTransform[i].gettranslation();
            const vector2& e = this->textureTransform[i].geteulerrotation();
            const vector2& s = this->textureTransform[i].getscale();
            if (!p.isequal(nullVec, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SUVP');
                cmd->In()->SetI(i);
                cmd->In()->SetF(p.x);
                cmd->In()->SetF(p.y);
                ps->PutCmd(cmd);
            }
            if (!e.isequal(nullVec, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SUVE');
                cmd->In()->SetI(i);
                cmd->In()->SetF(e.x);
                cmd->In()->SetF(e.y);
                ps->PutCmd(cmd);
            }
            if (!s.isequal(oneVec, 0.0f))
            {
                cmd = ps->GetCmd(this, 'SUVS');
                cmd->In()->SetI(i);
                cmd->In()->SetF(s.x);
                cmd->In()->SetF(s.y);
                ps->PutCmd(cmd);
            }
        }

        //--- settexture ---
        num = this->texNodeArray.Size();
        for (i = 0; i < num; i++)
        {
            TexNode& texNode = this->texNodeArray[i];
            cmd = ps->GetCmd(this, 'STXT');
            cmd->In()->SetS(nShaderState::ParamToString(texNode.shaderParameter));
            cmd->In()->SetS(texNode.texName.Get());
            ps->PutCmd(cmd);
        }

        //--- setint/setfloat/setvector ---
        num = this->shaderParams.GetNumValidParams();
        for (i = 0; i < num; i++)
        {
            nShaderState::Param param = this->shaderParams.GetParamByIndex(i);
            const nShaderArg& arg = this->shaderParams.GetArgByIndex(i);
            switch (arg.GetType())
            {
                case nShaderState::Void:
                    break;

                case nShaderState::Int:
                    cmd = ps->GetCmd(this, 'SINT');
                    cmd->In()->SetS(nShaderState::ParamToString(param));
                    cmd->In()->SetI(arg.GetInt());
                    ps->PutCmd(cmd);
                    break;
               case nShaderState::Bool:
                    cmd = ps->GetCmd(this, 'SBOO');
                    cmd->In()->SetS(nShaderState::ParamToString(param));
                    cmd->In()->SetB(arg.GetBool());
                    ps->PutCmd(cmd);
                    break;

                case nShaderState::Float:
                    cmd = ps->GetCmd(this, 'SFLT');
                    cmd->In()->SetS(nShaderState::ParamToString(param));
                    cmd->In()->SetF(arg.GetFloat());
                    ps->PutCmd(cmd);
                    break;

                case nShaderState::Float4:
                    {
                        cmd = ps->GetCmd(this, 'SVEC');
                        const nFloat4& f = arg.GetFloat4();
                        cmd->In()->SetS(nShaderState::ParamToString(param));
                        cmd->In()->SetF(f.x);
                        cmd->In()->SetF(f.y);
                        cmd->In()->SetF(f.z);
                        cmd->In()->SetF(f.w);
                        ps->PutCmd(cmd);
                    }
                    break;

                case nShaderState::Texture:
                    // ignore textures as they are already saved above.
                    break;

                default:
                    n_error("nAbstractShaderNode::SaveCmds(): Invalid variable type!\n");
                    break;
            }
        }

        // shader overrides from instance variables
        //...

        int numPasses = nSceneServer::Instance()->GetNumPasses();

        /// pass enable
        if (!this->GetDefaultPassEnable())
        {
	        cmd = ps->GetCmd(this, 'CSPA'); // enable or diable flags
	        cmd->In()->SetB(this->GetDefaultPassEnable() );
	        ps->PutCmd(cmd);
	
	        cmd = ps->GetCmd(this, 'CDPD'); // set the state
	        cmd->In()->SetB(this->GetDefaultPassEnable() );
	        ps->PutCmd(cmd);
		}

        for (int i = 0; i < numPasses; ++i)
        {
            nCmd *cmd;
            char buf[5];

            nFourCC fourcc = nSceneServer::Instance()->GetPassAt(i);

            bool passEnable = (this->passEnabledFlags & (1<<i)) != 0 ;

            if ((passEnable && !this->defaultPassEnabled) ||
                (!passEnable && this->defaultPassEnabled))
            {
                cmd = ps->GetCmd(this, 'CSPE');
                cmd->In()->SetS(nVariableServer::FourCCToString(fourcc, buf, sizeof(buf)));
                cmd->In()->SetB(passEnable);
                ps->PutCmd(cmd);
            }
        }
        return true;
    }
    return false;
}
