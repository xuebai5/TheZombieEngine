#ifndef NCGAMEPLAYDOORCLASS_H
#define NCGAMEPLAYDOORCLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncGamePlayDoorClass
    @brief Game Play Door class component, it creates a within game door

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Game Play Door Class
    
    @cppclass ncGamePlayDoorClass
    
    @superclass nComponentObjectClass

    @classinfo Game Play Door component, it creates a within game door.
*/    

//-----------------------------------------------------------------------------

#include "mathlib/polar.h"
#include "rnsgameplay/ncgameplay.h"

//-----------------------------------------------------------------------------

class nPhyHingeJoint;

//-----------------------------------------------------------------------------

class ncGamePlayDoorClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncGamePlayDoorClass,nComponentClass);

public:
    typedef enum {
        slidding = 0,
        hinge // default
    } type;

    typedef enum {
        none = 0,
        free, // default
        closesalone
    } attrib;

    /// constructor
    ncGamePlayDoorClass();

    /// destructor
    ~ncGamePlayDoorClass();

    /// returns the door type
    const type GetType() const;

    /// sets the door type
    void SetType(const type);

    /// gets doors attributes
    const int GetAttributes() const;

    /// sets attribs to the door
    void SetAttributes(const int);

    /// gets the openning angle
    const float GetOpeningAngle() const;

    /// sets the openning angle
    void SetOpeningAngle(const float);

    /// gets the slidding speed
    const float GetSliddingSpeed() const;

    /// sets the slidding speed
    void SetSliddingSpeed(const float);

    /// gets the slidding lenght
    const float GetSliddingLenght() const;

    /// sets the slidding lenght
    void SetSliddingLenght(const float);

    /// gets the waiting time b4 closing
    const float GetWaitingTime() const;

    /// sets the waiting time b4 closing
    void SetWaitingTime(const float);

    /// gets the angle speed
    const float GetAngleSpeed() const;

    /// sets the angle speed
    void SetAngleSpeed(const float);

    /// set the connect to spatial portal flag
    void SetConnectToPortal(bool);

    /// get the connect to spatial portal flag
    bool GetConnectToPortal() const;

    /// sets the the door max angle
    void SetMaxAngle( const float value );
    
    /// sets the the door min angle
    void SetMinAngle( const float value );

    /// gets the the door max angle
    const float GetMaxAngle() const;
    
    /// gets the the door min angle
    const float GetMinAngle() const;

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

private:

    /// door's type
    type doorType;

    /// stores the door's attributes
    int attributes;

    /// stores the angles for hinge doors
    polar2 doorAngles;

    /// stores the sliding speed in game units per second
    float slidingSpeed;

    /// sotres max slidding
    float sliddingMax;

    /// time to wait to star closing the door
    float waitingTime;

    /// stores the speed in grads per unit of time
    float angleSpeed;

    /// says if the door has to try to connect to a portal when it's mounted
    bool connectToPortal;

};
    
//-----------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(ncGamePlayDoorClass::type, "i", (value = (ncGamePlayDoorClass::type) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );
N_CMDARGTYPE_NEW_TYPE(ncGamePlayDoorClass::attrib, "i", (value = (ncGamePlayDoorClass::attrib) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );

//-----------------------------------------------------------------------------

#endif // NCGAMEPLAYDOOR_H