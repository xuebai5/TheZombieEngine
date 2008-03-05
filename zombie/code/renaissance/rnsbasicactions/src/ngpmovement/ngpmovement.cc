#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpmoviment.cc
//------------------------------------------------------------------------------

#include "ngpmovement/ngpmovement.h"
#include "nastar/nastar.h"
#include "ncaimovengine/ncaimovengine.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "ncgameplayliving/ncgameplayliving.h"

nNebulaScriptClass(nGPMovement, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPMovement)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, const vector3&), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPMovement::nGPMovement() : 
    nGPBasicAction(),
    movEngine(0),
    previousSpeed(0.f),
    restoreSettings(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPMovement::~nGPMovement()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init

    @params the entity to move and the goal point
*/
bool
nGPMovement::Init (nEntityObject* entity, const vector3& goal)
{
    n_assert(nAStar::Instance());

    nAStar* pathFinder = nAStar::Instance();
    bool valid = bool (entity != 0 && pathFinder != 0 ) ;

    if ( valid )
    {
        // get entity's movement engine
        this->movEngine = entity->GetComponent <ncAIMovEngine>();
        valid = this->movEngine != 0;
    }

    if ( valid )
    {
        // check if the goal is valid
        valid = pathFinder->IsValidGoal (goal, entity);
    }

    if ( valid )
    {
        // set the basic action's entity
        this->entity = entity;
        valid = true;        
    }

    if ( valid && this->movEngine )
    {
        // backup previous speed
        this->previousSpeed = this->movEngine->GetSpeed();

        // set speed
        ncGameplayLivingClass *livingClass = this->entity->GetClassComponentSafe<ncGameplayLivingClass>();
        this->movEngine->SetSpeed(livingClass->GetSpeed());

        // set restore settings true
        this->restoreSettings = true;

        // set the movement mode and direction
        ncGameplayLiving * livingComp = this->entity->GetComponent<ncGameplayLiving>();
        if ( livingComp && livingComp->IsProne() )
        {
            // If move mode is walk and entity is proned, set prone mode
            this->movEngine->SetMovMode(ncAIMovEngine::MOV_WALKPRONE);
        }
        else
        {
            this->movEngine->SetMovMode(ncAIMovEngine::MOV_WALK);
        }

        this->movEngine->SetMovDirection(ncAIMovEngine::MD_FORWARD);

        // move the entity
        this->movEngine->MoveTo(goal);
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPMovement::IsDone() const
{
    n_assert(this->init);

    return !this->movEngine->IsMoving();
}

//------------------------------------------------------------------------------
/**
    End
*/
void
nGPMovement::End()
{
    nGPBasicAction::End();

    if (this->restoreSettings)
    {
        // restore previous speed
        this->movEngine->SetSpeed(this->previousSpeed);
    }
}
