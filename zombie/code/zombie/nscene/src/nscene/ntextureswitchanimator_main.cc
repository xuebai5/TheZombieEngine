#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntextureswitchanimator_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntextureswitchanimator.h"
#include "nscene/nabstractshadernode.h"
#include "zombieentity/ncdictionary.h"

nNebulaClass(nTextureSwitchAnimator, "ntextureanimator");

//------------------------------------------------------------------------------
/**
*/
nTextureSwitchAnimator::nTextureSwitchAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTextureSwitchAnimator::~nTextureSwitchAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This does the actual work of selecting a texture.
    
    @param  sceneNode       object to texture (must be of class nAbstractShaderNode)
    @param  renderContext   current render context
*/
void
nTextureSwitchAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(this->channelVarHandle != nVariable::InvalidHandle);
    n_assert(this->param != nShaderState::InvalidParameter);

    //n_assert(sceneNode->IsA( kernelServer->FindClass("nabstractshadernode")));
    nAbstractShaderNode *targetNode = (nAbstractShaderNode*) sceneNode;

    int numTextures = this->GetNumTextures();
    if (numTextures > 0)
    {
        ncDictionary* varContext = entityObject->GetComponentSafe<ncDictionary>();
        nVariable* var = varContext->GetVariable(this->channelVarHandle);
        if (var)
        {
            int texIndex = var->GetInt();
            if (this->textureArray[texIndex].refTexture.isvalid())
            {
                nTexture2 *texture = this->textureArray[texIndex].refTexture.get();
                targetNode->SetTextureOverride(this->param, texture);
            }
        }
    }
}
