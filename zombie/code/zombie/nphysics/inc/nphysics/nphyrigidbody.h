#ifndef N_PHYRIGIDBODY_H
#define N_PHYRIGIDBODY_H
//-----------------------------------------------------------------------------
/**
    @class nPhyRigidBody
    @ingroup NebulaPhysicsSystem
    @brief An Rigid Body Representation

    (C) 2004 Conjurer Services, S.A.
*/

#include "nphysics/nphysicsconfig.h"

//-----------------------------------------------------------------------------
class nPhysicsWorld;
class nPhysicsGeom;
class ncPhysicsObj;
//-----------------------------------------------------------------------------

class nPhyRigidBody
{
public:
    /// type of container
    typedef nKeyArray<nPhysicsGeom*> tContainerGeometries;
    
    /// constructor
    nPhyRigidBody();

    /// destructor
    ~nPhyRigidBody();

    /// creates a rigid body
    void CreateIn( nPhysicsWorld* world );

    /// returns the body Id
    bodyid Id() const;

    /// sets the position of the rigid body
    void SetPosition( const vector3& newposition );

    /// gets the position of the rigid body
    void GetPosition( vector3& position ) const;

    /// sets the orientation of the rigid body
    void SetOrientation( const matrix33& neworientation );

    /// sets the euler orientation
    void SetOrientation( phyreal ax, phyreal ay, phyreal az );

    /// returns the body orientation
    void GetOrientation( matrix33& orientation );

    /// returns the total mass of the body
    phyreal GetMass() const;

    /// updates the body information
    void Update( tContainerGeometries& container, phyreal mass, phyreal density, const vector3& offset = vector3(0,0,0) );

    /// updates the body information
    void Update( nPhysicsGeom* geom, phyreal mass, phyreal density, const vector3& offset = vector3(0,0,0) );

    /// sets the linear velocity
    void SetLinearVelocity( const vector3& velocity );

    /// gets the linear velocity
    void GetLinearVelocity( vector3& velocity ) const;
    
    /// sets the angular velocity
    void SetAngularVelocity( const vector3& velocity );

    /// gets the angular velocity
    void GetAngularVelocity( vector3& velocity ) const;

    /// adds a force to the body
    void AddForce( const vector3& force );

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

    /// sets the absolute force applied to the object
    void SetForce( const vector3& force );

    /// sets the absolute torque applied to the object
    void SetTorque( const vector3& torque );

    /// Gets the absolute force applied to the object
    void GetForce( vector3& force ) const;

    /// Gets the absolute torque applied to the object
    void GetTorque( vector3& torque ) const;

    /// returns the world which this object belongs
    nPhysicsWorld* GetWorld() const;

    /// enables the body
    void Enable();

    /// disables the body
    void Disable();

    /// returns if a body it's enabled
    bool IsEnabled() const;

    /// returns the physic obj
    ncPhysicsObj* GetPhysicsObj() const;

    /// set the physics obj
    void SetPhysicsObj( ncPhysicsObj* obj );

    /// returns if it's connected to other body
    bool IsConnectedTo( nPhyRigidBody const* body ) const;

    /// returns the inertia of the object
    void GetInertia( vector3& inertia );

    /// retrieves a body linear velocity treshold for auto-disable
    phyreal GetLinearVelocityThresholdAutoDisable();

    /// retrieves a angular linear velocity treshold for auto-disable
    phyreal GetAngularVelocityThresholdAutoDisable();

    /// returns if the object is to be tweaked
    bool GetTweaked() const;

    /// sets if the body is to be tweaked
    void SetTweaked( bool is );

    /// sets the tweaked offset
    void SetTweakedOffset( const vector3& offset );

    /// returns the tweaked offset
    const vector3& GetTweakedOffset() const;

    /// returns if the atmosphera afects this body
    const bool IsAtmospheraAffected() const;

    /// sets if the atnosphera afects this body
    void SetIfAtmospheraAffected( const bool is );

    /// returns if will be contact if connected to another body already joined
    bool ContactWithOtherBodies() const;

    /// sets if will be contact if connected to another body already joined
    void SetContactWithOtherBodies( const bool is );
    
private:

    /// storage of the rigid body id
    bodyid bodyID;

    /// stores the world
    nPhysicsWorld* worldOwner;

    /// stores the physics object
    ncPhysicsObj* physicsObj;

    /// stores the intertia information for step
    vector3 inertiaBody;

    /// stores last update inertia
    unsigned long lastUpdateInertia;

    /// stores if it's object to be tweaked
    bool tweaked;

    /// stores the tweaked offset
    vector3 tweakedOffset;

    /// stores if the atmosphera affects this body
    bool atmosphera;

    /// stores if enters in contact with other bodies when joined
    bool contactWhenJoined;
};

//-----------------------------------------------------------------------------
/**
    Returns the body Id

    @return body ids

    history:
        - 27-Sep-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline 
bodyid nPhyRigidBody::Id() const
{
    return this->bodyID;
}

//-----------------------------------------------------------------------------
/**
    Returns the physic obj.

    @return physics object

    history:
        - 30-Nov-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
ncPhysicsObj* nPhyRigidBody::GetPhysicsObj() const
{
    return this->physicsObj;
}

//-----------------------------------------------------------------------------
/**
    Returns if it's connected to other body.

    @param body

    history:
        - 07-Dec-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
bool nPhyRigidBody::IsConnectedTo( nPhyRigidBody const* body ) const
{
    return phyAreConnected( this->Id(), body->Id() );
}

//-----------------------------------------------------------------------------
/**
    Returns the world which this object belongs.

    @return world

    history:
        - 04-Nov-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
nPhysicsWorld* nPhyRigidBody::GetWorld() const
{
    return this->worldOwner;
}

//-----------------------------------------------------------------------------
/**
    Enabled the body.

    history:
        - 29-Nov-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyRigidBody::Enable()
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyEnableBody( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Disables the body.

    history:
        - 29-Nov-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyRigidBody::Disable()
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyDisableBody( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Sets the position of the rigid body.

    @param newposition  new body position

    history:
        - 19-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetPosition( const vector3& newposition )
{
    phySetBodyPosition( this->Id(), newposition );
}

//-----------------------------------------------------------------------------
/**
    Gets the position of the rigid body.

    @param position  body position

    history:
        - 25-Nov-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::GetPosition( vector3& position ) const
{
    phyGetBodyPosition( this->Id(), position );
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation of the rigid body

    @param neworientation  new body oritentation

    history:
        - 19-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetOrientation( const matrix33& neworientation )
{
    phySetBodyOrientation( this->Id(), neworientation ); 
}

//-----------------------------------------------------------------------------
/**
    Returns the orientation of the rigid body

    @param orientation  body oritentation

    history:
        - 19-Nov-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::GetOrientation( matrix33& orientation )
{
    phyGetBodyOrientation( this->Id(), orientation ); 
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation of the rigid body

    @param ax   euler angle for the x axis
    @param ay   euler angle for the y axis
    @param az   euler angle for the z axis

    history:
        - 19-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetOrientation( phyreal ax, phyreal ay, phyreal az )
{
    matrix33 neworientation;

    neworientation.from_euler( vector3( ax, ay, az ) );

    phySetBodyOrientation( this->Id(), neworientation ); 
}

//-----------------------------------------------------------------------------
/**
    Returns the mass of the body

    @return bodys mass

    history:
        - 19-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
phyreal nPhyRigidBody::GetMass() const
{
    phymass mass;

    phyGetBodyMass( this->Id(), mass );

    return phyGetScalarMass( mass );
}

//-----------------------------------------------------------------------------
/**
    Sets the linear velocity.

    @param velocity new velocity

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetLinearVelocity( const vector3& velocity )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phySetLinearVelocity( this->Id(), velocity );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Gets the linear velocity.

    @param velocity     rigid body velocity

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::GetLinearVelocity( vector3& velocity ) const
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyGetLinearVelocity( this->Id(), velocity );
}

//-----------------------------------------------------------------------------
/**
    Sets the angular velocity.

    @param velocity     new velocity

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetAngularVelocity( const vector3& velocity )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phySetAngularVelocity( this->Id(), velocity );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Gets the angular velocity.

    @param velocity     rigid body velocity

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::GetAngularVelocity( vector3& velocity ) const
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyGetAngularVelocity( this->Id(), velocity );
}

//-----------------------------------------------------------------------------
/**
    Adds a force to the body.

    @param force    new force to be added

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddForce( const vector3& force )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyAddForce( this->Id(), force );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a torque to the body.

    @param torque    new torque to be added

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddTorque( const vector3& torque )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyAddTorque( this->Id(), torque );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a relative force to the body.

    @param force    new force to be added

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddRelativeForce( const vector3& force )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyAddRelativeForce( this->Id(), force );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a relative torque to the body.

    @param torque    new torque to be added

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddRelativeTorque( const vector3& torque )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );

    phyAddRelativeTorque( this->Id(), torque );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a force at a given position.

    @param force    new force to be added
    @param position position of the force

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddForceAtPosition( const vector3& force, const vector3& position )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phyAddForceAtPosition( this->Id(), force, position );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a relative force at a given position.

    @param force    new force to be added
    @param position position of the force

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddRelativeForceAtPosition( const vector3& force, const vector3& position )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phyAddRelativeForceAtPosition( this->Id(), force, position );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Sets the absolute force applied to the object.

    @param force    new force

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetForce( const vector3& force )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phySetForce( this->Id(), force );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a relative force at a relative position.

    @param force    new force

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddRelativeForceAtRelativePosition( const vector3& force, const vector3& position )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phyAddRelativeForceAtRelativePosition( this->Id(), force, position );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Adds a force at a relative position.

    @param force    new force

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::AddForceAtRelativePosition( const vector3& force, const vector3& position )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phyAddForceAtRelativePosition( this->Id(), force, position );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Sets the absolute torque applied to the object.

    @param torque    new torque

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetTorque( const vector3& torque )
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phySetForce( this->Id(), torque );

    this->Enable();
}

//-----------------------------------------------------------------------------
/**
    Gets the absolute force applied to the object.

    @param force    rigid body absolute force

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::GetForce( vector3& force ) const
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phyGetForce( this->Id(), force );
}

//-----------------------------------------------------------------------------
/**
    Gets the absolute torque applied to the object.

    @param torque    rigid body absolute torque

    history:
        - 22-Oct-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::GetTorque( vector3& torque ) const
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
    
    phyGetTorque( this->Id(), torque );
}

//-----------------------------------------------------------------------------
/**
    Returns if a body it's enabled.

    @return a boolean

    history:
        - 29-Nov-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
bool nPhyRigidBody::IsEnabled() const
{
    n_assert2( this->Id() != NoValidID, "No valid rigid body" );
 
    return phyIsEnabled( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Set the physics obj.

    @param obj   physics object

    history:
        - 30-Nov-2004   Zombie         created
        - 31-Jan-2005   Zombie         inlined
*/
inline
void nPhyRigidBody::SetPhysicsObj( ncPhysicsObj* obj )
{
    this->physicsObj = obj;
}

//-----------------------------------------------------------------------------
/**
    Retrieves a body linear velocity treshold for auto-disable.

    @return speed threshold

    history:
        - 30-Nov-2004   Zombie         created
*/
inline
phyreal nPhyRigidBody::GetLinearVelocityThresholdAutoDisable()
{
    return phyGetLinearVelocityThresholdAutoDisable( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Retrieves a angular linear velocity treshold for auto-disable.

    @return speed threshold

    history:
        - 30-Nov-2004   Zombie         created
*/
inline
phyreal nPhyRigidBody::GetAngularVelocityThresholdAutoDisable()
{
    return phyGetAngularVelocityThresholdAutoDisable( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Returns if the object is to be tweaked.

    @return true/false

    history:
        - 08-Jul-2004   Zombie         created
*/
inline
bool nPhyRigidBody::GetTweaked() const
{
    return this->tweaked;
}

//-----------------------------------------------------------------------------
/**
    Sets if the body is to be tweaked.

    @param is true/false

    history:
        - 08-Jul-2004   Zombie         created
*/
inline
void nPhyRigidBody::SetTweaked( bool is )
{
    this->tweaked = is;
}

//-----------------------------------------------------------------------------
/**
    Sets the tweaked offset.

    @param offset vector direction

    history:
        - 08-Jul-2004   Zombie         created
*/
inline
void nPhyRigidBody::SetTweakedOffset( const vector3& offset )
{
    this->tweakedOffset = offset;
}

//-----------------------------------------------------------------------------
/**
    Returns the tweaked offset.

    @return vector direction

    history:
        - 08-Jul-2004   Zombie         created
*/
inline
const vector3& nPhyRigidBody::GetTweakedOffset() const
{
    return this->tweakedOffset;
}

//-----------------------------------------------------------------------------
/**
    Returns if the atmosphera afects this body.

    @return true/false

    history:
        - 10-Aug-2004   Zombie         created
*/
inline
const bool nPhyRigidBody::IsAtmospheraAffected() const
{
    return this->atmosphera;
}

//-----------------------------------------------------------------------------
/**
    Sets if the atmosphera afects this body.

    @param is true/false

    history:
        - 10-Aug-2004   Zombie         created
*/
inline
void nPhyRigidBody::SetIfAtmospheraAffected( const bool is )
{
    this->atmosphera = is;
}

#endif