#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdollclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncragdoll/ncragdollclass.h"

//-----------------------------------------------------------------------------
/**
*/
ncRagDollClass::ncRagDollClass() :
    ragdollJoints(0,2),
    lodRagCorrespondence(0,2),
    ragDollType(INVALIDTYPE)
{
    // empy
}

//-----------------------------------------------------------------------------
/**
*/
ncRagDollClass::~ncRagDollClass()
{
    for (int i=0; i<this->lodRagCorrespondence.Size(); i++)
    {
        n_delete(this->lodRagCorrespondence[i]);
    }
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this class.
*/
bool
ncRagDollClass::LoadResources()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
ncRagDollClass::UnloadResources()
{
    //empty
}

//------------------------------------------------------------------------------
/**
    joint is from gfx skeleton, radolljointIndex is physics ragdoll skeleton
*/
void
ncRagDollClass::SetRagdollJoint(int jointIndex, int ragdollJointIndex)
{
    this->ragdollJoints.Add(jointIndex, ragdollJointIndex);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::GetRagdollJoint(int jointIndex)
{
    int ragdollJointIndex;
    if (this->ragdollJoints.Find(jointIndex, ragdollJointIndex))
    {
        return ragdollJointIndex;
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
nKeyArray<int>&
ncRagDollClass::GetRagdollJointsArray()
{
    return this->ragdollJoints;
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::GetNumRagdollJoints() const
{
    return this->ragdollJoints.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
ncRagDollClass::SetJointCorrespondence(int lodLevel, int lodJointIndex, int ragdollJointIndex)
{
    if (this->lodRagCorrespondence.Size() < (lodLevel+1))
    {
        this->lodRagCorrespondence.Reserve(1);
        this->lodRagCorrespondence.Set(lodLevel, n_new(nRagCorrespondence()));
    }

    this->lodRagCorrespondence[lodLevel]->Add(lodJointIndex, ragdollJointIndex);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::GetGfxLodJointCorrespondence(int lodLevel, int ragdollJointIndex)
{
    return this->lodRagCorrespondence[lodLevel]->GetLodCorrespondence(ragdollJointIndex);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::GetGfxRagJointCorrespondence(int lodLevel, int lodJointIndex)
{
    return this->lodRagCorrespondence[lodLevel]->GetRagCorrespondence(lodJointIndex);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::GetGfxLodJointCorrespondenceByIndex(int lodLevel, int index)
{
    return this->lodRagCorrespondence[lodLevel]->GetLodCorrespondenceByIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::GetGfxRagJointCorrespondenceByIndex(int lodLevel, int index)
{
    return this->lodRagCorrespondence[lodLevel]->GetRagCorrespondenceByIndex(index);
}


//------------------------------------------------------------------------------
/**
*/
ncRagDollClass::nRagCorrespondence::nRagCorrespondence() :
    ragCorrespondence(0,2)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
ncRagDollClass::nRagCorrespondence::~nRagCorrespondence()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncRagDollClass::nRagCorrespondence::Add(int lodJoint, int ragJoint)
{
    this->ragCorrespondence.Add(lodJoint,ragJoint);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::nRagCorrespondence::GetRagCorrespondence(int lodJoint)
{ 
    int ragJoint;
    if (this->ragCorrespondence.Find(lodJoint, ragJoint))
    {
        return ragJoint;
    }
    else 
    {
        return -1;
    }
}

//------------------------------------------------------------------------------
/**
*/
#ifdef __ZOMBIE_EXPORTER__
bool
ncRagDollClass::CleanData()
{
    ncSkeletonClass::CleanData();

    this->ragdollJoints.Clear();
    
    for (int i=0; i<this->lodRagCorrespondence.Size(); i++)
    {
        n_delete(this->lodRagCorrespondence[i]);
    }
    this->lodRagCorrespondence.Clear();

    return true;
}
#endif

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::nRagCorrespondence::GetLodCorrespondence(int ragJoint)
{
    for (int i=0; i< this->ragCorrespondence.Size(); i++)
    {
        if (this->ragCorrespondence.GetElementAt(i) == ragJoint)
        {
            return this->ragCorrespondence.GetKeyAt(i);
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::nRagCorrespondence::GetRagCorrespondenceByIndex(int index)
{ 
    return this->ragCorrespondence.GetElementAt(index);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::nRagCorrespondence::GetLodCorrespondenceByIndex(int index)
{
    return this->ragCorrespondence.GetKeyAt(index);
}

//------------------------------------------------------------------------------
/**
*/
int
ncRagDollClass::nRagCorrespondence::GetNumCorrespondences()
{
    return this->ragCorrespondence.Size();
}

//------------------------------------------------------------------------------
/**
*/
const char*
ncRagDollClass::TypeToString(RagDollType t)
{
    switch (t)
    {
        case Human:          return "human";

        default:
            n_error("ncRagDollClass::TypeToString(): invalid ragtype value '%d'!", t);
            return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
ncRagDollClass::RagDollType
ncRagDollClass::StringToType(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "human"))       return Human;

    n_error("nFxEventTrigger::StringToType(): invalid ragtype string '%s'!", str);
        return INVALIDTYPE;
}

//------------------------------------------------------------------------------
/**
*/
void
ncRagDollClass::SetRagType(const char* type)
{
    this->ragDollType = this->StringToType(type);
}

//------------------------------------------------------------------------------
/**
*/
ncRagDollClass::RagDollType
ncRagDollClass::GetRagDollType() const
{
    return this->ragDollType;
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncRagDollClass::GetRagType() const
{
    return this->TypeToString(this->ragDollType);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
