//-----------------------------------------------------------------------------
//  nphyrigidbody_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphygeomcylinder.h"
#include "nphysics/nphygeombox.h"

#include "kernel/nlogclass.h"

//-----------------------------------------------------------------------------

namespace
{
    // scales the weight around 10 to 0.1 (more or less)
    const phyreal MassRelation(phyreal(200.f));
}

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 27-Sep-2004   Zombie         created
*/
nPhyRigidBody::nPhyRigidBody() : 
    worldOwner(0),
    physicsObj(0),
    lastUpdateInertia(0),
    tweaked(false),
    atmosphera(true),
    contactWhenJoined(true)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 27-Sep-2004   Zombie         created
*/
nPhyRigidBody::~nPhyRigidBody()
{
    if( this->Id() == NoValidID )
        return;
    
    phyDestroyBody( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Creates the body.

    @param world it's the world where the object will be come to existance

    history:
        - 14-Oct-2004   Zombie         created
*/
void nPhyRigidBody::CreateIn( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer" );

    bodyID = phyCreateBody( world->Id() );

    world->Add(this);

    worldOwner = world;

    // attaching this information to the body id
    phyAttachDataBody( this->Id(), this );
}


//-----------------------------------------------------------------------------
/**
    Updates the body information.

    @param geom geometry
    @param mass body mass
    @param density body density
    @param offset mass offset

    history:
        - 14-Oct-2004   Zombie         created
*/
void nPhyRigidBody::Update( nPhysicsGeom* geom, phyreal mass, phyreal density, const vector3& offset )
{
    tContainerGeometries container(1,1);

    container.Add( phyGeomIDToInt( geom->Id() ), geom );

    this->Update( container, mass, density, offset );
}

//-----------------------------------------------------------------------------
/**
    Updates the body information.

    @param container geometry list
    @param mass body mass
    @param density body density
    @param offset mass offset

    history:
        - 20-Oct-2004   Zombie         created
*/
void nPhyRigidBody::Update( tContainerGeometries& container, phyreal mass, phyreal density, const vector3& offset )
{
    // Checks all the geometries involved in this body
    phymass massData;

    // computing mass for the physics engine
    phyreal massPhysics( mass / MassRelation );

    bool applyOffsetMass(false);

    // checks if there's an offset
    if( offset.len() )
    {
        applyOffsetMass = true;
    }


    for( int index(0); index < container.Size(); ++index )
    {
        nPhysicsGeom* geometry(container.GetElementAt(index));

        bool trans(false);

        geometry->SetBody( this );

        phyAttachBodyToGeom( geometry->Id(), this->Id() );

        if( geometry->Type() == nPhysicsGeom::GeometryTransform )
        {
            geometry = static_cast<nPhyGeomTrans*>(geometry)->GetGeometry();

            trans = true;            

            geometry->SetBody( this );
        }

        phymass localMass;

        switch( geometry->Type() )
        {
        case nPhysicsGeom::Sphere:
            phyComputeSphereMass( localMass, density, 
                static_cast<nPhyGeomSphere*>(geometry)->GetRadius() );

            break;
        case nPhysicsGeom::Box:
            {
                vector3 lengths;

                static_cast<nPhyGeomBox*>(geometry)->GetLengths( lengths );

                phyComputeBoxMass( localMass, density, lengths );
            }
            break;
        case nPhysicsGeom::RegularFlatEndedCylinder:
            {
                nPhyGeomCylinder* cylinder(static_cast<nPhyGeomCylinder*>(geometry));

                phyComputeCylinderMass( localMass, density, 
                    cylinder->GetRadius() ,cylinder->GetLength() );
            }
            break;
        default:
            NLOG( physicsLog , (1, "#INFO#: Body without a defined mass distribution." ) );
            continue;
        } // switch

        if( trans )
        {
            vector3 position;

            geometry->GetPosition( position );

            if( applyOffsetMass )
            {
                position += offset;
            }

            phyMassTranslate( localMass, position );
        }
        else
        {
            if( applyOffsetMass )
            {
                phyMassTranslate( localMass, offset );
            }
        }
        
        matrix33 orientation;

        geometry->GetOrientation( orientation );

        phyMassRotate( localMass, orientation );

        phyAddMass( massData, massData, localMass );

    }  // for

    if(container.Size())
    {
        phySetBodyMass( this->Id(), massPhysics, massData );
    }
}


//-----------------------------------------------------------------------------
/**
    Returns the inertia of the object.

    @param inertia body's inertia

    history:
        - 22-Mar-2005   Zombie         created
*/
void nPhyRigidBody::GetInertia( vector3& inertia )
{
    if( this->lastUpdateInertia == this->GetWorld()->GetStep() )
    {
        inertia = this->inertiaBody;
        return;
    }

    vector3 linearVelocity;
    
    this->GetLinearVelocity( linearVelocity );

    this->GetAngularVelocity( inertia );

    inertia += linearVelocity;

    inertia *= this->GetMass();

    this->lastUpdateInertia = this->GetWorld()->GetStep();

    this->inertiaBody = inertia;
}

//-----------------------------------------------------------------------------
/**
    Returns if will be contact if connected to another body.

    @return true/false

    history:
        - 23-Aug-2005   Zombie         created
*/
bool nPhyRigidBody::ContactWithOtherBodies() const
{
    return this->contactWhenJoined;
}

//-----------------------------------------------------------------------------
/**
    Sets if will be contact if connected to another body already joined.

    @param is true/false

    history:
        - 23-Aug-2005   Zombie         created
*/
void nPhyRigidBody::SetContactWithOtherBodies( const bool is )
{
    this->contactWhenJoined = is;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
