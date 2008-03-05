#include "precompiled/pchnanimation.h"

#include "ncragdoll/ncragdoll.h"

#include "animcomp/nccharacter.h"

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 08-Nov-2005   David Reyes    created
*/
ncRagDoll::ncRagDoll() :
    character(0),
    ragAnimator(0),
    active(false)
{
    // empy
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 08-Nov-2005   David Reyes    created
*/
ncRagDoll::~ncRagDoll()
{
    // empy
}

//-----------------------------------------------------------------------------
/**
    Switches off the ragdoll.

    history:
        - 08-Nov-2005   David Reyes    created
*/
void ncRagDoll::SwitchOff()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncRagDoll::IsValid()
{
    return (this->character->GetSkeleton().GetNumJoints() != 0);
}

//------------------------------------------------------------------------------
/**
*/
void
ncRagDoll::SetCharacter(nCharacter2* curCharacter)
{
    this->character = curCharacter;
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2*
ncRagDoll::GetCharacter()
{
    return this->character;
}

//------------------------------------------------------------------------------
/**
*/
void
ncRagDoll::SetJointData(int /*jointIndex*/, vector3& /*midPos*/, vector3& /*pos*/, const quaternion& /*rot*/, const vector3& /*scale*/)
{
    //empty
}

//------------------------------------------------------------------------------
/**
    get data from physics
*/
void
ncRagDoll::GetJointData(int jointIndex, vector3& pos, quaternion& rot, vector3& scale)
{
    nCharJoint& charJoint = this->character->GetSkeleton().GetJointAt(jointIndex);
    pos = charJoint.GetTranslate();
    rot = charJoint.GetRotate();
    scale = charJoint.GetScale();
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDoll::GetNumRagJoints()
{
    return this->character->GetSkeleton().GetNumJoints();
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncRagDoll::Limbo()
{
    this->refEntityParentObj->GetComponentSafe<ncCharacter>()->DoEnterLimbo();
}

//------------------------------------------------------------------------------
/**
*/
void
ncRagDoll::UnLimbo()
{
    this->refEntityParentObj->GetComponentSafe<ncCharacter>()->DoExitLimbo();
}

#endif

//------------------------------------------------------------------------------
/**
*/
void
ncRagDoll::SetParentObject(nEntityObject* eobj)
{
    this->refEntityParentObj = eobj;
}

//------------------------------------------------------------------------------
/**
*/
nEntityObject* 
ncRagDoll::GetParentObject() const
{
    if (this->refEntityParentObj.isvalid())
    {
        return this->refEntityParentObj.get();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncRagDoll::CreateRagdoll()
{
    //empty
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
