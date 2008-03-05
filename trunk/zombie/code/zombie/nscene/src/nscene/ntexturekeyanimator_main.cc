#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntextureanimator_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntexturekeyanimator.h"
#include "nscene/nabstractshadernode.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nTextureKeyAnimator, "nshaderanimator");

//------------------------------------------------------------------------------
/**
*/
nTextureKeyAnimator::nTextureKeyAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTextureKeyAnimator::~nTextureKeyAnimator()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nTextureKeyAnimator::LoadResources()
{
    if (this->LoadTextures())
    {
        return nAnimator::LoadResources();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nTextureKeyAnimator::UnloadResources()
{
    this->UnloadTextures();
    nAnimator::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nTextureKeyAnimator::LoadTextures()
{
    int numTextures = this->GetNumKeys();
    for (int texIndex = 0; texIndex < numTextures; texIndex++)
    {
        TextureKey& textureKey = this->keyArray[texIndex];
        nTexture2 *texture = nGfxServer2::Instance()->NewTexture(textureKey.texName.Get());
        n_assert(texture);
        
        if (!texture->IsValid())
        {
            texture->SetFilename(textureKey.texName.Get());
            if (!texture->Load())
            {
                return false;
            }
        }
        textureKey.refTexture = texture;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nTextureKeyAnimator::UnloadTextures()
{
    int numTextures = this->GetNumKeys();
    for (int texIndex = 0; texIndex < numTextures; texIndex++)
    {
        TextureKey textureKey = this->keyArray[texIndex];
        if (textureKey.refTexture.isvalid())
        {
            textureKey.refTexture->Unload();
            textureKey.refTexture.invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTextureKeyAnimator::EntityCreated(nEntityObject* entityObject)
{
    nAnimator::EntityCreated(entityObject);

    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTextureKeyAnimator::EntityDestroyed(nEntityObject* entityObject)
{
    nAnimator::EntityDestroyed(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nTextureKeyAnimator::ResetKeys()
{
    this->UnloadResources();
    this->keyArray.Reset();
}

//------------------------------------------------------------------------------
/**
    This does the actual work of selecting a texture.
    
    @param  sceneNode       object to texture (must be of class nAbstractShaderNode)
    @param  renderContext   current render context
*/
void
nTextureKeyAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(this->channelVarHandle != nVariable::InvalidHandle);
    n_assert(this->param != nShaderState::InvalidParameter);
    
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;
    n_assert(targetNode->IsA(kernelServer->FindClass("nabstractshadernode")));
    
    //ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //float curTime = (float) renderContext->GetTime();
    float curTime = this->GetTime(entityObject);
    
    nTexture2 *texture = this->SampleKey(curTime);
    targetNode->SetTextureOverride(this->param, texture);
}

//------------------------------------------------------------------------------
/**
    Compute an interpolated key value from a time and a key array.

    @param  sampleTime  [in] time position at which to sample
    @param  keyArray    [in] reference of a key array containing the keys
    @param  result      [out] the result
    @return             true if the result is valid, false otherwise
*/
nTexture2 *
nTextureKeyAnimator::SampleKey(float sampleTime)
{
    if (keyArray.Size() > 1)
    {
        float minTime = keyArray.Front().time;
        float maxTime = keyArray.Back().time;
        if (maxTime > 0.0f)
        {
            if (this->GetLoopType() == nAnimLoopType::Loop)
            {
                // in loop mode, wrap time into loop time
                sampleTime = sampleTime - (float(floor(sampleTime / maxTime)) * maxTime);
            }
            
            // clamp time to range
            if (sampleTime < minTime)       sampleTime = minTime;
            else if (sampleTime >= maxTime) sampleTime = maxTime - 0.001f;
            
            // find the surrounding keys
            n_assert(keyArray.Front().time == 0.0f);
            int i = 0;
            while (keyArray[i].time <= sampleTime)
            {
                i++;
            }
            n_assert((i > 0) && (i < keyArray.Size()));
            
            const TextureKey& key0 = keyArray[i - 1];
            //const TextureKey& key1 = keyArray[i];
            //float time0 = key0.time;
            //float time1 = key1.time;
            
            // compute the actual interpolated values
            //float lerp;
            //if (time1 > time0) lerp = (float) ((sampleTime - time0) / (time1 - time0));
            //else               lerp = 1.0f;
            
            //result = n_frnd((float)key0.value + (((float)key1.value - (float)key0.value) * lerp));
            //return true;
            
            return key0.refTexture.get();
        }
    }

    return false;
}
