#include "precompiled/pchnnebula.h"
//------------------------------------------------------------------------------
//  ntexturearray.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gfx2/ntexturearray.h"

//------------------------------------------------------------------------------
/**
*/
void
nTextureArray::SetTexture(nShaderState::Param param, const char* texName)
{
    n_assert(texName);

    // silently ignore invalid parameters
    if (nShaderState::InvalidParameter == param)
    {
        n_printf("WARNING: invalid shader parameter\n");
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
nTextureArray::GetTexture(nShaderState::Param param) const
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
    Load texture resources.
*/
bool
nTextureArray::LoadResources()
{
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        this->LoadTexture(i);
    }
    this->resourcesValid = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload texture resources.
*/
void
nTextureArray::UnloadResources()
{
    int i;
    int num = this->texNodeArray.Size();
    for (i = 0; i < num; i++)
    {
        this->UnloadTexture(i);
    }
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
    Unload a texture resource.
*/
void
nTextureArray::UnloadTexture(int index)
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
nTextureArray::LoadTexture(int index)
{
    TexNode& texNode = this->texNodeArray[index];
    if ((!texNode.refTexture.isvalid()) && (!texNode.texName.IsEmpty()))
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
    return true;
}
