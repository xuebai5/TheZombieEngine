#ifndef NDAMAGEMANAGER_H
#define NDAMAGEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nDamageManager

    @brief Module that calculate the damage produced to an entity.
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "nphysics/nphysicsgeom.h"

//------------------------------------------------------------------------------
class ncGPWeapon;
class ncGPWeaponMelee;

//------------------------------------------------------------------------------
class nDamageManager : public nObject
{
public:
    enum DamageType{
        DAMAGE_MELEE	    = ( 1 << 0 ), // Damage coming from melee hits
        DAMAGE_BULLET	    = ( 1 << 1 ), // Damage made my most of the weapon ammunition
        DAMAGE_PSIONIC	    = ( 1 << 2 ), // Damage made by alien psionic area attacks
        DAMAGE_SPLASH	    = ( 1 << 3 ), // Damage applied over an area, with a central point and a radius
        DAMAGE_ENVIRONMENT	= ( 1 << 4 ), // Damage coming from enviromental sources like fire
        DAMAGE_GAS	        = ( 1 << 5 ), // Environmental damage that can be avoided by using a gas mask
        DAMAGE_VACUUM	    = ( 1 << 6 ), // Environmental damage that only applies when the entity is out of breath
        DAMAGE_STUN         = ( 1 << 7 ), // Damage that stuns the target instead of killing it
        DAMAGE_FALLING	    = ( 1 << 8 ), // Damage taken when falling from certain heights

        DAMAGE_LAST
    };

    /// constructor
    nDamageManager();
    /// destructor
    ~nDamageManager();

    /// return singleton instance
    static nDamageManager * Instance();

    /// calculate damage from a weapon
    float CalculateDamage( ncGPWeapon * weapon, 
                           const vector3 &point, 
                           geomid obj, 
                           nEntityObject * entity );

    /// calculate damage from a weapon
    float CalculateDamage( ncGPWeaponMelee * weapon, 
                           const vector3 &point, 
                           geomid obj, 
                           nEntityObject * entity);

    /// calculate the damage to apply to the given entity using the weapon damage
    float CalculateDamage( float weaponDamage, 
                           geomid obj, 
                           nEntityObject * entity );

    /// calculate damage from area types
    float CalculateDamage( DamageType type, const float power, const vector3 &center, const float radius, nEntityObject * entity );

    /// calculate damage from explosion
    float CalculateDamage( const float power, const vector3 & center, const float radius, nEntityObject * entity );

    bool MeleeAttackHitTarget ( nEntityObject* attacker, nEntityObject* victim, ncGPWeaponMelee* weapon);

private:

    static nDamageManager * singleton;
    const static float MIN_FALL_DISTANCE;
};

//------------------------------------------------------------------------------
#endif//NDAMAGEMANAGER_H
