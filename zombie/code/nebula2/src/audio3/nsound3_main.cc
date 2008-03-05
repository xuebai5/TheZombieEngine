//------------------------------------------------------------------------------
//  nsound3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "audio3/nsound3.h"

nNebulaClass(nSound3, "nresource");

//------------------------------------------------------------------------------
/**
    - 17-May-04     floh    bugfix: missing cone member initializations
*/
nSound3::nSound3() :
    numTracks(5),
    ambient(false),
    streaming(false),
    looping(false),
    freqControl(false),
    priority(0),
    volume(1.0f),
    minDist(10.0f),
    maxDist(100.0f),
    insideConeAngle(0),
    outsideConeAngle(360),
    coneOutsideVolume(1.0f),
    category(nAudioServer3::Effect),
    volumeDirty(true),
    props3DDirty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSound3::~nSound3()
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
nSound3::Start()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Stop()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief stop the sound in a given buffer index
*/
void
nSound3::Stop( int /*index*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Update()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Update( int /*index*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nSound3::IsPlaying()
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSound3::IsPlaying(int /*index*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
int
nSound3::GetStatus(int /*index*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::SetTrackPosition(int /*index*/, float /*seconds*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
float
nSound3::GetDuration()
{
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
int 
nSound3::GetLastBufferUsed()
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSound3::Update3DProperties( int /*index*/ )
{
    // empty
}