#ifndef N_SURFACENODE_H
#define N_SURFACENODE_H
//------------------------------------------------------------------------------
/**
    @class nSurfaceNode
    @ingroup SceneNodes

    @brief A material node defines a shader resource and associated shader
    variables. A shader resource is an external file (usually a text file)
    which defines a surface shader.
    
    Material nodes themselves cannot render anything useful, they can just
    adjust render states as preparation of an actual rendering process.
    Thus, subclasses should be derived which implement some sort of
    shape rendering.
    
    See also @ref N2ScriptInterface_nsurfacenode

    (C) 2002 RadonLabs GmbH
*/
#include "nscene/nabstractshadernode.h"
#include "kernel/nref.h"

class nShader2;
class nGfxServer2;
class nSceneGraph;

//------------------------------------------------------------------------------
class nSurfaceNode : public nAbstractShaderNode
{
public:
    /// constructor
    nSurfaceNode();
    /// destructor
    virtual ~nSurfaceNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// apply shader
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// render shader
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// get number of render levels
    virtual int GetNumLevels();
    /// get number of render passes by level
    virtual int GetNumLevelPasses(int level);
    /// get scene pass index by level and pass
    virtual int GetLevelPassIndex(int level, int pass);

    /// get shader tree
    virtual nShaderTree* GetShaderTree(int level, int passIndex);

    /// set shader resource name
    void SetShader(nFourCC fourcc, const char* name);
    /// get shader resource name
    const char* GetShader(nFourCC fourcc) const;
    /// get number of shaders
    int GetNumShaders() const;
    /// get shader at index
    void GetShaderAt(int index, nFourCC& fourcc, const char*& name) const;

protected:
    class ShaderEntry
    {
    public:
        /// default constructor
        ShaderEntry();
        /// constructor
        ShaderEntry(nFourCC shaderFourCC, const char* shaderName);
        /// set fourcc code
        void SetFourCC(nFourCC shaderFourCC);
        /// get shader fourcc code
        nFourCC GetFourCC() const;
        /// get shader pass index
        int GetPassIndex() const;
        /// set shader name
        void SetName(const char* shaderName);
        /// get shader name
        const char* GetName() const;
        /// set shader
        void SetShader(nShader2* shd);
        /// get shader pointer
        nShader2* GetShader() const;
        /// set bucket index of shader
        void SetShaderIndex(int shaderIndex);
        /// set bucket index of shader
        int GetShaderIndex() const;
        /// is shader ref valid?
        bool IsShaderValid() const;
        /// invalidate entry
        void Invalidate();

        nFourCC fourcc;
        int passIndex;
        nString name;
        int shaderIndex;

        nRef<nShader2> refShader;
    };

    /// load the shader resource
    bool LoadShaders();
    /// unload the shader resource
    void UnloadShaders();
    /// find a shader entry by its fourcc code
    ShaderEntry* FindShaderEntry(nFourCC fourcc) const;
    /// checks if shader uses texture passed in param
    virtual bool IsTextureUsed(nShaderState::Param param);

    nArray<ShaderEntry> shaderArray;
    nArray<nRef<nShaderTree> > shaderTreeArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nSurfaceNode::GetNumShaders() const
{
    return this->shaderArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return shader attributes at given index.

    @param  index   [in] index
    @param  fourcc  [out] the fourcc code associated with the shader
    @param  name    [out] the resource name of the shader
*/
inline
void
nSurfaceNode::GetShaderAt(int index, nFourCC& fourcc, const char*& name) const
{
    fourcc = this->shaderArray[index].GetFourCC();
    name   = this->shaderArray[index].GetName();
}

//------------------------------------------------------------------------------
/**
*/
inline
nSurfaceNode::ShaderEntry::ShaderEntry() :
    fourcc(0),
    passIndex(-1),
    shaderIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nSurfaceNode::ShaderEntry::ShaderEntry(nFourCC shaderFourCC, const char* shaderName) :
    fourcc(shaderFourCC),
    name(shaderName),
    shaderIndex(-1)
{
    // empty
    this->passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(shaderFourCC);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSurfaceNode::ShaderEntry::SetFourCC(nFourCC shaderFourCC)
{
    this->fourcc = shaderFourCC;
    this->passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(shaderFourCC);
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nSurfaceNode::ShaderEntry::GetFourCC() const
{
    return this->fourcc;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSurfaceNode::ShaderEntry::GetPassIndex() const
{
    return this->passIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSurfaceNode::ShaderEntry::SetName(const char* shaderName)
{
    n_assert(shaderName);
    this->name = shaderName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nSurfaceNode::ShaderEntry::GetName() const
{
    return this->name.IsEmpty() ? 0 : this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSurfaceNode::ShaderEntry::IsShaderValid() const
{
    return this->refShader.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nSurfaceNode::ShaderEntry::GetShader() const
{
    return this->refShader.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSurfaceNode::ShaderEntry::Invalidate()
{
    this->refShader.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSurfaceNode::ShaderEntry::SetShader(nShader2* shd)
{
    this->refShader = shd;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSurfaceNode::ShaderEntry::SetShaderIndex(int shaderIndex)
{
    this->shaderIndex = shaderIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSurfaceNode::ShaderEntry::GetShaderIndex() const
{
    return this->shaderIndex;
}

//------------------------------------------------------------------------------
#endif
