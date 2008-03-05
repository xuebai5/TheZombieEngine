#ifndef N_ABSTRACTSHADERNODE_H
#define N_ABSTRACTSHADERNODE_H
//------------------------------------------------------------------------------
/**
    @class nAbstractShaderNode
    @ingroup Scene

    @brief This is the base class for all shader related scene node classes
    (for instance material and light nodes).
    
    All those classes need to hold named, typed shader variables, as well
    as texture resource management.

    See also @ref N2ScriptInterface_nabstractshadernode
    
    (C) 2003 RadonLabs GmbH
*/
#include "nscene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nshaderparams.h"
#include "mathlib/transform33.h"
#include "variable/nvariableserver.h"

class nSceneGraph;

//------------------------------------------------------------------------------
class nAbstractShaderNode : public nSceneNode
{
public:
    /// constructor
    nAbstractShaderNode();
    /// destructor
    virtual ~nAbstractShaderNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// apply per set shader parameters
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// render per instance shader parameters
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    
    /// set uv position for texture layer
    void SetUvPos(uint layer, const vector2& p);
    /// get uv position for texture layer
    const vector2& GetUvPos(uint layer) const;
    /// set uv euler rotation for texture layer
    void SetUvEuler(uint layer, const vector2& p);
    /// get uv euler rotation for texture layer
    const vector2& GetUvEuler(uint layer) const;
    /// set uv scale for texture layer
    void SetUvScale(uint layer, const vector2& p);
    /// get uv scale for texture layer
    const vector2& GetUvScale(uint layer) const;
    
    /// bind a texture resource to a shader variable
    void SetTexture(nShaderState::Param param, const char* texName);
    /// get texture resource bound to variable
    const char* GetTexture(nShaderState::Param param) const;
    /// bind a int value to a a shader variable
    void SetInt(nShaderState::Param param, int val);
    /// get an int value bound to a shader variable
    int GetInt(nShaderState::Param param) const;
    /// bind a bool value to a a shader variable
    void SetBool(nShaderState::Param param, bool val);
    /// get an bool value bound to a shader variable
    bool GetBool(nShaderState::Param param) const;
    /// bind a float value to a shader variable
    void SetFloat(nShaderState::Param param, float val);
    /// get a float value bound to a shader variable
    float GetFloat(nShaderState::Param param) const;
    /// bind a vector value to a shader variable
    void SetVector(nShaderState::Param param, const vector4& val);
    /// get a vector value bound to a shader variable
    const vector4& GetVector(nShaderState::Param param) const;
    /// get shaderparams
    nShaderParams& GetShaderParams();
    /// returns true, if node possesses the param
    bool HasParam(nShaderState::Param param);
    
    /// get number of textures
    int GetNumTextures() const;
    /// get texture resource name at index
    const char* GetTextureAt(int index) const;
    /// get texture shader parameter at index
    nShaderState::Param GetTextureParamAt(int index) const;
    /// clear texture parameter
    void ClearTexture(nShaderState::Param param);
    /// clear texture parameter at index
    void ClearTextureAt(int index);
    
    /// get number of parameters
    int GetNumParams() const;
    /// get parameter name by index
    const char* GetParamNameByIndex(int index) const;
    /// get parameter type by index
    const char* GetParamTypeByIndex(int index) const;
    
    /// initialize overrides (for debug options only)
    void SetShaderOverrides(nShaderParams& shaderOverrides);
    /// override int param (for animators)
    void SetIntOverride(nShaderState::Param param, int val);
    /// override bool param (for animators)
    void SetBoolOverride(nShaderState::Param param, bool val);
    /// override float param (for animators)
    void SetFloatOverride(nShaderState::Param param, float val);
    /// override vector param (for animators)
    void SetVectorOverride(nShaderState::Param param, const vector4& val);
    /// override texture param (for animators)
    void SetTextureOverride(nShaderState::Param param, nTexture2 *texture);

    /// set override shader param (for variables)
    void SetParamFromVar(nShaderState::Param param, const char* varName);
    /// get override shader param (for variables)
    const char* GetParamFromVar(nShaderState::Param param);

    /// set selector type
    void SetSelectorType(const nFourCC fourcc);
    /// get selector type
    nFourCC GetSelectorType();
    /// set pass disabled
    void SetPassEnabled(nFourCC fourcc, bool value);
    /// get pass disabled
    bool GetPassEnabled(nFourCC fourcc) const;
    /// return pass flags 
    uint GetPassEnabledFlags() const;
    /// use in script, put all true or false
    void SetPassAllFlags(bool val);
    /// if pass is not defined in persistence the default value
    void SetDefaultPassEnable(bool val);
    /// Get if pass is not defined in persistence the default value
    bool GetDefaultPassEnable() const;
    
protected:
    /// load a texture resource
    bool LoadTexture(int index);
    /// unload a texture resource
    void UnloadTexture(int index);
    /// abstract method: returns always true
    virtual bool IsTextureUsed(nShaderState::Param param);
    /// transfer standard parameters to shader (matrices, etc...)
    void UpdateShader(nShader2* shd, nEntityObject* entityObject);
    /// clear array of parameter variable override
    void ClearVarParams();
    
    class TexNode
    {
    public:
        /// default constructor
        TexNode();
        /// constructor
        TexNode(nShaderState::Param shaderParam, const char* texName);

        nShaderState::Param shaderParameter;
        nString texName;
        nRef<nTexture2> refTexture;
    };

    nArray<TexNode> texNodeArray;

    nFourCC selectorType;

    transform33 textureTransform[nGfxServer2::MaxTextureStages];
    bool useTextureTransform[nGfxServer2::MaxTextureStages];
    nShaderParams shaderParams;

    /// parameters overriden from animators
    nShaderParams shaderOverrides; 
    
    /// parameters overriden from entity variables
    struct ParamAndVar
    {
    public:
        ParamAndVar();
        ParamAndVar(nShaderState::Param, nVariable::Handle);
        nVariable::Handle varHandle;
        nShaderState::Param param;
    };
    nArray<ParamAndVar> varParamArray;
    char paramIndex[nShaderState::NumParameters];// index into varParamArray, -1 for invalid params
    uint passEnabledFlags;
    /// if default value pass for not defined.
    bool defaultPassEnabled;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::TexNode::TexNode() :
    shaderParameter(nShaderState::InvalidParameter)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::TexNode::TexNode(nShaderState::Param shaderParam, const char* name) :
    shaderParameter(shaderParam),
    texName(name)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::ClearVarParams()
{
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        this->paramIndex[i] = -1;
    }
    this->varParamArray.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetUvPos(uint layer, const vector2& p)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->textureTransform[layer].settranslation(p);
    this->useTextureTransform[layer] = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nAbstractShaderNode::GetUvPos(uint layer) const
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->textureTransform[layer].gettranslation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetUvEuler(uint layer, const vector2& e)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->textureTransform[layer].seteulerrotation(e);
    this->useTextureTransform[layer] = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nAbstractShaderNode::GetUvEuler(uint layer) const
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->textureTransform[layer].geteulerrotation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetUvScale(uint layer, const vector2& s)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->textureTransform[layer].setscale(s);
    this->useTextureTransform[layer] = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nAbstractShaderNode::GetUvScale(uint layer) const
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->textureTransform[layer].getscale();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetInt(nShaderState::Param param, int val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAbstractShaderNode::GetInt(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetBool(nShaderState::Param param, bool val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAbstractShaderNode::GetBool(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetFloat(nShaderState::Param param, float val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAbstractShaderNode::GetFloat(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetVector(nShaderState::Param param, const vector4& val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderParams.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nAbstractShaderNode::GetVector(nShaderState::Param param) const
{
    return this->shaderParams.GetArg(param).GetVector4();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::ParamAndVar::ParamAndVar() :
    param(nShaderState::InvalidParameter),
    varHandle(nVariable::InvalidHandle)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAbstractShaderNode::ParamAndVar::ParamAndVar(nShaderState::Param p, nVariable::Handle h) :
    param(p),
    varHandle(h)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetParamFromVar(nShaderState::Param param, const char* name)
{
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    int paramIndex = this->paramIndex[param];
    if (paramIndex == -1)
    {
        this->varParamArray.Append(ParamAndVar(param, varHandle));
    }
    else
    {
        this->varParamArray.At(paramIndex).varHandle = varHandle;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAbstractShaderNode::GetParamFromVar(nShaderState::Param param)
{
    int paramIndex = this->paramIndex[param];
    if (paramIndex == -1)
    {
        return "";
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->varParamArray.At(paramIndex).varHandle);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAbstractShaderNode::GetNumTextures() const
{
    return this->texNodeArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAbstractShaderNode::GetTextureAt(int index) const
{
    return this->texNodeArray[index].texName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderState::Param
nAbstractShaderNode::GetTextureParamAt(int index) const
{
    return this->texNodeArray[index].shaderParameter;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::ClearTextureAt(int index)
{
    this->texNodeArray.EraseQuick(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAbstractShaderNode::GetNumParams() const
{
    return this->shaderParams.GetNumValidParams();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAbstractShaderNode::GetParamNameByIndex(int index) const
{
  return nShaderState::ParamToString(this->shaderParams.GetParamByIndex(index));
}
//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAbstractShaderNode::GetParamTypeByIndex(int index) const
{
  return nShaderState::TypeToString(this->shaderParams.GetArgByIndex(index).GetType());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetSelectorType(const nFourCC fourcc)
{
    this->selectorType = fourcc;
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nAbstractShaderNode::GetSelectorType()
{
    return this->selectorType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetShaderOverrides(nShaderParams& shaderOverrides)
{
    this->shaderOverrides.Copy(shaderOverrides);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetIntOverride(nShaderState::Param param, int val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderOverrides.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetBoolOverride(nShaderState::Param param, bool val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderOverrides.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetFloatOverride(nShaderState::Param param, float val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    this->shaderOverrides.SetArg(param, nShaderArg(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetVectorOverride(nShaderState::Param param, const vector4& val)
{
    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }
    nFloat4 float4Val = 
    {
        val.x, val.y, val.z, val.w
    };
    this->shaderOverrides.SetArg(param, nShaderArg(float4Val));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAbstractShaderNode::SetTextureOverride(nShaderState::Param shaderParam, nTexture2 *texture)
{
    this->shaderOverrides.SetArg(shaderParam, nShaderArg(texture));
}

//------------------------------------------------------------------------------
#endif
