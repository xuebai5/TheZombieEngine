#ifndef NCGPWEAPONMELEE_H
#define NCGPWEAPONMELEE_H
//------------------------------------------------------------------------------
/**
    @class ncGPWeaponMelee
    @ingroup Entities
    
    Component object with the behaviour of melee weapons
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "nphysics/nphysicsgeom.h"

//------------------------------------------------------------------------------
class ncPhySimpleObj;

//------------------------------------------------------------------------------
class ncGPWeaponMelee : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGPWeaponMelee,nComponentObject);

public:
    /// constructor
    ncGPWeaponMelee();
    /// destructor
    virtual ~ncGPWeaponMelee();

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// @name Script interface
    //@{
    /// Get the melee damage
    float GetDamageMelee ()const;

    /// Get the collision geomid
    geomid GetGeomid ();
    /// Set the collision geomid
    void SetGeomid(geomid);
    //@}

private:
    ncPhySimpleObj * collisionObject;
    geomid collisionid;

};

#endif//NCGPWEAPONMELEE_H
