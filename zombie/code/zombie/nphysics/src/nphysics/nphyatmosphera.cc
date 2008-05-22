//-----------------------------------------------------------------------------
//  nphyatmosphera_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyatmosphera.h"


//-----------------------------------------------------------------------------
const phyreal nPhyAtmosphera::airResistance = phyreal(-.01);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 29-Nov-2004   Zombie         created
*/
nPhyAtmosphera::nPhyAtmosphera()
{
    // Empty
}


//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 29-Nov-2004   Zombie         created
*/
nPhyAtmosphera::~nPhyAtmosphera()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Sets the density of the atmosphere 1 by default.

    @param density  atmosphere density

    history:
        - 29-Nov-2004   Zombie         created
*/
void nPhyAtmosphera::SetDensity( phyreal density )
{
    n_assert2( density > 0, "Atmosphera density cannot be negative" );

    this->densityAtmosphera = density;
}

//-----------------------------------------------------------------------------
/**
    Sets the wind.

    @param wind     direction and force

    history:
        - 29-Nov-2004   Zombie         created
*/
void nPhyAtmosphera::SetWind( const vector3& wind )
{
    this->windAtmosphera = wind;
}

//-----------------------------------------------------------------------------
/**
    Gets the density.

    @return atmosphera density

    history:
        - 29-Nov-2004   Zombie         created
*/
phyreal nPhyAtmosphera::GetDensity() const
{
    return this->densityAtmosphera;
}

//-----------------------------------------------------------------------------
/**
    Gets the wind.

    @param wind     direction and force

    history:
        - 29-Nov-2004   Zombie         created
*/
void nPhyAtmosphera::GetWind( vector3& wind ) const
{
    wind = this->windAtmosphera;
}

//-----------------------------------------------------------------------------
/**
    Sets wind origin.

    @param windorigin   world position where the wind starts

    history:
        - 29-Nov-2004   Zombie         created
*/
void nPhyAtmosphera::SetWindOrigin( const vector3& windorigin )
{
    this->windAtmoespheraOrigin = windorigin;
}

//-----------------------------------------------------------------------------
/**
    Gets wind origin.

    @return   world position where the wind starts

    history:
        - 29-Nov-2004   Zombie         created
*/
void nPhyAtmosphera::GetWindOrigin( vector3& windorigin ) const
{
    windorigin = this->windAtmoespheraOrigin;
}

//-----------------------------------------------------------------------------
/**
    Process the atmosphera effects.

    @param  list of objects

    history:
        - 29-Nov-2004   Zombie         created
*/
void nPhyAtmosphera::Process( const nKeyArray< nPhyRigidBody* >& bodies )
{
    // begin: temporary code
    /// Applying wind resistant
    int NumberRigidBodies( bodies.Size() );

    for( int index(0); index < NumberRigidBodies; ++index )
    {
#ifdef PHY_ATMOSPHERE_RESISTANCE_BY_FORCE
        nPhyRigidBody* body(bodies.GetElementAt( index ));

        n_assert2( body, "Data corruption" );

        /// linear velocity
        vector3 velocity;

        body->GetLinearVelocity( velocity );

        phyreal absolutvelocity(velocity.len());

        vector3 windresistance( velocity );

        windresistance.norm();

        windresistance*= (absolutvelocity * -airResistance);

        body->AddForce( windresistance );

        /// angular velocity
        body->GetAngularVelocity( velocity );

        absolutvelocity = velocity.len();

        windresistance = velocity;

        windresistance.norm();

        windresistance*= (absolutvelocity * -airResistance);

        body->AddTorque( windresistance );
#else
        nPhyRigidBody* body(bodies.GetElementAt( index ));

        n_assert2( body, "Data corruption" );

        if( !body->IsEnabled() )
            continue;

        if( !body->IsAtmospheraAffected() )
            continue;

        /// checking it is under water
        if( body->GetPhysicsObj()->IsState( ncPhysicsObj::InWater ) )
            continue;

        /// linear velocity
        vector3 velocity;

        body->GetLinearVelocity( velocity );

        static phyreal minvalue( phyreal(1.0e-006) ) ;

        if( velocity.len() > body->GetLinearVelocityThresholdAutoDisable() )
            body->AddForce( velocity * airResistance );

        /// angular velocity
        body->GetAngularVelocity( velocity );

        if( velocity.len() > body->GetAngularVelocityThresholdAutoDisable() )
            body->AddTorque( velocity * airResistance);
#endif
    }
    // end: temporary code    
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

