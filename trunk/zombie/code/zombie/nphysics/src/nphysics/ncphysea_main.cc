//-----------------------------------------------------------------------------
//  ncphyareadensity_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphysea.h"
#include "nphysics/nphygeomplane.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhySea,ncPhyAreaDensity);

//-----------------------------------------------------------------------------

const phyreal ncPhySea::DefaultSeaLevel(0);
const phyreal ncPhySea::DefaultResistance(phyreal(.01));

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 14-Jan-2005   David Reyes    created
*/
ncPhySea::ncPhySea() :
    sealevel( DefaultSeaLevel )
{
    this->SetResistance( DefaultResistance );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 14-Jan-2005   David Reyes    created
*/
ncPhySea::~ncPhySea()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Process of creating of the object.

    @param world it's the world where the object will be come to existance

    history:
        - 14-Jan-2005   David Reyes    created
*/
void ncPhySea::Create( nPhysicsWorld* world )
{
    // create geometry

    // the sea is represented trough a plane
    nPhyGeomPlane* plane( static_cast<nPhyGeomPlane*>( nObject::kernelServer->New("nphygeomplane")));

    n_assert2( plane, "Failed to allocate memory" );

    plane->SetEquation( vector4( 0, 1, 0, this->GetSeaLeavel() ) );

    // collides with everithing but static and other fluids
    plane->SetCollidesWith( 0 );
    // belongs to the fluids categories
    plane->SetCategories( nPhysicsGeom::Fluid );

    // assigning the geometry
    this->SetGeometry( plane );

    // creating the object in the world
    ncPhysicsObj::Create( world );
}

//-----------------------------------------------------------------------------
/**
    Check if the box is under the fluid.

    @param box axis aligned bounding box

    @return true/false

    history:
        - 14-Jan-2005   David Reyes    created
*/
bool ncPhySea::IsUnderFluid( const nPhysicsAABB& box ) const
{
    // only checking sealevel in y

    if( box.maxy > this->GetSeaLeavel() )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Sets the sea level.

    @param level sea's height

    history:
        - 14-Jan-2005   David Reyes    created
*/
void ncPhySea::SetSeaLeavel( phyreal level )
{
    this->sealevel = level;

    nPhyGeomPlane* plane(static_cast<nPhyGeomPlane*>(this->GetGeometry()));

    if( !plane )
    {
        return;
    }

    // updating already existing sea level
    plane->SetEquation( vector4( 0,1, 0, this->GetSeaLeavel() ) );
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 17-May-2005   David Reyes    created
*/
void ncPhySea::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhyAreaDensity::InitInstance(initType);
}

//-----------------------------------------------------------------------------
/**
    Function to be procesed during the collision check.

    @param numContacts maximun number of contacts expected
    @param contacts container of contacts

    @return if any collision was processed

    history:
        - 14-Jan-2005   David Reyes    created
*/
bool ncPhySea::CollisionProcess( int numContacts, nPhyCollide::nContact* contacts )
{
   n_assert2( contacts, "Null pointer" );

   nPhysicsGeom* geom(contacts->GetGeometryA());
   ncPhysicsObj* obj(0);

   if( geom == this->GetGeometry() )
   {
       geom = contacts->GetGeometryB();
       obj = contacts->GetPhysicsObjB();
   }
   else
   {
       obj = contacts->GetPhysicsObjA();
   }

   phyreal relationdensities( obj->GetDensity() / this->GetDensity() );

   relationdensities = phyreal(1) - relationdensities;

   phyreal maxForce(0);

   if( relationdensities < 0 )
   {
       // not real but optimizes speed and gives the feel
        maxForce = phyreal(.1);
   }
   else
   {

        nPhysicsAABB bbox;

        geom->GetAABB( bbox );

        phyreal distance(bbox.maxy - bbox.miny);

        phyreal underwater(this->GetSeaLeavel() - bbox.miny);

        phyreal under(underwater / distance);

        if( under > phyreal(1) )
        {
            under = phyreal(1);
        }

        maxForce = (under+ relationdensities);
   }

   vector3 force( this->GetWorld()->GetGravity() );

   force *= -maxForce * (obj->GetMass() / obj->GetNumGeometries());

   force *= phyreal(1)/phyreal(numContacts);

   nPhyRigidBody *body(obj->GetBody());

   n_assert2( body, "Data corruption" );

   for( int i(0); i < numContacts; ++i, ++contacts )
   {
       vector3 pos;
       
       contacts->GetContactPosition( pos );

       body->AddForceAtPosition( force, pos );
   }

   obj->SetState( InWater );

   this->Resistance( obj );

   return true;
}

/// sets the position of the rigid body
void ncPhySea::SetPosition( const vector3& newposition )
{
    if( this->GetGeometry() )
    {
        ncPhySimpleObj::SetPosition( newposition );
    }

    this->SetSeaLeavel( newposition.y );
}

/// sets the rotation of this physic object
void ncPhySea::SetRotation( const matrix33& /*newrotation*/ )
{
    // empty
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
