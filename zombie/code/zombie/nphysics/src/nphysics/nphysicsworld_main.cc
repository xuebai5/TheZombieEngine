//-----------------------------------------------------------------------------
//  nphysicsworld_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/ncphyterrain.h"
#include "nphysics/nphyjointgroup.h"
#include "nphysics/nphycontactjoint.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/nphyspace.h"
#include "nphysics/nphygeomtrimesh.h"
#include "kernel/nprofiler.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#include "nlayermanager/nlayermanager.h"
#endif

#include "kernel/nlogclass.h"
//-----------------------------------------------------------------------------
nNebulaScriptClass(nPhysicsWorld, "nroot");

//-----------------------------------------------------------------------------
static bool initializedProfPhysics = false;
static nProfiler profPhysicsMobilObjects;
static nProfiler profPhysicsPreProcessObjects;
static nProfiler profPhysicsProcessBooms;
static nProfiler profPhysicsResetObjects;
static nProfiler profPhysicsCollideSpaces;
static nProfiler profPhysicsRunSimulation;
static nProfiler profPhysicsReleaseContacts;
static nProfiler profPhysicsUpdateTransform;
static nProfiler profPhysicsBreakableJoints;
static nProfiler profPhysicsPostProcessObjects;

//-----------------------------------------------------------------------------
// POLICIES
#define __POLICY_NO_CREATE_CONTACTS_IF_BODIES_DISABLED__
#define __POLICY_NO_CREATE_CONTATCS_IF_GEOMETRY_DISABLED__
//-----------------------------------------------------------------------------

nPhyCollide::nContact nPhysicsWorld::contactsbuffer[nPhysicsWorld::BufferNumberOfContacts];

#if !NGAME && WIN32
int nPhysicsWorld::DEBUG_NumberOfContacts(0);
int nPhysicsWorld::DEBUG_NumberOfBodies(0);
#endif // _DEBUG

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 23-Sep-2004   David Reyes   created
*/
nPhysicsWorld::nPhysicsWorld() :
    worldID( NoValidID ),
    enabled( true ),
    containerPhysicsObj(NumInitialPhysicsObj,NumGrowthPhysicsObj),
    containerMobilObj(NumInitialPhysicsObj,NumGrowthPhysicsObj),
    containerRigidBodies(NumInitialPhysicsObj,NumGrowthPhysicsObj),
    containerBreakableJoints(NumInitialBreakableJoints,NumGrowthBreakableJoints),
    containerObjectsPreProcess(NumInitialPhysicsObj,NumGrowthPhysicsObj),
    containerObjectsPostProcess(NumInitialPhysicsObj,NumGrowthPhysicsObj),
    worldSpace( NoValidID ),
    jointsgroup( 0 ),
    generaljointsgroup(0),
    NumberExplosions(0),
    stepCounter(0),
    hasBodies(true),
    simNumContactsInBuffer(0),
    objectManager( this )
{
    if (!initializedProfPhysics)
    {
        profPhysicsMobilObjects.Initialize("profPhysicsMobilObjects", true);
        profPhysicsPreProcessObjects.Initialize("profPhysicsPreProcessObjects", true);
        profPhysicsProcessBooms.Initialize("profPhysicsProcessBooms", true);
        profPhysicsResetObjects.Initialize("profPhysicsResetObjects", true);
        profPhysicsCollideSpaces.Initialize("profPhysicsCollideSpaces", true);
        profPhysicsRunSimulation.Initialize("profPhysicsRunSimulation", true);
        profPhysicsReleaseContacts.Initialize("profPhysicsReleaseContacts", true);
        profPhysicsUpdateTransform.Initialize("profPhysicsUpdateTransform", true);
        profPhysicsBreakableJoints.Initialize("profPhysicsBreakableJoints", true);
        profPhysicsPostProcessObjects.Initialize("profPhysicsPostProcessObjects", true);
        initializedProfPhysics = true;
    }

    this->Create();

    nPhysicsServer::Instance()->Add( this );

    // creating space for the joints of this world
    this->jointsgroup = n_new( nPhyJointGroup );

    n_assert2( jointsgroup, "Failed to create the worlds joints group" );

    this->jointsgroup->SetSize( DefaultSizeJointGroup );

    this->jointsgroup->Create();

    // creating space for the joints of this world
    this->generaljointsgroup = n_new( nPhyJointGroup );

    n_assert2( generaljointsgroup, "Failed to create the worlds joints group" );

    this->generaljointsgroup->SetSize( DefaultSizeJointGroup );

    this->generaljointsgroup->Create();

    this->InitExplosions();

    this->InitStorageContactJoints();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 23-Sep-2004   David Reyes   created
*/
nPhysicsWorld::~nPhysicsWorld()
{
    this->Destroy();
}

//-----------------------------------------------------------------------------
/**
    Creates the world

    history:
        - 23-Sep-2004   David Reyes   created
*/
void nPhysicsWorld::Create()
{
    n_assert2( this->Id() == NoValidID , "Trying to create a world already created" );

    this->worldID = phyCreateWorld();

    phyEngineWorldParameters( this->Id() );

    n_assert2( this->Id() != NoValidID , "Error trying to create a new world" );

    // sets default gravity
    this->SetGravity( vector3(0, phyreal(-9.8),0));
}

//-----------------------------------------------------------------------------
/**
    Sets if the geometries has to be created enabled

    @param enabled  sets the stat of enabling in creation

    history:
        - 23-Sep-2004   David Reyes    created
*/
void nPhysicsWorld::Enable()
{
    n_assert2( this->Id() != NoValidID , "No valid world id" );

    this->enabled = true;

    if( this->worldSpace != NoValidID )
    {
        this->worldSpace->Enable();
        return;
    }

    
    /// Enabling all the physic objects of this world
    for( int index = 0; index < this->containerPhysicsObj.Size(); ++index)
    {
        this->containerPhysicsObj.GetElementAt(index)->Enable();
    }
}

//-----------------------------------------------------------------------------
/**
    Disables this world

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::Disable()
{
    n_assert2( this->Id() != NoValidID , "No valid world id" );

    this->enabled = false;

    if( this->worldSpace != NoValidID )
    {
        this->worldSpace->Disable();
        return;
    }

    /// Disabling all the physic objects of this world
    int Size(this->containerPhysicsObj.Size());

    for( int index(0); index < Size; ++index )
    {
        this->containerPhysicsObj.GetElementAt( index )->Disable(); 
    }

}

//-----------------------------------------------------------------------------
/**
    Destroys the world

    history:
        - 23-Sep-2004   David Reyes    created
*/
void nPhysicsWorld::Destroy()
{
    this->SetBeenDestroyed( true );

    if( this->Id() == NoValidID )
        return;

    for( int index(0); index < this->containerPhysicsObj.Size(); ++index )
    {
        this->containerPhysicsObj.GetElementAt(index)->SetWorld(0);
    }

    for( int index(0); index < this->containerRigidBodies.Size(); ++index )
    {
        this->Remove( this->containerRigidBodies.GetElementAt( index ) );
    }

    this->DestroyStorageContactJoints();


    if( this->generaljointsgroup )
    {
        n_delete( this->generaljointsgroup );
        this->generaljointsgroup = 0;
    }

    if( this->jointsgroup )
    {
        n_delete(this->jointsgroup);
        this->jointsgroup = 0;
    }

    if( this->GetSpace() )
    {
        this->GetSpace()->Release();
    }

    /// free all explosions related resources
    this->FreeExplosions();

    /// remove this world from the server
    nPhysicsServer::Instance()->Remove( this );

    phyDestroyWorld( this->Id() );

    /// Resetting values
    this->worldID = NoValidID;

    /// releasing mesh data
    nPhyGeomTriMesh::ReleaseMeshesData();
}

//-----------------------------------------------------------------------------
/**
    Adds a physic object to the world.

    @param obj object to be added to the world

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::Add( nEntityObject* obj )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( obj , "Null pointer" );

    ncPhysicsObj* ncObj(obj->GetComponent<ncPhysicsObj>());

    n_assert2( ncObj, "The entity object have to be composed by ncPhysicsObj." );

    this->containerPhysicsObj.Add( ncObj->Id(), ncObj );

    ncObj->Create( this );
}

//-----------------------------------------------------------------------------
/**
    Removes a physic object from the world.

    @param obj object to be removed from the world

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::Remove( ncPhysicsObj* obj )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( obj , "Null pointer" );

    this->containerPhysicsObj.Rem( obj->Id() );

    if( obj->GetBody() )
    {
        this->Remove( obj->GetBody() );
    }

    obj->MoveToSpace( NoValidID );
}

//-----------------------------------------------------------------------------
/**
    Adds a rigid body to the world.

    @param obj rigid body to be added from the world

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::Add( nPhyRigidBody* obj )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( obj , "Null pointer" );

    this->containerRigidBodies.Add( int(reinterpret_cast<size_t>(obj)), obj );
}

//-----------------------------------------------------------------------------
/**
    Removes a rigid body to the world.

    @param obj rigid body to be added from the world

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::Remove( nPhyRigidBody* obj )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( obj , "Null pointer" );

    this->containerRigidBodies.Rem( int(reinterpret_cast<size_t>(obj)) );
}


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Debug tool to draw the geometries.

    @param server graphics server

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::Draw( nGfxServer2* server )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    if( this->worldSpace != NoValidID )
    {
        this->worldSpace->Draw( server );
    }

    // Drawing each object of the world
    for( int index(0); index < this->containerPhysicsObj.Size(); ++index )
    {
        // only draws the bounding box
        this->containerPhysicsObj.GetElementAt(index)->Draw(server);
    }

    if( nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyAreas )
    {
        if( this->HasBodies() )
        {
            this->GetObjectManager()->DrawAreas( server );
        }
    }

    if( nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyJoints )
    {
        for( int index(0); index < this->GetGeneralJointGroup()->GetNumJoints(); ++index )
        {
            this->GetGeneralJointGroup()->GetJoint( index )->Draw( server );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Enables/disables depending of the active layers.

    history:
        - 04-Aug-2004   David Reyes    created
*/
void nPhysicsWorld::EnableByLayers()
{
    nAutoRef<nLayerManager> layerManager( "/sys/servers/layermanager" );

    for( int index(0); index < this->containerPhysicsObj.Size(); ++index )
    {
        ncPhysicsObj* obj(this->containerPhysicsObj.GetElementAt(index));

        ncEditor* editor(obj->GetComponent<ncEditor>());

        if( !editor )
            continue;

        int layerId(editor->GetLayerId());

        nLayer *layer(layerManager->SearchLayer(layerId));

        if ( !layer )
        {
            continue;
        }

        if( layer->IsActive() )
        {
            obj->Enable();
        }
        else
        {
            obj->Disable();
        }
    }
}

#endif
//-----------------------------------------------------------------------------
/**
    Returns if a geometry it's colliding with any of the others.

    @param geom         geom to be checked
    @param numContacts  how many contacts should be reported if any
    @param contact      list of contacts

    @return num of contacts reported

    history:
        - 11-Oct-2004   David Reyes    created
        - 31-Jan-2005   David Reyes    new implememtation
*/
int nPhysicsWorld::Collide( 
    const nPhysicsGeom *geom,
    int numContacts,
    nPhyCollide::nContact* contact )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( geom , "Null pointer" );

    n_assert2( numContacts , "No contacts required" );

    n_assert2( contact , "Null pointer" );

    nPhysicsGeom* space(this->GetSpace());

    if( !space )
    {
        return 0;
    }

    return nPhyCollide::Collide( geom->Id(), space->Id(), numContacts, contact );    
}

//-----------------------------------------------------------------------------
/**
    Creates a space for this world.

    @param worldspace a space

    history:
        - 11-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::SetSpace( nPhySpace* worldspace )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    this->worldSpace = worldspace;

    worldspace->SetWorld(this);

    int Size(this->containerPhysicsObj.Size());

    for( int index(0); index < Size; ++index )
    {
        this->containerPhysicsObj.GetElementAt( index )->MoveToSpace( worldspace );
    }
}

//-----------------------------------------------------------------------------
/**
    Runs the world.

    @param step step factor

    history:
        - 15-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::RunWorld( phyreal step )
{
#ifndef NGAME
    // reseting debug info
    DEBUG_NumberOfContacts = 0;
    DEBUG_NumberOfBodies = this->containerRigidBodies.Size();
#endif // _DEBUG

    if( this->worldSpace == NoValidID )
        return;

    if( step > phyreal(1) )
        step = phyreal(1);

#ifndef NGAME
    if( this->HasBodies() )
    {
#endif
        // processes the mobil objects
        this->ProcessMobilObjects();

        this->PreProcessObjects();

        // processes the explosions for this step
        this->ProcessBooms();

#ifdef __ENABLE_ATMOSPHERA__
        // processes the atmosfera for this step
        this->atmosphera.Process(this->containerRigidBodies);
#endif

        // resets the step info
        this->ResetObjects();

        // runs the object manager
        this->objectManager.Run();

        /// check for collisionrag
        profPhysicsCollideSpaces.StartAccum();
        phyCollideWithinSpace( reinterpret_cast<spaceid>(this->worldSpace->Id()), PossibleCollision );
        profPhysicsCollideSpaces.StopAccum();

        /// runs the physics simulation
        profPhysicsRunSimulation.StartAccum();
        phyRunSimulation( this->Id(), step );
        profPhysicsRunSimulation.StopAccum();

        /// releases the joints crected for the simulation
        ReleaseJointContacts();
        
        /// updates the transformation after the simulation
        this->UpdateTransform();

        // treat breakable joints
        this->RunBreakableJoints();

        this->PostProcessObjects();

#ifdef __ENABLE_CHECK__
        // check everything it's fine
        this->Check();
#endif

        ++this->stepCounter;
#ifndef NGAME
    }
    else
    {
        this->PreProcessObjects();
        this->PostProcessObjects();
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Collisions reaction.

    @param geomA geometry id
    @param geomB geometry id

    history:
        - 15-Oct-2004   David Reyes    created
*/
void nPhysicsWorld::PossibleCollision(geomid geomA, geomid geomB)
{
    n_assert2( geomA && geomB, "Data corruption" );

    nPhysicsGeom* phyGeomA(static_cast<nPhysicsGeom*>(phyRetrieveDataGeom(geomA)));
    nPhysicsGeom* phyGeomB(static_cast<nPhysicsGeom*>(phyRetrieveDataGeom(geomB)));

#ifdef __POLICY_NO_CREATE_CONTATCS_IF_GEOMETRY_DISABLED__
    if( !phyGeomA->IsEnabled() )
    {
        return;
    }
    if( !phyGeomB->IsEnabled() )
    {
        return;
    }
#endif // #ifdef __POLICY_NO_CREATE_CONTATCS_IF_GEOMETRY_DISABLED__
    
    const bool spaceA(phyGeomA->IsSpace());
    const bool spaceB(phyGeomB->IsSpace());

    if( spaceA || spaceB ) 
    {
        const bool StaticA(phyGeomA->GetCategories() & nPhysicsGeom::Static);
        const bool StaticB(phyGeomB->GetCategories() & nPhysicsGeom::Static);

        if( StaticA && StaticB )
            return;

        phyCollideSpaces( geomA, geomB, &PossibleCollision );

        if( spaceA )
        {
            if( !StaticA )
            {
                if( static_cast<nPhySpace*>(phyGeomA)->GetNumGeometries() > 1)
                    phyCollideWithinSpace( reinterpret_cast<spaceid>(geomA), &PossibleCollision );
            }
        }
        if( spaceB )
        {
            if( !StaticB )
            {
                if( static_cast<nPhySpace*>(phyGeomB)->GetNumGeometries() > 1)
                    phyCollideWithinSpace( reinterpret_cast<spaceid>(geomB), &PossibleCollision );
            }
        }
        return;
    }

    nPhyRigidBody* bodyA(phyGeomA->GetBody()); 
    nPhyRigidBody* bodyB(phyGeomB->GetBody());

#ifdef PHY_JOINED_BODIES_DONT_COLLIDE
    // chech they're not together already
    if( bodyA && bodyB )
    {
        if( phyAreConnected( bodyA->Id(), bodyB->Id() ) )
        {
            if( !bodyA->ContactWithOtherBodies() )
            {
                return;
            }
            if( !bodyB->ContactWithOtherBodies() )
            {
                return;
            }
        }
    }
#endif // !PHY_JOINED_BODIES_DONT_COLLIDE

#ifdef __POLICY_NO_CREATE_CONTACTS_IF_BODIES_DISABLED__

        if( bodyA && bodyB ) 
        {
            if( !bodyA->IsEnabled() && !bodyB->IsEnabled() )
            {
                return;
            }
        } 
        else if( bodyA )
        {
            if( !bodyA->IsEnabled() )
            {
                return;
            }
        } 
        else if( bodyB )
        {
            if( !bodyB->IsEnabled() )
            {
                return;
            }
        }

#endif // __POLICY_NO_CREATE_CONTACTS_IF_BODIES_DISABLED__

    int numContacts(phyGeomA->GetNumContacts());

    if( numContacts < phyGeomB->GetNumContacts() )
        numContacts = phyGeomB->GetNumContacts();

    n_assert2( numContacts >= 0, "Error: Contacs number cannot be negative" );

#if 1
    static int Contacts = 20;

    numContacts = Contacts;
#endif // NOTE: for the DEMO only

    numContacts = nPhyCollide::Collide( geomA,geomB,numContacts, contactsbuffer );

    if ( numContacts )
    {
        ncPhysicsObj* objA(phyGeomA->GetOwner());
        ncPhysicsObj* objB(phyGeomB->GetOwner());


        n_assert2( objA, "Can't exist geometries without objects." );
        n_assert2( objB, "Can't exist geometries without objects." );

        if( objA->CollisionProcess( numContacts, contactsbuffer ) )
            return;
        if( objB->CollisionProcess( numContacts, contactsbuffer ) )
            return;

        nPhyJointGroup* jgroup(nPhysicsServer::GetWorldBeenProcessed()->GetJointGroup());
        
        nPhyMaterial::idmaterial materialA(phyGeomA->GetMaterial());
        nPhyMaterial::idmaterial materialB(phyGeomB->GetMaterial());

        if( phyGeomA->Type() == nPhysicsGeom::HeightMap )
        {
            nPhyMaterial* mat(contactsbuffer[0].GetPhysicsMaterialA());

            if( mat )
                materialA = mat->Id();
        }

        if( phyGeomB->Type() == nPhysicsGeom::HeightMap )
        {
            nPhyMaterial* mat(contactsbuffer[0].GetPhysicsMaterialB());

            if( mat )
                materialB = mat->Id();
        }
        //#ifndef NGAME // NOTE!!!!!!: This should uncomented b4 compiling the real last definitive game
        if( !(materialA + materialB) )
        {
            materialA = nPhysicsServer::DefaultMaterial; // get a default material
        }
/*#else
        n_assert2( (materialA + materialB), "None of the colliding geometries has material." );
#endif*/

        const bool IsDynamicMaterialA( materialA ? nPhyMaterial::GetMaterial( materialA )->IsType( nPhyMaterial::FrictionDirection ) : false );
        const bool IsDynamicMaterialB( materialB ? nPhyMaterial::GetMaterial( materialB )->IsType( nPhyMaterial::FrictionDirection ) : false );

        nPhyContactJoint* contactJoint( 0 );

        surfacedata& sdata( *nPhyMaterial::GetMaterial( materialA, materialB ) );

        // begin: impact process
        nPhysicsServer::impactsReporting ireport(nPhysicsServer::Instance()->GetImpactCallBack());

        if( ireport )
        {
            (*ireport)( objA, objB, contactsbuffer );
        }
        // end: impact process

        for (int i(0); i< numContacts; ++i, ++nPhysicsServer::GetWorldBeenProcessed()->simNumContactsInBuffer ) 
        {
            n_assert2( nPhysicsServer::GetWorldBeenProcessed()->simNumContactsInBuffer < BufferNumberOfContacts, "Run out of contacts." );

            // TODO: Too slow, once we got arrays of nobjects, make the pointer travers the array
            contactJoint = nPhysicsServer::GetWorldBeenProcessed()->simContactJoints[ nPhysicsServer::GetWorldBeenProcessed()->simNumContactsInBuffer ];

            n_assert2( contactJoint, "Failed to create a contact joint" );

            contactJoint->SetData( sdata );

            contactJoint->SetData( contactsbuffer[i] );

            if( IsDynamicMaterialA )
            {
                objA->ProcessDynamicMaterial( contactJoint );
            }

            if( IsDynamicMaterialB )
            {
                objB->ProcessDynamicMaterial( contactJoint );
            }

            contactJoint->CreateIn( nPhysicsServer::GetWorldBeenProcessed(), 
                jgroup );

            contactJoint->Attach( bodyA, bodyB );
        }
#ifndef NGAME
        DEBUG_NumberOfContacts += numContacts;
#endif // _DEBUG
    }
}


//-----------------------------------------------------------------------------
/**
    Adds a breakable joint.

    @param breakablejoint a breakable joint

    history:
        - 10-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::Add( nPhysicsJoint* breakablejoint )
{
    n_assert2( breakablejoint, "Null pointer" );

    this->containerBreakableJoints.Add( 
        int(reinterpret_cast<size_t>(breakablejoint)),
        breakablejoint );
}

//-----------------------------------------------------------------------------
/**
    Removes a breakable joint.

    @param breakablejoint a breakable joint

    history:
        - 10-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::Remove( nPhysicsJoint* breakablejoint )
{
    n_assert2( breakablejoint, "Null pointer" );

    this->containerBreakableJoints.Rem( 
        int(reinterpret_cast<size_t>(breakablejoint)));

}

//-----------------------------------------------------------------------------
/**
    Runs the breakable joints.

    history:
        - 10-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::RunBreakableJoints()
{
    profPhysicsBreakableJoints.StartAccum();
    for( int index(0); index < this->containerBreakableJoints.Size(); ++index )
    {
        this->containerBreakableJoints.GetElementAt(index)->Break();
    }
    profPhysicsBreakableJoints.StopAccum();
}

//-----------------------------------------------------------------------------
/**
    Checks of the world simulation.

    history:
        - 25-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::Check()
{
    // begin: temporary code
    /// Checking rigid body it isn't spinning too fast
    for( int index(0); index < this->containerRigidBodies.Size(); ++index )
    {
        nPhyRigidBody* body(this->containerRigidBodies.GetElementAt(index));

        n_assert2( body, "Data corruption" );

        if( !body->IsEnabled() )
            continue;

        if( body->GetPhysicsObj()->IsFastRotatingObject() )
            continue;
#if 1
        {

            /// checking angular speed
            vector3 velocity;

            body->GetAngularVelocity( velocity );

            static phyreal angularVelocityStopAt = 30;
            static phyreal angularVelocityStopAtSquared = angularVelocityStopAt*angularVelocityStopAt;

            if( velocity.lensquared() >= angularVelocityStopAtSquared )
            {            
                velocity.norm();
                velocity *= phyreal(angularVelocityStopAt*phyreal(.90));
                body->SetAngularVelocity( velocity );
            }

        }
#endif
    }
    // end: temporary code
}

//-----------------------------------------------------------------------------
/**
    Processes the explosions.

    history:
        - 25-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::ProcessBooms()
{
    if( !this->NumberExplosions )
    {
        /// no explosions to process
        return;
    }

    profPhysicsProcessBooms.StartAccum();

    // begin: first implementation

    int NumberBodies( this->containerRigidBodies.Size() );

    for( int indexBodies(0); indexBodies < NumberBodies; ++indexBodies )
    {
        vector3 bodyPosition;

        nPhyRigidBody* body( this->containerRigidBodies.GetElementAt( indexBodies ) );

        n_assert2( body, "Data corruption" );

        body->GetPosition( bodyPosition );

        int counter(0);

        for( int index(0); counter < this->NumberExplosions; ++index, ++counter )
        {
            nPhyBoom* boom(this->booms[index]);

            vector3 positionBoom( boom->At() );

            // Check if it's affected
            vector3 distance( bodyPosition - positionBoom );

            if( distance.len() > boom->GetLength() )
            {
                // out of reach
                continue;
            }

            // checking if the body it's cover
            if( !boom->IsAffected( body ) )
                continue;

            phyreal length(distance.len());

            // explosion direction
            distance.norm();

            distance *= (boom->GetForce() * (length / boom->GetLength()));

            body->SetForce( distance );
        }
    }

    /// reseting explosion
    this->NumberExplosions = 0;

    // end: first implementation
    profPhysicsProcessBooms.StopAccum();
}

//-----------------------------------------------------------------------------
/**
    Inits the explosions resources.

    history:
        - 25-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::InitExplosions()
{
    for( int index(0); index < MaxNumberExplosions; ++index )
    {
        this->booms[ index ] = static_cast<nPhyBoom*>( nObject::kernelServer->New("nphyboom"));
        n_assert2( this->booms[ index ], "Failed to allocate memory" );
    }
}

//-----------------------------------------------------------------------------
/**
    Frees the explosions resources.

    history:
        - 25-Nov-2004   David Reyes   created
*/
void nPhysicsWorld::FreeExplosions()
{
    for( int index(0); index < MaxNumberExplosions; ++index )
    {
        this->booms[ index ]->Release();

        this->booms[ index ] = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the worlds space.

    @return a space

    history:
        - 15-Dec-2004   David Reyes   created
*/
nPhySpace* nPhysicsWorld::GetSpace() const
{
    return this->worldSpace;
}

//-----------------------------------------------------------------------------
/**
    Processes the objects b4 run the world.

    history:
        - 16-Dec-2004   David Reyes   created
*/
void nPhysicsWorld::PreProcessObjects()
{
    profPhysicsPreProcessObjects.StartAccum();

    int indexEnd(this->containerObjectsPreProcess.Size());
    for( int index(0); index < indexEnd; ++index )
    {
        ncPhysicsObj* obj(this->containerObjectsPreProcess.GetElementAt(index));
        if( obj->IsEnabled() )
            obj->PreProcess();
    }

    profPhysicsPreProcessObjects.StopAccum();
}

//-----------------------------------------------------------------------------
/**
    Processes the objects after run the world.

    history:
        - 21-Apr-2005   David Reyes   created
*/
void nPhysicsWorld::PostProcessObjects()
{
    profPhysicsPostProcessObjects.StartAccum();

    int indexEnd(this->containerObjectsPostProcess.Size());

    for( int index(0); index < indexEnd; ++index )
    {
        ncPhysicsObj* obj(this->containerObjectsPostProcess.GetElementAt(index));
        if( obj->IsEnabled() )
            obj->PostProcess();
    }

    profPhysicsPostProcessObjects.StopAccum();
}

//-----------------------------------------------------------------------------
/**
    Resets objects for each step.

    history:
        - 17-Mar-2005   David Reyes   created
*/
void nPhysicsWorld::ResetObjects()
{
    profPhysicsResetObjects.StartAccum();

    int indexEnd(this->containerRigidBodies.Size());

    for( int index(0); index < indexEnd; ++index )
    {
        nPhyRigidBody* obj(this->containerRigidBodies.GetElementAt(index));
        if( obj->IsEnabled() )
            obj->GetPhysicsObj()->Resets();
    }

    profPhysicsResetObjects.StopAccum();
}

//-----------------------------------------------------------------------------
/**
    Returns if the world it's been destroyed.

    @return true/false

    history:
        - 17-Mar-2005   David Reyes   created
*/
bool nPhysicsWorld::IsBeenDestroyed() const
{
    return this->beenDestroyed;
}

//-----------------------------------------------------------------------------
/**
    Sets if the world it's been destroyed.

    @param is true/false

    history:
        - 17-Mar-2005   David Reyes   created
*/
void nPhysicsWorld::SetBeenDestroyed( const bool is )
{
    this->beenDestroyed = is;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Returns if the world has bodies.

    @return true/false

    history:
        - 06-Apr-2005   David Reyes   created
*/
bool nPhysicsWorld::HasBodies() const
{
    return this->hasBodies;
}

//-----------------------------------------------------------------------------
/**
    Sets if the world has bodies.

    @param is true/false

    history:
        - 06-Apr-2005   David Reyes   created
*/
void nPhysicsWorld::SetHasBodies( bool has )
{
    this->hasBodies = has;
}
#endif
//-----------------------------------------------------------------------------
/**
    Updates the transform.

    history:
        - 06-Apr-2005   David Reyes   created
*/
void nPhysicsWorld::UpdateTransform()
{
    profPhysicsUpdateTransform.StartAccum();

    // NOTE: Slow point

#ifndef NGAME
    if( this->HasBodies() == false )
    {
        return;
    }
#endif

    for( int index(0); index < this->containerRigidBodies.Size(); ++index )
    {
        nPhyRigidBody* body( this->containerRigidBodies.GetElementAt( index ));
        if( !body->IsEnabled() )
        {
            continue;
        }
    
        body->GetPhysicsObj()->UpdateTransformInfo();
    }

    profPhysicsUpdateTransform.StopAccum();
}

//-----------------------------------------------------------------------------
/**
    Creates the storage for the contacts joints of the simulation.

    history:
        - 09-Apr-2005   David Reyes   created
*/
void nPhysicsWorld::InitStorageContactJoints()
{
    for( int index(0); index < BufferNumberOfContacts; ++index )
    {
        this->simContactJoints[ index ] = static_cast<nPhyContactJoint*>(nObject::kernelServer->New( "nphycontactjoint" ));
        
        n_assert2( this->simContactJoints[ index ], "Failed to allocate contact joints." );
    }
}

//-----------------------------------------------------------------------------
/**
    Releases the joint contacts after the simulation.

    history:
        - 09-Apr-2005   David Reyes   created
*/
void nPhysicsWorld::ReleaseJointContacts()
{
    profPhysicsReleaseContacts.StartAccum();
    phyEmptyJointGroup( this->GetJointGroup()->Id() );
    profPhysicsReleaseContacts.StopAccum();

    this->simNumContactsInBuffer = 0;
}

//-----------------------------------------------------------------------------
/**
    Destroys the storage for the contacts joints of the simulation.

    history:
        - 09-Apr-2005   David Reyes   created
*/
void nPhysicsWorld::DestroyStorageContactJoints()
{
    for( int index(0); index < BufferNumberOfContacts; ++index )
    {
        this->simContactJoints[ index ]->Release();
    }    
}


//-----------------------------------------------------------------------------
/**
    Adds a Mobil Physics object to the world.

    @param obj a physic's object

    history:
        - 02-Jul-2005   David Reyes    created
*/
void nPhysicsWorld::AddMobil( ncPhysicsObj* obj )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( obj , "Null pointer" );

    this->containerMobilObj.Add( obj->Id(), obj );
}

//-----------------------------------------------------------------------------
/**
    Removes a Mobil Physics object to the world.

    @param obj a physic's object

    history:
        - 02-Jul-2005   David Reyes    created
*/
void nPhysicsWorld::RemoveMobil( ncPhysicsObj* obj )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    n_assert2( obj , "Null pointer" );

    this->containerMobilObj.Rem( obj->Id() );
}

//-----------------------------------------------------------------------------
/**
    Processes mobil objects.

    history:
        - 02-Jul-2005   David Reyes    created
*/
void nPhysicsWorld::ProcessMobilObjects()
{
    profPhysicsMobilObjects.StartAccum();

    n_assert2( this->Id() != NoValidID , "No valid world" );

    int endIndex( this->containerMobilObj.Size());

    for( int index(0); index < endIndex; ++index )
    {
        ncPhysicsObj* obj( this->containerMobilObj.GetElementAt( index ) );

        if( !obj->IsDirty() )
            continue;

        this->objectManager.Insert( obj );

        obj->Clean();
    }

    profPhysicsMobilObjects.StopAccum();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
