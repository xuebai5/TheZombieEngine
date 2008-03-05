#ifndef NCGAMEPLAYDOOR_H
#define NCGAMEPLAYDOOR_H

//-----------------------------------------------------------------------------
/**
    @class ncGamePlayDoor
    @brief Game Play Door component, it creates a within game door

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Game Play Door
    
    @cppclass ncGamePlayDoor
    
    @superclass ncGameplay

    @classinfo Game Play Door component, it creates a within game door.
*/    

//-----------------------------------------------------------------------------

#include "mathlib/polar.h"

#include "rnsgameplay/ncgameplay.h"

#include "ncgameplaydoor/ncgameplaydoorclass.h"

#include "entity/nentity.h"

//-----------------------------------------------------------------------------

class nPhyHingeJoint;
class ncSpatialPortal;

//-----------------------------------------------------------------------------

class ncGamePlayDoor : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGamePlayDoor,ncGameplay);

private:
    /// types of door state
    typedef enum {
        none,
        openning,
        waiting,
        clossing,
        open,
        close,
        locked,
        broken
    } state;

public:
    /// returns the door type
    const ncGamePlayDoorClass::type GetType() const;

    /// sets the door type
    void SetType(const ncGamePlayDoorClass::type);

    /// gets doors attributes
    const int GetAttributes() const;

    /// sets attribs to the door
    void SetAttributes(const int);

    /// returns if the door is locked
    const bool IsLocked() const;

    /// locks the door
    const bool Lock();

    /// unlocks the door
    const bool Unlock();

    /// opens the door
    bool Open();

    /// close door
    bool Close();

    /// returns if the door is close
    const bool IsClosed() const;
    /// returns if the door is open
    const bool IsOpened() const;
    
    /// constructor
    ncGamePlayDoor();

    /// destructor
    ~ncGamePlayDoor();

    /// returns if the door is broken
    const bool IsBroken() const;

    /// sets the door entity A
    void SetDoorEntityA(nEntityObject*);

    /// gets the door entity A
    nEntityObject* GetDoorEntityA() const;

    /// sets the door entity B
    void SetDoorEntityB(nEntityObject*);

    /// gets the door entity B
    nEntityObject* GetDoorEntityB() const;

    /// mounts the door
    void Mount();

    /// gets the slidding speed
    const float GetSliddingSpeed() const;

    /// sets the slidding speed
    void SetSliddingSpeed(const float);

    /// gets the slidding lenght
    const float GetSliddingLenght() const;

    /// sets the slidding lenght
    void SetSliddingLenght(const float);

    /// gets the angle speed
    const float GetAngleSpeed() const;

    /// sets the angle speed
    void SetAngleSpeed(const float);

    /// gets the openning angle
    const float GetOpeningAngle() const;

    /// sets the openning angle
    void SetOpeningAngle(const float);

    /// gets the waiting time b4 closing
    const float GetWaitingTime() const;

    /// sets the waiting time b4 closing
    void SetWaitingTime(const float);

    /// unmonts the door
    void UnMount();

    /// searches the corresponding spatial portal to this door
    bool ConnectToPortal();

    /// Update the portal state
    void UpdatePortal();

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// sets the the door max angle
    void SetMaxAngle( const float value );
    
    /// sets the the door min angle
    void SetMinAngle( const float value );

    /// gets the the door max angle
    const float GetMaxAngle() const;
    
    /// gets the the door min angle
    const float GetMinAngle() const;

    /// do action
    const bool DoAction(const float);

    /// changes the state
    void SetNewState(const int);
    
    /// runs the gameplay logic 
    void Run (const float deltaTime);

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

private:

    /// stores the door state
    state doorState;

    /// door's type
    ncGamePlayDoorClass::type doorType;

    /// stores the door's attributes
    int attributes;

    /// stores the object representing one of the doors if double
    nEntityObject* objectDoorA;

    /// stores the object representing one of the doors if double
    nEntityObject* objectDoorB;

    /// stores the join for the object A if any
    nPhyHingeJoint* jointA;

    /// stores the join for the object A if any
    nPhyHingeJoint* jointB;

    /// mounts slidding doors
    void MountSlidding();

    /// mounts hinge doors
    void MountHinge();

    /// do action for slidding
    const bool DoSliddingAction( const nTime& time );

    /// do action hinge
    const bool DoHingeAction( const nTime& time );

    /// restores the door stat after loading
    void Restore();

    /// fakes for plugs (temporary)
    nRef<nEntityObject> FakeA;

    /// fakes for plugs (temporary)
    nRef<nEntityObject> FakeB;

    /// plugs the fakes
    void PlugFakes();

    /// plugs a fake
    void PlugFake( nEntityObject*& fake, nEntityObject* to );

    /// destroys dynamic data
    void Destroy();

    /// stores the angles for hinge doors
    polar2 doorAngles;

    /// stores if the object has been mount
    bool mounted;

    /// stores the sliding speed in game units per second
    float slidingSpeed;

    /// stores how much sliding done
    float slidingSpace;

    /// sotres max slidding
    float sliddingMax;

    /// stores the original position of the doorA
    vector3 originalPositionA;

    /// stores the original position of the doorA
    vector3 originalPositionB;

    /// stores the original matrix of the doorA
    matrix33 originalMatrixA;

    /// stores the original matrix of the doorA
    matrix33 originalMatrixB;

    /// pivot door points
    vector3 pivotA;

    /// pivot door points
    vector3 pivotB;

    /// spatial portal
    ncSpatialPortal* portal;

    /// stores the original state
    void OriginalState();

    /// stores the time to close/open the door in miliseconds
    float timeToDoAction;

    /// does some precomputation
    void PreCompute();

    /// stores the time elapsed
    nTime timeElapsed;

    /// stores the direction of opening
    float openingAngle;

    /// hinge openning action
    const bool DoHingeOpenningAction();

    /// hinge closenning action
    const bool DoHingeClosenningAction();

    /// hinge openning action
    const bool DoSliddingOpenningAction();

    /// hinge closenning action
    const bool DoSliddingClosenningAction();

    /// waits for a time
    const bool DoWaiting();

    /// time to wait to star closing the door
    float waitingTime;

    /// mounts a free door
    void MountFreeDoor();

    /// stores the speed in grads per unit of time
    float angleSpeed;

    /// stores last time incremenet
    float lastTimeIncrement;

    /// stores the angle done already
    float angleDone;

    /// frame proportion
    float frameProportion;

    /// slides an object with collision
    const bool Slide( nEntityObject* object, const vector3& axis, const vector3& from );

    /// mounts a hinge door
    void MountHingeDoor();

    /// retrieves the doors from the superentity component
    void RetrieveDoors();

    /// decides what to do with the door
    void ProcessDoor();

};
    
//-----------------------------------------------------------------------------

#endif // NCGAMEPLAYDOOR_H