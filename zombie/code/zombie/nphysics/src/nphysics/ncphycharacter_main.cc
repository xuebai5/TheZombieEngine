//-----------------------------------------------------------------------------
//  ncphycharacter_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

#include "nphysics/ncphycharacter.h"

#include "nphysics/nphygeomcylinder.h"

#include "nphysics/nphyangularmotorjoint.h"

#include "nphysics/nphygeomcylinder.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyCharacter,ncPhyCompositeObj);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 29-Aug-2005   Zombie         created
*/
ncPhyCharacter::ncPhyCharacter() : 
    height(phyreal(2)),
    wideness(phyreal(0.5)),
    motor(0),
    cylinder(0),
    active(true)
{
    //empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 29-Aug-2005   Zombie         created
*/
ncPhyCharacter::~ncPhyCharacter()
{
    if( this->motor )
    {
        this->motor->Release();
    }

    this->cylinder = 0;
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 29-Aug-2005   Zombie         created
*/
void ncPhyCharacter::InitInstance(nObject::InitInstanceMsg initType)
{   
    ncPhyCompositeObj::InitInstance( initType );
}

//-----------------------------------------------------------------------------
/**
    Sets the character height.

    @param newheight height in game units

    history:
        - 29-Aug-2005   Zombie         created
*/
void ncPhyCharacter::SetHeight( phyreal newheight )
{
    n_assert2( newheight >= 0, "The character height has to be positive and bigger than zero." );

    this->height = newheight;

    if( this->cylinder )
    {
        this->cylinder->SetLength( this->height );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the character wideness.

    @param wide wideness in game units

    history:
        - 29-Aug-2005   Zombie         created
*/
void ncPhyCharacter::SetWide( phyreal wide )
{
    n_assert2( wide >= 0, "The character wide has to be positive and bigger than zero." );

    this->wideness = wide;

    if( this->cylinder )
    {
        this->cylinder->SetRadius( this->wideness / phyreal(2)  );
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world world where the object will be created

    history:
        - 29-Aug-2005   Zombie         created
*/
void ncPhyCharacter::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer." );

    if( this->GetMass() == 0 )
    {
        // default mass
        this->SetMass( 90 );
    }

    this->cylinder = static_cast<nPhyGeomCylinder*>(nKernelServer::Instance()->New( "nphygeomcylinder" ));

    n_assert2( this->cylinder, "Failed to create a geometry." );

    this->cylinder->SetRadius( this->wideness / phyreal(2) );

    this->cylinder->SetLength( this->height );

    this->cylinder->SetOrientation( n_deg2rad(90.0f), 0, 0 );

    this->cylinder->SetPosition( vector3(0, this->height / phyreal(2),0) );

    this->Add( this->cylinder );
    
    ncPhyCompositeObj::Create( world );

    this->RegisterForPreProcess();
    this->RegisterForPostProcess();

    this->motor = static_cast<nPhyAngularMotorJoint*>(nKernelServer::Instance()->New("nphyangularmotorjoint"));

    n_assert2( this->motor, "Failed to create an angular motor for the nPhyCharacter." );

    this->motor->CreateIn( world, 0 );

    this->motor->SetNumberOfAxis( 3 );

    this->motor->SetFirstAxis( vector3(1, 0, 0 ));
    this->motor->SetSecondAxis( vector3(0, 1, 0 ));
    this->motor->SetThirdAxis( vector3(0, 0, 1 ));

    this->motor->SetParam( phy::maxForce, phy::axisA, phyreal(100000) );
    this->motor->SetParam( phy::maxForce, phy::axisB, phyreal(100000) );
    this->motor->SetParam( phy::maxForce, phy::axisC, phyreal(100000) );

    this->motor->Attach( this->GetBody(), 0 );

    this->SetCollidesWith( nPhysicsGeom::All & (~nPhysicsGeom::Stairs) );
}

//-----------------------------------------------------------------------------
/**
    Function to be process b4 running the simulation.

    history:
        - 29-Aug-2005   Zombie         created
*/
void ncPhyCharacter::PreProcess()
{
    if( !this->active )
    {
        return;
    }

    this->GetBody()->SetOrientation( matrix33() );
    // enables the geometry
    this->GetGeometry(0)->Enable();
}

//-----------------------------------------------------------------------------
/**
    Function to be process after running the simulation.

    history:
        - 29-Aug-2005   Zombie         created
*/
void ncPhyCharacter::PostProcess()
{
    if( !this->active )
    {
        return;
    }

    this->GetBody()->SetOrientation( matrix33() );
    // disables the geometry
    this->GetGeometry(0)->Disable();
}

//-----------------------------------------------------------------------------
/**
    Gets the character height.

    history:
        - 26-Sep-2005   Zombie         created
*/
phyreal ncPhyCharacter::GetHeight() const 
{
    return this->height;
}

//-----------------------------------------------------------------------------
/**
    Gets the character wideness.

    history:
        - 26-Sep-2005   Zombie         created
*/
phyreal ncPhyCharacter::GetWide() const
{
    return this->wideness;
}

//-----------------------------------------------------------------------------
/**
    Adds a force to the body.

    history:
        - 26-Sep-2005   Zombie         created
*/
void ncPhyCharacter::AddForce( const vector3& force )
{
    const bool enabled( this->cylinder->IsEnabled() );

    ncPhysicsObj::AddForce( force );

    if( !enabled )
    {
        this->GetGeometry(0)->Disable();
    }
}

//-----------------------------------------------------------------------------
/**
    Deactivates the object.

    history:
        - 27-Jan-2006   Zombie         created
*/
void ncPhyCharacter::Deactivate()
{
    this->active = false;

    this->Disable();
    
    this->MoveToSpace(0);
}

//-----------------------------------------------------------------------------
/**
    Activates the object.

    history:
        - 27-Jan-2006   Zombie         created
*/
void ncPhyCharacter::Activates()
{
    this->active = true;
 
    this->Enable();

    this->AutoInsertInSpace();
}

//-----------------------------------------------------------------------------
/**
    Changes character height.

    history:
        - 18-May-2006   Zombie         created
*/
void ncPhyCharacter::ChangeHeight( const phyreal newHeight )
{
    nPhysicsGeom * geometry = static_cast<nPhyGeomTrans*>(this->GetGeometry(0))->GetGeometry();

    // calculate final height
    float height = ( newHeight > this->GetWide() ) ? ( newHeight - this->GetWide() ) : N_TINY;

    // set geometry position to mantain the same object position
    vector3 position;
    geometry->GetPosition( position );

    position.y = position.y + ( height - this->GetHeight() ) / 2.0f;

    geometry->SetPosition( position );

    // set final height
    this->SetHeight( height );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
