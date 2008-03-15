#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpgotoally.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpgotoally/ngpgotoally.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncaistate/ncaistate.h"
#include "zombieentity/nctransform.h"
#include "nastar/nastar.h"
#include "ncaimovengine/ncaimovengine.h"
#include "nspatial/ncspatial.h"
#include "npathfinder/npathfinder.h"

nNebulaScriptClass(nGPGoToAlly, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPGoToAlly)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, int, float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPGoToAlly::nGPGoToAlly() : 
    nGPBasicAction(),
    callingAlly(0),
    animator(0),
    movEngine(0),
    restoreSettings(false)
{
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPGoToAlly::~nGPGoToAlly() 
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPGoToAlly::Init (nEntityObject* entity, int style, float speed)
{
    n_assert(nAStar::Instance());

    nAStar* pathFinder = nAStar::Instance();
    bool valid = bool (entity != 0 && pathFinder != 0 ) ;

    if ( valid )
    {
        this->movEngine = entity->GetComponentSafe<ncAIMovEngine>();
        valid = this->movEngine != 0;
    }

    if ( valid )
    {
        // set the entity
        this->entity = entity;

        // get the animator
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();
        if (!this->animator)
        {
            valid = false;
        }

        // get the target
        ncAIState* state = entity->GetComponentSafe<ncAIState>();
        if (state)
        {
            this->callingAlly = state->GetCallingAlly();
        }

        valid = this->callingAlly != 0;
    }

    if ( valid )
    {
        // calculate the destination point
        this->CalculateDestPoint();
        this->destPoint.y = 0;

        // backup movement style
        this->previousMoveStyle = this->movEngine->GetStyle();

        // set the movement style
        this->movEngine->SetStyle(style);

        // backup previous speed
        this->previousSpeed = this->movEngine->GetSpeed();

        // set new speed
        this->movEngine->SetSpeed(speed);

        // set restore settings true
        this->restoreSettings = true;

        valid = pathFinder->IsValidGoal(this->destPoint, entity);
    }

    if ( valid )
    {        
        this->movEngine->MoveTo(this->destPoint);
        // put run animation
        this->animator->SetRun();
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    End
*/
void
nGPGoToAlly::End()
{
    nGPBasicAction::End();

    if (this->restoreSettings)
    {
        // restore movement style
        this->movEngine->SetStyle(this->previousMoveStyle);
        // restore previous speed
        this->movEngine->SetSpeed(this->previousSpeed);
    }
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPGoToAlly::IsDone() const
{
    n_assert(this->init);

    return !this->movEngine || !this->movEngine->IsMoving();
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPGoToAlly::Run()
{
    n_assert(this->init);

    bool done = this->IsDone();

    return done;
}

//------------------------------------------------------------------------------
/**
    Calculate a point near the calling ally
*/
void
nGPGoToAlly::CalculateDestPoint()
{
    n_assert(this->entity);
    n_assert(this->callingAlly);
    if (this->entity && this->callingAlly)
    {
        ncSpatial* spatialComp = this->callingAlly->GetComponentSafe<ncSpatial>();
        if (spatialComp)
        {
            float allySize = spatialComp->GetBBox().diagonal_size();
            float radius = allySize + allySize * 0.10f;

            const vector3& entityPos = this->entity->GetComponentSafe<ncTransform>()->GetPosition();
            const vector3& allyPos   = this->callingAlly->GetComponentSafe<ncTransform>()->GetPosition();
            vector3 dir(entityPos - allyPos);
            dir.norm();
            dir = dir * radius;
            this->destPoint = allyPos + dir;
        }
    }
}
