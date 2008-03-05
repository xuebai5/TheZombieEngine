#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformswitchanimator_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntransformoverrideanimator.h"
#include "nscene/ntransformnode.h"

#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nTransformOverrideAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nTransformOverrideAnimator::nTransformOverrideAnimator() :
    positionChannel(nVariable::InvalidHandle),
    rotationChannel(nVariable::InvalidHandle)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformOverrideAnimator::~nTransformOverrideAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nTransformOverrideAnimator::GetAnimatorType() const
{
    return Transform;
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformOverrideAnimator::SetPositionChannel(const char* varName)
{
    n_assert(varName);
    this->positionChannel = nVariableServer::Instance()->GetVariableHandleByName(varName);
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformOverrideAnimator::SetRotationChannel(const char* varName)
{
    n_assert(varName);
    this->rotationChannel = nVariableServer::Instance()->GetVariableHandleByName(varName);
}

//------------------------------------------------------------------------------
/**
*/
void
nTransformOverrideAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);

    // FIXME: dirty cast, make sure that it is a nTransformNode
    nTransformNode* targetNode = (nTransformNode*) sceneNode;

    ncDictionary* varContext = entityObject->GetComponent<ncDictionary>();

    if (this->positionChannel != nVariable::InvalidHandle)
    {
        nVariable* var = varContext->GetVariable(this->positionChannel);
        if (var)
        {
            vector4 v4(var->GetVector4());
            targetNode->SetPosition(vector3(v4.x, v4.y, v4.z));
        }
    }

    if (this->rotationChannel != nVariable::InvalidHandle)
    {
        nVariable* var = varContext->GetVariable(this->rotationChannel);
        if (var)
        {
            vector4 v4(var->GetVector4());
            targetNode->SetEuler(vector3(v4.x, v4.y, v4.z));
        }
    }
}
