#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nabstractshadernode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nabstractshadernode.h"
#include "nscene/ncscene.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/ntimeserver.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nAbstractShaderNode, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nAbstractShaderNode::nAbstractShaderNode() :
    selectorType(0),
    passEnabledFlags(0xffffffff),
    defaultPassEnabled(true)
{
    this->ClearVarParams();

    int layer;
    for (layer = 0; layer < nGfxServer2::MaxTextureStages; ++layer)
    {
        this->useTextureTransform[layer] = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
nAbstractShaderNode::~nAbstractShaderNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Unload a texture resource.
*/
void
nAbstractShaderNode::UnloadTexture(int index)
{
    TexNode& texNode = this->texNodeArray[index];
    if (texNode.refTexture.isvalid())
    {
        texNode.refTexture->Release();
        texNode.refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Load a texture resource.
*/
bool
nAbstractShaderNode::LoadTexture(int index)
{
    TexNode& texNode = this->texNodeArray[index];
    if ((!texNode.refTexture.isvalid()) && (!texNode.texName.IsEmpty()))
    {
        // load only if the texture is used in the shader
        if (this->IsTextureUsed(texNode.shaderParameter))
        {
            nTexture2* tex = nGfxServer2::Instance()->NewTexture(texNode.texName.Get());
            n_assert(tex);
            if (!tex->IsLoaded())
            {
                tex->SetFilename(texNode.texName.Get());
                if (!tex->Load())
                {
                    n_printf("nAbstractShaderNode: Error loading texture '%s'\n", texNode.texName.Get());
                    return false;
                }
            }
            texNode.refTexture = tex;
            this->shaderParams.SetArg(texNode.shaderParameter, nShaderArg(tex));
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nAbstractShaderNode::LoadResources()
{
    if (nSceneNode::LoadResources())
    {
        int i;
        int num = this->texNodeArray.Size();
        for (i = 0; i < num; i++)
        {
            this->LoadTexture(i);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nAbstractShaderNode::UnloadResources()
{
    nSceneNode::UnloadResources();
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        this->UnloadTexture(i);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nAbstractShaderNode::SetTexture(nShaderState::Param param, const char* texName)
{
    n_assert(texName);

    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter in object '%s'\n", this->GetName());
        return;
    }

    // see if texture variable already exists
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].shaderParameter == param)
        {
            break;
        }
    }
    if (i == num)
    {
        // add new texnode to array
        TexNode newTexNode(param, texName);
        this->texNodeArray.Append(newTexNode);
    }
    else
    {
        // invalidate existing texture
        this->UnloadTexture(i);
        this->texNodeArray[i].texName = texName;
    }
    // flag to load resources
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nAbstractShaderNode::GetTexture(nShaderState::Param param) const
{
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].shaderParameter == param)
        {
            return texNodeArray[i].texName.Get();
        }
    }
    // fallthrough: invalid variable name
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nAbstractShaderNode::ClearTexture(nShaderState::Param param)
{
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->texNodeArray[i].shaderParameter == param)
        {
            texNodeArray.Erase(i);
            return;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nAbstractShaderNode::IsTextureUsed(nShaderState::Param /*param*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This sets standard parameters, like the various matrices in the
    provided shader object. Provided to subclasses as a convenience method.
*/
void
nAbstractShaderNode::UpdateShader(nShader2* shd, nEntityObject* entityObject)
{
    n_assert(shd);
    n_assert(entityObject);

    // write global parameters the shader
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    //const matrix44& invModelView  = gfxServer->GetTransform(nGfxServer2::InvModelView);
    if (shd->IsParameterUsed(nShaderState::Time))
    {
        nTime time = this->kernelServer->GetTimeServer()->GetTime();
        shd->SetFloat(nShaderState::Time, float(time));
    }

    // FIXME: this should be a shared shader parameter
    if (shd->IsParameterUsed(nShaderState::DisplayResolution))
    {
        const nDisplayMode2& mode = gfxServer->GetDisplayMode();
        nFloat4 dispRes;
        dispRes.x = (float) mode.GetWidth();
        dispRes.y = (float) mode.GetHeight();
        dispRes.z = 0.0f;
        dispRes.w = 0.0f;
        shd->SetFloat4(nShaderState::DisplayResolution, dispRes);
    }

    // set shader overrides
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    shd->SetParams(renderContext->GetShaderOverrides());
}

//------------------------------------------------------------------------------
/**
    Set per instance set shader parameters to current shader.
*/
bool
nAbstractShaderNode::Apply(nSceneGraph* /*sceneGraph*/)
{
    nShader2* shader = nGfxServer2::Instance()->GetShader();
    n_assert(shader);

    // apply int, float and vector variables to the shader
    // transfer shader parameters en block
    shader->SetParams(this->shaderParams);
    
    return true;
}

//------------------------------------------------------------------------------
/**
    Set owned parameters to current shader. 
    This is provided as a convenience for reusing code in derived classes.
    Should be called by subclasses in their own Render() implementation.
*/
bool
nAbstractShaderNode::Render(nSceneGraph* /*sceneGraph*/, nEntityObject* entityObject)
{
    nShader2* shader = nGfxServer2::Instance()->GetShader();
    n_assert(shader);
    
    // resolve parameters assigned from variables
    ncDictionary* varContext = entityObject->GetComponent<ncDictionary>();
    if (varContext)
    {
        int index;
        for (index = 0; index < this->varParamArray.Size(); ++index)
        {
            ParamAndVar& paramAndVar = this->varParamArray.At(index);
            if (shader->IsParameterUsed(paramAndVar.param))
            {
                nVariable* var = varContext->GetVariable(paramAndVar.varHandle);
                if (var)
                {
                    switch (var->GetType())
                    {
                    case nVariable::Vector4:
                        shader->SetVector4(paramAndVar.param, var->GetVector4());
                        break;
                    //...rest of parameter types
                    }
                }
            }
        }
    }

    // set texture transforms if assigned
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    static matrix44 m;
    if (this->useTextureTransform[0])
    {
        this->textureTransform[0].getmatrix44(m);
        gfxServer->SetTransform(nGfxServer2::Texture0, m);
    }

    if (this->useTextureTransform[1])
    {
        this->textureTransform[1].getmatrix44(m);
        gfxServer->SetTransform(nGfxServer2::Texture1, m);
    }

    // transfer parameters overriden from animators
    shader->SetParams(this->shaderOverrides);

    return true;
}

//------------------------------------------------------------------------------
/**
   Returns true, if param is valid
*/
bool
nAbstractShaderNode::HasParam(nShaderState::Param param)
{
    return this->shaderParams.IsParameterValid(param);
}

//------------------------------------------------------------------------------
/**
   get nShaderParams from node
*/
nShaderParams&
nAbstractShaderNode::GetShaderParams()
{
    return this->shaderParams;
}

//------------------------------------------------------------------------------
/**
    set pass disabled
*/
void
nAbstractShaderNode::SetPassEnabled(nFourCC fourcc, bool value)
{
    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
    if (value)
    {
        this->passEnabledFlags |= (1<<passIndex);
    }
    else
    {
        this->passEnabledFlags &= ~(1<<passIndex);
    }
}

//------------------------------------------------------------------------------
/**
    get pass disabled
*/
bool
nAbstractShaderNode::GetPassEnabled(nFourCC fourcc) const
{
    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
    return ((this->passEnabledFlags & (1<<passIndex)) != 0);
}

//------------------------------------------------------------------------------
/**
    get pass disabled
*/
uint 
nAbstractShaderNode::GetPassEnabledFlags() const
{
    return this->passEnabledFlags;
}

//------------------------------------------------------------------------------
/**
    set default pass is enable
*/
void
nAbstractShaderNode::SetDefaultPassEnable(bool value)
{
    this->defaultPassEnabled = value;
}
//------------------------------------------------------------------------------
/**
    get default pass is enable
*/
bool
nAbstractShaderNode::GetDefaultPassEnable() const
{
    return this->defaultPassEnabled;
}

//------------------------------------------------------------------------------
/**
    set pass disabled
*/
void
nAbstractShaderNode::SetPassAllFlags(bool value)
{
    this->passEnabledFlags = 0;
    if (value)
    {
        this->passEnabledFlags = ~this->passEnabledFlags;
    }
}

