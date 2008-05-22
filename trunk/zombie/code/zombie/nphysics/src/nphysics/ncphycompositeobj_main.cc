//-----------------------------------------------------------------------------
//  ncphycompositeobj_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphycompositeobj.h"
#include "nphysics/nphysicsserver.h"
#include "zombieentity/nctransform.h"

#include "kernel/nlogclass.h"

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 18-Oct-2004   Zombie         created
*/
ncPhyCompositeObj::ncPhyCompositeObj() :
    containerGeometries(NumInitialGeometries,NumGrowthGeometries),
    objectSpace(0),
    subSpaceAllowed(true)
{   
    this->ResetsContainer();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 18-Oct-2004   Zombie         created
*/
ncPhyCompositeObj::~ncPhyCompositeObj()
{
    for( ;this->containerGeometries.Size(); )
    {
        nPhysicsGeom* geom(this->containerGeometries.GetElementAt(0));
        this->Remove( geom );
    }

    if( this->objectSpace )
    {
        this->objectSpace->Release();
        this->objectSpace = 0;
    }
}


//-----------------------------------------------------------------------------

/**
    Adds a geometry to the object

    @param geometry  geometry to be added

    history:
        - 29-Sep-2004   Zombie         created
*/
void ncPhyCompositeObj::AddContainer( nPhysicsGeom* geometry )
{
    this->containerGeometries.Add( phyGeomIDToInt( geometry->Id() ), geometry );

    if( this->GetParentSpace() != NoValidID ) 
    {
        geometry->MoveToSpace( this->GetParentSpace() );
    }

    geometry->SetOwner(this); // now this object it's the owner

    if( this->GetMaterial() )
    {
        geometry->SetMaterial( this->GetMaterial() );
    }
}

//-----------------------------------------------------------------------------
/**
    Removes a geometry from the object

    @param geometry  geometry to be removed

    history:
        - 29-Sep-2004   Zombie         created
*/
void ncPhyCompositeObj::RemoveContainer( nPhysicsGeom* geometry )
{
    this->containerGeometries.Rem( phyGeomIDToInt( geometry->Id() ) );

    geometry->MoveToSpace( NoValidID );

    geometry->SetOwner(0); // resets owner
}

//-----------------------------------------------------------------------------
/**
    Adds a geometry to the object

    @param geometry  geometry to be added

    history:
        - 18-Oct-2004   Zombie         created
*/

void ncPhyCompositeObj::Add( nPhysicsGeom* geometry )
{
    n_assert2( geometry, "Null pointer" );

    n_assert2( !geometry->IsSpace(), "It cannot be a space" );
    
#ifndef NGAME
    geometry->AddCollidesWith( nPhysicsGeom::Check );
#endif

    nPhyGeomTrans* geomTrans( static_cast<nPhyGeomTrans*>(nObject::kernelServer->New( "nphygeomtrans")));

    n_assert2( geometry, "Run out of memory" );

    geomTrans->SetOwner( this );

    geomTrans->AddGeometry( geometry );

    this->AddContainer( geomTrans );
}

//-----------------------------------------------------------------------------
/**
    Removes a geometry from the object

    @param geometry  geometry to be removed

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhyCompositeObj::Remove( nPhysicsGeom* geometry )
{
    nPhysicsGeom* geomTrans(0);

    this->containerGeometries.Find( phyGeomIDToInt( geometry->Id() ), geomTrans );

    /// removing the geometry
    static_cast<nPhyGeomTrans*>(geomTrans)->AddGeometry(0);

    this->RemoveContainer( geomTrans );

    geomTrans->Release();
}

//-----------------------------------------------------------------------------
/**
    Sets the position of this physic object

    @param newposition  new position

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhyCompositeObj::SetPosition( const vector3& newposition )
{
    nPhyRigidBody* body( this->GetBody() );

    if( body )
    {
        if( body->GetTweaked() )
        {
            matrix33 rotation;

            this->GetOrientation( rotation );

            vector3 pos(newposition + (rotation * body->GetTweakedOffset()));

            body->SetPosition( pos  );
            ncPhysicsObj::SetPosition( pos );
        }
        else
        {
            body->SetPosition( newposition );
            ncPhysicsObj::SetPosition( newposition );
        }
        return;
    }

    int end(this->containerGeometries.Size());

    for( int index(0); index < end; ++index )
    {
        this->containerGeometries.GetElementAt(index)->SetPosition( newposition );
    }

    ncPhysicsObj::SetPosition( newposition );
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation of the rigid body.

    @param neworientation  new orientation

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhyCompositeObj::SetRotation( const matrix33& neworientation )
{
    if( this->GetBody() )
    {
        this->GetBody()->SetOrientation( neworientation );
        ncPhysicsObj::SetRotation( neworientation );
        return;
    }

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->SetOrientation( neworientation );
    }

    ncPhysicsObj::SetRotation( neworientation );
}

//-----------------------------------------------------------------------------
/**
    Sets the euler orientation.

    @param ax   orientation in the x axis
    @param ay   orientation in the y axis
    @param az   orientation in the z axis

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhyCompositeObj::SetRotation( phyreal ax, phyreal ay, phyreal az )
{
    matrix33 neworientation;

    neworientation.from_euler( vector3( ax,ay,az ) );

    this->SetRotation( neworientation );
}

//-----------------------------------------------------------------------------
/**
    Sets the body of this object.

    @param body new object body

    history:
        - 19-Oct-2004   Zombie         created
*/
void ncPhyCompositeObj::SetBody( nPhyRigidBody* body )
{
    ncPhysicsObj::SetBody( body );

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->SetBody(body);
    }

    body->SetPhysicsObj( this );
}

//-----------------------------------------------------------------------------
/**
    Enables the physic object.

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhyCompositeObj::Enable()
{
    // Enabling every object belonging to this physics object
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->Enable();
    }

    if( this->GetBody() )
    {
        this->GetBody()->Enable();
    }

    ncPhysicsObj::Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the physic object.

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhyCompositeObj::Disable()
{
    // Disabling every object belonging to this physics object
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->Disable();
    }

    if( this->GetBody() )
    {
        this->GetBody()->Disable();
    }

    ncPhysicsObj::Disable();
}


//-----------------------------------------------------------------------------
/**
    Moves the physic object to another space.

    @param newspace the new object's space

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhyCompositeObj::MoveToSpace( nPhySpace* newspace )
{
    if( this->objectSpace )
    {
        this->objectSpace->MoveToSpace( newspace );
    }
    else
    {
        for( int index(0); index < this->containerGeometries.Size(); ++index )
        {
            this->containerGeometries.GetElementAt(index)->MoveToSpace( newspace );
        }
    }

    ncPhysicsObj::MoveToSpace( newspace );
}

//-----------------------------------------------------------------------------
/**
    Checks if collision with a geometry.

    @param geom geometry which whom collide
    @param numContacts number maximun of contacts expected
    @param contact array of contacts structures information

    @return number of contacts found

    history:
        - 29-Nov-2004   Zombie         created
*/
int ncPhyCompositeObj::Collide( const nPhysicsGeom* geom,
    int numContacts, nPhyCollide::nContact* contact ) const
{
    n_assert2( geom , "Null pointer" );

#ifndef NGAME
    if( !numContacts )
    {
        // nothing to do
        return 0; 
    }
#endif

    n_assert2( contact , "Null pointer" );

    int nContacts(numContacts);
    int nReportedContacts(0);

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {

        int nReported (
            this->containerGeometries.GetElementAt(index)->Collide( geom,
                nContacts,
                contact + nReportedContacts )
        );

        if( !nReported )
            continue;

        nContacts -= nReported;

        nReportedContacts += nReported;

        if( !nContacts )
            break; // no more space for more contacts
    }

    return nReportedContacts;
}

//-----------------------------------------------------------------------------
/**
    Checks if collision with other physics objects.

    @param obj object which whom collide
    @param numContacts number maximun of contacts expected
    @param contact array of contacts structures information

    @return number of contacts found

    history:
        - 29-Nov-2004   Zombie         created
*/
int ncPhyCompositeObj::Collide( const ncPhysicsObj* obj,
    int numContacts, nPhyCollide::nContact* contact ) const
{
    n_assert2( obj , "Null pointer" );

#ifndef NGAME
    if( !numContacts )
    {
        // nothing to do
        return 0; 
    }
#endif

    n_assert2( contact , "Null pointer" );

    int nContacts(numContacts);
    int nReportedContacts(0);

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        int nReported (
            obj->Collide( 
                this->containerGeometries.GetElementAt(index), 
                nContacts,
                contact + nReportedContacts )
        );

        if( !nReported )
            continue;

        nContacts -= nReported;

        nReportedContacts += nReported;

        if( !nContacts )
            break; // no more space for more contacts
    }

    return nReportedContacts;
}



//-----------------------------------------------------------------------------
/**
    Sets the material for all the geometries contain in the object.

    @param material material id

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhyCompositeObj::SetMaterial( nPhyMaterial::idmaterial material )
{
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        static_cast<nPhyGeomTrans*>(this->containerGeometries.GetElementAt(index))->GetGeometry()->SetMaterial( material );
    }    

    ncPhysicsObj::SetMaterial( material );
}


//-----------------------------------------------------------------------------
/**
    Returns the bounding box of this geometry.

    @param material material id

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhyCompositeObj::GetAABB( nPhysicsAABB& boundingbox )
{
    if( this->objectSpace )
    {
        this->objectSpace->GetAABB( boundingbox );
    }

#ifndef NGAME
    int NumGeometries(this->containerGeometries.Size());

    if( !NumGeometries )
    {
        return;
    }
#endif

    this->containerGeometries.GetElementAt(0)->GetAABB( boundingbox );

    for( int index(1); index < this->containerGeometries.Size(); ++index )
    {
        nPhysicsAABB localBox;

        this->containerGeometries.GetElementAt(index)->GetAABB( localBox );

        boundingbox += localBox;
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhyCompositeObj::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer" );

    if( this->subSpaceAllowed )
    {
        if( this->GetNumGeometries() > 1 )
        {
            // if more than one geometry creating a sub-space
            this->CreateSpace();

            // adding each geometry to the sub-space
            for( int index(0); index < this->GetNumGeometries(); ++index )
            {
                this->GetGeometry(index)->MoveToSpace( this->objectSpace );
            }
        }
    }

    ncPhysicsObj::Create( world );

    nPhyRigidBody *body(this->GetBody());

    if( body )
    {
        this->SetBody( body );

        vector3 position;

        this->GetPosition(position);

        this->GetBody()->SetPosition(position);

        nPhysicsAABB aabb;

        this->GetAABB( aabb );

        vector3 center(
            aabb.maxx + aabb.minx,
            aabb.maxy + aabb.miny,
            aabb.maxz + aabb.minz );

        center *= phyreal(.5);

        center = center - position;

        vector3 tweakedOffset;

        this->GetTweakedOffset( tweakedOffset );

        if( tweakedOffset.lensquared() )
        {
            center = tweakedOffset;
        }

        if( center.len() > phyreal( .001 ) )
        {
            this->GetBody()->SetTweaked( true );

            this->GetBody()->SetTweakedOffset( center );

            for( int index(0); index < this->GetNumGeometries(); ++index )
            {
                nPhyGeomTrans* trans( static_cast<nPhyGeomTrans*>
                    (this->containerGeometries.GetElementAt(index)));

                vector3 geometryPosition;

                if( tweakedOffset.lensquared() == 0 )
                {
                    trans->GetGeometry()->GetPosition( geometryPosition );

                    trans->GetGeometry()->SetPosition( geometryPosition - center );
                }
            }

            this->SetPosition( position );
        }
        
        /// assigns to each geometry de body
        body->Update( this->containerGeometries, this->GetMass(), this->GetDensity() );

        body->SetPhysicsObj( this );

        if( this->objectSpace )
        {
            this->objectSpace->SetCategories( this->GetCategories() );
            this->objectSpace->SetCollidesWith( this->GetCollidesWith() );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the orientation of the object.

    @param rotation matrix33 that will be filled with the object orientation

    history:
        - 08-Apr-2005   Zombie         created
*/
void ncPhyCompositeObj::GetOrientation( matrix33& rotation )
{
    nPhyRigidBody* body( this->GetBody() );
    
    if( body )
    {
        body->GetOrientation( rotation );
        return;
    }

#ifndef NGAME
    if( !this->containerGeometries.Size() )
    {
        NLOG( physicsLog , (1, "# Physics Object without collision." ) );
        return;
    }
#endif

    this->containerGeometries.GetElementAt(0)->GetOrientation( rotation );
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded
    
    history:
        - 09-May-2005   Zombie         created
*/
void ncPhyCompositeObj::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhysicsObj::InitInstance( initType );
}

//-----------------------------------------------------------------------------
/**
    Creates an object composited.

    @param name name of the geometry class to be created.

    @return the created geometry

    history:
        - 09-May-2005   Zombie         created
*/
nPhysicsGeom* ncPhyCompositeObj::CreateGeometryObj( const nString& name ) 
{
    n_assert2( name != "", "Invalid geometry name object." );

    nPhysicsGeom* geometry(static_cast<nPhysicsGeom*>(nKernelServer::Instance()->New( name.Get() )));

    n_assert2( geometry, "Failed to create a geometry." );

    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(geometry, nObject::LoadedInstance);
    
    this->Add( geometry );

    return geometry;
}

//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void ncPhyCompositeObj::Scale( const phyreal factor )
{
    if( factor == this->GetLastScaledFactor() )
        return;    

    /// updating geometry
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->Scale( factor / this->GetLastScaledFactor() );
    }

    /// updating mass distribution
    if( this->GetBody() )
    {
        this->GetBody()->Update( this->containerGeometries, this->GetMass(), this->GetDensity() );
    }

    this->SetLastScaledFactor( factor );
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
int ncPhyCompositeObj::Collide( int numContacts, nPhyCollide::nContact* contact ) const
{
    n_assert2( contact, "Null pointer." );

    int contacts( 0 );

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        contacts += nPhysicsServer::Instance()->Collide( this->containerGeometries.GetElementAt(index), numContacts - contacts, &contact[contacts] );
        if( numContacts == contacts )
            break;
    }

    return contacts;
}


//-----------------------------------------------------------------------------
/**
    Sets the categories where the object belongs.

    @param categories bit's mask with the a new set of categories

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhyCompositeObj::SetCategories( int categories ) 
{
#ifndef NGAME
    if( !this->containerGeometries.Size() )
    {
        NLOG( physicsLog , (1, "# It requieres at least a geometry." ) );
        return;
    }
#endif

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->SetCategories( categories );
    }

    ncPhysicsObj::SetCategories( categories );
}

//-----------------------------------------------------------------------------
/**
    Sets the categories wich the object will collide with.

    @param categories bit's mask with the a new set of categories

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhyCompositeObj::SetCollidesWith( int categories ) 
{

#ifndef NGAME
    if( !this->containerGeometries.Size() )
    {
        NLOG( physicsLog , (1, "# Physics Object without collision." ) );
        return;
    }
#endif

    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->SetCollidesWith( categories );
    }
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Removes a collide with.

    @param category a category

    history:
        - 13-Jun-2005   Zombie         created
*/
void ncPhyCompositeObj::RemovesCollidesWith( int category )
{
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->RemovesCollidesWith( nPhysicsGeom::Category(category) );
    }

    ncPhysicsObj::RemovesCollidesWith( category );
}

#endif

//-----------------------------------------------------------------------------
/**
    Creates composite space.

    history:
        - 18-Jul-2005   Zombie         created
*/
void ncPhyCompositeObj::CreateSpace()
{
    this->objectSpace = static_cast<nPhyGeomSpace*>(nKernelServer::Instance()->New("nphygeomspace"));

    n_assert2( this->objectSpace, "Failed to create an space for the composite object" );
    
}

//-----------------------------------------------------------------------------
/**
    Sets if the creation of an object's sub-space.

    @param is specifies if the object can create a sub-space

    history:
        - 18-Jul-2005   Zombie         created
*/
void ncPhyCompositeObj::AllowCreateSubSpace( const bool is )
{
    this->subSpaceAllowed = is;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
