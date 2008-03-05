#include "precompiled/pchngrass.h"
#include "nvegetation/ngrassmodule.h"
#include "napplication/napplication.h"

namespace nGrassModule
{
    static const char * logNames[] = {
        "Profile",
        "Creation",
        0
        };
}

/// level 0..3 Error , level 4..7 warnings  , level 8 ..11 info
NCREATELOGLEVELGROUP ( grass , "Grass" , false , 12 ,  nGrassModule::logNames , NLOG_GROUP_MASK)

namespace nGrassModule
{

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__

const float lifeTimeDebugText = 20.f;


profile meshLoadProfile(0, "mesh load");
profile meshGenerationProfile(0, "mesh generation");
profile seedLoadProfile(0, "seed load");

//------------------------------------------------------------------------------
/**
*/
profile::profile(uint lastFrameId , const char* name) :
    lastFrameId(lastFrameId),
    name(name),
    time(0.f),
    totalCount(0),
    countFrames(0),
    accumTime(0)
{
    //Empty
}

//------------------------------------------------------------------------------
/**
*/
void
profile::AddTime( float time)
{
    uint currentId = nApplication::Instance()->GetFrameId();
    if ( currentId != this->lastFrameId )
    {
        this->lastFrameId = currentId;
        this->accumTime += this->time;

        this->totalCount += this->count;
        this->countFrames++;

        NLOG( grass, ( 8 | nGrassModule::PROFILE, 
                        " %22.20s : lastFrameId %u , time %2.3f , call in this frame %4.4u ,  avg time %2.3f , avg time  by frame %2.3f , avg call/by frame %2.3f , total frames %u" ,
                        this->name, this->lastFrameId , 
                        this->time, this->count, 
                        float( this->accumTime / this->totalCount) ,
                        float(this->accumTime /this->countFrames),
                        float(float(this->totalCount) /this->countFrames),
                        countFrames
                    ) 
            );


        this->time = 0.f;
        this->count =0;
    }
    this->time  +=time;
    this->count++; 
    NLOG( grass, ( 10 | nGrassModule::PROFILE, " %s: time %f " , this->name, this->time ) );
}

//------------------------------------------------------------------------------
/**
*/
void AddMeshLoadTime(float time)
{
    meshLoadProfile.AddTime( time);
}

//------------------------------------------------------------------------------
/**
*/
void AddMeshGenerationTime(float time)
{
    meshGenerationProfile.AddTime( time);
}

//------------------------------------------------------------------------------
/**
*/
void AddSeedLoadTime(float time)
{
    seedLoadProfile.AddTime( time );
}

//------------------------------------------------------------------------------
/**
*/
const profile& GetMeshLoadTime()
{
    return meshLoadProfile;
}

//------------------------------------------------------------------------------
/**
*/
const profile& GetMeshGenerationTime()
{
    return meshGenerationProfile;
}

//------------------------------------------------------------------------------
/**
*/
const profile& GetSeedLoadTime()
{
    return seedLoadProfile;
}

#endif //!__ZOMBIE_EXPORTER__
#endif //!NGAME


}
