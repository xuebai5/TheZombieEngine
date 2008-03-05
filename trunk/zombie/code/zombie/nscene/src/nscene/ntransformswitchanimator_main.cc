#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformswitchanimator_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntransformswitchanimator.h"
#include "nscene/ntransformnode.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nTransformSwitchAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nTransformSwitchAnimator::nTransformSwitchAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformSwitchAnimator::~nTransformSwitchAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nTransformSwitchAnimator::GetAnimatorType() const
{
    return Transform;
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformSwitchAnimator::ResetKeys()
{
    this->positionArray.Reset();
    this->rotationArray.Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformSwitchAnimator::AddPosition(const vector3& pos)
{
    this->positionArray.Append(pos);
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformSwitchAnimator::AddEuler(const vector3& rot)
{
    this->rotationArray.Append(rot);
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformSwitchAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);

    // FIXME: dirty cast, make sure that it is a nTransformNode
    nTransformNode* targetNode = (nTransformNode*) sceneNode;

    ncDictionary* varContext = entityObject->GetComponent<ncDictionary>();
    nVariable* var = varContext->GetVariable(this->channelVarHandle);
    if (var)
    {
        int index = var->GetInt();
        if (!this->positionArray.Empty() && index < this->positionArray.Size())
        {
            targetNode->SetPosition(this->positionArray.At(index));
        }

        if (!this->rotationArray.Empty())
        {
            targetNode->SetEuler(this->rotationArray.At(index));
        }
}
}
