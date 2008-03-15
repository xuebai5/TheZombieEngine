#ifndef N_AISTATE_H
#define N_AISTATE_H
//------------------------------------------------------------------------------
/**
    @class ncAIState

    @brief Stores state information for a entity
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class nPersistServer;

//------------------------------------------------------------------------------
class ncAIState : public nComponentObject
{

    NCOMPONENT_DECLARE(ncAIState,nComponentObject);

public:
    /// Constructor
    ncAIState();

    /// Destructor
    ~ncAIState();

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);
    /// Init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Set target ID
    void SetTargetID(nEntityObjectId);
    /// Get target ID
    nEntityObjectId GetTargetID() const;
    /// Get target
    nEntityObject* GetTarget() const;
    /// Tell if the agent has a target
    bool HasTarget() const;
    /// Set abandon current target
    void SetAbandonCurrentTarget(bool);
    /// Get abandon current target
    bool GetAbandonCurrentTarget() const;
    /// Set active weapon ID
    void SetActiveWeapon(nEntityObjectId);
    /// Get active weapon ID
    nEntityObjectId GetActiveWeaponID() const;
    /// Set the calling ally id
    void SetCallingAllyId(nEntityObjectId);
    /// Get the calling ally id
    nEntityObjectId GetCallingAllyId() const;
    /// Says if there is a calling ally
    bool ThereIsCallingAlly() const;
    /// Sets/gets the fight ring index
    void SetFightRingIndex(int);
    int GetFightRingIndex() const;
    /// Sets/gets leap attack success flag
    void SetLeapSuccess(bool);
    bool IsLeapSuccess() const;
    /// Set point to move
    void SetDestPoint(vector3);
    /// Get point to move
    vector3 GetDestPoint() const;
    /// Set point where target is when ring is determinated
    void SetTargetPoint(vector3);
    /// Get point where target is when ring is determinated
    vector3 GetTargetPoint() const;
    /// Set a  target
    void SetTarget (nEntityObject* target);
    /// Set active weapon
    void SetActiveWeapon (nEntityObject* weapon);
    /// Get the active weapon
    nEntityObject* GetActiveWeapon() const;

    /// Set the calling ally
    void SetCallingAlly(nEntityObject* ally);
    /// Get the calling ally
    nEntityObject* GetCallingAlly() const;

    // TODO: Remove when a system to generically store/restore the agent full state is available
    //       Meanwhile this method is just called by the AI test state to reset the agent state
    /// Clear all dynamic data and references, setting the component in an initial state
    void Reset();

private:

    /// Current target
    nEntityObject* target;

    /// Abandon current target flag
    bool abandonCurrentTarget;

    /// Current weapon
    nEntityObject* activeWeapon;

    /// calling ally
    nEntityObject* callingAlly;

    // Fight ring index the entity is in
    int fightRingIdx;

    // Leap attack success flag
    bool leapSuccess;

    // Point that the agent must move
    vector3 destPoint;

    // Point where the target is at the moment that the ring is determintaed
    vector3 targetPoint;

};

//------------------------------------------------------------------------------
/**
    SetDestPoint
*/
inline
void
ncAIState::SetDestPoint ( vector3 destPoint )
{
    this->destPoint = destPoint;
}

//------------------------------------------------------------------------------
/**
SetTargetPoint
*/
inline
void
ncAIState::SetTargetPoint ( vector3 destPoint )
{
    this->targetPoint = destPoint;
}

//------------------------------------------------------------------------------
/**
GetTargetPoint
*/
inline
vector3
ncAIState::GetTargetPoint() const
{
    return this->targetPoint;
}

//------------------------------------------------------------------------------
/**
    GetDestPoint
*/
inline
vector3
ncAIState::GetDestPoint() const
{
    return this->destPoint;
}

//------------------------------------------------------------------------------
/**
    GetTarget
*/
inline
nEntityObject*
ncAIState::GetTarget() const
{
    return this->target;
}

//------------------------------------------------------------------------------
/**
    HasTarget
*/
inline
bool
ncAIState::HasTarget() const
{
    return this->target != NULL;
}

//------------------------------------------------------------------------------
/**
    SetActiveWeapon
*/
inline
void
ncAIState::SetActiveWeapon (nEntityObject* weapon)
{
    this->activeWeapon = weapon;
}

//------------------------------------------------------------------------------
/**
    GetActiveWeapon
*/
inline
nEntityObject*
ncAIState::GetActiveWeapon() const
{
    return this->activeWeapon;
}

//------------------------------------------------------------------------------
/**
    SetAbandonCurrentTarget
*/
inline
void
ncAIState::SetAbandonCurrentTarget (bool abandonFlag)
{
    this->abandonCurrentTarget = abandonFlag;
}

//------------------------------------------------------------------------------
/**
    GetAbandonCurrentTarget
*/
inline
bool
ncAIState::GetAbandonCurrentTarget() const
{
    return this->abandonCurrentTarget;
}

//------------------------------------------------------------------------------
/**
    Set the calling ally
*/
inline
void
ncAIState::SetCallingAlly(nEntityObject* ally)
{
    this->callingAlly = ally;
}

//------------------------------------------------------------------------------
/**
    Get the calling ally
*/
inline
nEntityObject* 
ncAIState::GetCallingAlly() const
{
    return this->callingAlly;
}

//------------------------------------------------------------------------------
/**
    Set the fight ring index
*/
inline
void 
ncAIState::SetFightRingIndex(int index)
{
    this->fightRingIdx = index;
}

//------------------------------------------------------------------------------
/**
    Get the fight ring index
*/
inline
int
ncAIState::GetFightRingIndex() const
{
    return this->fightRingIdx;
}

//------------------------------------------------------------------------------
/**
    Set the leap attack success flag
*/
inline
void 
ncAIState::SetLeapSuccess(bool flag)
{
    this->leapSuccess = flag;
}

//------------------------------------------------------------------------------
/**
    Get the leap attack success flag
*/
inline
bool
ncAIState::IsLeapSuccess() const
{
    return this->leapSuccess;
}

#endif
