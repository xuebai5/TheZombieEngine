#ifndef N_TEXTUREANIMATOR_H
#define N_TEXTUREANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nTextureAnimator
    @ingroup SceneAnimators

    @brief nTextureAnimator selects between one of several possible textures.

    See also @ref N2ScriptInterface_ntextureanimator

    (C) 2004 Conjurer Services, S.A.
*/

#include "nscene/nshaderanimator.h"
#include "kernel/nref.h"
#include "util/narray.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class nTextureAnimator : public nShaderAnimator
{
public:
    /// constructor
    nTextureAnimator();
    /// destructor
    virtual ~nTextureAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by app when render context is going to be released
    virtual void EntityDestroyed(nEntityObject* entityObject);
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

    /// reset all keys
    virtual void ResetKeys();

    /// Add a texture to the array (with index number equal to the number of textures added thus far)
    void AddTexture(const char* path);
    /// Returns the number of textures that have been added so far
    int GetNumTextures() const;
    /// Returns the nth texture in the (zero based) array
    nString GetTextureAt(int index) const;

    //<OBSOLETE>
    /// Sets the shader state parameter that will be passed to nAbstractShaderNode::SetTexture
    void SetShaderParam(const char* param);
    /// a convenience function for SaveCmds
    const char* GetShaderParam() const;
    //</OBSOLETE>

    /// set the frequency of the animation (frames per second)
    void SetFrequency(const int frequency);
    /// get the frequency of the animation (frames per second)
    const ushort GetFrequency();

protected:
    /// load texture resources
    virtual bool LoadTextures();
    /// unload texture resources
    virtual void UnloadTextures();

    struct TexNode
    {
        nString texName;
        nRef<nTexture2> refTexture;
    };
    
    nArray<TexNode> textureArray;
    ushort frequency;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureAnimator::SetShaderParam(const char* param)
{
    nShaderAnimator::SetParamName(param);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureAnimator::SetFrequency(const int frequency)
{
    this->frequency = (short) frequency;
}

//------------------------------------------------------------------------------
/**
*/
inline
const ushort
nTextureAnimator::GetFrequency()
{
    return this->frequency;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureAnimator::AddTexture(const char* path)
{ 
    TexNode newTexNode;
    newTexNode.texName = path;
    this->textureArray.PushBack(newTexNode);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTextureAnimator::GetNumTextures() const
{
    return this->textureArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nTextureAnimator::GetTextureAt(int index) const
{
    n_assert(index < this->GetNumTextures());
    return this->textureArray[index].texName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nTextureAnimator::GetShaderParam() const
{
    return nShaderAnimator::GetParamName();
}

//------------------------------------------------------------------------------
#endif
