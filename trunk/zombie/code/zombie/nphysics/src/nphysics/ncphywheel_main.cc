#include "precompiled/pchnphysics.h"
//-----------------------------------------------------------------------------
//  ncphywheel_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "nphysics/ncphywheel.h"
#include "nphysics/ncphyvehicle.h"
#include "nphysics/nphycontactjoint.h"
#include "zombieentity/ncloaderclass.h"
#include "entity/nentityclassserver.h"
#include "nspatial/ncspatialclass.h"

#include "kernel/nkernelserver.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyWheel,ncPhyCompositeObj);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 18-Apr-2005   David Reyes    created
*/
ncPhyWheel::ncPhyWheel() : 
    wheelOwner(0),
    contacts(0),
    preComputedValues(false)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 18-Apr-2005   David Reyes    created
*/
ncPhyWheel::~ncPhyWheel() 
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Sets the vehicle that owns this wheel.

    @param vehicle a vehicle

    history:
        - 18-Apr-2005   David Reyes    created
*/
void ncPhyWheel::SetVehicle( ncPhyVehicle* vehicle )
{
    this->wheelOwner = vehicle;
}

//-----------------------------------------------------------------------------
/**
    Gets the vehicle that owns this wheel.

    @return a vehicle

    history:
        - 18-Apr-2005   David Reyes    created
*/
ncPhyVehicle* ncPhyWheel::GetVehicle() const
{
    return this->wheelOwner;
}


//-----------------------------------------------------------------------------
/**
    Processes the information when a dynamic material is finded.

    @param cjoint a contact joint

    history:
        - 22-Apr-2005   David Reyes    created
*/
void ncPhyWheel::ProcessDynamicMaterial( nPhyContactJoint* cjoint )
{
    n_assert2( cjoint, "Null pointer." );

    if( this->preComputedValues == false )
    {
        this->PreCompute();
    }

    /// update slipenes with the speed of the wheel.
    cjoint->SetSlipCoeficient( cjoint->GetSlipCoeficient( false ) * this->preComputedSpeed, false );
    cjoint->SetSlipCoeficient( cjoint->GetSlipCoeficient( true ) * this->preComputedSpeed, true );

    vector3 normal;

    cjoint->GetContactNormal( normal );

    if( normal.dot( this->preComputeFrontDirection ) > phyreal(0.5) )
    {
        cjoint->SetFrictionDirection( this->preComputeUpDirection );           
    }
    else
    {
        cjoint->SetFrictionDirection( this->preComputeFrontDirection );    
    }

    ++this->contacts;
    // TODO: use this information to determining if the wheels are sliding and other fancy effects.
}

//-----------------------------------------------------------------------------
/**
    Resets the wheel state.

    history:
        - 22-Apr-2005   David Reyes    created
*/
void ncPhyWheel::Resets()
{
    // will be used to trigger events about sliding, sound, particles, etc...
    this->contacts = 0;
    this->preComputedValues = false;
}

//-----------------------------------------------------------------------------
/**
    Precomputes the values that will be repeated during a single step.

    history:
        - 26-Apr-2005   David Reyes    created
*/
void ncPhyWheel::PreCompute()
{
    this->preComputedValues = true;

    vector3 velocity;

    this->GetLinearVelocity( velocity );

    this->preComputedSpeed = velocity.len();

    matrix33 orientation;

    this->GetOrientation( orientation );

    this->preComputeFrontDirection = -orientation.z_component();

    this->preComputeUpDirection = -orientation.y_component();
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 26-Apr-2005   David Reyes    created
*/
void ncPhyWheel::InitInstance(nObject::InitInstanceMsg initType)
{
    if( !this->GetMass() )
    {
        // setting default mass
        this->SetMass( phyreal(1) );
    }

    ncPhyCompositeObj::InitInstance( initType );

#ifndef NGAME
    // make it not selectable
    ncEditor* editor( this->GetComponent<ncEditor>() );

    n_assert2( editor, "The component ncEditor must be present in this entity." );

    editor->SetEditorKeyInt( "editable", 0 );
#endif
}

//-----------------------------------------------------------------------------
/**
    Process b4 run the world.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyWheel::PreProcess()
{
    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        if( static_cast<nPhyGeomTrans*>(this->GetGeometry(index))->GetGeometry()->Type() == nPhysicsGeom::TriangleMesh )
        {
            this->GetGeometry(index)->Disable();
        }
        else
        {
            this->GetGeometry(index)->Enable();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Process after run the world.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyWheel::PostProcess()
{
    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        if( static_cast<nPhyGeomTrans*>(this->GetGeometry(index))->GetGeometry()->Type() == nPhysicsGeom::TriangleMesh )
        {
            this->GetGeometry(index)->Enable();
        }
        else
        {
            this->GetGeometry(index)->Disable();
        }
    }
}
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
