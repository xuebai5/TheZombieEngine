#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nintanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nintanimator.h"
#include "nscene/nabstractshadernode.h"
#include "nscene/ncscene.h"

nNebulaScriptClass(nIntAnimator, "nshaderanimator");

//------------------------------------------------------------------------------
/**
*/
nIntAnimator::nIntAnimator() :
    keyArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nIntAnimator::~nIntAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nIntAnimator::ResetKeys()
{
    this->keyArray.Reset();
}

//------------------------------------------------------------------------------
/**
    Add a key to the animation key array.
*/
void
nIntAnimator::AddKey(float time, const int key)
{
    nAnimKey<int> newKey(time, key);
    this->keyArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nIntAnimator::GetNumKeys() const
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nIntAnimator::GetKeyAt(int index, float& time, int& key) const
{
    const nAnimKey<int>& animKey = this->keyArray[index];
    time = animKey.GetTime();
    key  = animKey.GetValue();
}

//------------------------------------------------------------------------------
/**
*/
void
nIntAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nAbstractShaderNode!
    nAbstractShaderNode* targetNode = (nAbstractShaderNode*) sceneNode;

    // get the sample time from the render context
    //ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //float curTime = (float) renderContext->GetTime();
    float curTime = this->GetTime(entityObject);

    // get sampled key
    static nAnimKey<int> key;
        if (this->keyArray.Sample(curTime, this->loopType, key))
    {
        targetNode->SetIntOverride(this->param, key.GetValue());
    }
}
