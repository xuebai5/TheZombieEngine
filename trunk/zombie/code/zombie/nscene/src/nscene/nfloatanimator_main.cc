#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nfloatanimator_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "nscene/nfloatanimator.h"
#include "nscene/nabstractshadernode.h"
#include "nscene/ncscene.h"

nNebulaScriptClass(nFloatAnimator, "nshaderanimator");

//------------------------------------------------------------------------------
/**
*/
nFloatAnimator::nFloatAnimator() :
    keyArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nFloatAnimator::~nFloatAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add a key to the animation key array.
*/
void
nFloatAnimator::AddKey(float time, float key)
{
    nAnimKey<float> newKey(time, key);
    this->keyArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nFloatAnimator::GetNumKeys() const
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nFloatAnimator::GetKeyAt(int index, float& time, float& key) const
{
    const nAnimKey<float>& animKey = this->keyArray[index];
    time = animKey.GetTime();
    key  = animKey.GetValue();
}

//------------------------------------------------------------------------------
/**
*/
void
nFloatAnimator::ResetKeys()
{
    this->keyArray.Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nFloatAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);

    // FIXME: dirty cast, make sure that it is a nAbstractShaderNode!
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;

    float curTime = this->GetTime(entityObject);
    // get the sample time from the render context
    //ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //float curTime = (float) renderContext->GetTime();

    // get sampled key
    static nAnimKey<float> key;
    if (this->keyArray.Sample(curTime, this->loopType, key))
    {
        targetNode->SetFloatOverride(this->param, key.GetValue());
    }
}
