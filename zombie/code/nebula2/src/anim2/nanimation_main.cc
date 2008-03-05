//------------------------------------------------------------------------------
//  nanimation_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "anim2/nanimation.h"

nNebulaClass(nAnimation, "nresource");

//------------------------------------------------------------------------------
/**
*/
nAnimation::nAnimation() :
    groupArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimation::~nAnimation()
{
    if (!this->IsUnloaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimation::UnloadResource()
{
    this->groupArray.Clear();
}

//------------------------------------------------------------------------------
/**
    This method should be overwritten by subclasses.
*/
void 
nAnimation::SampleCurves(float /*time*/, int /*groupIndex*/, int /*firstCurveIndex*/, int /*numCurves*/, vector4* /*keyArray*/, nAnimation::Group::LoopType /*loopType*/, float /*velocityFactor*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Returns the playback duration in seconds of an animation group.
*/
nTime
nAnimation::GetDuration(int groupIndex) const
{
    return this->groupArray[groupIndex].GetDuration();
}

//------------------------------------------------------------------------------
/**
*/
const vector4 
nAnimation::GetKeyAt(int /*index*/) const
{
    return vector4();
}
