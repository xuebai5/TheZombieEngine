//-----------------------------------------------------------------------------
//  ncphysimpleobj_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphysimpleobj.h"
#include "nphysics/nphysicsserver.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhySimpleObj,ncPhysicsObj);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 29-Nov-2004   Zombie         created
*/
ncPhySimpleObj::ncPhySimpleObj() :
    geometry(0)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 29-Nov-2004   Zombie         created
*/
ncPhySimpleObj::~ncPhySimpleObj()
{
    if( this->GetGeometry() )
    {
        this->GetGeometry()->MoveToSpace( NoValidID );
        this->GetGeometry()->SetOwner(0); // resets owner
        this->GetGeometry()->Release();
        this->geometry = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the geometry.

    @param geom     the new geometry

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhySimpleObj::SetGeometry( nPhysicsGeom* geom )
{
    n_assert2( geom, "Null pointer" );

    n_assert2( !geom->IsSpace(), "It cannot be a space" );

    this->geometry = geom;

    geom->SetOwner(this);

#ifndef NGAME
    geom->AddCollidesWith( nPhysicsGeom::Check );
#endif
}

//-----------------------------------------------------------------------------
/**
    Enables the physic object.

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhySimpleObj::Enable()
{
    n_assert2( this->GetGeometry(), "Not valid object" );

    this->GetGeometry()->Enable();

    nPhyRigidBody* body(this->GetGeometry()->GetBody());

    if( body )
    {
        body->Enable();
    }

    ncPhysicsObj::Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the physic object.

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhySimpleObj::Disable()
{
    n_assert2( this->GetGeometry(), "Not valid object" );
     
    this->GetGeometry()->Disable();

    nPhyRigidBody* body(this->GetBody());

    if( body )
    {
        body->Disable();
    }
 
    ncPhysicsObj::Disable();
}


//-----------------------------------------------------------------------------
/**
    Picks an object trough a given geometry.

    @param geom     geometry to use to pick
    @param contact  contact information

    history:
        - 29-Nov-2004   Zombie         created
*/
geomid ncPhySimpleObj::ObjectPicking( const nPhysicsGeom* geom, nPhyCollide::nContact& contact )
{
    n_assert2( geom, "Null pointer" );

    if( !this->GetGeometry() )
    {
        return NoValidID;
    }

    if( nPhyCollide::Collide(
        geom, 
        this->GetGeometry(),
        1,
        &contact ) )
    {
        return contact.GetGeometryIdB();
    }

    return NoValidID;
}

//-----------------------------------------------------------------------------
/**
    Moves the physic object to another space.

    @param newspace     space

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhySimpleObj::MoveToSpace( nPhySpace* newspace )
{
    n_assert2( this->GetGeometry(), "Not valid object" );

    this->GetGeometry()->MoveToSpace( newspace );
    
    ncPhysicsObj::MoveToSpace( newspace );
}

//-----------------------------------------------------------------------------
/**
    Checks if collision with other physics object.

    @param obj          physics object
    @param numContacts  max number of contacts to check
    @param contact      buffer with the information of the contacts

    history:
        - 29-Nov-2004   Zombie         created
*/
int ncPhySimpleObj::Collide( const ncPhysicsObj* obj,
    int numContacts, nPhyCollide::nContact* contact ) const
{
    if( !this->GetGeometry() )
        return 0;

    return obj->Collide( this->GetGeometry(), numContacts, contact );
}

//-----------------------------------------------------------------------------
/**
    Checks if collision with a geometry.

    @param geom         geometry to collide with
    @param numContacts  max number of contacts to check
    @param contact      buffer with the information of the contacts

    history:
        - 29-Nov-2004   Zombie         created
*/
int ncPhySimpleObj::Collide( const nPhysicsGeom* geom,
    int numContacts, nPhyCollide::nContact* contact ) const
{
    if( !this->GetGeometry() )
        return 0;

    return this->GetGeometry()->Collide( geom, numContacts, contact );
}


//-----------------------------------------------------------------------------
/**
    Create the object.

    @param world it's the world where the object will be come to existance

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhySimpleObj::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer" );

    ncPhysicsObj::Create( world );

    nPhyRigidBody *body(this->GetBody());

    if( body )
    {
        this->SetBody( body );

        vector3 position;

        this->GetPosition(position);

        this->GetBody()->SetPosition(position);

        matrix33 orientation;

        this->GetOrientation(orientation);

        this->GetBody()->SetOrientation(orientation);

        /// assigns to each geometry de body
        body->Update( this->GetGeometry(), this->GetMass(), this->GetDensity() );

        body->SetPhysicsObj( this );
    }
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 09-May-2004   Zombie         created
*/
void ncPhySimpleObj::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhysicsObj::InitInstance(initType);
}

//-----------------------------------------------------------------------------
/**
    Creates an object composited.

    @param name geometry class name
    @return geometry

    history:
        - 09-May-2005   Zombie         created
*/
nPhysicsGeom* ncPhySimpleObj::CreateGeometryObj( const nString& name ) 
{
    n_assert2( name != "", "Invalid geometry name object." );

    nPhysicsGeom* geometry(static_cast<nPhysicsGeom*>(nKernelServer::Instance()->New( name.Get() )));

    n_assert2( geometry, "Failed to create a geometry." );

    this->SetGeometry( geometry );

    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(geometry, nObject::LoadedInstance);
    
    return geometry;
}

//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void ncPhySimpleObj::Scale( const phyreal factor )
{
    if( factor == this->GetLastScaledFactor() )
        return;    

    /// updating geometry
    if( this->GetGeometry() )
    {
        this->GetGeometry()->Scale( factor / this->GetLastScaledFactor() );
    }

    /// updating mass distribution
    if( this->GetBody() )
    {
        this->GetBody()->Update( this->GetGeometry(), this->GetMass(), this->GetDensity() );
    }
}

//-----------------------------------------------------------------------------
/**
    Checks if the object collides with anything in his own world.

    @param numContacts number maximun of contacts expected
    @param contact array of contacts structures information

    @return number of contacts reported

    history:
        - 13-May-2005   Zombie         created
*/
int ncPhySimpleObj::Collide( int numContacts, nPhyCollide::nContact* contact ) const
{
    n_assert2( contact, "Null pointer." );

    return nPhysicsServer::Instance()->Collide( this->GetGeometry(), numContacts, contact );
}

//-----------------------------------------------------------------------------
/**
    Sets the categories where the object belongs.

    @param categories categories flags

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhySimpleObj::SetCategories( int categories ) 
{
    n_assert2( this->GetGeometry(), "It requieres the geometry." );

    this->GetGeometry()->SetCategories( categories );

    ncPhysicsObj::SetCategories( categories );
}

//-----------------------------------------------------------------------------
/**
    Sets the categories wich the object will collide with.

    @param categories categories flags

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhySimpleObj::SetCollidesWith( int categories ) 
{
    n_assert2( this->GetGeometry(), "It requieres the geometry." );

    this->GetGeometry()->SetCollidesWith( categories );
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Adds a collide with.

    @param category category flag

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhySimpleObj::RemovesCollidesWith( int category )
{
    this->GetGeometry()->RemovesCollidesWith( nPhysicsGeom::Category(category) );

    ncPhysicsObj::RemovesCollidesWith( category );
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
