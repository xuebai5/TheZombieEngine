#ifndef N_TEXTUREKEYANIMATOR_H
#define N_TEXTUREKEYANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nTextureKeyAnimator
    @ingroup SceneAnimators
    
    @brief nTextureKeyAnimator selects one of several possible textures
    sampling a key from a sequence. It is intended to perform smooth
    texture transitions, instead of the steady texture animation 
    provided by nTextureAnimator.
    
    nTextureKeyAnimator does not perform texture blending, but several
    texture animator can be used on the same surface with a constant offset
    in their keys.
    
    See also @ref N2ScriptInterface_ntexturekeyanimator
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/nshaderanimator.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class nTextureKeyAnimator : public nShaderAnimator
{
public:
    /// constructor
    nTextureKeyAnimator();
    /// destructor
    ~nTextureKeyAnimator();
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
    
    //<OBSOLETE> keep for backwards compatibility
    /// set the name of the texture parameter to manipulate
    void SetTextureName(const char* name);
    /// get the name of the texture parameter to manipulate
    const char* GetTextureName() const;
    //</OBSOLETE>

    /// reset animator keys
    virtual void ResetKeys();

    /// Add a texture to the key array
    void AddKey(float time, const char* path);
    /// get number of texture keys
    int GetNumKeys() const;
    /// get texture key at
    void GetKeyAt(int index, float& time, const char*& path) const;
    
protected:
    struct TextureKey
    {
        float time;
        nString texName;
        nRef<nTexture2> refTexture;
    };
    
    nArray<TextureKey> keyArray;
    
    /// load texture resources
    virtual bool LoadTextures();
    /// unload texture resources
    virtual void UnloadTextures();
    /// get an interpolated key
    nTexture2* SampleKey(float time);
};

//<OBSOLETE> keep for backwards compatibility
//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureKeyAnimator::SetTextureName(const char* param)
{
    nShaderAnimator::SetParamName(param);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nTextureKeyAnimator::GetTextureName() const
{
    return nShaderAnimator::GetParamName();
}
//</OBSOLETE>

//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureKeyAnimator::AddKey(const float time, const char* path)
{ 
    TextureKey newTexKey;
    newTexKey.time = time;
    newTexKey.texName = path;
    this->keyArray.PushBack(newTexKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTextureKeyAnimator::GetNumKeys() const
{ 
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureKeyAnimator::GetKeyAt(int index, float& time, const char*& path) const
{
    n_assert(index < this->GetNumKeys());
    TextureKey& textureKey = this->keyArray[index];
    time = textureKey.time;
    path = textureKey.texName.Get();
}

//------------------------------------------------------------------------------
#endif
