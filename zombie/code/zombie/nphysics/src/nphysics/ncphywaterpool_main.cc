//-----------------------------------------------------------------------------
//  ncphywaterpool_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphywaterpool.h"
#include "nphysics/nphygeombox.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyWaterPool,ncPhyAreaDensity);

//-----------------------------------------------------------------------------

const phyreal ncPhyWaterPool::DefaultResistance(phyreal(.01));

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 14-Jan-2005   Zombie         created
*/
ncPhyWaterPool::ncPhyWaterPool() : 
    lengthsPool( 100, 10, 100 )
{
    this->SetResistance( DefaultResistance );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 14-Jan-2005   Zombie         created
*/
ncPhyWaterPool::~ncPhyWaterPool()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Process of creating of the object.

    @param world it's the world where the object will be come to existance

    history:
        - 14-Jan-2005   Zombie         created
*/
void ncPhyWaterPool::Create( nPhysicsWorld* world )
{
    // create geometry

    // the sea is represented trough a plane
    nPhyGeomBox* pool( static_cast<nPhyGeomBox*>( nObject::kernelServer->New("nphygeombox")));

    n_assert2( pool, "Failed to allocate memory" );

    // collides with everithing but static and other fluids
    pool->SetCollidesWith( nPhysicsGeom::Dynamic | nPhysicsGeom::Check );
    // belongs to the fluids and static categories
    pool->SetCategories( nPhysicsGeom::Static | nPhysicsGeom::Fluid );

    // assigning the geometry
    this->SetGeometry( pool );
    
    // creating the object in the world
    ncPhysicsObj::Create( world );

    pool->SetLengths( this->lengthsPool );

    // retrieving the bounding box
    this->UpdateBoundingBox();
}


//-----------------------------------------------------------------------------
/**
    Sets the position of this physic object.

    @param newposition new position

    history:
        - 24-Jan-2005   Zombie         created
*/
void ncPhyWaterPool::SetPosition( const vector3& newposition )
{
    ncPhySimpleObj::SetPosition( newposition );
    this->UpdateBoundingBox();
}

//-----------------------------------------------------------------------------
/**
    Sets the rotation of this physic object.

    @param newrotation new orientation

    history:
        - 24-Jan-2005   Zombie         created
*/
void ncPhyWaterPool::SetRotation( const matrix33& newrotation )
{
    ncPhySimpleObj::SetRotation( newrotation );
    this->UpdateBoundingBox();
}

//-----------------------------------------------------------------------------
/**
    Sets the lengths of the pool.

    @param lengths axis lengths of the pool

    history:
        - 24-Jan-2005   Zombie         created
*/
void ncPhyWaterPool::SetPoolsLength( const vector3& lengths )
{
    n_assert2( lengths.x != 0, "The x length it's null" );
    n_assert2( lengths.y != 0, "The y length it's null" );
    n_assert2( lengths.z != 0, "The z length it's null" );

    this->lengthsPool = lengths;

    nPhyGeomBox* box( static_cast<nPhyGeomBox*>(this->GetGeometry()) );

    n_assert2( box, "Data corruption" );

    if( box )
    {
        box->SetLengths( this->lengthsPool );
        this->UpdateBoundingBox();
    }
 
}

//-----------------------------------------------------------------------------
/**
    Computes the bounding box (not an AA).

    history:
        - 25-Jan-2005   Zombie         created
*/
void ncPhyWaterPool::UpdateBoundingBox()
{
    vector3 worldPosition;

    this->GetPosition( worldPosition );

    // get the max/min y
    this->GetGeometry()->GetAABB( this->bboxPool );

    // computing the x
    this->bboxPool.maxx = lengthsPool.x / phyreal(2) + worldPosition.x;
    this->bboxPool.minx = lengthsPool.x / phyreal(-2) + worldPosition.x;

    // computing the z
    this->bboxPool.maxz = lengthsPool.z / phyreal(2) + worldPosition.z;
    this->bboxPool.minz = lengthsPool.z / phyreal(-2) + worldPosition.z;

    // updating the matrix transformation
    this->GetGeometry()->GetOrientation( this->matrixPool );

    vector3 euler(this->matrixPool.to_euler());

    this->matrixPool.from_euler( vector3(0, -euler.y, 0) );
}

//-----------------------------------------------------------------------------
/**
    Helper function to find out if a point belongs to the pool area.

    @param worldpoint world position

    @return true/false

    history:
        - 26-Jan-2005   Zombie         created
*/
bool ncPhyWaterPool::ItBelongs( const vector3& worldpoint ) const
{
    vector3 newposition;
    
    this->matrixPool.mult( worldpoint, newposition );

    if( this->bboxPool.maxy < worldpoint.y )
        return false;
    if( this->bboxPool.miny < worldpoint.y )
        return false;

    if( this->bboxPool.maxx < worldpoint.x )
        return false;
    if( this->bboxPool.minx < worldpoint.x )
        return false;

    if( this->bboxPool.maxz < worldpoint.z )
        return false;
    if( this->bboxPool.minz < worldpoint.z )
        return false;

    return true;
}

//-----------------------------------------------------------------------------
/**
    Check if the box is under the fluid.

    @param box axis aligned bounding box

    history:
        - 14-Jan-2005   Zombie         created
*/
bool ncPhyWaterPool::IsUnderFluid( const nPhysicsAABB& box ) const
{

    for( int index(0); index < 6; ++index )
    {
        vector3 point;

        box.GetVertex( index, point );

        if( this->ItBelongs( point ) == false )
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Function to be procesed during the collision check.

    @param numContacts maximun number of contacts expected
    @param contacts container of contacts

    @return if any collision was processed

    history:
        - 24-Jan-2005   Zombie         created
*/
bool ncPhyWaterPool::CollisionProcess( int numContacts, nPhyCollide::nContact* contacts )
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

        phyreal underwater(this->bboxPool.maxy - bbox.miny);

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

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 17-May-2005   Zombie         created
*/
void ncPhyWaterPool::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhySimpleObj::InitInstance(initType);
}

//-----------------------------------------------------------------------------
/**
    Gets the lengths of the pool.

    @return pools lenghts

    history:
        - 26-Sep-2005   Zombie         created
*/
const vector3& ncPhyWaterPool::GetPoolsLength() const
{
    return this->lengthsPool;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
