#ifndef NC_PHYSICSOBJ_H
#define NC_PHYSICSOBJ_H
//-----------------------------------------------------------------------------
/**
    @class ncPhysicsObj
    @ingroup NebulaPhysicsSystem
    @brief An abstract physics object interface with some build-in functionality.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Object.
    
    @cppclass ncPhysicsObj
    
    @superclass nComponentObject

    @classinfo An abstract physics object interface with some build-in functionality.

*/

//-----------------------------------------------------------------------------

#include "entity/nentity.h"
#include "nphysics/nphycollide.h"
#include "nphysics/nphymaterial.h"
#include "nphysics/nphyrigidbody.h"
#include "nphysics/nphysicsserver.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhysicsGeom;
class nPhyContactJoint;
class ncTransform;
class nPhySpace;

#ifndef NGAME
    class nGfxServer2;  
#endif // !NGAME

//-----------------------------------------------------------------------------
class ncPhysicsObj : public nComponentObject
{

    NCOMPONENT_DECLARE(ncPhysicsObj,nComponentObject);

public:
    /// types of physics objects
    typedef enum {
        General = 0,
        Terrain,
        Vehicle,
        Fluid,
        RagDoll,
        Indoor,
#ifndef NGAME
        Pickeable,
#endif
        Num,
        Invalid
    } ObjectType;

    typedef enum {
        InWater = 1 << 0
    } ObjectState;

    /// constructor
    ncPhysicsObj();

    /// destructor
    virtual ~ncPhysicsObj();

    /// returns the ID of this object
    int Id() const;

    /// creates the physic object
    virtual void Create( nPhysicsWorld* world );
    
    /// gets the object type
    ObjectType GetType() const;

    /// sets the object type
    void SetType( ObjectType newtype );

#ifndef NGAME
    /// draws the physic object
    virtual void Draw( nGfxServer2* server );
#endif // !NGAME

    /// returns the world of this object
    nPhysicsWorld* GetWorld() const;
   
    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// object chunk persistency
    bool SaveChunk(nPersistServer *ps);

    /// checks if collision with other physics object
    virtual int Collide( const ncPhysicsObj* obj,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// checks if collision with a geometry
    virtual int Collide( const nPhysicsGeom* geom,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// checks if the object collides with anything in his own world.
    virtual int Collide( int numContacts, nPhyCollide::nContact* contact ) const;

    /// sets the rotation of this physic object
    virtual void SetRotation( const matrix33& newrotation );

    /// sets the rotation of this physic object
    virtual void SetRotation( phyreal ax, phyreal ay, phyreal az );

    /// sets the position of this physic object
    virtual void SetPosition( const vector3& );

    /// returns current position
    virtual void GetPosition( vector3& position ) const;

    /// gets the rotation of this physic object
    virtual void GetOrientation( matrix33& rotation );

    /// sets the material for all the geometries contain in the object
    virtual void SetMaterial( nPhyMaterial::idmaterial material );

    /// returns a geometry based on the index
    virtual nPhysicsGeom* GetGeometry( int index ) const;

    /// returns the bounding box of this geometry
    virtual void GetAABB( nPhysicsAABB& boundingbox );

    /// sets the world of this object
    void SetWorld( nPhysicsWorld* world );

    /// returns the object material
    nPhyMaterial::idmaterial GetMaterial() const;

    /// returns the body
    virtual nPhyRigidBody* GetBody() const;

    /// begin: interface nPhyRigidBody
    /// sets the linear velocity
    void SetLinearVelocity( const vector3& velocity );

    /// gets the linear velocity
    void GetLinearVelocity( vector3& velocity ) const;
    
    /// sets the angular velocity
    void SetAngularVelocity( const vector3& velocity );

    /// gets the angular velocity
    void GetAngularVelocity( vector3& velocity ) const;

    /// adds a torque to the body
    void AddTorque( const vector3& torque );

    /// adds a relative force to the body
    void AddRelativeForce( const vector3& force );

    /// adds a relative torque to the body
    void AddRelativeTorque( const vector3& torque );

    /// adds a force at a given position
    void AddForceAtPosition( const vector3& force, const vector3& position );

    /// adds a force at a relative position
    void AddForceAtRelativePosition( const vector3& force, const vector3& position );

    /// adds a relative force at a given position
    void AddRelativeForceAtPosition( const vector3& force, const vector3& position );

    /// adds a relative force at a relative position
    void AddRelativeForceAtRelativePosition( const vector3& force, const vector3& position );

    /// sets the absolute torque applied to the object
    void SetTorque( const vector3& torque );

    /// Gets the absolute force applied to the object
    void GetForce( vector3& force ) const;

    /// Gets the absolute torque applied to the object
    void GetTorque( vector3& torque ) const;

    /// end: interface nPhyRigidBody

    /// function to be process b4 running the simulation
    virtual void PreProcess();

    /// registers the object for pre process
    void RegisterForPreProcess();

    /// unregisters the object for pre process
    void UnregisterForPreProcess();

    /// function to be process after running the simulation
    virtual void PostProcess();

    /// registers the object for post process
    void RegisterForPostProcess();

    /// unregisters the object for post process
    void UnregisterForPostProcess();

    /// function to be procesed during the collision check
    virtual bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

    /// returns the number of geometries
    virtual int GetNumGeometries() const;

    /// returns if it's in a given set of state(s)
    bool IsState( uint32 flags ) const;

    /// sets a set of state(s)
    void SetState( uint32 flags );

    /// removes a set of states
    void RemoveState( uint32 flags );

    /// returns the parent space
    nPhySpace* GetParentSpace() const;

    /// Impact Processing for a given force
    void ShowImpactWhenForceLargeThan( const phyreal force );

    /// returns the impact treshold
    phyreal GetImpactTreshold() const;

    /// POD with the impact information
    typedef struct {
        vector3 impact;
        vector3 point;
        nPhysicsGeom* geom;
    } tImpactInfo;

    /// function call when an impact ocurs
    virtual void OnImpact( const tImpactInfo& info );

    /// resets the physics
    virtual void Reset();

    /// processes the information when a dynamic material is finded
    virtual void ProcessDynamicMaterial( nPhyContactJoint* cjoint );

    /// sets if the object it's a fast rotating object.
    void SetFastRotatingObject( const bool is );

    /// returns if the object it's a fast rotation object
    const bool IsFastRotatingObject() const;

    /// begin:scripting
    
    /// enables the object
   virtual void Enable();
    /// disables the object
   virtual void Disable();
    /// returns if the object it's enabled
    const bool IsEnabled() const;
    /// sets the material for all the geometries contain in the object
   virtual void SetMaterial(const nPhyMaterial*);
    /// sets the body mass
    void SetMass(phyreal);
    /// gets the body mass
    phyreal GetMass() const;
    /// sets the body density
    void SetDensity(phyreal);
    /// gets the body density
    phyreal GetDensity() const;
    /// sets the physic object to another space
    void SetSpace(nPhySpace*);
    /// sets the absolute force applied to the object
    void SetForce(const vector3&);
    /// moves the physic object to another space
   virtual void MoveToSpace(nPhySpace*);
    /// adds a force to the body
   virtual void AddForce(const vector3&);
    /// adds a force to the body
    void SetOrientation(phyreal,phyreal,phyreal);
    /// resets flags for each physics step
   virtual void Resets();
    /// scales the object
   virtual void Scale(const phyreal);
    /// scales the object
    void SetLastScaledFactor(const phyreal);
    /// sets the categories where the object belongs
   virtual void SetCategories(int);
    /// gets the categories where the object belongs
   virtual int GetCategories() const;
    /// sets the categories wich the object will collide with
   virtual void SetCollidesWith(int);
    /// gets the categories wich the object will collide with
   virtual int GetCollidesWith() const;
    /// sets the physic object to another space by name
    void SetSpaceByName(const nString&);
    /// adds the object to the world
   virtual void InsertInTheWorld();
    /// sets the object position by scripting
   virtual void SetPositionPhyObj(const vector3&);
    /// stores tweaked information (offset)
   virtual void SetTweakedOffset(const vector3&);
    /// runs chunk
   virtual void PreparePhyObject();

    /// end:scripting

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// sets to update transfrom
    void UpdateTransform();

    /// unsets to update transfrom
    void NoUpdateTransform();

    /// sets to update transfrom info
    void UpdateTransformInfo();

    /// updates from transfrom the orientation and position
    void Update( const matrix44& transform );

    /// alienates the object from relations
    void Alienate();

    /// makes the object mobil
    void MakeItMobil();

    /// makes the object clean
    void Clean();

    /// returns if the object is dirty
    bool IsDirty() const;

    /// returns if the chunk is loaded
    bool IsLoaded() const;

    /// gets the tweaked offset loaded
    void GetTweakedOffset( vector3& position );

#ifndef NGAME

    /// adds a collide with
    virtual void RemovesCollidesWith( int category );

    /// moves the object to limbo
    virtual void YouShallDwellIntoTheLimbo();

    /// recovers an object from the limbo
    virtual void YourSoulMayComeBackFromLimbo();

#endif
    /// auto-inserts the object in the space
    void AutoInsertInSpace();

    /// sets the owner
    void SetOwner( ncPhysicsObj* obj );

    /// returns the owner
    ncPhysicsObj* GetOwner() const;

#ifdef __ZOMBIE_EXPORTER__
    /// returns the space name
    const nString& GetSpaceName() const;

    /// helper var
    bool isInIndoor;
#endif 

    /// loader
    virtual void Load();

protected:
    /// sets the body of the object
    void SetBody( nPhyRigidBody* body );

    /// returns the last scaled factor
    phyreal GetLastScaledFactor() const;

private:

    /// loads chunk information
    bool LoadChunk(nObject::InitInstanceMsg initType);

    /// stores if the object it's a fast rotation object
    bool isFastRotating : 1;

    /// stores if the object has been translated or rotated
    bool isDirty : 1;

    /// stores if the object chunk has been loaded
    bool isLoaded : 1;

    /// stores if the object it's enabled or not
    bool enabled : 1;

    /// stores if the objects it's registered for process
    bool registeredForPreProcess : 1;

    /// stores if the objects it's registered for process
    bool registeredForPostProcess : 1;

    /// value for no impact report
    static const phyreal NoImpactReport;

    /// force needed to trigger impact call
    phyreal impactForce;

    /// force impact squared
    phyreal impactForceSquared;

    /// stores the rigid body (from 0 to 1 per each object )
    nPhyRigidBody* bodyObject;

    /// stores the objects type
    ObjectType type;

    /// reference to the world of this object
    nPhysicsWorld* worldRef;

    /// physics object parent space
    nPhySpace* parentSpace;

    /// stores the object material
    nPhyMaterial::idmaterial material;

    /// stores the mass of the object
    phyreal objectMass;

    /// stores the density of the object
    phyreal objectDensity;

    /// object states
    uint32 flagsState;

    /// keeps a reference to the transform
    ncTransform* refTransform;

    /// update transform
    bool updateTransform;

    /// stores the owner if any
    ncPhysicsObj* owner;

    // category object
    int categories;

    // collision relationship
    int collidesWith;

#ifdef __ZOMBIE_EXPORTER__
    /// stores the space name
    nString spaceName;
#endif

    // last scale
    phyreal lastScale;

    // tweaked offset
    vector3 tweakedOffsetLoaded;

};

//-----------------------------------------------------------------------------
/**
    Returns if the object it's enabled

    @return true/false

    history:
        - 24-Sep-2004   Zombie         created
        - 28-Jan-2005   Zombie         inlined
*/
inline 
const bool ncPhysicsObj::IsEnabled() const
{
    return this->enabled;
}

//-----------------------------------------------------------------------------
/**
    Enables the physic object

    history:
        - 24-Sep-2004   Zombie         created
        - 28-Jan-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::Enable()
{
    this->enabled = true;
}

//-----------------------------------------------------------------------------
/**
    Disables the physic object.

    history:
        - 24-Sep-2004   Zombie         created
        - 28-Jan-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::Disable() 
{
    this->enabled = false;
}

//-----------------------------------------------------------------------------
/**
    Returns the impact treshold.

    @return treshold value

    history:
        - 22-Mar-2005   Zombie         created
*/
inline
phyreal ncPhysicsObj::GetImpactTreshold() const
{
    return this->impactForce;
}

//-----------------------------------------------------------------------------
/**
    Impact Processing for a given force.

    @param force treshold value

    history:
        - 22-Mar-2005   Zombie         created
*/
inline
void ncPhysicsObj::ShowImpactWhenForceLargeThan( const phyreal force )
{
    this->impactForce = force;
    this->impactForceSquared = force*force;
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation of the object.

    @param ax orientation angle for the x-axis
    @param ay orientation angle for the y-axis
    @param az orientation angle for the z-axis

    history:
        - 22-Mar-2005   Zombie         created
*/
inline
void ncPhysicsObj::SetOrientation( phyreal ax, phyreal ay, phyreal az )
{
    this->SetRotation( ax, ay, az );
}

//-----------------------------------------------------------------------------
/**
    Sets to update transfrom.

    history:
        - 11-May-2005   Zombie         created
*/
inline
void ncPhysicsObj::UpdateTransform()
{
    this->updateTransform = true;
}

//-----------------------------------------------------------------------------
/**
    Unsets to update transfrom.

    history:
        - 11-May-2005   Zombie         created
*/
inline
void ncPhysicsObj::NoUpdateTransform()
{
    this->updateTransform = false;
}

//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
inline
void ncPhysicsObj::Scale( const phyreal /*factor*/ )
{
}

//-----------------------------------------------------------------------------
/**
    Returns the ID of this object

    @return id

    history:
        - 24-Sep-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
int ncPhysicsObj::Id() const
{
    return int(reinterpret_cast<size_t>(this));
}

//-----------------------------------------------------------------------------
/**
    Sets the world of this object

    @param world  represents the world where this object exists

    history:
        - 27-Sep-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetWorld( nPhysicsWorld* world )
{
    this->worldRef = world;
}

//-----------------------------------------------------------------------------
/**
    Sets the world of this object

    @return world where this object exists

    history:
        - 27-Sep-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
nPhysicsWorld* ncPhysicsObj::GetWorld() const
{
    return this->worldRef;
}

//-----------------------------------------------------------------------------
/**
    Moves the physic object to another space.

    @param newspace     newspace

    history:
        - 11-Oct-2004   Zombie         created
        - 29-Nov-2004   Zombie         virtualized
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::MoveToSpace( nPhySpace* newspace )
{
    this->parentSpace = newspace;
}

//-----------------------------------------------------------------------------
/**
    Sets the physic object to another space.

    @param newspace     newspace

    history:
        - 28-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetSpace( nPhySpace* newspace )
{
    this->parentSpace = newspace;
}

//-----------------------------------------------------------------------------
/**
    Gets the object type.

    @return object type

    history:
        - 14-Oct-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhysicsObj::ObjectType ncPhysicsObj::GetType() const
{
    return this->type;
}

//-----------------------------------------------------------------------------
/**
    Sets the object type.

    @param newtype new object type

    history:
        - 14-Oct-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetType( ObjectType newtype )
{
    this->type = newtype;
}

//-----------------------------------------------------------------------------
/**
    Sets the material for all the geometries contain in the object.

    @param material material id

    history:
        - 27-Oct-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetMaterial( nPhyMaterial::idmaterial material )
{
    this->material = material;
}

//-----------------------------------------------------------------------------
/**
    Returns the number of geometries.

    @return num geometries

    history:
        - 29-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
int ncPhysicsObj::GetNumGeometries() const
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Returns the number of geometries.

    @param index    geometry index
    
    @return geometry

    history:
        - 29-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
nPhysicsGeom* ncPhysicsObj::GetGeometry( int /*index*/ ) const
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Returns the object material.

    @return material id

    history:
        - 02-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
nPhyMaterial::idmaterial ncPhysicsObj::GetMaterial() const
{
    return this->material;
}

//-----------------------------------------------------------------------------
/**
    Returns the parent space.

    @return space

    history:
        - 02-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
nPhySpace* ncPhysicsObj::GetParentSpace() const
{
    return this->parentSpace;
}

//-----------------------------------------------------------------------------
/**
    Returns the body.

    @return body

    history:
        - 02-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
nPhyRigidBody* ncPhysicsObj::GetBody() const
{
    return this->bodyObject;
}

//-----------------------------------------------------------------------------
/**
    Sets the linear velocity.

    @param velocity new velocity

    history:
        - 02-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetLinearVelocity( const vector3& velocity )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->SetLinearVelocity( velocity );
}

//-----------------------------------------------------------------------------
/**
    Gets the linear velocity.

    @param velocity

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::GetLinearVelocity( vector3& velocity ) const
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->GetLinearVelocity( velocity );
}

//-----------------------------------------------------------------------------
/**
    Sets the angular velocity.

    @param velocity     new angular velocity

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetAngularVelocity( const vector3& velocity )
{
    n_assert2( this->bodyObject, "No valid object" );
 
    this->GetBody()->SetAngularVelocity( velocity );
}

//-----------------------------------------------------------------------------
/**
    Gets the angular velocity.

    @param velocity

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::GetAngularVelocity( vector3& velocity ) const
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->GetAngularVelocity( velocity );
}

//-----------------------------------------------------------------------------
/**
    Adds a force to the body.

    @param force

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddForce( const vector3& force )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddForce( force );
}

//-----------------------------------------------------------------------------
/**
    Adds a torque to the body.

    @param torque

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddTorque( const vector3& torque )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddTorque( torque );
}

//-----------------------------------------------------------------------------
/**
    Adds a relative force to the body.

    @param force

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddRelativeForce( const vector3& force )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddRelativeForce( force );
}

//-----------------------------------------------------------------------------
/**
    Adds a relative torque to the body.

    @param torque

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddRelativeTorque( const vector3& torque )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddRelativeTorque( torque );
}

//-----------------------------------------------------------------------------
/**
    Adds a force at a given position.

    @param force
    @param position

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddForceAtPosition( const vector3& force, const vector3& position )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddForceAtPosition( force, position );
}

//-----------------------------------------------------------------------------
/**
    Adds a force at a relative position.

    @param force
    @param position

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddForceAtRelativePosition( const vector3& force, const vector3& position )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddForceAtRelativePosition( force, position );
}

//-----------------------------------------------------------------------------
/**
    Adds a relative force at a given position.

    @param force
    @param position

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddRelativeForceAtPosition( const vector3& force, const vector3& position )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddForceAtRelativePosition( force, position );
}

//-----------------------------------------------------------------------------
/**
    Adds a relative force at a relative position.

    @param force
    @param position

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::AddRelativeForceAtRelativePosition( const vector3& force, const vector3& position )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->AddRelativeForceAtRelativePosition( force, position );
}

//-----------------------------------------------------------------------------
/**
    Sets the absolute force applied to the object.

    @param force

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetForce( const vector3& force )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->SetForce( force );
}

//-----------------------------------------------------------------------------
/**
    Sets the absolute torque applied to the object.

    @param torque

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetTorque( const vector3& torque )
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->SetTorque( torque );
}

//-----------------------------------------------------------------------------
/**
    Gets the absolute force applied to the object.

    @param force

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::GetForce( vector3& force ) const
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->GetForce( force );
}

//-----------------------------------------------------------------------------
/**
    Gets the absolute torque applied to the object.

    @param torque

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::GetTorque( vector3& torque ) const  
{
    n_assert2( this->bodyObject, "No valid object" );

    this->GetBody()->GetTorque( torque );
}

//-----------------------------------------------------------------------------
/**
    Sets the body of the object.

    @param body

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetBody( nPhyRigidBody* body )
{
    this->bodyObject = body;
}

//-----------------------------------------------------------------------------
/**
    Sets the body mass.

    @param mass

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetMass( phyreal mass )
{
    n_assert2( mass >= 0, "The mass of an object cannot be negative" );
    this->objectMass = mass;
}

//-----------------------------------------------------------------------------
/**
    Sets the body density.

    @param density

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetDensity( phyreal density )
{
    n_assert2( density >= 0, "The mass of an object cannot be negative" );
    this->objectDensity = density;
}

//-----------------------------------------------------------------------------
/**
    Gets the body mass.

    @return mass

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal ncPhysicsObj::GetMass() const
{
    return this->objectMass;
}

//-----------------------------------------------------------------------------
/**
    Gets the body density.

    @return density

    history:
        - 13-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal ncPhysicsObj::GetDensity() const
{
    return this->objectDensity;
}

//-----------------------------------------------------------------------------
/**
    Returns if it's in a given set of state(s).

    @param flags state flags mask

    @return if all the flags are present with true otherwise false

    history:
        - 24-Jan-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
bool ncPhysicsObj::IsState( uint32 flags ) const
{
    return (this->flagsState & flags) == flags;
}

//-----------------------------------------------------------------------------
/**
    Sets a set of state(s).

    @param flags mask of state flags

    history:
        - 24-Jan-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetState( uint32 flags )
{
    this->flagsState |= flags;
}

//-----------------------------------------------------------------------------
/**
    Removes a set of states.

    @param flags mask of state flags

    history:
        - 24-Jan-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::RemoveState( uint32 flags )
{
    this->flagsState &= ~flags;
}

//-----------------------------------------------------------------------------
/**
    Resets flags for each physics step.

    history:
        - 17-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::Resets()
{
    this->flagsState = 0;
}

//-----------------------------------------------------------------------------
/**
    Returns if the object it's a fast rotation object.

    @return true/false

    history:
        - 26-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
const bool ncPhysicsObj::IsFastRotatingObject() const
{
    return this->isFastRotating;
}

//-----------------------------------------------------------------------------
/**
    Sets the material for all the geometries contain in the object.

    @param material new object's material

    history:
        - 28-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void ncPhysicsObj::SetMaterial( const nPhyMaterial* material )
{
    n_assert2( material, "Null pointer." );

    this->SetMaterial( material->Id() );
}

//-----------------------------------------------------------------------------
/**
    Auto-inserts the object in the space.

    history:
        - 01-Jun-2005   Zombie         created
*/
inline
void ncPhysicsObj::AutoInsertInSpace()
{
    nPhysicsServer::Instance()->InsertInSpace( this );
}

//-----------------------------------------------------------------------------
/**
    Sets the owner.

    history:
        - 02-Jun-2005   Zombie         created
*/
inline
void ncPhysicsObj::SetOwner( ncPhysicsObj* obj )
{
    this->owner = obj;
}

//-----------------------------------------------------------------------------
/**
    Returns the owner.

    @return physics object

    history:
        - 02-Jun-2005   Zombie         created
*/
inline
ncPhysicsObj* ncPhysicsObj::GetOwner() const
{
    return this->owner;
}

#ifdef __ZOMBIE_EXPORTER__
//-----------------------------------------------------------------------------
/**
    Returns the space name.

    @return space name

    history:
        - 06-Jun-2005   Zombie         created
*/
inline
const nString& ncPhysicsObj::GetSpaceName() const
{
    return this->spaceName;
}
#endif 

//-----------------------------------------------------------------------------
/**
    Returns the last scaled factor.

    @return scale factor value

    history:
        - 15-Jun-2005   Zombie         created
*/
inline
phyreal ncPhysicsObj::GetLastScaledFactor() const
{
    return this->lastScale;
}

//-----------------------------------------------------------------------------
/**
    Set the last scaled factor.

    @param factor scale factor

    history:
        - 15-Jun-2005   Zombie         created
*/
inline
void ncPhysicsObj::SetLastScaledFactor( const phyreal factor )
{
    this->lastScale = factor;
}

//-----------------------------------------------------------------------------
/**
    Adds the object to the default world.

    history:
        - 22-Jun-2005   Zombie         created
*/
inline
void ncPhysicsObj::InsertInTheWorld()
{
    this->AutoInsertInSpace();
}

//-----------------------------------------------------------------------------
/**
    Sets the object position by scripting.

    @param position new object's position

    history:
        - 22-Jun-2005   Zombie         created
*/
inline
void ncPhysicsObj::SetPositionPhyObj( const vector3& position )
{
    this->SetPosition( position );
}

//-----------------------------------------------------------------------------
/**
    Makes the object clean.

    history:
        - 02-Jul-2005   Zombie         created
*/
inline
void ncPhysicsObj::Clean()
{
    this->isDirty = false;
}

//-----------------------------------------------------------------------------
/**
    Returns if the object is dirty.

    @return true/false

    history:
        - 02-Jul-2005   Zombie         created
*/
inline
bool ncPhysicsObj::IsDirty() const
{
    return this->isDirty;
}

//-----------------------------------------------------------------------------
/**
    Returns if the object chunk is loaded.

    @return true/false

    history:
        - 01-Jan-2006   MA Garcias      created
*/
inline
bool ncPhysicsObj::IsLoaded() const
{
    return this->isLoaded;
}

#endif
