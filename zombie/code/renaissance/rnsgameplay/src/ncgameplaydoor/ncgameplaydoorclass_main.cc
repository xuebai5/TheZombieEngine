//-----------------------------------------------------------------------------
//  ncgameplaydoorclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchrnsgameplay.h"

#include "ncgameplaydoor/ncgameplaydoorclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncGamePlayDoorClass,nComponentClass);

//-----------------------------------------------------------------------------
namespace
{
    const float pi_value(float(3.14159265358979323846));
}

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 06-Dec-2005   David Reyes    created
*/
ncGamePlayDoorClass::ncGamePlayDoorClass() :
    doorType( hinge ),
    attributes( closesalone ),
    doorAngles(float(pi_value/2.), 0),
    slidingSpeed(float(.1)),
    sliddingMax(5),
    waitingTime(1),
    angleSpeed(float(.1)),
    connectToPortal(false)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 06-Dec-2005   David Reyes    created
*/
ncGamePlayDoorClass::~ncGamePlayDoorClass()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Sets the the door max angle

    @param value angle

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetMaxAngle( const float value )
{
    this->doorAngles.theta = value;
}

//-----------------------------------------------------------------------------
/**
    Sets the the door min angle

    @param value angle

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetMinAngle( const float value )
{
    this->doorAngles.rho = value;
}

//-----------------------------------------------------------------------------
/**
    Gets the the door max angle

    @return angle

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetMaxAngle() const
{
    return this->doorAngles.theta;
}

//-----------------------------------------------------------------------------
/**
    Gets the the door min angle

    @return angle

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetMinAngle() const
{
    return this->doorAngles.rho;
}

//-----------------------------------------------------------------------------
/**
    Returns the door type

    @return door type

    history:
        - 06-Dec-2005   David Reyes    created
*/
const ncGamePlayDoorClass::type ncGamePlayDoorClass::GetType() const
{
    return this->doorType;
}

//-----------------------------------------------------------------------------
/**
    Sets the door type

    @param newtype new door's type

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetType( const type newtype )
{
    this->doorType = newtype;
}

//-----------------------------------------------------------------------------
/**
    Gets doors attributes

    @return door's attributes

    history:
        - 06-Dec-2005   David Reyes    created
*/
const int ncGamePlayDoorClass::GetAttributes() const
{
    return this->attributes;
}

//-----------------------------------------------------------------------------
/**
    Sets attribs to the door

    @param newattributes new door's attributes set

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetAttributes( const int newattributes )
{
    this->attributes = newattributes;
}

//-----------------------------------------------------------------------------
/**
    Gets the openning angle

    @return angle in degrees

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetOpeningAngle() const
{
    float angle( this->GetMaxAngle() == 0 ? this->GetMinAngle() : this->GetMaxAngle() );

    // convert from radians to degree
    return angle * float(360)/(float(2) * float(pi_value));
}

//-----------------------------------------------------------------------------
/**
    Sets the openning angle

    @param angle angle in degrees

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetOpeningAngle( const float angle )
{
    n_assert2( angle != 0, "Angle has to different of zero." );

    float radians( (angle / float(360)) * (float(2) * float(pi_value)) );

    if( radians > 0 )
    {
        this->SetMaxAngle( radians );
        this->SetMinAngle( 0 );
    }
    else
    {
        this->SetMinAngle( radians );
        this->SetMaxAngle( 0 );
    }
}

//-----------------------------------------------------------------------------
/**
    Gets the slidding speed

    @return speed in space game units per second

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetSliddingSpeed() const
{
    return this->slidingSpeed;
}

//-----------------------------------------------------------------------------
/**
    Sets the slidding speed

    @param speed speed in space game units per second

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetSliddingSpeed( const float speed )
{
    n_assert2( speed != 0, "Speed has to be different of zero." );

    this->slidingSpeed = speed;
}

//-----------------------------------------------------------------------------
/**
    Gets the slidding lenght

    @return lenght in game units

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetSliddingLenght() const
{
    return this->sliddingMax;
}

//-----------------------------------------------------------------------------
/**
    Sets the slidding lenght

    @param lenght lenght in game units

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetSliddingLenght( const float length )
{
    this->sliddingMax = length;
}

//-----------------------------------------------------------------------------
/**
    Gets the waiting time b4 closing

    @return time in seconds

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetWaitingTime() const
{
    return this->waitingTime;
}

//-----------------------------------------------------------------------------
/**
    Sets the waiting time b4 closing

    @param time time in seconds

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetWaitingTime( const float time )
{
    this->waitingTime = time;
}

//-----------------------------------------------------------------------------
/**
    Gets the angle speed

    @return angle speed, degrees per second

    history:
        - 06-Dec-2005   David Reyes    created
*/
const float ncGamePlayDoorClass::GetAngleSpeed() const
{
    return this->angleSpeed;
}

//-----------------------------------------------------------------------------
/**
    Sets the angle speed

    @param speed angle speed, degrees per second

    history:
        - 06-Dec-2005   David Reyes    created
*/
void ncGamePlayDoorClass::SetAngleSpeed( const float speed )
{
    n_assert2( speed != 0, "Speed can't be zero." );

    this->angleSpeed = speed;
}

//-----------------------------------------------------------------------------
/**
    Set the connect to spatial portal flag

    history:
        -17-Mar-2006    Miquel Angel Rujula    created
*/
void ncGamePlayDoorClass::SetConnectToPortal(bool flag)
{
    this->connectToPortal = flag;
}

//-----------------------------------------------------------------------------
/**
    Get the connect to spatial portal flag

    history:
        -17-Mar-2006    Miquel Angel Rujula    created
*/
bool ncGamePlayDoorClass::GetConnectToPortal() const
{
    return this->connectToPortal;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
