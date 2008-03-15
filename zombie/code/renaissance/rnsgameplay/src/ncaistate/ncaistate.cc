#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncaistate.cc
//------------------------------------------------------------------------------

#include "ncaistate/ncaistate.h"
#include "entity/nentityobjectserver.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncAIState,nComponentObject);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncAIState::ncAIState() : 
    target(0), 
    activeWeapon(0),
    abandonCurrentTarget(false),
    callingAlly(0),
    fightRingIdx(-1),
    leapSuccess(false)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncAIState::~ncAIState()
{
    this->SetTarget(0);
}

//------------------------------------------------------------------------------
/**
*/
void
ncAIState::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType == nObject::ReloadedInstance)
    {
        this->Reset();
    }
}

//------------------------------------------------------------------------------
/**
    SetTarget
*/
void
ncAIState::SetTarget (nEntityObject* target)
{
    this->target = target;
}

//------------------------------------------------------------------------------
/**
    SetTargetID
*/
void
ncAIState::SetTargetID (nEntityObjectId idTarget)
{
    nEntityObjectServer* entityServer = nEntityObjectServer::Instance();
    nEntityObject* target = 0;
    n_assert(entityServer);

    if ( entityServer && idTarget != 0 )
    {
        target = entityServer->GetEntityObject (idTarget);
    }

    this->SetTarget(target);
}

//------------------------------------------------------------------------------
/**
    GetTargetID
*/
nEntityObjectId
ncAIState::GetTargetID() const
{
    nEntityObjectId idTarget = 0;

    if ( this->target )
    {
        idTarget = this->target->GetId();
    }

    return idTarget;
}

//------------------------------------------------------------------------------
/**
    SetActiveWeapon
*/
void
ncAIState::SetActiveWeapon (nEntityObjectId idWeapon)
{
    nEntityObjectServer* entityServer = nEntityObjectServer::Instance();
    nEntityObject* weapon = 0;
    n_assert(entityServer);

    if ( entityServer && idWeapon != 0 )
    {
        weapon = entityServer->GetEntityObject (idWeapon);
    }

    this->activeWeapon = weapon;
}

//------------------------------------------------------------------------------
/**
    GetActiveWeaponID
*/
nEntityObjectId
ncAIState::GetActiveWeaponID() const
{
    nEntityObjectId idWeapon = 0;

    if ( this->activeWeapon )
    {
        idWeapon = this->activeWeapon->GetId();
    }

    return idWeapon;
}

//-----------------------------------------------------------------------------
/**
    Clear all dynamic data and references, setting the agent in an initial state
*/
void
ncAIState::Reset()
{
    this->SetTarget(0);
    this->activeWeapon = 0;
    this->abandonCurrentTarget = false;
    this->callingAlly = 0;
    this->fightRingIdx = -1;
    this->leapSuccess = false;
}

//------------------------------------------------------------------------------
/**
    Set the calling ally id
*/
void
ncAIState::SetCallingAllyId(nEntityObjectId id)
{
    if ( id == nEntityObjectServer::IDINVALID )
    {
        this->callingAlly = 0;
    }
    else
    {
        this->callingAlly = nEntityObjectServer::Instance()->GetEntityObject(id);
    }
}

//------------------------------------------------------------------------------
/**
    Get the calling ally id
*/
nEntityObjectId
ncAIState::GetCallingAllyId() const
{
    if (this->callingAlly)
    {
        return this->callingAlly->GetId();
    }
    else
    {
        return nEntityObjectServer::IDINVALID;
    }
}

//------------------------------------------------------------------------------
/**
    Says if there is a calling ally
*/
bool 
ncAIState::ThereIsCallingAlly() const
{
    return this->GetCallingAllyId() != nEntityObjectServer::IDINVALID;
}
