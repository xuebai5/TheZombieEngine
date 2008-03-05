#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nblendshapeanimator_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nblendshapeanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/nblendshapenode.h"
#include "animcomp/nccharacter.h"

nNebulaScriptClass(nBlendShapeAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nBlendShapeAnimator::nBlendShapeAnimator() : 
    animationGroup(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBlendShapeAnimator::~nBlendShapeAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nBlendShapeAnimator::GetAnimatorType() const
{
    return nAnimator::BlendShape;
}

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
    Unload animation resource if valid.
*/
void
nBlendShapeAnimator::UnloadAnimation()
{
    if (this->refAnimation.isvalid())
    {
        this->refAnimation->Release();
        this->refAnimation.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
    Load new animation, release old one if valid.
*/
bool
nBlendShapeAnimator::LoadAnimation()
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
                n_printf("nBlendShapeAnimator: Error loading animation '%s'\n", this->animationName.Get());
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
nBlendShapeAnimator::LoadResources()
{
    if(this->LoadAnims())
    {
        this->resourcesValid = true;
        return true;
    }
    else
    {
        if(this->LoadAnimation() )
        {
            return true;
        }
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nBlendShapeAnimator::UnloadResources()
{
    this->UnloadAnimation(); //mantain for backwards compatibility
    this->UnloadAnims();
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nBlendShapeNode)
    @param  renderContext   current render context
*/
void
nBlendShapeAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);
    
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }
    
    n_assert(sceneNode->GetClass() == kernelServer->FindClass("nblendshapenode"));
    nBlendShapeNode* targetNode = (nBlendShapeNode*) sceneNode;
    
    ncCharacter* charComp = entityObject->GetComponent<ncCharacter>();
    int activeState = charComp->GetActiveMorphStateByIndex();

    // get the sample time from the render context
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    float curTime = (float) renderContext->GetTime();
    
    // sample curves and manipulate target object
    vector4 keyArray[nBlendShapeNode::MaxShapes];
    
    // backwards compatibility
    if (this->refAnimation.isvalid() && this->refAnimation->IsLoaded())
    {
        int numCurves = this->refAnimation->GetGroupAt(0).GetNumCurves();
        this->refAnimation->SampleCurves(curTime, this->animationGroup, 0, numCurves, keyArray, nAnimation::Group::Repeat); 
        int curveIndex;
        for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
        {
            targetNode->SetWeightAt(curveIndex, keyArray[curveIndex].x);
        }
    }
    else
    {
        // real active state
        if( (activeState != -1 ) && (activeState < this->blendStateArray.GetNumStates() ))
        {
            nBlendState& blendState = this->blendStateArray.GetStateAt(activeState);
            n_assert(blendState.GetAnim()->IsValid());

            // change active state
            if(charComp->GetDirtyMorphActiveState())
            {
                blendState.SetStartTime( curTime );
                charComp->SetDirtyMorphActiveState(false);
                targetNode->SetNumActiveTargets(blendState.GetNumCurves());
            }
        
            // update time
            curTime -= blendState.GetStartTime();

            // look for target changes
            nArray<int> curveIndices, targetIndices;
            blendState.UpdateTargets( curTime, curveIndices, targetIndices );
            if( curveIndices.Size() > 0 ) //sth has changed
            {
                targetNode->FillInstantMeshArray( entityObject, curveIndices, targetIndices );
            }

            // sample
            blendState.GetAnim()->SampleCurves(curTime, blendState.GetAnimGroupIndex(), 0, blendState.GetNumCurves(), keyArray, blendState.GetAnim()->GetGroupAt(0).GetLoopType());

            // set weights
            for (int curveIndex = 0; curveIndex < blendState.GetNumCurves() ; curveIndex++)
            {
                targetNode->SetWeightAt(curveIndex, keyArray[curveIndex].x);
            } 
        }
        // non correct active state or non active state
        else
        { 
            // all the weight is for the first target
            targetNode->SetWeightAt(0, 1.0f);
            // other targets set to zero weight
            for (int curveIndex = 1; curveIndex < this->blendStateArray.GetNumCurves(); curveIndex++)
            {
                targetNode->SetWeightAt(curveIndex, 0.0f);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nBlendShapeAnimator::BeginStates(int num)
{
    this->blendStateArray.Begin(num);
}

//------------------------------------------------------------------------------
/**
    Assign morph state
*/
void
nBlendShapeAnimator::SetState(int stateIndex, int animGroupIndex)
{
    nBlendState newState;
    newState.SetAnimGroupIndex(animGroupIndex);
    this->blendStateArray.SetState(stateIndex, newState);
}

//------------------------------------------------------------------------------
/**
    Assign state name
*/
void
nBlendShapeAnimator::SetStateName(int stateIndex, const nString& name)
{
    this->blendStateArray.GetStateAt(stateIndex).SetName(name);
}

//------------------------------------------------------------------------------
/**
    Assign per-state animation file.
*/
void
nBlendShapeAnimator::SetStateAnim(int stateIndex, const char *filename)
{
    this->blendStateArray.GetStateAt(stateIndex).SetAnimFile(filename);
}

//------------------------------------------------------------------------------
/**
    Get per-state animation file.
*/
const char *
nBlendShapeAnimator::GetStateAnim(int stateIndex)
{
    return this->blendStateArray.GetStateAt(stateIndex).GetAnimFile();
}

//------------------------------------------------------------------------------
/**
*/
void
nBlendShapeAnimator::EndStates()
{
    this->blendStateArray.End();
}

//------------------------------------------------------------------------------
/**
*/
int
nBlendShapeAnimator::GetNumStates() const
{
    return this->blendStateArray.GetNumStates();
}

//------------------------------------------------------------------------------
/**
*/
const nBlendState&
nBlendShapeAnimator::GetStateAt(int stateIndex)
{
    return this->blendStateArray.GetStateAt(stateIndex);
}
//------------------------------------------------------------------------------
/**
*/
bool 
nBlendShapeAnimator::LoadAnims()
{
    int stateIndex;
    int numCurves = 0;
    for (stateIndex = 0; stateIndex < this->GetNumStates(); stateIndex++)
    {
        nBlendState& blendState = this->blendStateArray.GetStateAt(stateIndex);
        if ((!blendState.IsAnimValid()) && (blendState.GetAnimFile()))
        {
            const char* fileName = blendState.GetAnimFile();
            nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(fileName);
            n_assert(anim);
            if (!anim->IsValid())
            {
                anim->SetFilename(fileName);
                if (!anim->Load())
                {
                    n_printf("nBlendShapeAnimator: Error loading anim file '%s'\n", fileName);
                    anim->Release();
                    return false;
                }
            }
            blendState.SetAnim(anim);
            numCurves = anim->GetGroupAt(0).GetNumCurves();
            blendState.SetNumCurves( numCurves );

            if(this->blendStateArray.GetNumCurves() < numCurves)
            {
                this->blendStateArray.SetNumCurves( numCurves );
            }
        }
    }
    return ( this->GetNumStates() != 0 );
}

//------------------------------------------------------------------------------
/**
*/
void 
nBlendShapeAnimator::UnloadAnims()
{
    int stateIndex;
    for (stateIndex = 0; stateIndex < this->GetNumStates(); stateIndex++)
    {
        if (this->blendStateArray.GetStateAt(stateIndex).IsAnimValid())
        {
            this->blendStateArray.GetStateAt(stateIndex).GetAnim()->Release();
            this->blendStateArray.GetStateAt(stateIndex).Invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nBlendShapeAnimator::SetTargetChange( int stateIndex, int keyIndex, int curveIndex, int targetIndex )
{
    this->blendStateArray.GetStateAt(stateIndex).SetTargetChange( keyIndex, curveIndex, targetIndex );
}
