//------------------------------------------------------------------------------
//  ncgpweaponmelee_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponmelee.h"
#include "rnsgameplay/ncgpweaponmeleeclass.h"

#include "nphysics/ncphysimpleobj.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPWeaponMelee,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponMelee::ncGPWeaponMelee():
    collisionObject( 0 ), collisionid( NoValidID )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponMelee::~ncGPWeaponMelee()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncGPWeaponMelee::InitInstance( nObject::InitInstanceMsg initType )
{
    this->collisionObject = this->GetComponent<ncPhySimpleObj>();

    if( this->collisionObject && ( this->collisionObject->GetGeometry() != NoValidID ) )
    {
        bool valid = this->collisionObject->GetGeometry() != NoValidID;

        n_assert2( valid, "Weapon with collision, without collision geometry" );

        if( valid )
        {
            if (initType != nObject::ReloadedInstance)
            {
                this->collisionObject->InsertInTheWorld();
            }

            this->collisionObject->Disable();
        }
    }
}

//------------------------------------------------------------------------------
/**
    @returns the melee damage
*/
float
ncGPWeaponMelee::GetDamageMelee()const
{
    float damage = 0.0f;
    const ncGPWeaponMeleeClass * weaponClass = this->GetClassComponent<ncGPWeaponMeleeClass>();
    n_assert( weaponClass );
    if( weaponClass )
    {
        damage = weaponClass->GetDamageMelee();
    }

    return damage;
}

//------------------------------------------------------------------------------
/**
    @return the geomid of the collision object
*/
geomid
ncGPWeaponMelee::GetGeomid()
{
    if( this->collisionid != NoValidID )
    {
        return this->collisionid;
    }

    if( this->collisionObject )
    {
        if( this->collisionObject->GetGeometry() )
        {
            return this->collisionObject->GetGeometry()->Id();
        }
    }

    return NoValidID;
}

//------------------------------------------------------------------------------
/**
    @param id the new collision geometry
*/
void
ncGPWeaponMelee::SetGeomid( geomid id )
{
    n_assert2( id != NoValidID, "The geomid is not valid" );

    if( id != NoValidID )
    {
        this->collisionObject = 0;

        this->collisionid = id;
    }
}

//------------------------------------------------------------------------------
