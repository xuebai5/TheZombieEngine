#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nanimationserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nanimation/nanimationserver.h"
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "nphysics/ncphycharacterobj.h"
#include "entity/nentityobjectserver.h"

nNebulaClass(nAnimationServer, "nroot");

nAnimationServer* nAnimationServer::Singleton = 0;

//------------------------------------------------------------------------------
static const char * animationLogNames[] = {
    "Low-Level", // per-frame animations and basic character animations     NLOG1
    "Mid-Level", // selections and set of animations in animator            NLOG2
    "High-Level", // selections and set in top layers                       NLOG3
    0
};

/**
    Animation Log declaration:
    0- Events (SetState, GetDisplacement, etc.)
    1- per-frame basic information (UpdateCharacter, etc.)
    2- per-frame complete information
*/
NCREATELOGLEVELGROUP( animation, "Animation System", false, 2, animationLogNames, NLOG_GROUP_MASK )

//------------------------------------------------------------------------------
/**
*/
nAnimationServer::nAnimationServer() :
    refResourceServer("/sys/servers/resource"),
    characterEntityPool(0, 16),
    fixedFPS(100),
    fixedPhysicsFPS(30),
    accumTime(0),
    interFrameTime(0)
    #ifndef NGAME
   ,physicsEnabled(true)
    #endif
    #if __NEBULA_STATS__
   ,profAnim("profAnim", false),
    profAnimCollision("profAnimCollision", false)
    #endif
{
    n_assert(0 == Singleton);
    Singleton = this;

    this->interFrameTime = 1.0f / this->fixedFPS;
    this->interPhysicsFrameTime = 1.0f / this->fixedPhysicsFPS;

    // bind to signal of delete entity
    nEntityObjectServer::Instance()->BindSignal(nEntityObjectServer::SignalEntityDeleted, 
                                                this,
                                                &nAnimationServer::EntityDeleted,
                                                0);
}

//------------------------------------------------------------------------------
/**
*/
nAnimationServer::~nAnimationServer()
{
    // unload all animation resources
    if (this->refResourceServer.isvalid())
    {
        this->refResourceServer->UnloadResources(nResource::Animation);
    }
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a new memory animation object. Memory animations keep the
    entire animation data in memory, but are generally shared. Memory anims
    should be used when many objects are driven by the same animation.

    @param  rsrcName    the resource name for resource sharing
*/
nAnimation*
nAnimationServer::NewMemoryAnimation(const char* rsrcName)
{
    n_assert(rsrcName);
    return (nAnimation*) this->refResourceServer->NewResource("nmemoryanimation", rsrcName, nResource::Animation);
}

//------------------------------------------------------------------------------
/**
    Create a new streaming animation. Streaming animations stream their
    data from disk, which has the advantage that animations can be very
    long (hours if you want), but each streaming animation needs its
    own small streaming buffer, which may add up when many objects are driven
    by streamed animations, also disk bandwidth and seek latency may become
    a limiting factor.

    Streaming animations generally cannot be shared, thus this method
    has no resource name parameter.
*/
nAnimation*
nAnimationServer::NewStreamingAnimation()
{
    return (nAnimation*) this->refResourceServer->NewResource("nstreaminganimation", 0, nResource::Animation);
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimationServer::Trigger(nTime time, float frames)
{
    #if __NEBULA_STATS__
    //publish this profiler here, characters can be updated during the scene loop
    this->profAnim.Publish();
    this->profAnim.ResetAccum();
    #endif

    if (this->DoTrigger(frames))
    {
        // reset counter
        this->accumTime = 0;

        #if __NEBULA_STATS__
        this->profAnimCollision.Publish();
        this->profAnimCollision.ResetAccum();
        #endif

        //trigger all characters and updates them
        if (!this->characterEntityPool.Empty())
        {
            CharacterEntityPool::iterator characterIter = this->characterEntityPool.Begin();
            while (characterIter != this->characterEntityPool.End())
            {
                ncCharacter* charComp = (*characterIter)->GetComponent<ncCharacter>();
                if (!charComp->GetRagdollActive())
                {
                    //check for minimum activity distance
                    ncCharacterClass* characterClass = (*characterIter)->GetClassComponent<ncCharacterClass>();
                    const vector3& characterPos = (*characterIter)->GetComponent<ncTransform>()->GetPosition();
                    const matrix44& viewer = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
                    float viewDistanceSq = (viewer.pos_component() - characterPos).lensquared();
                    if (!characterClass || viewDistanceSq < characterClass->GetActivityDistanceSq())
                    {
                        //update geometries
                        charComp->UpdateCharacter(charComp->GetPhysicsSkelIndex(), time);

                        ncPhyCharacterObj* phyCharComp = (*characterIter)->GetComponent<ncPhyCharacterObj>();
                        if (phyCharComp)
                        {
                            #if __NEBULA_STATS__
                            this->profAnimCollision.StartAccum();
                            #endif
                            phyCharComp->UpdateCharacterCollision();
                            #if __NEBULA_STATS__
                            this->profAnimCollision.StopAccum();
                            #endif
                        }
                    }
                }
                else
                {
                    //remove old geometries
                    ncPhyCharacterObj* phyCharComp = (*characterIter)->GetComponentSafe<ncPhyCharacterObj>();
                    for (int i=0; i< phyCharComp->GetNumGeometries(); i++)
                    {
                        phyCharComp->Remove(phyCharComp->GetGeometry(i));
                    }
                }

                ++characterIter;
            }
        }
    }
}

//------------------------------------------------------------------------------
/** 
*/
bool
nAnimationServer::Register(nEntityObject* charEntity)
{
    this->characterEntityPool.Append(charEntity);
    return true;
}

//------------------------------------------------------------------------------
/** 
*/
void
nAnimationServer::Unregister(nEntityObject* charEntity)
{
    CharacterEntityPool::iterator characterIter = this->characterEntityPool.Begin();
    while (characterIter != this->characterEntityPool.End())
    {
        if ((*characterIter)->GetId() == charEntity->GetId())
        {
            this->characterEntityPool.Erase(characterIter);
            break;
        }
        ++characterIter;
    }
}

//------------------------------------------------------------------------------
/** 
*/
void
nAnimationServer::EntityDeleted(int id)
{
    CharacterEntityPool::iterator characterIter = this->characterEntityPool.Begin();
    while (characterIter != this->characterEntityPool.End())
    {
        if ((*characterIter)->GetId() == static_cast<nEntityObjectId>(id))
        {
            this->characterEntityPool.Erase(characterIter);
            break;
        }
        ++characterIter;
    }
}

//------------------------------------------------------------------------------
/** 
*/
bool 
nAnimationServer::DoTrigger(float frames)
{
    #ifndef NGAME
    if (!this->physicsEnabled)
    {
        return false;
    }
    #endif

    //current proportion
    float proportionValue = 1.0f / frames;

    // begin: safety
    if (proportionValue > float(4))
    {
        proportionValue = float(4);
    }
    // end: safety

    /// counting time
    this->accumTime += proportionValue;

    //must be updated (true if since last update have passed more than keytime)
    return this->accumTime >= this->interPhysicsFrameTime;
}

//------------------------------------------------------------------------------
/** 
*/
float 
nAnimationServer::GetAccumFrameTime() const
{
    return this->accumTime;
}

//------------------------------------------------------------------------------
/** 
*/
bool 
nAnimationServer::DoUpdate()
{
    //must be updated (true if since last update have passed more than keytime)
    return this->accumTime >= this->interFrameTime || this->accumTime == 0;
}
