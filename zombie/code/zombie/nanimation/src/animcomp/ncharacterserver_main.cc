#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nCharacterServer_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "animcomp/ncharacterserver.h"
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "nphysics/ncphycharacterobj.h"
#include "zombieentity/nctransform.h"
#include "entity/nentityobjectserver.h"

nNebulaClass(nCharacterServer, "nroot");

nCharacterServer* nCharacterServer::Singleton = 0;

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
NCREATELOGLEVELGROUP( character, "Character System", false, 2, animationLogNames, NLOG_GROUP_MASK )

//------------------------------------------------------------------------------
/**
*/
nCharacterServer::nCharacterServer() :
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
                                                &nCharacterServer::EntityDeleted,
                                                0);
}

//------------------------------------------------------------------------------
/**
*/
nCharacterServer::~nCharacterServer()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacterServer::Trigger(nTime time, float frames)
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
nCharacterServer::Register(nEntityObject* charEntity)
{
    this->characterEntityPool.Append(charEntity);
    return true;
}

//------------------------------------------------------------------------------
/** 
*/
void
nCharacterServer::Unregister(nEntityObject* charEntity)
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
nCharacterServer::EntityDeleted(int id)
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
nCharacterServer::DoTrigger(float frames)
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
nCharacterServer::GetAccumFrameTime() const
{
    return this->accumTime;
}

//------------------------------------------------------------------------------
/** 
*/
bool 
nCharacterServer::DoUpdate()
{
    //must be updated (true if since last update have passed more than keytime)
    return this->accumTime >= this->interFrameTime || this->accumTime == 0;
}
