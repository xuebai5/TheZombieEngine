#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpcombatidle.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpcombatidle/ngpcombatidle.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"
#include "ncaimovengine/ncaimovengine.h"

nNebulaScriptClass(nGPCombatIdle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPCombatIdle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPCombatIdle::nGPCombatIdle() : 
    nGPBasicAction(),
    animator(0),
    soundLayer(0),
    animIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPCombatIdle::~nGPCombatIdle() 
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPCombatIdle::Init (nEntityObject* entity)
{
    bool valid = entity != 0;

    if ( entity )
    {
        // set the entity
        this->entity = entity;

        // get the animator
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();

        // get the sound layer
        this->soundLayer = entity->GetComponentSafe<ncSoundLayer>();

        valid = (this->animator != 0)  && (this->soundLayer != 0);
    }

    if ( valid )
    {
        // stop the movement engine
        this->entity->GetComponentSafe<ncAIMovEngine>()->Stop();

        // put combat idle animation
        this->animIndex = this->animator->SetCombatIdle();

        // put the combat idle sound
        // @todo check if all cidle anims last the same (then the 3rd param should be the animIndex anim duration)
        nString realName = this->soundLayer->GetRealEventName( "cidle", this->animator->GetRandomIndexAnimation() );                
        this->soundLayer->PlayAlone(realName,1);
        //this->soundLayer->PlaySoundTime(GP_ACTION_COMBATIDLE, 0, 0.0f, true);
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPCombatIdle::IsDone() const
{
    n_assert(this->init);

    bool done = false;

    // Done if animation has finished
    if ( this->animator->HasFinished(this->animIndex) )
    {
        done = true;
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPCombatIdle::Run()
{
    n_assert(this->init);

    bool done = this->IsDone();

    return done;
}
