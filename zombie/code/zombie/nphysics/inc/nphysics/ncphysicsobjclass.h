#ifndef NC_PHYSICSOBJCLASS_H
#define NC_PHYSICSOBJCLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncPhysicsObjClass
    @ingroup NebulaPhysicsSystem
    @brief An abstract physics object interface with some build-in functionality.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "entity/nentity.h"
#include "nphysics/nphysicsconfig.h"
//-----------------------------------------------------------------------------

class ncPhysicsObjClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncPhysicsObjClass,nComponentClass);

public:
    // constructor
    ncPhysicsObjClass();
    // destructor
    virtual ~ncPhysicsObjClass();

    /// sets the body mass
    void SetMass(phyreal);
    /// sets the body density
    void SetDensity(phyreal);

    /// gets the body mass
    const phyreal GetMass() const;
    /// gets the body density
    const phyreal GetDensity() const;

    /// sets if the object is mobile
    void SetMobile(const bool);
    /// gets if the object is mobile
    const bool GetMobile() const;

    /// Return the chukFile, create it by demand
    nFile* GetChunkFile();
    /// unload the chunk file;
    void UnLoadChunkFile();

    /// object persistency
    bool SaveCmds(nPersistServer *ps);   

private:

    /// stores the mass of the object
    phyreal objectMass;

    /// stores the density of the object
    phyreal objectDensity;

    /// stores if the object it's mobile
    bool mobile;

    ///
    nFile* chunkFile;

};

//-----------------------------------------------------------------------------
/**
    Sets the body mass.

    @param mass object's mass

    history:
        - 15-Jun-2005   Zombie         copied from nPhysicsObj
*/
inline
void ncPhysicsObjClass::SetMass( phyreal mass )
{
    n_assert2( mass >= 0, "The mass of an object cannot be negative" );
    this->objectMass = mass;
}

//-----------------------------------------------------------------------------
/**
    Sets the body mass.

    @param density object's density

    history:
        - 15-Jun-2005   Zombie         copied from nPhysicsObj
*/
inline
void ncPhysicsObjClass::SetDensity( phyreal density )
{
    n_assert2( density >= 0, "The density of an object cannot be negative" );
    this->objectDensity = density;
}

//-----------------------------------------------------------------------------
/**
    Gets the body mass.

    @return object's mass

    history:
        - 15-Jun-2005   Zombie         copied from nPhysicsObj
*/
inline
const phyreal ncPhysicsObjClass::GetMass() const
{
    return this->objectMass;
}

//-----------------------------------------------------------------------------
/**
    Gets the body density.

    @return object's density

    history:
        - 15-Jun-2005   Zombie         copied from nPhysicsObj
*/
inline
const phyreal ncPhysicsObjClass::GetDensity() const
{
    return this->objectDensity;
}

#endif