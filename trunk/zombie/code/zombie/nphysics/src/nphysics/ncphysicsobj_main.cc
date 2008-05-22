//-----------------------------------------------------------------------------
//  ncphysicsobj_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphycontactjoint.h"
#include "zombieentity/nctransform.h"
#include "nphysics/ncphysicsobjclass.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"

#include "kernel/nfileserver2.h"

#include "kernel/nlogclass.h"

#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncsubentity.h"
//-----------------------------------------------------------------------------

const phyreal ncPhysicsObj::NoImpactReport( phyreal(-1.) );

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 24-Sep-2004   Zombie         created
        - 04-Oct-2004   Zombie         adding itself to the physics server
*/
ncPhysicsObj::ncPhysicsObj() :
    enabled(true),
    parentSpace(NoValidID),
    type(Invalid),
    material(nPhyMaterial::None),
    bodyObject(0),
    objectMass(0),
    objectDensity(1),
    registeredForPreProcess(false),
    registeredForPostProcess(false),
    worldRef(0),
    flagsState(0),
    impactForce( NoImpactReport ),
    isFastRotating(false),
    refTransform(0),
    updateTransform(true),
    owner(0),
    categories(0),
    collidesWith(0),
    lastScale(phyreal(1)),
    isDirty(true),
    isLoaded(false),
    tweakedOffsetLoaded(0,0,0)
#ifdef __ZOMBIE_EXPORTER__
    ,isInIndoor(false)
#endif
{    
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 24-Sep-2004   Zombie         created
*/
ncPhysicsObj::~ncPhysicsObj()
{
    this->Alienate();

    if( this->GetWorld() )
    {
        if( this->GetCategories() & nPhysicsGeom::Mobil )
        {
            this->GetWorld()->RemoveMobil( this );
        }

        this->GetWorld()->Remove( this );

        UnregisterForPreProcess(); // Just in case
        UnregisterForPostProcess(); // Just in case
    }

    /// remove the body
    if( this->GetBody() )
    {
        n_delete( this->GetBody() );
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the physics object

    @param world it's the world where the object will be come to existance

    history:
        - 24-Sep-2004   Zombie         created
        - 01-Oct-2004   Zombie         supressed name
*/
void ncPhysicsObj::Create( nPhysicsWorld* world )
{
    this->Scale(this->GetComponent<ncTransform>()->GetScale().x);

    this->SetWorld( world );

    if( !this->parentSpace )
    {
        this->MoveToSpace( world->GetSpace() );
    }

    if( this->GetMass() ) // only body if the object has mass
    {
        this->bodyObject = n_new( nPhyRigidBody );

        n_assert2( this->bodyObject, "Failed to allocate memory" );

        this->bodyObject->CreateIn( world );

        /// updating categories
        this->SetCategories( this->GetCategories() & (~nPhysicsGeom::Static) | nPhysicsGeom::Dynamic );
        this->SetCollidesWith( nPhysicsGeom::All & (~nPhysicsGeom::Ramp) );
    }

    if( this->GetClassComponent<ncPhysicsObjClass>()->GetMobile() )
    {
        this->MakeItMobil();
    }
}

//-----------------------------------------------------------------------------
/**
    Draw all the geometries

    @param nGfxServer2  drawing server

    history:
        - 23-Sep-2004   Zombie         created
*/
#ifndef NGAME
void ncPhysicsObj::Draw( nGfxServer2* /*server*/ )
{
#if 0

    nPhysicsAABB bbox;

    this->GetAABB( bbox );

    bbox.Draw( server, phy_color_physicsobj_boundingbox );
#endif
}

//-----------------------------------------------------------------------------
/**
    Adds a collide with.

    @param category category flags

    history:
        - 13-Jun-2004   Zombie         created
*/
void ncPhysicsObj::RemovesCollidesWith( int category )
{
    this->categories &= ~category;
}

#endif // !NGAME


//-----------------------------------------------------------------------------
/**
    Checks if collision with other physics object.

    @param obj object which whom collide
    @param numContacts number maximun of contacts expected
    @param contact array of contacts structures information

    @return number of contacts found

    history:
        - 15-Oct-2004   Zombie         created
        - 29-Nov-2004   Zombie         virtualized
*/
int ncPhysicsObj::Collide( const ncPhysicsObj* /*obj*/,
    int /*numContacts*/, nPhyCollide::nContact* /*contact*/ ) const
{
    // Empty
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Checks if collision with a geometry.

    @param geom geometry which whom collide
    @param numContacts number maximun of contacts expected
    @param contact array of contacts structures information

    @return number of contacts found

    history:
        - 15-Oct-2004   Zombie         created
        - 29-Nov-2004   Zombie         virtualized
*/
int ncPhysicsObj::Collide( const nPhysicsGeom* /*geom*/,
    int /*numContacts*/, nPhyCollide::nContact* /*contact*/ ) const
{
    // Empty
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Sets the position of this physic object.

    @param newposition  sets the new position of this object

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhysicsObj::SetPosition( const vector3& newposition )
{
    if( this->updateTransform )
    {
        if( !this->refTransform )
        {
           return;
        }
        this->refTransform->DisableUpdate( ncTransform::cPhysics );
        this->refTransform->SetPosition( newposition );
        this->refTransform->EnableUpdate( ncTransform::cPhysics );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the rotation of this physic object.

    @param newrotation  sets the new rotation of this object

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhysicsObj::SetRotation( const matrix33& newrotation )
{
    if( this->updateTransform )
    {
        if( !this->refTransform )
        {
           return;
        }

        vector3 euler;

        euler = newrotation.to_euler();

        this->refTransform->DisableUpdate( ncTransform::cPhysics );
        this->refTransform->SetEuler( euler );
        this->refTransform->EnableUpdate( ncTransform::cPhysics );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the rotation of this physic object.

    @param ax angle orientation for the x-axis
    @param ay angle orientation for the y-axis
    @param az angle orientation for the z-axis

    history:
        - 16-Mar-2005   Zombie         created
*/
void ncPhysicsObj::SetRotation( phyreal ax, phyreal ay, phyreal az )
{
    if( this->updateTransform )
    {
        if( !this->refTransform )
        {
           return;
        }

        this->refTransform->DisableUpdate( ncTransform::cPhysics );
        this->refTransform->SetEuler( vector3(ax,ay,az) );
        this->refTransform->EnableUpdate( ncTransform::cPhysics );
    }
}

//-----------------------------------------------------------------------------
/**
    Returns current position.

    @param position returned position

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhysicsObj::GetPosition( vector3& position ) const
{
    position = this->refTransform->GetPosition();
}

//-----------------------------------------------------------------------------
/**
    Gets the rotation of this physic object

    @param rotation matrix33

    history:
        - 18-Oct-2004   Zombie         created
*/
void ncPhysicsObj::GetOrientation( matrix33& /*rotation*/ )
{
    n_assert2_always( "Not supported" );    
}

//-----------------------------------------------------------------------------
/**
    Returns the bounding box of this geometry.

    @param boundingbox axis aligned bounding box

    history:
        - 29-Nov-2004   Zombie         created
*/
void ncPhysicsObj::GetAABB( nPhysicsAABB& /*boundingbox*/ )
{
    // Empty
}


//-----------------------------------------------------------------------------
/**
    Function to be process b4 running the simulation.

    history:
        - 13-Dec-2004   Zombie         created
*/
void ncPhysicsObj::PreProcess()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Registers the object for process.

    history:
        - 13-Dec-2004   Zombie         created
*/
void ncPhysicsObj::RegisterForPreProcess()
{
    n_assert2( this->GetWorld(), "It cannot be it doesn't belong to a world." );

    if( this->registeredForPreProcess == true )
    {
        return; // It's already registered
    }

    this->registeredForPreProcess = true;

    this->GetWorld()->RegisterPreProcess( this );
}

//-----------------------------------------------------------------------------
/**
    Unregisters the object for process.

    history:
        - 13-Dec-2004   Zombie         created
*/
void ncPhysicsObj::UnregisterForPreProcess()
{
    if( this->registeredForPreProcess == false )
    {
        return;
    }

    this->GetWorld()->UnregisterPreProcess( this );
}

//-----------------------------------------------------------------------------
/**
    Function to be process after running the simulation.

    history:
        - 21-Apr-2005   Zombie         created
*/
void ncPhysicsObj::PostProcess()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Registers the object for post process.

    history:
        - 21-Apr-2005   Zombie         created
*/
void ncPhysicsObj::RegisterForPostProcess()
{
    n_assert2( this->GetWorld(), "It cannot be it doesn't belong to a world." );

    if( this->registeredForPostProcess == true )
    {
        return; // It's already registered
    }

    this->registeredForPostProcess = true;

    this->GetWorld()->RegisterPostProcess( this );
}

//-----------------------------------------------------------------------------
/**
    Unregisters the object for post process.

    history:
        - 21-Apr-2005   Zombie         created
*/
void ncPhysicsObj::UnregisterForPostProcess()
{
    if( this->registeredForPostProcess == false )
    {
        return;
    }

    this->GetWorld()->UnregisterPostProcess( this );
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
bool ncPhysicsObj::CollisionProcess( int /*numContacts*/, nPhyCollide::nContact* contacts )
{
    if( this->impactForceSquared > 0 )
    {
        if( contacts->GetPhysicsObjA()->GetType() == ncPhysicsObj::Fluid )
            return false;

        if( contacts->GetPhysicsObjB()->GetType() == ncPhysicsObj::Fluid )
            return false;

        tImpactInfo info;

        contacts->GetContactPosition(info.point);

        /// check how much force in the impact
        nPhyRigidBody* bodyA(contacts->GetGeometryA()->GetBody());

        if( bodyA )
        {            
            bodyA->GetInertia( info.impact );

        }

        /// check how much force in the impact
        nPhyRigidBody* bodyB(contacts->GetGeometryB()->GetBody());

        if( bodyB )
        {         
            vector3 force;

            bodyB->GetInertia( force );

            info.impact += force;
        }

        if( info.impact.lensquared() > this->impactForceSquared )
        {
            if( contacts->GetPhysicsObjA() == this )
                info.geom = contacts->GetGeometryA();
            else
                info.geom = contacts->GetGeometryB();

            /// filling the impact info
            this->OnImpact( info );
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Function call when an impact ocurs.

    @param info impact information

    history:
        - 22-Mar-2005   Zombie         created
*/
void ncPhysicsObj::OnImpact( const tImpactInfo& /*info*/ )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Resets the physics.

    history:
        - 22-Mar-2005   Zombie         created
*/
void ncPhysicsObj::Reset()
{
    if( !this->GetBody() )
    {
        return;
    }

    this->SetLinearVelocity( vector3(0,0,0) );
    this->SetAngularVelocity( vector3(0,0,0) );
}

//-----------------------------------------------------------------------------
/**
    Processes the information when a dynamic material is finded.

    @param cjoint contact joint

    history:
        - 22-Apr-2005   Zombie         created
*/
void ncPhysicsObj::ProcessDynamicMaterial( nPhyContactJoint* cjoint )
{
    cjoint->SetFrictionDirection( vector3( 0,0, phyreal(-1.) ) );
}

//-----------------------------------------------------------------------------
/**
    Sets if the object it's a fast rotating object.

    @param is true/false rotation object

    history:
        - 26-Apr-2005   Zombie         created
*/
void ncPhysicsObj::SetFastRotatingObject( const bool is )
{
    n_assert2( this->GetBody(), "It doesn't have a body, it's requiread by this operation." );

    this->isFastRotating = is;

    phySetFastRotating( this->GetBody()->Id(), is );
}


//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 09-May-2005   Zombie         created
*/
void ncPhysicsObj::InitInstance(nObject::InitInstanceMsg initType)
{

    if( initType == nObject::ReloadedInstance )
    {
        this->Reset();
    }

    this->refTransform = this->GetComponent<ncTransform>();

    if( initType == nObject::NewInstance )
    {
        this->LoadChunk(initType);

        ncPhysicsObjClass* classComponent( this->GetClassComponent<ncPhysicsObjClass>() );

        if (classComponent)
        {
            if( !this->GetMass() )
            {
                this->SetMass( classComponent->GetMass() );
                this->SetDensity( classComponent->GetDensity() );
            }
        }
    } 
    else
    {
        /// todo: now only subentities get physic position from ncTransform
        /// in the future, all objcet get the physic position from ncTRansform
        ncSubentity * subentity = this->GetComponent<ncSubentity>();
        const bool isSubentity = subentity && subentity->IsSubentity();
        if ( isSubentity ) 
        {

            if (  this->refTransform && 
                0 == ncPhyIndoor::Loading() // this is not a member of indoor,
                )
            {
                //Update the physic transform from ncTransform, ( Not use a physic transform persistence)
                //The indoor members physic has identity transform when persist it, only has a physic position.
                this->Update( this->refTransform->GetTransform() );
            }

            if( this->GetType() != Terrain && 
                this->GetType() != Indoor &&
                0 == ncPhyIndoor::Loading() // this is not a member of indoor,
            )
            {
                this->InsertInTheWorld();
            }
        }
    }
    
    n_assert2( this->refTransform, "All the entities with physics objects, requires the ncTransform component." );

#ifndef NGAME
    if( initType != nObject::ReloadedInstance )
    {
        // Register to EnterLimbo and ExitLimbo signals
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncPhysicsObj::YouShallDwellIntoTheLimbo, 0 );
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, ExitLimbo), this, &ncPhysicsObj::YourSoulMayComeBackFromLimbo, 0 );
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Updates from transfrom the orientation and position.

    @param transform new object's transformation

    history:
        - 11-May-2005   Zombie         created
*/
void ncPhysicsObj::Update( const matrix44& transform )
{
    this->NoUpdateTransform();

    vector3 position;

    this->SetPosition( transform.pos_component() );

    matrix33 orientation( transform.x_component(), transform.y_component(), transform.z_component() );

    this->SetRotation( orientation );

    this->UpdateTransform();

    this->isDirty = true;
}

//-----------------------------------------------------------------------------
/**
    Sets to update transfrom info.

    history:
        - 11-May-2005   Zombie         created
*/
void ncPhysicsObj::UpdateTransformInfo()
{
    vector3 position;

    this->refTransform->DisableUpdate( ncTransform::cPhysics );

    this->GetPosition( position );

    matrix33 orientation;

    this->GetOrientation( orientation );

    nPhyRigidBody* body( this->GetBody() );

    if( body )
    {
        if( body->GetTweaked() )
        {
            vector3 tweakedOffset(orientation * body->GetTweakedOffset());

            position -= tweakedOffset;            
        }
    }

    this->refTransform->SetPosition( position );

    quaternion quat = orientation.get_quaternion();

    this->refTransform->SetQuat( quat );

    this->refTransform->EnableUpdate( ncTransform::cPhysics );   
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
int ncPhysicsObj::Collide( int /*numContacts*/, nPhyCollide::nContact* /*contact*/ ) const
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Sets the categories where the object belongs.

    @param cat categories flags

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhysicsObj::SetCategories(int cat) 
{
    this->categories = cat;    
}

//-----------------------------------------------------------------------------
/**
    Sets the categories wich the object will collide with.

    @param cat categories flags

    history:
        - 13-May-2005   Zombie         created
*/
void ncPhysicsObj::SetCollidesWith(int cat) 
{
    this->collidesWith = cat;
}

//-----------------------------------------------------------------------------
/**
    Returns the object categories.

    @param categories flags

    history:
        - 01-Jun-2005   Zombie         created
*/
int ncPhysicsObj::GetCategories() const
{
    return this->categories;

}

//-----------------------------------------------------------------------------
/**
    Returns the object categories.

    @param categories flags

    history:
        - 03-Jun-2005   Zombie         created
*/
int ncPhysicsObj::GetCollidesWith() const
{
    return this->collidesWith;
}

//-----------------------------------------------------------------------------
/**
    Sets the physic object to another space by name.

    @param name space name

    history:
        - 02-Jun-2005   Zombie         created
*/
#ifdef __ZOMBIE_EXPORTER__
void ncPhysicsObj::SetSpaceByName( const nString& name ) 
{
    this->spaceName = name;
}
#else
void ncPhysicsObj::SetSpaceByName( const nString& name )
{
    n_assert2( ncPhyIndoor::Loading(), "Operation only supported by indoor objects." );
    
    nPhySpace* space(0);

    ncPhyIndoor::Loading()->GetSubSpaces().Find( nTag(name).KeyMap() , space );

    n_assert2( space, "Space doesn't exists." );

    this->MoveToSpace( space );
}
#endif
//-----------------------------------------------------------------------------
/**
    Alienates the object from relations.

    history:
        - 02-Jun-2005   Zombie         created
*/
void ncPhysicsObj::Alienate()
{
    if( this->GetOwner() )
    {
        if( this->GetOwner()->GetType() == Indoor )
        {
            ncPhyIndoor* indoor(static_cast<ncPhyIndoor*>(this->GetOwner()));

            indoor->Remove( this );
        }
    }

    if( this->GetWorld() )
    {
        this->MoveToSpace( this->GetWorld()->GetSpace() );
    }
}

//-----------------------------------------------------------------------------
/**
    Makes the object mobil.

    history:
        - 02-Jul-2005   Zombie         created
*/
void ncPhysicsObj::MakeItMobil()
{
    this->SetCategories( this->GetCategories() | nPhysicsGeom::Mobil );
    this->SetCategories( this->GetCategories() & ~nPhysicsGeom::Static );

#ifndef NGAME
    if( !(this->GetCategories() & nPhysicsGeom::Mobil) )
    {
        // failed to set the categories check the logs.
        return;
    }
#endif

    this->GetWorld()->AddMobil( this );

    this->isDirty = true;
}

//-----------------------------------------------------------------------------
/**
    Stores tweaked information (offset).

    @param offset 3d offset

    history:
        - 24-Aug-2005   Zombie         created
*/
void ncPhysicsObj::SetTweakedOffset( const vector3& offset )
{
    this->tweakedOffsetLoaded = offset;
}

//-----------------------------------------------------------------------------
/**
    Gets the tweaked offset loaded.

    @param offset 3d offset

    history:
        - 24-Aug-2005   Zombie         created
*/
void ncPhysicsObj::GetTweakedOffset( vector3& position )
{
    position = this->tweakedOffsetLoaded;
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Moves the object to limbo.

    history:
        - 07-Sep-2005   Zombie         created
*/
void ncPhysicsObj::YouShallDwellIntoTheLimbo()
{
    this->MoveToSpace(0);
    
    this->Disable();

    if( this->GetBody() )
    {
        this->GetBody()->Disable();
    }
}

//-----------------------------------------------------------------------------
/**
    Recovers an object from the limbo.

    history:
        - 07-Sep-2005   Zombie         created
*/
void ncPhysicsObj::YourSoulMayComeBackFromLimbo()
{
    if( this->GetType() == ncPhysicsObj::Pickeable ) 
    {
        this->MoveToSpace( this->GetWorld()->GetSpace() );
    }
    else
    {
        this->AutoInsertInSpace();
    }

    this->Enable();

    if( this->GetBody() )
    {
        this->GetBody()->Enable();
    }
}

#endif

//-----------------------------------------------------------------------------
/**
    Loads chunk information.

    history:
        - 13-Sep-2005   Zombie         created
*/
bool ncPhysicsObj::LoadChunk(nObject::InitInstanceMsg /*initType*/)
{
    if (this->isLoaded)
    {
        return true;
    }

    ncPhysicsObjClass *phyObjClass(this->GetClassComponent<ncPhysicsObjClass>());

    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();
    if (charClass)
    {
        //get physics skeleton
        if( charClass->GetPhysicsSkeletonIndex()!= -1 && charClass->GetSkeletonClassPointer(charClass->GetPhysicsSkeletonIndex()) )
        {
            nEntityClass *skelClass = charClass->GetSkeletonClassPointer(charClass->GetPhysicsSkeletonIndex())->GetEntityClass();

            // skeleton loader
            phyObjClass = skelClass->GetComponent<ncPhysicsObjClass>();
        }
    }
    else
    {
        phyObjClass = this->GetClassComponent<ncPhysicsObjClass>();
    }


    if( phyObjClass )
    {
        // begin: horrible hack
        nObject* clone(0);

        nPersistServer* pserver(nKernelServer::Instance()->GetPersistServer());

        if( pserver->GetSaveMode() == nPersistServer::SAVEMODE_CLONE )
        {
            clone = pserver->GetClone();

            pserver->SetSaveMode(nPersistServer::SAVEMODE_FOLD);
        }
        // end: horrible hack


        // load resource
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);
        if( ps )
        {
            // load physics chunk for objects created on the fly (not loaded from disk)
            ncPhysicsObj *phyComp = this->entityObject->GetComponent<ncPhysicsObj>();
            n_assert(this == phyComp);
            if (phyComp)
            {
#ifndef NGAME
                if ( this->GetType() != ncPhysicsObj::Pickeable )
                {
#endif
                    nFile* chunkFile = phyObjClass->GetChunkFile();

                    if( chunkFile && chunkFile->IsOpen()  ) // if file is not open then file is not existed
                    {
                        nKernelServer::Instance()->PushCwd( this->GetEntityObject() );
                        nKernelServer::Instance()->Load( chunkFile, false );
                        nKernelServer::Instance()->PopCwd();
                    }
#ifndef NGAME
                }
#endif
            }
        }

        // begin: horrible hack
        if( clone )
        {
            pserver->SetClone( clone );

            pserver->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
        }
        // end: horrible hack
    }

    this->isLoaded = true;

    return true;
}

//-----------------------------------------------------------------------------
/**
    Runs chunk.

    history:
        - 13-Sep-2005   Zombie         created
*/
void ncPhysicsObj::PreparePhyObject()
{
    this->LoadChunk(nObject::LoadedInstance);
}

//-----------------------------------------------------------------------------
/**
    Loader.

    history:
        - 22-Dec-2005   Zombie         created
*/
void ncPhysicsObj::Load()
{
    // Empty
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
