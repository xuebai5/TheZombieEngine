#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nanimator_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nanimator.h"
#include "variable/nvariableserver.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/nabstractshadernode.h"
#include "zombieentity/ncdictionary.h"


nNebulaScriptClass(nAnimator, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nAnimator::nAnimator() :
    loopType(nAnimLoopType::Loop),
    fixedTimeOffset(0)
{
    this->channelVarHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
    this->channelOffsetVarHandle = nVariableServer::Instance()->GetVariableHandleByName("timeOffset");
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::~nAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Returns the type of the animator object. Subclasses should return
    something meaningful here.
*/
nAnimator::Type
nAnimator::GetAnimatorType() const
{
    return InvalidType;
}

//------------------------------------------------------------------------------
/**
    This method is called back by scene node objects which wish to be
    animated. 
*/
void
nAnimator::Animate(nSceneNode* /*sceneNode*/, nEntityObject* /*entityObject*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Sets the "animation channel" which drives this animation.
    This could be something like "time", but the actual names are totally
    up to the application. The actual channel value will be pulled from
    the render context provided in the Animate() method.
*/
void
nAnimator::SetChannel(const char* name)
{
    n_assert(name);
    this->channelVarHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    this->channelOffsetVarHandle = nVariableServer::Instance()->GetVariableHandleByName((nString(name) + "Offset").Get());
}

//------------------------------------------------------------------------------
/**
    Return the animation channel which drives this animation.
*/
const char*
nAnimator::GetChannel()
{
    if (nVariable::InvalidHandle == this->channelVarHandle)
    {
        return 0;
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->channelVarHandle);
    }
}


//------------------------------------------------------------------------------
/**
*/
void
nAnimator::SetFixedTimeOffset(float timeKey)
{
    this->fixedTimeOffset = timeKey;
}

//------------------------------------------------------------------------------
/**
*/
float
nAnimator::GetFixedTimeOffset() const
{
    return this->fixedTimeOffset;
}

//------------------------------------------------------------------------------
/**
    Override in subclasses to initialize all key lists-
    this is called before loading persisted keys to reset arrays
*/
void
nAnimator::ResetKeys()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
float
nAnimator::GetTime(nEntityObject* entityObject) const 
{
    // get the sample time from the render context
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    float curTime = (float) renderContext->GetTime();

    if (this->channelOffsetVarHandle != nVariable::InvalidHandle)
    {
        ncDictionary *varContext = entityObject->GetComponentSafe<ncDictionary>();
        nVariable* var = varContext->FindLocalVar(this->channelOffsetVarHandle);
        if (var)
        {
            curTime = curTime - var->GetFloat();
        }
        else
        {
            varContext->AddLocalVar(nVariable(this->channelOffsetVarHandle, curTime));
        }
    }

    if (this->fixedTimeOffset > 0.0f)
    {
        curTime = n_floor(curTime / this->fixedTimeOffset) * this->fixedTimeOffset;
    }

    return curTime;
}
