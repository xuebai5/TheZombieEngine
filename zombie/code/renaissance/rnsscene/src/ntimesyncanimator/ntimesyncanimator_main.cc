//------------------------------------------------------------------------------
//  ntimesyncanimator_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsscene.h"
#include "ntimesyncanimator/ntimesyncanimator.h"
#include "nscene/ncscene.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nTimeSyncAnimator , "nanimator");


//------------------------------------------------------------------------------
/**
*/
nTimeSyncAnimator::nTimeSyncAnimator() :
    cycleTime(0.f),
    offsetTime(0.f)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
nTimeSyncAnimator::~nTimeSyncAnimator()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
nTimeSyncAnimator::Type
nTimeSyncAnimator::GetAnimatorType() const
{
    return nTimeSyncAnimator::Transform;
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeSyncAnimator::EntityCreated(nEntityObject* entityObject)
{
     ncDictionary *varContext = entityObject->GetComponentSafe<ncDictionary>();

     // If variable not found then append it
     if (! varContext->FindLocalVar(this->channelOffsetVarHandle) )
     {
         varContext->AddLocalVar( nVariable(this->channelOffsetVarHandle, 0.f) );
     }

}

//------------------------------------------------------------------------------
/**
    This method is called back by scene node objects which wish to be
    animated. 
    it calculate the time offset.
    The variable holding the time offset should have been created by
    some other node in the entity.
*/
void
nTimeSyncAnimator::Animate(nSceneNode* /*sceneNode*/, nEntityObject* entityObject)
{
    
    if ( this->cycleTime > N_TINY )
    {
        // get the frame time from the render context
        ncScene *renderContext = entityObject->GetComponent<ncScene>();
        float curTime = (float) renderContext->GetTime();


        if (this->channelOffsetVarHandle != nVariable::InvalidHandle)
        {
            ncDictionary* varContext = entityObject->GetComponentSafe<ncDictionary>();
            nVariable* var = varContext->FindLocalVar(this->channelOffsetVarHandle);
            if (var)
            {
                float offset = curTime - fmodf( curTime - this->offsetTime + this->cycleTime, this->cycleTime );
                // Add this->cycleTime bcause  if to case curTime < this->offsetTime, fmod is a remain , it is not a module
                var->SetFloat(offset);
            }
        }   
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeSyncAnimator::SetCycleTime(float time)
{
    this->cycleTime = time;
}

//------------------------------------------------------------------------------
/**
*/
float
nTimeSyncAnimator::GetCycleTime() const
{
    return this->cycleTime;
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeSyncAnimator::SetOffsetTime(float time)
{
    this->offsetTime = time;
}

//------------------------------------------------------------------------------
/**
*/
float
nTimeSyncAnimator::GetOffsetTime() const
{
    return this->offsetTime;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------