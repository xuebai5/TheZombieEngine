#ifndef NC_PHYVEHICLE_H
#define NC_PHYVEHICLE_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyVehicle
    @ingroup NebulaPhysicsSystem
    @brief Base physic object to create a vehicle.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphycompositeobj.h"
#include "nphysics/nphytwohingejoint.h"
#include "nphysics/nphyjointgroup.h"

#include "gfx2\ncamera2.h"

//-----------------------------------------------------------------------------

class ncPhyWheel;
class ncPhyVehicleClass;
class neFakeEntity;

//-----------------------------------------------------------------------------
class ncPhyVehicle : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyVehicle,ncPhyCompositeObj);

public:
    /// constructor
    ncPhyVehicle();

    /// destructor
    virtual ~ncPhyVehicle(); // To be inheret for the game vehicle code

    /// sets the acceleration (-1..0..1)
    void SetAcceleration( const phyreal acceleration );

    /// sets the steering (-1..0..1)
    void SetSteering( const phyreal steering );
       
    /// get speed in km/h
    phyreal GetSpeed() const;

    /// set the hand brake set
    void SetHandBrake( const bool isset );

    /// returns if the hand brake is set
    const bool IsSetHandBrake() const;

    /// returns the acceleration value
    const phyreal GetAccelerateValue() const;

#ifndef NGAME
    /// Process car
    void Process( matrix44* camera, nGfxServer2* server );
#endif

    /// sets the orientation of the rigid body
    virtual void SetRotation( const matrix33& neworientation );

    /// sets the euler orientation
    virtual void SetRotation( phyreal ax, phyreal ay, phyreal az );

    /// sets the position of the vehicle
    void SetPosition( const vector3& newposition );

    /// returns the bhps per wheel
    phyreal GetBHPsPerWheel() const;

    /// scales the object
    void Scale( const phyreal factor );

    /// enables the physic object
    void Enable();

    /// disables the physic object
    void Disable();

    /// wheel definition
    class Wheel {

        public:
            /// construtor
            Wheel();

            /// construtor
            ~Wheel();

            /// sets if the wheel has direction
            void SetDirection( const bool dir );

            /// sets if the wheel has traction
            void SetTraction( const bool tract );

            /// set wheel position relative to the vehicle position
            void SetPosition( const vector3& pos );

            /// sets suspension direction
            void SetSpringDirection( const vector3& direction );

            /// set wheel axis direction
            void SetWheelAxis( const vector3& direction );

            /// set wheel object
            void SetWheelObject( ncPhyWheel* wheelobj );

            /// returns if it can be steer
            const bool IsDirection() const;

            /// returns if it has traction
            const bool HasTraction() const;

            /// returns the wheel object
            ncPhyWheel*  GetWheelObject() const;

            /// returns if the wheel it's slidding
            const bool IsSliding() const;

            /// sets if the wheels are a set of track wheels
            void SetTrackWheel( const bool is );

            /// sets the max steering value
            void SetSteeringMax( const phyreal max );

            /// attaches the wheel to the body
            void Attach( nPhyRigidBody* body );

            /// sets the joint group to be used
            void SetJointGroup( nPhyJointGroup *jg );

            /// sets the torque speed
            void SetSpeed( phyreal speed, const phyreal bhps );

            /// set the steering
            void SetSteering( const phyreal steering );

            /// sets the position given another
            void SetWorldPosition( const vector3& pos );

            /// returns the max steering for this wheel
            const phyreal GetMaxSteering() const;

            /// sets steering force
            void SetSteeringForce( const phyreal value );

            /// returns the steering force
            phyreal GetSteeringForce() const;

            /// sets the steering velocity
            void SetSteeringVelocity( phyreal value );

            /// returns the steering velocity
            phyreal GetSteeringVelocity() const;

            /// sets the suspension recover
            void SetSuspensionRecover( phyreal value );

            /// returns the suspension recover
            phyreal GetSuspensionRecover() const;

            /// sets the suspension recover
            void SetSuspensionRecoverForce( phyreal value );

            /// returns the suspension recover
            phyreal GetSuspensionRecoverForce() const;

            /// adjust rotation axis
            void AdjustRotationAxis();

            /// returns the suspension
            nPhyTwoHingeJoint& GetSuspension();

            /// scales the object
            void Scale( const phyreal factor );

            /// enables the physic object
            void Enable();

            /// disables the physic object
            void Disable();           

    private:

            /// is direction wheel
            bool direction : 1;

            /// is a traction wheel
            bool traction : 1;

            /// If it's attached
            bool attached : 1;

            /// wheel representation
            nRef<nEntityObject> wheelObject; 

            /// wheel joint
            nPhyTwoHingeJoint* suspension;

            /// joint group
            nPhyJointGroup* jointGroup;

            /// wheels position
            vector3 position;

            /// suspension direction
            vector3 suspensionDirection;

            /// wheel axis
            vector3 wheelAxis;

            /// steering maximun
            phyreal maxSteering;

            /// force when steering
            phyreal forceSteering;

            /// steering velocity
            phyreal steeringVelocity;

            /// stores the suspension recover value
            phyreal suspensionRecover;

            /// stores the suspension force recover value
            phyreal suspensionRecoverForce;
    };

    /// acces to the wheel
    Wheel* operator [] (unsigned int index) const;

    /// access to the steering wheel
    nEntityObject* GetSteeringWheel() const;

    /// returns access to the vehicle joint group
    nPhyJointGroup* GetJointGroup();

    /// process b4 run the world
    void PreProcess();

    /// process after run the world
    void PostProcess();

    /// creates the vehicle in the world
    void Create( nPhysicsWorld* world );

    /// resets the physics
    virtual void Reset();

    /// function call when an impact ocurs
    void OnImpact( const tImpactInfo& info );

    /// add a breakable part
    void BreakableAt( nPhysicsGeom* where, phyreal force /*, add signal to be send to the entity*/ );

    /// gets the accelerate ratio
    const phyreal GetAccelerateRatio() const;

    /// gets the engine resistance
    const phyreal GetEngineResitance() const;

    /// gets the steer ratio
    const phyreal GetSteerRatio() const;

    /// gets the steer recover ratio
    const phyreal GetSteerRecoverRatio() const;

    /// Sets a wheel object
    void SetWheelObject(const unsigned int wheelIndex, ncPhyWheel* wheelObject );

    /// sets the object position by scripting
    void SetPositionPhyObj( const vector3& newpos );

    /// checks if collision with other physics object
    virtual int Collide( const ncPhysicsObj* obj,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// checks if collision with a geometry
    virtual int Collide( const nPhysicsGeom* geom,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// checks if the object collides with anything in his own world.
    virtual int Collide( int numContacts, nPhyCollide::nContact* contact ) const;

    /// begin:scripting

    /// try's to unflip the vehicle
    void TryUnflip();
    /// build the vehicle
    void Assemble();
    /// set piece treshold
    void SetPieceImpact(int,phyreal);
    /// sets the object it's been load or a clone
    void SetLoadOrClone();

#ifndef NGAME
    /// returns the number of geometries
    int GetNumberPieces() const;
    /// marks a geometry
    void MarkPiece(int);
    /// unmarks a geometry
    void UnMarkPiece(int);
#endif
    
    /// end:scripting

#ifndef NGAME
#endif

    // save state of the component
    bool SaveCmds(nPersistServer *);

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// loader
    virtual void Load();

#ifndef NGAME
    /// draws the physic object
    virtual void Draw( nGfxServer2* server );

    /// moves the object to limbo
    virtual void YouShallDwellIntoTheLimbo();

    /// recovers an object from the limbo
    virtual void YourSoulMayComeBackFromLimbo();

#endif // !NGAME

private:

    /// stores a reference to the class of this component
    ncPhyVehicleClass* refClass;

    /// applies dampening
    void Dampening();

    /// applies anti-roll bars.
    void AntiRollBars();

    /// updates all vehicle wheels axis
    void UpdateWheelsAxis();

    /// checks if the vehicle it's flipped
    bool ItsFlipped() const;

    /// apply the acceleration
    void ApplyAcceleration();

    /// apply the acceleration
    void ApplySteer();

    /// list of vehicle's wheels
    Wheel* wheels;

    /// joint group for the car
    nPhyJointGroup jointGroup;

    /// if the vehicle it's functional
    bool assembled : 1;

    /// if the vehicle is breaking
    bool braking : 1;

    /// stores if the hand brake it's set
    bool handBrake : 1;

    /// forces physics action
    bool forcePhysicsAction : 1;

#ifndef NGAME
    /// sets camera respect the car position
    void SetCamera( matrix44* camera );
#endif // NGAME

    /// stores the steer command
    phyreal steerValue;

    /// stores the accelerate command
    phyreal accelerateValue;

    /// POD of breakable objects
    struct breakablePart {
        nPhysicsGeom* where;
        phyreal tresholdForce;
        /* Add  signal holder */
    };

    /// type of the container for breakable parst
    typedef nArray<breakablePart> tPartsContainer;

    /// breakable parts
    tPartsContainer vehicleParts;

    /// number of initial geometries space
    static const int NumInitialParts = 0;

    /// growth pace
    static const int NumGrowthParts = 1;

    /// helper function to get the wheel position
    const vector3 GetWheelPosition( const unsigned int index ) const;

    /// helper function to get the wheel orientation
    const vector3 GetWheelOrientation( const unsigned int index ) const;

    /// returns the brush class
    nEntityClass* GetBrushClass() const;

    /// stores the steering wheel object
    nRef<nEntityObject> steeringWheel;

    /// stores steering wheel offset
    vector3 steeringWheelOffset;

    /// stores steering wheel orientation
    matrix33 steeringWheelOrientation;

    /// creates the steering wheel object if any
    void CreateSteeringWheel();

    /// updates steering wheel
    void UpdateSteeringWheel();

    /// stores the value of acceleration to mantain
    float maintainedAcceleration;
    /// stores the value of turning to mantain
    float maintainedTurning;

    /// computes the final acceleration and turning
    void ComputeInput();

};

#endif 