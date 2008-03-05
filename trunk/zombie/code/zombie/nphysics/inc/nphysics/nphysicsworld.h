#ifndef N_PHYSICSWORLD_H
#define N_PHYSICSWORLD_H
//-----------------------------------------------------------------------------
/**
    @class nPhysicsWorld
    @ingroup NebulaPhysicsSystem
    @brief An abstract physics world

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics World
    
    @cppclass nPhysicsWorld
    
    @superclass nRoot

    @classinfo An abstract physics world.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphyboom.h"
#include "nphysics/nphyatmosphera.h"
#include "nphysics/nphycollide.h"
#include "nphysics/phyobjmanager.h"

//-----------------------------------------------------------------------------

class nPhysicsGeom;
class ncPhysicsObj;
class nPhysicsJoint;
class nPhyJointGroup;
class nPhySpace;
class nPhyContactJoint;

//-----------------------------------------------------------------------------

class nPhysicsWorld : public nRoot
{
public:
    /// constructor
    nPhysicsWorld();

    /// destructor
    ~nPhysicsWorld();

    /// creates the world
    void Create();

    /// returns if the world is enabled
    bool IsEnabled() const;

    /// returns the Id to the world
    worldid Id() const;

    /// returns the gravity of this world
    const vector3& GetGravity() const;

    /// removes a physic object from the world
    void Remove( ncPhysicsObj* obj );

    /// adds a rigid body to the world
    void Add( nPhyRigidBody* obj );

    /// removes a rigid body from the world
    void Remove( nPhyRigidBody* obj );

    /// adds a Mobil Physics object to the world
    void AddMobil( ncPhysicsObj* obj );

    /// removes a Mobil Physics object to the world
    void RemoveMobil( ncPhysicsObj* obj );

    /// returns if a geometry it's colliding with any of the others
    int Collide( 
        const nPhysicsGeom *geom,
        int numContacts,
        nPhyCollide::nContact* contact );

#ifndef NGAME
    /// debug tool to draw the geometries 
    void Draw( nGfxServer2* server );

    /// enables/disables depending of the active layers
    void EnableByLayers();
#endif

    /// runs the world
    void RunWorld( phyreal step );

    /// returns the number of physics object
    int GetNumPhysicObjs() const;

    /// returns the physics object by index
    ncPhysicsObj* GetPhysicObj( int index ) const;

#ifndef NGAME
    /// debug info
    static int DEBUG_NumberOfContacts;
    static int DEBUG_NumberOfBodies;
#endif // _DEBUG

    /// adds a breakable joint
    void Add( nPhysicsJoint* breakablejoint );

    /// removes a breakable joint
    void Remove( nPhysicsJoint* breakablejoint );

    /// checks of the world simulation
    void Check();

    /// registers a explosion
    void RegisterExplosion( nPhyBoom& boom );

    /// processes the explosions
    void ProcessBooms();

    /// returns the worlds space
    nPhySpace* GetSpace() const;

    /// registers a physics object to be process b4 run world
    void RegisterPreProcess( ncPhysicsObj* object );

    /// unregisters a physics object to be process b4 run world
    void UnregisterPreProcess( ncPhysicsObj* object );

    /// registers a physics object to be process after run world
    void RegisterPostProcess( ncPhysicsObj* object );

    /// unregisters a physics object to be process after run world
    void UnregisterPostProcess( ncPhysicsObj* object );

    /// returns the general joints group
    nPhyJointGroup* GetGeneralJointGroup() const;

    /// returns if the world it's been destroyed
    bool IsBeenDestroyed() const;

    /// returns the last step
    unsigned long GetStep() const;

    /// returns this world object manager
    phyObjManager* GetObjectManager();

    /// container type
    typedef nKeyArray< nPhyRigidBody* > tContainerRigidBodies;

    /// returns the list of rigid bodies
    tContainerRigidBodies const& GetRigidBodies() const;

#ifndef NGAME
    /// returns if the world has bodies
    bool HasBodies() const;

    /// sets if the world has bodies
    void SetHasBodies( bool has );

#endif

    /// begin:scripting

    /// adds a physic object to the world
    void Add(nEntityObject*);

    /// sets a space for this world
    void SetSpace(nPhySpace*);
    
    /// enables this world
    void Enable();
    
    /// disables this world
    void Disable();

    /// sets the gravity of this world (in m/s/s)
    void SetGravity(const vector3&);

    /// end:scripting

private:
    /// destroys the storage for the contacts joints of the simulation
    void DestroyStorageContactJoints();

    /// releases the joint contacts after the simulation
    void ReleaseJointContacts();

    /// creates the storage for the contacts joints of the simulation
    void InitStorageContactJoints();

    /// updates the transform
    void UpdateTransform();

    /// stores if the world has bodies
    bool hasBodies;

    /// sets if the world it's been destroyes
    void SetBeenDestroyed( const bool is );

    /// process atmosfera
    nPhyAtmosphera atmosphera;

    /// inits the explosions resources
    void InitExplosions();

    /// frees the explosions resources
    void FreeExplosions();

    /// destroys the world
    void Destroy();

    /// id to the created world
    worldid worldID;

    /// default value of the objects to be when created
    bool enable;

    /// holds the value of the gravity in this world
    vector3 gravityWorld;

    /// number of initial worlds space
    static const int NumInitialPhysicsObj = 30;

    /// growth pace
    static const int NumGrowthPhysicsObj = 10;

    /// type container physic objects
    typedef nKeyArray< ncPhysicsObj* > tContainer;

    /// container of physic objects
    tContainer containerPhysicsObj;
   
    /// container of mobil collision objects
    tContainer containerMobilObj;

    /// container of rigid bodies
    tContainerRigidBodies containerRigidBodies;

    /// type container physic objects
    typedef nKeyArray< ncPhysicsObj* > tContainerProcess;

    /// container of objects to be process b4 run the world
    tContainerProcess containerObjectsPreProcess;

    /// container of objects to be process after run the world
    tContainerProcess containerObjectsPostProcess;

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// space representing the whole world
    nPhySpace* worldSpace;

    /// stores the contactgroup for this world;
    nPhyJointGroup* jointsgroup;

    /// default number of contact groups
    static const int DefaultSizeJointGroup = 200;

    /// collisions reaction
    static void PossibleCollision(geomid geomA, geomid geomB);

    /// default value of contacts
    static const int BufferNumberOfContacts = 1000;

    /// contacts buffer
    static nPhyCollide::nContact contactsbuffer[BufferNumberOfContacts];

    /// stores if the world is enabled
    bool enabled;

    /// number of initial breakable joints
    static const int NumInitialBreakableJoints = 30;

    /// growth pace
    static const int NumGrowthBreakableJoints = 10;

    /// type container
    typedef nKeyArray< nPhysicsJoint* > tContainerBreakableJoints;

    /// stores the breakable joints
    tContainerBreakableJoints containerBreakableJoints;

    /// runs the breakable joints
    void RunBreakableJoints();

    /// number maximun of explosions per world
    static const int MaxNumberExplosions = 64;

    /// type of booms container
    typedef nPhyBoom* tContainerBooms[MaxNumberExplosions];
    
    /// explosions
    tContainerBooms booms;

    /// number of explosions for next step
    int NumberExplosions;

    /// returns the world joint group
    nPhyJointGroup* GetJointGroup() const;

    /// processes the objects b4 run the world
    void PreProcessObjects();

    /// processes the objects after run the world
    void PostProcessObjects();

    /// stores the general contactgroup for this world;
    nPhyJointGroup* generaljointsgroup;

    /// resets objects for each step
    void ResetObjects();

    /// stores if the world it's been destroyed
    bool beenDestroyed;

    /// step counter
    unsigned long stepCounter;

    /// stores the contact joints for simulation
    nPhyContactJoint* simContactJoints[ BufferNumberOfContacts ];

    /// stores the number of contacts already created in a step of the simulation
    int simNumContactsInBuffer;

    /// objects manager
    phyObjManager objectManager;

    /// processes mobil objects
    void ProcessMobilObjects();
};

//-----------------------------------------------------------------------------
/**
    Returns the Id to the world

    @return world id

    history:
        - 23-Sep-2004   David Reyes   created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
worldid nPhysicsWorld::Id() const
{
    return this->worldID;
}

//-----------------------------------------------------------------------------
/**
    Returns the world joint group.

    @return joint group

    history:
        - 12-Dec-2004   David Reyes     created
*/
inline
nPhyJointGroup* nPhysicsWorld::GetJointGroup() const
{
    return this->jointsgroup;
}

//-----------------------------------------------------------------------------
/**
    Returns if the geometries has to be created enabled

    @return boolean specyfing if the objects are created enabled or disabled

    history:
        - 23-Sep-2004   David Reyes    created
        - 31-Jan-2005   David Reyes    inlined
*/
inline
bool nPhysicsWorld::IsEnabled() const
{
    n_assert2( this->Id() != NoValidID , "No valid world id" );
    
    return this->enabled;
}

//-----------------------------------------------------------------------------
/**
    Sets the gravity of this world

    @param gravity  vector giving the gravity effect in each axis of the world

    history:
        - 23-Sep-2004   David Reyes    created
        - 31-Jan-2005   David Reyes    inlined
*/
inline
void nPhysicsWorld::SetGravity( const vector3& gravity )
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    this->gravityWorld = gravity;

    phySetGravityWorld( this->Id(), gravity );
}

//-----------------------------------------------------------------------------
/**
    Returns the gravity of this world

    @return vector giving the gravity effect in each axis of the world

    history:
        - 23-Sep-2004   David Reyes    created
        - 31-Jan-2005   David Reyes    inlined
*/
inline
const vector3& nPhysicsWorld::GetGravity() const
{
    n_assert2( this->Id() != NoValidID , "No valid world" );

    return this->gravityWorld;
}

//-----------------------------------------------------------------------------
/**
    Returns the number of physics object.

    @return number of physics objects in the world

    history:
        - 23-Sep-2004   David Reyes    created
        - 31-Jan-2005   David Reyes    inlined
*/
inline
int nPhysicsWorld::GetNumPhysicObjs() const
{
    return this->containerPhysicsObj.Size();
}

//-----------------------------------------------------------------------------
/**
    Returns the physics object by index.

    @return a physic object

    history:
        - 23-Sep-2004   David Reyes   created
        - 31-Jan-2005   David Reyes    inlined
*/
inline
ncPhysicsObj* nPhysicsWorld::GetPhysicObj( int index ) const
{
    n_assert2( index <= this->containerPhysicsObj.Size(), "Index out of bounds" );

    n_assert2( index >= 0, "Index out of bounds" );

    return this->containerPhysicsObj.GetElementAt(index);
}

//-----------------------------------------------------------------------------
/**
    Registers a explosion.

    @param boom an explosion

    history:
        - 25-Nov-2004   David Reyes   created
        - 31-Jan-2005   David Reyes   inlined
*/
inline
void nPhysicsWorld::RegisterExplosion( nPhyBoom& boom )
{
    n_assert2( this->NumberExplosions < MaxNumberExplosions, "Not enough space for the explosions" );

    *booms[ NumberExplosions++ ] = boom;
}

//-----------------------------------------------------------------------------
/**
    Registers a physics object to be process after run world.

    @param object a physics object

    history:
        - 21-Apr-2005   David Reyes   created
*/
inline
void nPhysicsWorld::RegisterPostProcess( ncPhysicsObj* object )
{
    n_assert2( object, "Null pointer" );

    this->containerObjectsPostProcess.Add( int(reinterpret_cast<size_t>(object)), object );
}

//-----------------------------------------------------------------------------
/**
    Unregisters a physics object to be process after run world.

    @param object a physics object

    history:
        - 21-Apr-2005   David Reyes   created
*/
inline
void nPhysicsWorld::UnregisterPostProcess( ncPhysicsObj* object )
{

    // not likely to be at any critical moment of speed

    n_assert2( object, "Null pointer" );

    this->containerObjectsPostProcess.Rem( int(reinterpret_cast<size_t>(object) ) );
}

//-----------------------------------------------------------------------------
/**
    Unregisters a physics object to be process b4 run world.

    @param object a physics object

    history:
        - 16-Dec-2004   David Reyes   created
        - 31-Jan-2005   David Reyes   inlined
*/
inline
void nPhysicsWorld::UnregisterPreProcess( ncPhysicsObj* object )
{

    // not likely to be at any critical moment of speed

    n_assert2( object, "Null pointer" );

    this->containerObjectsPreProcess.Rem( int(reinterpret_cast<size_t>(object) ) );
}

//-----------------------------------------------------------------------------
/**
    Returns the general joints group.

    @return joint group

    history:
        - 16-Dec-2004   David Reyes   created
        - 31-Jan-2005   David Reyes   inlined
*/
inline
nPhyJointGroup* nPhysicsWorld::GetGeneralJointGroup() const
{
    return this->generaljointsgroup;
}

//-----------------------------------------------------------------------------
/**
    Registers a physics object to be process b4 run world.

    @param object a physics object

    history:
        - 16-Dec-2004   David Reyes   created
        - 31-Jan-2005   David Reyes   inlined
*/
inline
void nPhysicsWorld::RegisterPreProcess( ncPhysicsObj* object )
{
    n_assert2( object, "Null pointer" );

    this->containerObjectsPreProcess.Add( int(reinterpret_cast<size_t>(object)), object );
}

//-----------------------------------------------------------------------------
/**
    Returns the last step.

    @return steps

    history:
        - 22-Mar-2005   David Reyes   created
*/
inline
unsigned long nPhysicsWorld::GetStep() const
{
    return this->stepCounter;
}

//-----------------------------------------------------------------------------
/**
    Returns this world object manager.

    @return object managers

    history:
        - 27-Jun-2005   David Reyes   created
*/
inline
phyObjManager* nPhysicsWorld::GetObjectManager()
{
    return &this->objectManager;
}

//-----------------------------------------------------------------------------
/**
    Returns the list of rigid bodies.

    @return list of rigid bodies

    history:
        - 27-Jun-2005   David Reyes   created
*/
inline
nPhysicsWorld::tContainerRigidBodies const& nPhysicsWorld::GetRigidBodies() const
{
    return this->containerRigidBodies;
}

#endif