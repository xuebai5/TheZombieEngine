#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nvectoranimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nvectoranimator.h"
#include "nscene/nabstractshadernode.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"

nNebulaScriptClass(nVectorAnimator, "nshaderanimator");

//------------------------------------------------------------------------------
/**
*/
nVectorAnimator::nVectorAnimator() :
    keyArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nVectorAnimator::~nVectorAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nVectorAnimator::ResetKeys()
{
    this->keyArray.Reset();
}

//------------------------------------------------------------------------------
/**
    Add a key to the animation key array.

    @param time time in seconds.
    @param key key values.
*/
void
nVectorAnimator::AddKey(float time, const vector4& key)
{
    nAnimKey<vector4> newKey(time, key);
    this->keyArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
    Return the number of keys in the animation key array.
*/
int
nVectorAnimator::GetNumKeys() const
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return information for a key index.
*/
void
nVectorAnimator::GetKeyAt(int index, float& time, vector4& key) const
{
    const nAnimKey<vector4>& animKey = this->keyArray[index];
    time = animKey.GetTime();
    key  = animKey.GetValue();
}

//------------------------------------------------------------------------------
/**
*/
void
nVectorAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
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
    static nAnimKey<vector4> key;
    if (this->keyArray.Sample(curTime, this->loopType, key))
    {
        // manipulate the target object
        targetNode->SetVectorOverride(this->param, key.GetValue());
    }
}
