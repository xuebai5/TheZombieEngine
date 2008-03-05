#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpmelee.cc
//------------------------------------------------------------------------------

#include "ngpmelee/ngpmelee.h"
#include "ncaistate/ncaistate.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "zombieentity/nctransform.h"
#include "nclogicanimator/nclogicanimator.h"
#include "nphysics/ncphycharacterobj.h"
#include "rnsgameplay/ndamagemanager.h"
#include "rnsgameplay/ncgpweaponmelee.h"
#include "rnsgameplay/ncgpweaponmeleeclass.h"
#include "nnetworkmanager/nnetworkmanager.h"
#include "nphysics/ncphycompositeobj.h"
#include "napplication/napplication.h"

#include "ncsoundlayer/ncsoundlayer.h"
#include "gameplay/ngamematerial.h"
#include "gameplay/nfxeventtrigger.h"

#define INIT_OFFSET 0.3f    // Time that have to pass for the collision to be valid
#define END_OFFSET 0.5f     // Time that have to pass for the collision to become invalid again

nNebulaScriptClass(nGPMelee, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPMelee)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPMelee::nGPMelee() : 
    nGPBasicAction(),
    animator(0),
    weapon(0),
    target(0),
    isPlayer(false),
    hitTarget(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPMelee::~nGPMelee() 
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPMelee::Init (nEntityObject* entity)
{
    bool valid = entity != 0;
    ncAIState* state = 0;

    if ( entity )
    {
        // set the entity
        this->entity = entity;

        // get the animator
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();

        // get the primary weapon
        ncGameplayLiving *gameplay = 0;
        gameplay = entity->GetComponentSafe<ncGameplayLiving>();
        if (gameplay)
        {
            // get melee weapon
            nEntityObject* weaponEntity = gameplay->GetCurrentWeapon();
            if (weaponEntity)
            {
                this->weapon = weaponEntity->GetComponentSafe<ncGPWeaponMelee>();
                n_assert( this->weapon );
            }
            else
            {
                // the entity has no weapon, so it can't attack
                this->init = false;
                return false;
            }
        }
        
        if (!entity->IsA("neplayer"))
        {
            this->isPlayer = false;

            // get the target
            state = entity->GetComponentSafe<ncAIState>();
            if (state)
            {
                this->target = state->GetTarget();
            }

            valid = (this->animator != 0) && (this->weapon != 0) && (this->state != 0);
        }
        else
        {
            this->isPlayer = true;
            valid = (this->animator != 0) && (this->weapon != 0);
        }

        this->initTime = static_cast<float> ( nApplication::Instance()->GetTime() );
        this->initOffset = INIT_OFFSET;
        this->endOffset = END_OFFSET;
    }

    if ( valid )
    {
        this->animIndex = this->animator->SetMeleeAttack(); 

        ncSoundLayer* sound( this->weapon->GetComponent<ncSoundLayer>() );
        if ( sound )
        {
            sound->PlayAloneMaterial( "melee", "air", 1 );
        }
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPMelee::IsDone() const
{
    n_assert(this->init);

    return !this->animator || this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPMelee::Run()
{
    n_assert(this->init);

    bool done = this->IsDone();

    if ( !done )
    {
        float runTime = static_cast< float > ( nApplication::Instance()->GetTime() ) - this->initTime;

        if ( runTime > this->initOffset && runTime < this->endOffset && !this->hitTarget )
        {
            this->hitTarget = this->HitTarget(); 
        }
    }    

    return done;
}

//------------------------------------------------------------------------------
/**
    HitTarget
*/
bool
nGPMelee::HitTarget() const
{
    n_assert(this->init);

    bool hit = false;
    
    if (!this->isPlayer && !this->target)
    {
        return hit;
    }

    if ( this->weapon )
    {
        // get the weapon's geometry
        geomid geomId = this->weapon->GetGeomid();
        if (geomId == NoValidID)
        {
            return hit;
        }
        
        nPhysicsGeom* weaponGeom = static_cast<nPhysicsGeom*>(phyRetrieveDataGeom( geomId ));
        bool disableGeom = false;
        if (!weaponGeom->IsEnabled())
        {
            // the weapon geometry is disabled. So, enable it to detect the collision to the target,
            // and remember to disable it after that
            weaponGeom->Enable();
            disableGeom = true;
        }

        // determine collision
        const int numContacts = 1;
        nPhyCollide::nContact contacts[numContacts];
        unsigned int num(0);

        this->entity->GetComponent<ncPhyCharacterObj>()->Disable();

        if (this->isPlayer)
        {
            num = nPhysicsServer::Instance()->Collide(weaponGeom, numContacts, contacts);
        }
        else
        {
            ncPhyCharacterObj* targetPhyChar = this->target->GetComponent<ncPhyCharacterObj>();
            if (targetPhyChar)
            {
                num = targetPhyChar->Collide(weaponGeom, numContacts, contacts);
            }
        }

        this->entity->GetComponent<ncPhyCharacterObj>()->Enable();

        if (disableGeom)
        {
            weaponGeom->Disable();
        }        

        if ( num )
        {
            hit = true;

            ncSoundLayer* sound( this->weapon->GetComponent<ncSoundLayer>() );

            // target touched
            vector3 impactPos;
            contacts[0].GetContactPosition( impactPos );
            float damage = 0.0f;
            ncGameplayLiving * living = 0;
            n_assert( nDamageManager::Instance() );
            if ( nDamageManager::Instance() )
            {
                geomid contactGeom;
                ncPhysicsObj* targetObj = 0;
                if (contacts[0].GetGeometryIdA() != geomId)
                {
                    contactGeom = contacts[0].GetGeometryIdA();
                    targetObj = contacts[0].GetPhysicsObjA();
                }
                else
                {
                    contactGeom = contacts[0].GetGeometryIdB();
                    targetObj = contacts[0].GetPhysicsObjB();
                }

                if (this->isPlayer)
                {
                    living = targetObj->GetEntityObject()->GetComponent<ncGameplayLiving>();
                    if (living)
                    {
                        if ( nDamageManager::Instance()->MeleeAttackHitTarget(this->entity, targetObj->GetEntityObject(), this->weapon))
                        {
                            // calculate damage
                            damage = nDamageManager::Instance()->CalculateDamage( this->weapon, impactPos, 
                                                                                  contactGeom, targetObj->GetEntityObject());

                            ///call nfxeventtriggermethod to test where comes from?
                            //special fx for player
                            nFxEventTrigger::Instance()->MeleeSplash(
                                this->entity->GetComponentSafe<ncTransform>()->GetPosition(),
                                targetObj->GetEntityObject());

                            // launch sounds according to hitted object
                            if ( sound )
                            {
                                // alien and human has diferent impact sounds
                                if ( targetObj->GetEntityObject()->HasComponent("ncGameplayAlien") )
                                {
                                    sound->PlayAloneMaterial( "melee", "alien", 1 );
                                }
                                else
                                {
                                    sound->PlayAloneMaterial( "melee", "flesh", 1 );
                                }
                            }                            
                        }
                        else
                        {
                            // if enemy is blocking sounds like impact between weapons
                            if( sound )
                            {
                                sound->PlayAloneMaterial( "melee", "concrete", 1 );
                            }
                        }
                    }
                    else
                    {
                        nPhysicsGeom * geom = static_cast<nPhysicsGeom*>( phyRetrieveDataGeom( contactGeom ) );
                        if( geom )
                        {
                            nPhyRigidBody * body = geom->GetBody();
                            if ( body )
                            {
                                vector3 targetPos = targetObj->GetComponentSafe<ncTransform>()->GetPosition();
                                vector3 entityPos = this->entity->GetComponentSafe<ncTransform>()->GetPosition();
                                vector3 dirVect( targetPos - entityPos);
                                dirVect.norm();

                                ncGPWeaponMeleeClass* meleeClass = this->weapon->GetClassComponentSafe<ncGPWeaponMeleeClass>();
                                body->AddForce( dirVect * float(.1f) * meleeClass->GetDamageMelee() );

                                // get the material for plays specific sound
                                if( sound )
                                {
                                    sound->PlayAloneMaterial( "melee", geom->GetGameMaterialName(), 1 );
                                }
                            }                            
                        }
                    }
                }
                else
                {
                    living = this->target->GetComponent<ncGameplayLiving>();
                    if (living)
                    {
                        if ( nDamageManager::Instance()->MeleeAttackHitTarget(this->entity, this->target, this->weapon))
                        {
                            // calculate damage
                            damage = nDamageManager::Instance()->CalculateDamage(
                                this->weapon, impactPos, contactGeom, this->target);
                        }                        
                    }
                }
            }

            // apply damage to the target
            if ( living )
            {
                if ( nNetworkManager::Instance()->IsServer() )
                {
                    living->ApplyDamage( static_cast<int>( damage ) );
                }                
            }
        }
    }

    return hit;
}

//------------------------------------------------------------------------------
/**
    IsAtRange
*/
bool
nGPMelee::IsAtRange() const
{
    n_assert(this->init);

    bool range = false;
    ncTransform* transEntity = this->entity ? this->entity->GetComponent <ncTransform>() : 0;
    ncTransform* transTarget = this->target ? this->target->GetComponent <ncTransform>() : 0;

    if ( transEntity && transTarget )
    {
        ncGameplayLivingClass* entityGPClass = static_cast<ncGameplayLivingClass*>(this->entity->GetClassComponent <ncGameplayClass>());
        ncGameplayLivingClass* targetGPClass = static_cast<ncGameplayLivingClass*>(this->target->GetClassComponent <ncGameplayClass>());
        float hitDistance = 0.6f; 
        
        if ( entityGPClass && targetGPClass )
        {
            hitDistance = entityGPClass->GetSize() + targetGPClass->GetSize();
        }

        range = (transEntity->GetPosition() - transTarget->GetPosition()).lensquared() < pow (hitDistance, 2);
    }

    return range;
}