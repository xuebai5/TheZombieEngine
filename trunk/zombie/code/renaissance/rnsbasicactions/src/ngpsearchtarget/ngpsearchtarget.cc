#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpsearchtarget.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpsearchtarget/ngpsearchtarget.h"
#include "ncaistate/ncaistate.h"
#include "ncagentmemory/ncagentmemory.h"
#include "zombieentity/nctransform.h"
#include "nspatial/nspatialserver.h"
#include "entity/nentityobjectserver.h"
#include "ncgameplayliving/ncgameplayliving.h"

nNebulaScriptClass(nGPSearchTarget, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPSearchTarget)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPSearchTarget::nGPSearchTarget() : 
    nGPBasicAction()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPSearchTarget::~nGPSearchTarget()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init

    @brief Search a target dinamically depending on the params (flags)
*/
bool
nGPSearchTarget::Init (nEntityObject* entity)
{
    n_assert(entity);

    nEntityObject *target = 0;

    if ( entity )
    {
        this->entity = entity;
        this->aistate = entity->GetComponentSafe<ncAIState>();
        this->living = entity->GetComponentSafe<ncGameplayLiving>();
        n_assert(this->aistate && this->living);
        
        if( this->aistate && this->living ) {
            this->curr_target = this->aistate->GetTarget();
            this->curr_fightring = this->aistate->GetFightRingIndex();

            target = this->FindClosest();

            //  If no convinient target has been found,
            // check the current target
            if( !target && this->curr_target )
            {
                ncGameplayLiving* curr_living = this->curr_target->GetComponent<ncGameplayLiving>();
                if( curr_living && !curr_living->IsDead() )
                {
                    target = this->curr_target;
                }
            }

            if ( target ) 
            {
                this->aistate->SetTarget(target);
                this->aistate->SetTargetPoint( target->GetComponentSafe<ncTransform>()->GetPosition() );
            }
            else
            {
                // Forget current target
                this->aistate->SetTarget(0);
            }
        }
    }

    this->init = target != 0;

    return this->init;
}

//------------------------------------------------------------------------------
/**
    FindClosest
*/
nEntityObject*
nGPSearchTarget::FindClosest() const
{
    ncAgentMemory* memory = 0;
    ncTransform* transform = 0;
    nEntityObject* candidate = 0;
    nEntityObject* closest = 0;
    
    if ( this->entity )
    {
        transform = this->entity->GetComponent <ncTransform>();
        memory = this->entity->GetComponentSafe<ncAgentMemory>();
    }

    n_assert(transform && memory);

    if ( transform && memory )
    {
        float minSqrdDist = FLT_MAX;
        const vector3 &myPos = transform->GetPosition();

        // search the closest one
        // Get the entities from the agent's memory
        for ( ncAgentMemory::EntitiesIterator it( memory->GetEntitiesIterator() ); !it.IsEnd(); it.Next() )
        {
            candidate = nEntityObjectServer::Instance()->GetEntityObject( it.Get() );
            if ( this->ConsiderEntity(candidate) )
            {
                const vector3& pos = candidate->GetComponentSafe<ncTransform>()->GetPosition();
                float sqrdDist = (pos - myPos).lensquared();

                if ( sqrdDist < minSqrdDist )
                {
                    closest = candidate;
                    minSqrdDist = sqrdDist;
                }
            }
        }
    }

    return closest;
}

//------------------------------------------------------------------------------
/**
    ConsiderEntity
*/
bool
nGPSearchTarget::ConsiderEntity(nEntityObject* target) const
{
    // @TODO: COMPLETE THIS WITH ENTITIES ALIGNMENT 
    if ( target )
    {
        ncGameplayLiving* target_living = target->GetComponent<ncGameplayLiving>();
        if ( target_living && !target_living->IsDead() )
        {
            // Don't consider others if in melee
            if( this->curr_fightring == 0 && target != this->curr_target )
            {
                return false;
            }
            // Don't consider current if in outer rings
            if( this->curr_fightring > 0 && target == this->curr_target )
            {
                return false;
            }
            return true;
        }
    }

    return false;
}
