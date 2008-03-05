//-----------------------------------------------------------------------------
//  nphycollide_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphycollide.h"
#include "nphysics/nphysicsgeom.h"
#include "gameplay/ngamematerial.h"
#include "nphysics/nphymaterial.h"
#include "gameplay/ngamematerialserver.h"
#include "nspatial/nspatialserver.h"
#include "ngeomipmap/ncterrainmaterialclass.h"

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 27-Sep-2004   David Reyes    created
*/
nPhyCollide::nPhyCollide()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 27-Sep-2004   David Reyes    created
*/
nPhyCollide::~nPhyCollide()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Check collision between two geometries

    @param geomA        first geometry to be test for collision
    @param geomB        second geometry to be test for collision
    @param numContacts  how many contacts to be reported in case of collision
    @param contact      array of contacts (at least as much as numContacts)

    @return number of contacts reported

    history:
        - 27-Sep-2004   David Reyes    created
        - 06-Oct-2004   David Reyes    check if enabled
        - 11-Oct-2004   David Reyes    check if it's the same geometry
*/
int nPhyCollide::Collide( 
    const nPhysicsGeom *geomA, 
    const nPhysicsGeom *geomB, 
    int numContacts, nContact* contact )
{
    n_assert2( geomA , "Null pointer" );
    n_assert2( geomB , "Null pointer" );

    n_assert2( numContacts > 0 , "The number of contacts must be bigger than zero" );

    n_assert2( contact , "Null pointer" );

    if( geomA == geomB )
        return 0; // Doesn't check againts itself.

    if( !geomA->IsEnabled() )
        return 0;

    if( !geomB->IsEnabled() )
        return 0;

    return phyCollide( geomA->Id(), geomB->Id(), numContacts, &contact->GetContact(), sizeof(nContact));
}

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 27-Sep-2004   David Reyes    created
*/
nPhyCollide::nContact::nContact()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 27-Sep-2004   David Reyes    created
*/
nPhyCollide::nContact::~nContact()
{
    // Empty
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the contact information

    @param server   graphics server

    history:
        - 27-Sep-2004   David Reyes    created
*/
void nPhyCollide::nContact::Draw( nGfxServer2* server )
{
    static phyreal lengthNormal(0.25);

    vector3 vertexes[2];

    this->GetContactPosition( vertexes[0] );

    this->GetContactNormal( vertexes[1] );

    vertexes[1] = vertexes[1] * lengthNormal;

    vertexes[1] = vertexes[1] + vertexes[0];

    matrix44 transformmatrix;
    server->SetTransform( nGfxServer2::Model, transformmatrix );

    server->BeginLines();

    server->DrawLines3d( vertexes, 2, phy_color_normal_contact );

    server->EndLines();
}
#endif 
//-----------------------------------------------------------------------------
/**
    Check collision between two geometries

    @param geomA        first geometry to be test for collision
    @param geomB        second geometry to be test for collision
    @param numContacts  how many contacts to be reported in case of collision
    @param contact      array of contacts (at least as much as numContacts)

    @return number of contacts reported

    history:
        - 26-Sep-2004   David Reyes    created
*/
int nPhyCollide::Collide( 
    const geomid idA, 
    const geomid idB, 
    int numContacts, nContact* contact )
{

    n_assert2( idA != NoValidID, "Null pointer" );
    n_assert2( idB != NoValidID, "Null pointer" );

    n_assert2( numContacts > 0 , "The number of contacts must be bigger than zero" );

    n_assert2( contact , "Null pointer" );

    if( idA == idB )
        return 0; // Doesn't check againts itself.

    return phyCollide( idA, idB, numContacts, &contact->GetContact(), sizeof(nContact));
}

//-----------------------------------------------------------------------------
/**
    Returns the physics object A of the collision pair.

    @return physics object

    history:
        - 14-Oct-2004   David Reyes    created
*/
ncPhysicsObj* nPhyCollide::nContact::GetPhysicsObjA() const
{
    return GetGeometryA()->GetOwner();
}

//-----------------------------------------------------------------------------
/**
    Returns the physics object B of the collision pair.

    @return physics object

    history:
        - 14-Oct-2004   David Reyes    created
*/
ncPhysicsObj* nPhyCollide::nContact::GetPhysicsObjB() const
{
    return GetGeometryB()->GetOwner();
}

//-----------------------------------------------------------------------------
/**
    Returns the A physics object material.

    @return physics material

    history:
        - 16-Jan-2006   David Reyes    created
*/
nPhyMaterial* nPhyCollide::nContact::GetPhysicsMaterialA() const
{
    nPhysicsGeom* geom(this->GetGeometryA());

    n_assert2( geom, "Null pointer." );

    nPhyMaterial::idmaterial idmat(0);

    if( geom->Type() == nPhysicsGeom::HeightMap )
    {
        // special case
        nGameMaterial* material(this->GetMaterialFromTheTerrain());

        if( !material )
            return 0;

        return material->GetPhysicsMaterial();
    }
    else
    {
        idmat = geom->GetMaterial();
    }

    return nPhyMaterial::GetMaterial( idmat );
}

//-----------------------------------------------------------------------------
/**
    Returns the B physics object material.

    @return physics material

    history:
        - 16-Jan-2006   David Reyes    created
*/
nPhyMaterial* nPhyCollide::nContact::GetPhysicsMaterialB() const
{
    nPhysicsGeom* geom(this->GetGeometryB());

    n_assert2( geom, "Null pointer." );

    nPhyMaterial::idmaterial idmat(0);

    if( geom->Type() == nPhysicsGeom::HeightMap )
    {
        // special case
        nGameMaterial* material(this->GetMaterialFromTheTerrain());

        if( !material )
            return 0;

        return material->GetPhysicsMaterial();
    }
    else
    {
        idmat = geom->GetMaterial();
    }

    return nPhyMaterial::GetMaterial( idmat );
}

//-----------------------------------------------------------------------------
/**
    Returns the A object material.

    @return physics material

    history:
        - 16-Jan-2006   David Reyes    created
*/
nGameMaterial* nPhyCollide::nContact::GetGameMaterialA() const
{
    nPhysicsGeom* geom(this->GetGeometryA());

    n_assert2( geom, "Null pointer." );

    if( geom->Type() == nPhysicsGeom::HeightMap )
    {
        return this->GetMaterialFromTheTerrain();
    }

    return nGameMaterialServer::Instance()->GetMaterial(geom->GetGameMaterialId());
}

//-----------------------------------------------------------------------------
/**
    Returns the B object material.

    @return physics material

    history:
        - 16-Jan-2006   David Reyes    created
*/
nGameMaterial* nPhyCollide::nContact::GetGameMaterialB() const
{
    nPhysicsGeom* geom(this->GetGeometryB());

    n_assert2( geom, "Null pointer." );

    if( geom->Type() == nPhysicsGeom::HeightMap )
    {
        return this->GetMaterialFromTheTerrain();
    }

    return nGameMaterialServer::Instance()->GetMaterial(geom->GetGameMaterialId());
}

//-----------------------------------------------------------------------------
/**
    Returns the material from the terrain.

    @return game material

    history:
        - 16-Jan-2006   David Reyes    created
*/
nGameMaterial* nPhyCollide::nContact::GetMaterialFromTheTerrain() const
{
        ncTerrainMaterialClass* materialClass(nSpatialServer::Instance()->GetOutdoorEntity()->GetClassComponent<ncTerrainMaterialClass>());
        
        n_assert2( materialClass,"Null pointer" );

        vector3 position;

        this->GetContactPosition( position );
        
#ifndef __ZOMBIE_EXPORTER__
        return materialClass->GetGameMaterialAt( position.x, position.z );
#else
        return 0;
#endif
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

