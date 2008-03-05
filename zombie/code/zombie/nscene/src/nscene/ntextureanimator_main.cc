#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntextureanimator_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntextureanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/nabstractshadernode.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nTextureAnimator, "nshaderanimator");

//------------------------------------------------------------------------------
/**
*/
nTextureAnimator::nTextureAnimator() :
    frequency(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTextureAnimator::~nTextureAnimator()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nTextureAnimator::LoadResources()
{
    if (nAnimator::LoadResources())
    {
        this->LoadTextures();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nTextureAnimator::UnloadResources()
{
    nAnimator::UnloadResources();
    this->UnloadTextures();
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nTextureAnimator::LoadTextures()
{
    int numTextures = this->GetNumTextures();
    for (int texIndex = 0; texIndex < numTextures; texIndex++)
    {
        TexNode &texNode = this->textureArray[texIndex];
        nTexture2 *texture = nGfxServer2::Instance()->NewTexture(texNode.texName.Get());
        n_assert(texture);

        if (!texture->IsValid())
        {
            texture->SetFilename(texNode.texName.Get());
            if (!texture->Load())
            {
                return false;
            }
        }
        texNode.refTexture = texture;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nTextureAnimator::UnloadTextures()
{
    int numTextures = this->GetNumTextures();
    for (int texIndex = 0; texIndex < numTextures; texIndex++)
    {
        TexNode& texNode = this->textureArray[texIndex];
        if (texNode.refTexture.isvalid())
        {
            texNode.refTexture->Release();
            texNode.refTexture.invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTextureAnimator::EntityCreated(nEntityObject* entityObject)
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
nTextureAnimator::EntityDestroyed(nEntityObject* entityObject)
{
    nAnimator::EntityDestroyed(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nTextureAnimator::ResetKeys()
{
    this->UnloadTextures();
    this->textureArray.Reset();
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
    This does the actual work of selecting a texture.
    
    @param  sceneNode       object to texture (must be of class nAbstractShaderNode)
    @param  renderContext   current render context
*/
void
nTextureAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(this->channelVarHandle != nVariable::InvalidHandle);
    n_assert(this->param != nShaderState::InvalidParameter);

    //n_assert(sceneNode->IsA( kernelServer->FindClass("nabstractshadernode")));
    nAbstractShaderNode *targetNode = (nAbstractShaderNode *) sceneNode;
    
    // get the active texture index from the render context
    float curTime = this->GetTime(entityObject);

    int texIndex = (int) floor(curTime * this->frequency);
    int numTextures = this->GetNumTextures();

    switch (this->loopType)
    {
    case nAnimLoopType::Loop:
        texIndex = texIndex % numTextures;
        break;
    case nAnimLoopType::PingPong:
        texIndex = abs(texIndex % (numTextures * 2 - 2) - numTextures + 1);
        break;
    case nAnimLoopType::Clamp:
        texIndex = min(texIndex, numTextures - 1);
        break;
    default:
        return;
    }
    
    if (this->textureArray[texIndex].refTexture.isvalid())
    {
        nTexture2 *texture = this->textureArray[texIndex].refTexture.get();
        targetNode->SetTextureOverride(this->param, texture);
    }
}
