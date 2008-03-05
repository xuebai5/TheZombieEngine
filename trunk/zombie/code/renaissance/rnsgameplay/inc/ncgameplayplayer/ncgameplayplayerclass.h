#ifndef NCGAMEPLAYPLAYERCLASS_H
#define NCGAMEPLAYPLAYERCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGameplayPlayer
    @ingroup Entities

    Component class with the information of a player

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncgameplayliving/ncgameplaylivingclass.h"

//------------------------------------------------------------------------------
class ncGameplayPlayerClass : public ncGameplayLivingClass
{

    NCOMPONENT_DECLARE(ncGameplayPlayerClass,ncGameplayLivingClass);

public:
    /// Constructor
    ncGameplayPlayerClass();
    /// Destructor
    ~ncGameplayPlayerClass();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);

    /// @name Script interface
    //@{
    /// set local player flag
    void SetLocalPlayer(bool);
    /// get if is local player
    bool IsLocalPlayer ()const;
    /// set vertical jump power
    void SetJumpVertical(float);
    /// get vertical jump power
    float GetJumpVertical ()const;
    /// set horizontal jump power
    void SetJumpHorizontal(float);
    /// get horizontal jump power
    float GetJumpHorizontal ()const;
    /// set time than the jump power is applied to player
    void SetJumpTime(float);
    /// get time than the jump power is applied to player
    float GetJumpTime ()const;
    /// Sets/gets the max amount of stamina
    void SetMaxStamina(int);
    int GetMaxStamina() const;
    /// Sets/gets the quantity loss of stamine when jumping
    void SetJumpStaminaLost(int);
    int GetJumpStaminaLost() const;
    /// Sets/gets the quantity loss of stamine when sprinting
    void SetSprintStaminaLost(int);
    int GetSprintStaminaLost() const;
    /// Sets/gets the time that the player is tired after use all the stamina
    void SetTiredTime(float);
    float GetTiredTime() const;
    /// Sets/gets the stamina recover rate
    void SetStaminaRecover(int);
    int GetStaminaRecover() const;
    /// Sets the player height
    void SetPlayerHeight(float);
    /// Gets the player height
    float GetPlayerHeight () const;
    /// Sets the player height
    void SetPlayerWide(float);
    /// Gets the player height
    float GetPlayerWide() const;
    /// Sets the height of the player in the stand state
    void SetStandHeight(float);
    /// Gets the height of the player in the stand state
    float GetStandHeight () const;
    /// Sets the height of the player in the crouch state
    void SetCrouchHeight(float);
    /// Gets the height of the player in the crouch state
    float GetCrouchHeight () const;
    /// Sets the height of the player in the prone state
    void SetProneHeight(float);
    /// Gets the height of the player in the prone state
    float GetProneHeight () const;
    /// Sets the lean distance for the player
    void SetLeanDistance(float);
    /// Gets the lean distance for the player
    float GetLeanDistance () const;
    //@}

private:
    bool isLocalPlayer;

    float jumpVertical;
    float jumpHorizontal;
    float jumpTime;
    float playerHeight;
    float playerWide;
    float standHeight;
    float crouchHeight;
    float proneHeight;
    float leanDistance;
    // Stamina properties
    int   maxStamina;
    int   sprintStamina;      // Stamina lost when sprinting
    int   jumpStamina;        // Stamina lost when jumping
    float tiredTime;          // The time in ticks that the player is tired after consume all the stamina
    int   staminaRecover;     // Is the amount of stamina recover per tick

};

//------------------------------------------------------------------------------
/**
SetTiredTime
*/
inline
void
ncGameplayPlayerClass::SetTiredTime (float value)
{
    this->tiredTime = value;
}

//------------------------------------------------------------------------------
/**
GetTiredTime
*/
inline
float
ncGameplayPlayerClass::GetTiredTime() const
{
    return this->tiredTime;
}
//------------------------------------------------------------------------------
/**
SetJumpStaminaLost
*/
inline
void
ncGameplayPlayerClass::SetJumpStaminaLost (int value)
{
    this->jumpStamina = value;
}

//------------------------------------------------------------------------------
/**
GetJumpStaminaLost
*/
inline
int
ncGameplayPlayerClass::GetJumpStaminaLost() const
{
    return this->jumpStamina;
}
//------------------------------------------------------------------------------
/**
SetSprintStaminaLost
*/
inline
void
ncGameplayPlayerClass::SetSprintStaminaLost (int value)
{
    this->sprintStamina = value;
}

//------------------------------------------------------------------------------
/**
GetSprintStaminaLost
*/
inline
int
ncGameplayPlayerClass::GetSprintStaminaLost() const
{
    return this->sprintStamina;
}
//------------------------------------------------------------------------------
/**
GetStamina
*/
inline
int
ncGameplayPlayerClass::GetStaminaRecover() const
{
    return this->staminaRecover;
}
//------------------------------------------------------------------------------
/**
SetStaminaRecover
*/
inline
void
ncGameplayPlayerClass::SetStaminaRecover(int staminaRecover)
{
    this->staminaRecover = staminaRecover;
}

//------------------------------------------------------------------------------
/**
SetMaxStamina
*/
inline
void
ncGameplayPlayerClass::SetMaxStamina (int stamina)
{
    this->maxStamina = stamina;
}

//------------------------------------------------------------------------------
/**
GetMaxStamina
*/
inline
int
ncGameplayPlayerClass::GetMaxStamina() const
{
    return this->maxStamina;
}
//------------------------------------------------------------------------------
/**
    @param value set the local player flag
*/
inline
void
ncGameplayPlayerClass::SetLocalPlayer( bool value )
{
    this->isLocalPlayer = value;
}

//------------------------------------------------------------------------------
/**
    @returns true if player is Local Player
*/
inline
bool
ncGameplayPlayerClass::IsLocalPlayer()const
{
    return this->isLocalPlayer;
}

//------------------------------------------------------------------------------
/**
    @param value new vertical jump power
*/
inline
void
ncGameplayPlayerClass::SetJumpVertical( float value )
{
    this->jumpVertical = value;
}

//------------------------------------------------------------------------------
/**
    @returns vertical jump power
*/
inline
float
ncGameplayPlayerClass::GetJumpVertical() const
{
    return this->jumpVertical;
}

//------------------------------------------------------------------------------
/**
    @param value new horizontal jump power
*/
inline
void
ncGameplayPlayerClass::SetJumpHorizontal( float value )
{
    this->jumpHorizontal = value;
}

//------------------------------------------------------------------------------
/**
    @returns horizontal jump power
*/
inline
float
ncGameplayPlayerClass::GetJumpHorizontal() const
{
    return this->jumpHorizontal;
}

//------------------------------------------------------------------------------
/**
    @param value new jump time
*/
inline
void
ncGameplayPlayerClass::SetJumpTime( float value )
{
    this->jumpTime = value;
}

//------------------------------------------------------------------------------
/**
    @returns jump time
*/
inline
float
ncGameplayPlayerClass::GetJumpTime() const
{
    return this->jumpTime;
}

//------------------------------------------------------------------------------
/**
    @param value new player height
*/
inline
void
ncGameplayPlayerClass::SetPlayerHeight( float value )
{
    this->playerHeight = value;
}

//------------------------------------------------------------------------------
/**
    @returns player height
*/
inline
float
ncGameplayPlayerClass::GetPlayerHeight() const
{
    return this->playerHeight;
}

//------------------------------------------------------------------------------
/**
    @param value new player wide
*/
inline
void
ncGameplayPlayerClass::SetPlayerWide( float value )
{
    this->playerWide = value;
}

//------------------------------------------------------------------------------
/**
    @returns player wide
*/
inline
float
ncGameplayPlayerClass::GetPlayerWide() const
{
    return this->playerWide;
}

//------------------------------------------------------------------------------
/**
    @param value new stand height
*/
inline
void
ncGameplayPlayerClass::SetStandHeight( float value )
{
    this->standHeight = value;
}

//------------------------------------------------------------------------------
/**
    @returns stand height
*/
inline
float
ncGameplayPlayerClass::GetStandHeight() const
{
    return this->standHeight;
}

//------------------------------------------------------------------------------
/**
    @param value new crouch height
*/
inline
void
ncGameplayPlayerClass::SetCrouchHeight( float value )
{
    this->crouchHeight = value;
}

//------------------------------------------------------------------------------
/**
    @returns crouch height
*/
inline
float
ncGameplayPlayerClass::GetCrouchHeight() const
{
    return this->crouchHeight;
}

//------------------------------------------------------------------------------
/**
    @param value new prone height
*/
inline
void
ncGameplayPlayerClass::SetProneHeight( float value )
{
    this->proneHeight = value;
}

//------------------------------------------------------------------------------
/**
    @returns prone height
*/
inline
float
ncGameplayPlayerClass::GetProneHeight() const
{
    return this->proneHeight;
}

//------------------------------------------------------------------------------
/**
    @param value new lean distance
*/
inline
void
ncGameplayPlayerClass::SetLeanDistance( float value )
{
    this->leanDistance = value;
}

//------------------------------------------------------------------------------
/**
    @returns lean distance
*/
inline
float
ncGameplayPlayerClass::GetLeanDistance() const
{
    return this->leanDistance;
}

//------------------------------------------------------------------------------
#endif//NCGAMEPLAYPLAYERCLASS_H
