//------------------------------------------------------------------------------
//  ndamagemanager.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "zombieentity/nctransform.h"

#include "rnsgameplay/ndamagemanager.h"
#include "rnsgameplay/ncgpweapon.h"
#include "rnsgameplay/ncgpweaponmelee.h"
#include "rnsgameplay/ncgpweaponmeleeclass.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgpperception/ncgpsight.h"

#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/ncphysicsobj.h"

#include "gameplay/ngamematerial.h"

#include "mathlib/cone.h"

//------------------------------------------------------------------------------
nNebulaClass(nDamageManager, "nobject");

nDamageManager * nDamageManager::singleton = 0;

const float nDamageManager::MIN_FALL_DISTANCE = 2.0f;
//------------------------------------------------------------------------------
/**
*/
nDamageManager::nDamageManager()
{
    nDamageManager::singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nDamageManager::~nDamageManager()
{
    nDamageManager::singleton = 0;
}

//------------------------------------------------------------------------------
/**
    @returns the nDamageManager singleton instance
*/
nDamageManager * 
nDamageManager::Instance(){
    n_assert( nDamageManager::singleton );
    return nDamageManager::singleton;
}

//------------------------------------------------------------------------------
/**
    @param weapon weapon that causes the damage
    @param point where causes the damage in the entity
    @param obj geometry object the weapon has collided with
    @param entity entity that gets the damage
    @returns the damage
*/
float 
nDamageManager::CalculateDamage( ncGPWeapon * weapon, 
                                 const vector3 &/*point*/, 
                                 geomid obj, 
                                 nEntityObject * entity)
{
    float damage = 0.0f;

    n_assert( weapon );
    n_assert( entity );

    if (weapon && entity)
    {
        damage = this->CalculateDamage( weapon->GetDamage(), obj, entity );
    }

    return damage;
}

//------------------------------------------------------------------------------
/**
    @param weapon melee weapon that causes the damage
    @param point where causes the damage in the entity
    @param obj geometry object the weapon has collided with
    @param entity entity that gets the damage
    @returns the damage
*/
float 
nDamageManager::CalculateDamage( ncGPWeaponMelee * weapon, 
                                 const vector3 &/*point*/, 
                                 geomid obj, 
                                 nEntityObject * entity )
{
    float damage = 0.0f;

    n_assert( weapon );
    n_assert( entity );

    if (weapon && entity)
    {
        damage = this->CalculateDamage( weapon->GetDamageMelee(), obj, entity );
    }

    return damage;
}

//------------------------------------------------------------------------------
/**
    @param weaponDamage damage caused by the weapon
    @param obj geometry object the weapon has collided with
    @param entity entity that gets the damage
    @returns the damage
*/
float 
nDamageManager::CalculateDamage( float weaponDamage, 
                                 geomid obj, 
                                 nEntityObject * entity )
{
    n_assert(entity);

    if ( entity )
    {
        ncGameplayLiving *gpLiving = entity->GetComponentSafe<ncGameplayLiving>();
        ncGameplayLivingClass *gpLivingClass = entity->GetClassComponentSafe<ncGameplayLivingClass>();
        if (gpLiving && gpLivingClass)
        {
            ncGameplayLivingClass::BodyPart bPart = gpLiving->GetBodyPart(obj);
            return gpLivingClass->GetDamageModification(bPart) * weaponDamage;
        }
    }

    // if there's no entity to hit or it's not a living entity, it's not damaged
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
    @param power damage of the area effect
    @param center center of the area
    @param radius radius of the area
    @param entity entity that gets the damage
    @returns the damage
*/
float
nDamageManager::CalculateDamage( 
    const float power, const vector3 & center, const float radius, nEntityObject * entity )
{
    float damage = 0.0f;

    float damageFactor = 1.0f;

    // get componentes
    ncTransform * transform = entity->GetComponentSafe<ncTransform>();
    ncPhysicsObj * physic = entity->GetComponentSafe<ncPhysicsObj>();

    nPhyGeomRay * explosionRay = static_cast<nPhyGeomRay*>( nKernelServer::ks->New( "nphygeomray" ) );
    n_assert( explosionRay );
    if( explosionRay )
    {
        static const int numContacts = 10;
        nPhyCollide::nContact contacts[ numContacts ];

        explosionRay->SetCategories(0);
        explosionRay->SetCollidesWith( ~nPhysicsGeom::Check );

        // check magnitude of damage
        if( transform )
        {
            // get block objects between area center and entity
            vector3 direction = transform->GetPosition() - center;
            float dist = direction.len();

            explosionRay->SetLength( dist );
            direction.norm();
            explosionRay->SetDirection( direction );
            explosionRay->SetPosition( center );

            bool restorePhysic = false;

            if( physic )
            {
                restorePhysic = physic->IsEnabled();
                physic->Disable();
            }

            int num = nPhysicsServer::Instance()->Collide( explosionRay, numContacts, contacts );

            if( physic && restorePhysic )
            {
                physic->Enable();
            }

            if( num )
            {
                // remove damage. Half the damage for every soft material 
                // and all the damage for hard material
                nGameMaterial * material = 0;

                for( int i = 0 ; i < num ; ++i )
                {
                    if( explosionRay->Id() != contacts[i].GetGeometryIdA() )
                    {
                        material = contacts[i].GetGameMaterialA();
                    }
                    else
                    {
                        material = contacts[i].GetGameMaterialB();
                    }

                    if( material && material->GetHard() )
                    {
                        damageFactor = 0.0f;
                        break;
                    }
                    else
                    {
                        damageFactor *= 0.5f;
                    }
                }

            }

            // calculate damage using distance from center
            if( ( radius > 0.0f ) && ( dist < radius ) )
            {
                damage = power * ( radius - dist ) * ( 1.0f / radius ) ;
            }
        
            // apply reduction of block objects
            damage = damage * damageFactor;
        }

        explosionRay->Release();
    }

    return damage;
}

//------------------------------------------------------------------------------
/**
    @param type type of damage
    @param power how hard is the damage
    @param center center of the damage effect
    @param radius radius of the damage effect
    @param entity entity that gets the damage
    @returns the damage
*/
float
nDamageManager::CalculateDamage(
    DamageType type, const float power, const vector3 &center, const float radius, nEntityObject * entity )
{
    float damage = 0.0f;

    switch( type )
    {
        // Damage coming from melee hits
    case DAMAGE_MELEE:
        damage = power;
        break;

    case DAMAGE_BULLET:
        n_assert2_always( "Use CalculateDamage( weapon, point, entity ) instead" );
        break;

        // Damage applied over an area, with a central point and a radius
    case DAMAGE_SPLASH:
        {
            vector3 pos;
            ncTransform * transform = entity->GetComponent<ncTransform >();
            n_assert( transform );
            if( transform )
            {
                pos = transform->GetPosition();
                float dist = (pos - center).len();
                if( dist < radius )
                {
                    damage = power * ( radius - dist ) * ( 1.0f / radius ) ;
                }
            }
        }
        break;

        // Damage coming from enviromental sources like fire
    case DAMAGE_ENVIRONMENT:
        damage = power;
        break;

        // Environmental damage that can be avoided by using a gas mask
    case DAMAGE_GAS:
        /// @todo ZHEN if( ! entity->HasGasMask() )
        {
            damage = power;
        }
        break;
        
        // Environmental damage that only applies when the entity is out of breath
    case DAMAGE_VACUUM:
        /// @todo ZHEN if( entity->Breath() <= 0 )
        {
            damage = power;
        }
        break;

        // Damage taken when falling from certain heights
    case DAMAGE_FALLING:
        // the center is used for get the init of fall
        {
            vector3 pos;
            ncTransform * transform = entity->GetComponent<ncTransform >();
            n_assert( transform );
            if( transform )
            {
                pos = transform->GetPosition();
                float dist = n_abs( pos.y  - center.y );
                if( dist > nDamageManager::MIN_FALL_DISTANCE )
                {
                    damage = power * dist;
                }
            }
        }
        break;

    default:
        n_assert2_always( "Damage Type not defined" );
    }

    return damage;
}

/*------------------------------------------------------------------------------
@param Entity that execute the attack
@param Entity that recive the attack
*/
bool
nDamageManager::MeleeAttackHitTarget( nEntityObject* giver , nEntityObject* receiver, ncGPWeaponMelee* /*weapon*/ )
{
    ncGameplayLiving* receiverLiving = receiver->GetComponentSafe<ncGameplayLiving>();    
    ncGameplayLivingClass* recLivingClass = receiver->GetClassComponentSafe<ncGameplayLivingClass>();

    bool hitTarget = true;

    if ( receiverLiving->IsBlocking() )
    {
        ncTransform* recTrComp = receiver->GetComponentSafe<ncTransform>();
        const vector3 &givPos = giver->GetComponentSafe<ncTransform>()->GetPosition();

        // Test if the giver is in the receiver sight
        const matrix44 &m = recTrComp->GetTransform();
        float coneRadius = 15.f * sin(n_deg2rad((recLivingClass->GetFOVAngle() + 5) * 0.5f));
        cone recTestCone(2.f, coneRadius, m);
        if ( recTestCone.containsInAngle( givPos ) )
        {
            hitTarget = false;
        }
    }

    return hitTarget;

}

//------------------------------------------------------------------------------
