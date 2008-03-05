#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nuvanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nuvanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/nabstractshadernode.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nUvAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nUvAnimator::nUvAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nUvAnimator::~nUvAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nUvAnimator::GetAnimatorType() const
{
    return Shader;
}

//------------------------------------------------------------------------------
/**
*/
void
nUvAnimator::ResetKeys()
{
    int texLayer;
    for (texLayer = 0; texLayer < nGfxServer2::MaxTextureStages; texLayer++)
    {
        this->posArray[texLayer].Reset();
        this->eulerArray[texLayer].Reset();
        this->scaleArray[texLayer].Reset();
    }
}

//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nTransformNode)
    @param  renderContext   current render context
*/
void
nUvAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nAbstractShaderNode!!!
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;

    float curTime = this->GetTime( entityObject );

    int texLayer;
    for (texLayer = 0; texLayer < nGfxServer2::MaxTextureStages; texLayer++)
    {
        // sample key arrays and manipulate target object
        static nAnimKey<vector2> key;
        if (this->posArray->Sample(curTime, this->loopType, key))
        {
            targetNode->SetUvPos(texLayer, key.GetValue());
        }
        if (this->eulerArray->Sample(curTime, this->loopType, key))
        {
            targetNode->SetUvEuler(texLayer, key.GetValue());
        }
        if  (this->scaleArray->Sample(curTime, this->loopType, key))
        {
            targetNode->SetUvScale(texLayer, key.GetValue());
        }
    }
}
