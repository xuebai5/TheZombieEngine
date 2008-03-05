#ifndef N_PHYCOLLIDE_H
#define N_PHYCOLLIDE_H

//-----------------------------------------------------------------------------
/**
    @class nPhyCollide
    @ingroup NebulaPhysicsSystem
    @brief A class to treat the different collisions.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsconfig.h"

//-----------------------------------------------------------------------------

#ifndef NGAME
    class nGfxServer2;
#endif

class nPhysicsGeom;
class ncPhysicsObj;
class nGameMaterial;
class nPhyMaterial;

//-----------------------------------------------------------------------------
class nPhyCollide {
public:

    class nContact {
    public:
        /// constructor
        nContact();
        /// destructor
        ~nContact();

        /// returns the contact information for the physics engine
        contactgeom& GetContact();

        /// returns the contact information for the physics engine
        const contactgeom& GetContact() const;

        /// returns the contact position
        void GetContactPosition( vector3& contact ) const;

        /// returns the normal vector where the contact has happened
        void GetContactNormal( vector3& normal ) const;

        /// returns how deep it's the contact
        phyreal GetContactPenetrationDepth() const;

        /// returns the geometry A id of the collision pair
        geomid GetGeometryIdA() const;

        /// returns the geometry B id of the collision pair
        geomid GetGeometryIdB() const;

        /// returns the geometry A of the collision pair
        nPhysicsGeom* GetGeometryA() const;

        /// returns the geometry B of the collision pair
        nPhysicsGeom* GetGeometryB() const;

        /// returns the physics object A of the colliding pair
        ncPhysicsObj* GetPhysicsObjA() const;

        /// returns the physics object B of the colliding pair
        ncPhysicsObj* GetPhysicsObjB() const;

        /// returns the A object material
        nGameMaterial* GetGameMaterialA() const;

        /// returns the B object material
        nGameMaterial* GetGameMaterialB() const;

        /// returns the A physics object material
        nPhyMaterial* GetPhysicsMaterialA() const;

        /// returns the B physics object material
        nPhyMaterial* GetPhysicsMaterialB() const;

#ifndef NGAME
        /// draws the contact information
        void Draw( nGfxServer2* server );
#endif

    private:
        /// contact structure from the physics engine
        contactgeom contactGeom;        

        /// returns the material from the terrain
        nGameMaterial* GetMaterialFromTheTerrain() const;

    };

    /// constructor
    nPhyCollide();

    /// destructor
    ~nPhyCollide();

    /// check collision between two geometries
    static int Collide( 
        const nPhysicsGeom *geomA, 
        const nPhysicsGeom *geomB, 
        int numContacts, nContact* contact );

    /// check collision between two geometries
    static int Collide( 
        const geomid idA, 
        const geomid idB, 
        int numContacts, nContact* contact );
};

//-----------------------------------------------------------------------------
/**
    Returns the contact information for the physics engine

    @return structure containing the contact info

    history:
        - 27-Sep-2004   David Reyes    created
*/
inline
contactgeom& nPhyCollide::nContact::GetContact()
{
    return this->contactGeom;
}

//-----------------------------------------------------------------------------
/**
    Returns the contact information for the physics engine

    @return structure containing the contact info

    history:
        - 27-Sep-2004   David Reyes    created
*/
inline
const contactgeom& nPhyCollide::nContact::GetContact() const
{
    return this->contactGeom;
}

//-----------------------------------------------------------------------------
/**
    Returns the contact position

    @param contact a vector with the position of the contact

    history:
        - 27-Sep-2004   David Reyes    created
*/
inline
void nPhyCollide::nContact::GetContactPosition( vector3& contact ) const
{
    phyGetContactPoint( this->GetContact(), contact );
}

//-----------------------------------------------------------------------------
/**
    Returns the normal vector where the contact has happened

    @param a vector with the normal of the contact

    history:
        - 27-Sep-2004   David Reyes    created
*/
inline
void nPhyCollide::nContact::GetContactNormal( vector3& normal ) const
{
    phyGetContactNormal( this->GetContact(), normal );
}

//-----------------------------------------------------------------------------
/**
    Returns how deep it's the contact

    @return Penetration Depth

    history:
        - 27-Sep-2004   David Reyes    created
*/
inline
phyreal nPhyCollide::nContact::GetContactPenetrationDepth() const
{
    return phyGetContactPenetrationDepth( this->GetContact() );
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry A id of the collision pair

    @return geometry id

    history:
        - 28-Sep-2004   David Reyes    created
*/
inline
geomid nPhyCollide::nContact::GetGeometryIdA() const
{
    return phyGetContactGeometryA( this->GetContact() );
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry B id of the collision pair

    @return geometry id

    history:
        - 28-Sep-2004   David Reyes    created
*/
inline
geomid nPhyCollide::nContact::GetGeometryIdB() const
{
    return phyGetContactGeometryB( this->GetContact() );
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry A of the collision pair.

    @return geometry

    history:
        - 11-Oct-2004   David Reyes    created
*/
inline
nPhysicsGeom* nPhyCollide::nContact::GetGeometryA() const
{
    return static_cast<nPhysicsGeom*>(phyRetrieveDataGeom(this->GetGeometryIdA()));
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry B of the collision pair.

    @return geometry

    history:
        - 11-Oct-2004   David Reyes    created
*/
inline
nPhysicsGeom* nPhyCollide::nContact::GetGeometryB() const
{
    return static_cast<nPhysicsGeom*>(phyRetrieveDataGeom( this->GetGeometryIdB() ));
}

#endif 