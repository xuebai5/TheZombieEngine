#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformanimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/ntransformanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/ntransformnode.h"

nNebulaScriptClass(nTransformAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nTransformAnimator::nTransformAnimator() :
    posArray(0, 4),
    eulerArray(0, 4),
    scaleArray(0, 4),
    quatArray(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformAnimator::~nTransformAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nTransformAnimator::GetAnimatorType() const
{
    return Transform;
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformAnimator::ResetKeys()
{
    this->posArray.Reset();
    this->eulerArray.Reset();
    this->scaleArray.Reset();
    this->quatArray.Reset();
}

//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nTransformNode)
    @param  renderContext   current render context
*/
void
nTransformAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    // FIXME: dirty cast, make sure that it is a nTransformNode
    nTransformNode* targetNode = (nTransformNode*) sceneNode;

    // get the sample time from the render context
    //ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //float curTime = (float) renderContext->GetTime();
    float curTime = this->GetTime(entityObject);

    // sample key arrays and manipulate target object
    static nAnimKey<vector3> key;
    static nAnimKey<quaternion> quatkey;
    if (this->posArray.Sample(curTime, this->loopType, key))
    {
        targetNode->SetPosition(key.GetValue());
    }
    if (this->quatArray.Sample(curTime, this->loopType, quatkey))
    {
        targetNode->SetQuat(quatkey.GetValue());
    }
    if (this->eulerArray.Sample(curTime, this->loopType, key))
    {
        targetNode->SetEuler(key.GetValue());
    }
    if (this->scaleArray.Sample(curTime, this->loopType, key))
    {
        targetNode->SetScale(key.GetValue());
    }
}
