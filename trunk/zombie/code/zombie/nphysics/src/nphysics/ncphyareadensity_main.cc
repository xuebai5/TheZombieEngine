//-----------------------------------------------------------------------------
//  ncphyareadensity_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyareadensity.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyAreaDensity,ncPhySimpleObj);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 14-Jan-2005   Zombie         created
*/
ncPhyAreaDensity::ncPhyAreaDensity() :
    fluidResistance(0)
{
    this->SetType( Fluid );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 14-Jan-2005   Zombie         created
*/
ncPhyAreaDensity::~ncPhyAreaDensity()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Check if the box is under the fluid.

    @param box nPhysicsAABB to mark the volume to check

    @return if it's under the fluid

    history:
        - 14-Jan-2005   Zombie         created
*/
bool ncPhyAreaDensity::IsUnderFluid( const nPhysicsAABB& /*box*/ ) const
{
    // Empty
    return false;
}

//-----------------------------------------------------------------------------
/**
    Process function.

    history:
        - 14-Jan-2005   Zombie         created
*/
void ncPhyAreaDensity::PreProcess()
{
    nPhysicsWorld* world(this->GetWorld());

    n_assert2( world, "The world must exists." );

    int end(world->GetNumPhysicObjs());

    for( int index(0); index < end; ++index )
    {
        ncPhysicsObj* obj( world->GetPhysicObj( index ) );

        n_assert2( obj, "Data corruption" );

        nPhyRigidBody* body(obj->GetBody());

        if( !body )
        {
            continue;
        }

        nPhysicsAABB box;

        obj->GetAABB( box );

        if( !this->IsUnderFluid( box ) )
        {
            continue;        
        }
    }
    
}

//-----------------------------------------------------------------------------
/**
    Function to be procesed during the collision check.

    @param numContacts maximun number of contacts expected
    @param contacts container of contacts

    @return if any collision was processed

    history:
        - 17-Jan-2005   Zombie         created
*/
bool ncPhyAreaDensity::CollisionProcess( int /*numContacts*/, nPhyCollide::nContact* /*contacts*/ )
{
    return false;
}

//-----------------------------------------------------------------------------
/**
    Resistance of the fluid.

    @param obj object that will be resisted by the fluid

    history:
        - 17-Mar-2005   Zombie         created
*/
void ncPhyAreaDensity::Resistance( ncPhysicsObj* obj )
{
    n_assert2( obj, "Null pointer" );

    nPhyRigidBody* body( obj->GetBody() );

    n_assert2( body, "Invalid object to be processed." );

    vector3 velocity;

    body->GetAngularVelocity( velocity );

    velocity *= this->GetResistance();

    body->SetAngularVelocity( velocity );

    body->GetLinearVelocity( velocity );

    velocity *= this->GetResistance();

    body->SetLinearVelocity( velocity );
}


//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 17-May-2005   Zombie         created
*/
void ncPhyAreaDensity::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhySimpleObj::InitInstance(initType);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

