#ifndef N_TEXTUREARRAY_H
#define N_TEXTUREARRAY_H
//------------------------------------------------------------------------------
/**
    @class nTextureArray
    @ingroup NebulaGraphicsSystem
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A class that encapsulates texture handling by resource name.
    It implements common behaviors for loading and setting parameters.
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "gfx2/nshaderparams.h"

class nTexture2;
//------------------------------------------------------------------------------
class nTextureArray
{
public:
    /// constructor
    nTextureArray();
    /// destructor
    ~nTextureArray();

    /// load texture resources
    bool LoadResources();
    /// unload texture resources
    void UnloadResources();

    /// bind a texture resource to a shader variable
    void SetTexture(nShaderState::Param param, const char* texName);
    /// get texture resource bound to variable
    const char* GetTexture(nShaderState::Param param) const;
    /// get number of textures
    int GetNumTextures() const;
    /// get texture resource name at index
    const char* GetTextureAt(int index) const;
    /// get texture shader parameter at index
    nShaderState::Param GetTextureParamAt(int index) const;
    /// get texture params
    nShaderParams& GetShaderParams();

private:
    /// load a texture resource
    bool LoadTexture(int index);
    /// unload a texture resource
    void UnloadTexture(int index);

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
    nShaderParams shaderParams;
    bool resourcesValid;
};

//------------------------------------------------------------------------------
/**
*/
inline
nTextureArray::TexNode::TexNode() :
    shaderParameter(nShaderState::InvalidParameter)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nTextureArray::TexNode::TexNode(nShaderState::Param shaderParam, const char* name) :
    shaderParameter(shaderParam),
    texName(name)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nTextureArray::nTextureArray() :
    resourcesValid(false)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nTextureArray::~nTextureArray()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTextureArray::GetNumTextures() const
{
    return this->texNodeArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nTextureArray::GetTextureAt(int index) const
{
    return this->texNodeArray[index].texName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderState::Param
nTextureArray::GetTextureParamAt(int index) const
{
    return this->texNodeArray[index].shaderParameter;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderParams&
nTextureArray::GetShaderParams()
{
    return this->shaderParams;
}

//------------------------------------------------------------------------------
#endif
