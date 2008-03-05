#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformcurveanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/ntransformcurveanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/ntransformnode.h"

nNebulaScriptClass(nTransformCurveAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nTransformCurveAnimator::nTransformCurveAnimator() : 
    animationGroup(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformCurveAnimator::~nTransformCurveAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nTransformCurveAnimator::GetAnimatorType() const
{
    return Transform;
}

//------------------------------------------------------------------------------
/**
    Unload animation resource if valid.
*/
void
nTransformCurveAnimator::UnloadAnimation()
{
    if (this->refAnimation.isvalid())
    {
        this->refAnimation->Release();
        this->refAnimation.invalidate();
    }
}


//------------------------------------------------------------------------------
/**
    Load new animation, release old one if valid.
*/
bool
nTransformCurveAnimator::LoadAnimation()
{
    if ((!this->refAnimation.isvalid()) && (!this->animationName.IsEmpty()))
    {
        nAnimation* animation = nAnimationServer::Instance()->NewMemoryAnimation(this->animationName.Get());
        n_assert(animation);
        if (!animation->IsLoaded())
        {
            animation->SetFilename(this->animationName.Get());
            if (!animation->Load())
            {
                n_printf("nTransformCurveAnimator: Error loading animation '%s'\n", this->animationName.Get());
                animation->Release();
                return false;
            }
        }
        this->refAnimation = animation;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nTransformCurveAnimator::LoadResources()
{
    this->LoadAnimation();
    this->resourcesValid = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nTransformCurveAnimator::UnloadResources()
{
    this->resourcesValid = false;
    this->UnloadAnimation();
}


//------------------------------------------------------------------------------
/**
    Set the resource name. The animation resource name consists of the
    filename of the animation.
*/
void
nTransformCurveAnimator::SetAnimation(const char* name)
{
    n_assert(name);
    this->UnloadAnimation();
    this->animationName = name;
}


//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nTransformNode)
    @param  renderContext   current render context
*/
void
nTransformCurveAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    n_assert(this->refAnimation->IsLoaded());

    // FIXME: dirty cast, make sure that it is a nTransformNode
    nTransformNode* targetNode = (nTransformNode*) sceneNode;

    // get the sample time from the render context
    //ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //float curTime = (float) renderContext->GetTime();
    float curTime = this->GetTime(entityObject);

    // sample curves and manipulate target object
    vector4 keyArray[3];
    
    this->refAnimation->SampleCurves(curTime, this->animationGroup, 0, 3, &keyArray[0], this->refAnimation->GetGroupAt(0).GetLoopType() ); 
    
    targetNode->SetPosition(vector3(keyArray[0].x, keyArray[0].y, keyArray[0].z));
    targetNode->SetQuat(quaternion(keyArray[1].x, keyArray[1].y, keyArray[1].z, keyArray[1].w));
    targetNode->SetScale(vector3(keyArray[2].x, keyArray[2].y, keyArray[2].z));
    
}
