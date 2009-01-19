#ifndef N_CHARSKELETON_H
#define N_CHARSKELETON_H
//------------------------------------------------------------------------------
/**
    @class nCharSkeleton
    @ingroup Character

    @brief Implements a character skeleton made of nCharJoint objects.

    (C) 2002 RadonLabs GmbH
*/

#include "character2/ncharjoint.h"
#include "character2/njointgroup.h"
#include "util/nfixedarray.h"

//------------------------------------------------------------------------------
class nCharSkeleton
{
public:
    /// constructor
    nCharSkeleton();
    /// destructor
    ~nCharSkeleton();
    /// copy constructor
    nCharSkeleton(const nCharSkeleton& src);
    /// assignment operator
    void operator=(const nCharSkeleton& src);
    /// clear content
    void Clear();
    /// begin adding joints
    void BeginJoints(int num);
    /// add a joint to the joint skeleton
    void SetJoint(int index, int parentIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale, const nString& name);
    /// finish adding joints
    void EndJoints();
    /// get number of joint
    int GetNumJoints() const;
    /// get joint by index
    nCharJoint& GetJointAt(int index) const;
    /// set son joint
    void SetChildJoint(int parentIndex, int childIndex);
    /// get joint index by name
    int GetJointIndexByName(const nString& name) const;
    /// evaluate all character joints
    void Evaluate();

    /// begin joint groups
    void BeginJointGroups(int numGroups);
    /// set group number of joints
    void SetGroupNumberJoints(int groupIndex, int numJoints);
    /// set joint group
    void SetGroup(int groupIndex, int joint0, int joint1, int joint2, int joint3);
    /// end joint groups
    void EndJointGroups();
    /// set common group joints
    //void SetCommonJoint(int jointIndex);
    /// get group at
    nJointGroup& GetJointGroupAt(int groupIndex);
    /// get numberjoint groups
    int GetNumberJointGroups();
    /// get group number of curves
    int GetNumberCurvesJointGroup(int groupIndex);

private:
    /// update the parent joint pointers from their indices
    void UpdateParentJointPointers();

    nFixedArray<nCharJoint> jointArray;
    nArray<nJointGroup> jointGroupArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::Evaluate()
{
    // first pass: clear uptodate flag
    int i;
    int num = this->jointArray.Size();
    //NOTE ma.garcias- keep this commented, overriden joints were re-computed
    //for (i = 0; i < num; i++)
    //{
    //    this->jointArray[i].ClearUptodateFlag();
    //}

    // seconds pass: evaluate joints
    for (i = 0; i < num; i++)
    {
        this->jointArray[i].Evaluate();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::nCharSkeleton() :
    jointGroupArray(0,0)
//    commonJoints(0,1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Update the parent joint pointers in the joint array.
*/
inline
void
nCharSkeleton::UpdateParentJointPointers()
{
    int i;
    for (i = 0; i < this->jointArray.Size(); i++)
    {
        int parentJointIndex = this->jointArray[i].GetParentJointIndex();
        if (parentJointIndex != -1)
        {
            jointArray[i].SetParentJoint(&(this->jointArray[parentJointIndex]));
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::nCharSkeleton(const nCharSkeleton& src) :
    jointGroupArray(0,0)
{
    *this = src;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::operator=(const nCharSkeleton& src)
{
    if (&src == this) return;

    this->jointArray = src.jointArray;

    // update parent joint references 
    int i;
    for (i=0; i<jointArray.Size(); i++)
    {
        int parentJointIndex = jointArray[i].GetParentJointIndex();
        if (parentJointIndex != -1)
        {
            nCharJoint* parentJoint = &jointArray[parentJointIndex];
            jointArray[i].SetParentJoint(parentJoint);

            // add a pointer from parent to child
            this->SetChildJoint(parentJointIndex, i);
        }
    }
    
    // update jointgroup data
    this->jointGroupArray = src.jointGroupArray;

    this->UpdateParentJointPointers();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::Clear()
{
    this->jointArray.SetSize(0);
    this->jointGroupArray.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::~nCharSkeleton()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Start joint skeleton definition.

    @param  num     number of joints in skeleton
*/
inline
void
nCharSkeleton::BeginJoints(int num)
{
    n_assert(num > 0);
    this->jointArray.SetSize(num);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the skeleton.
*/
inline
void
nCharSkeleton::SetJoint(int index, int parentIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale, const nString& name)
{
    nCharJoint newJoint;
    newJoint.SetParentJointIndex(parentIndex);
    if (-1 == parentIndex)
    {
        newJoint.SetParentJoint(0);
    }
    else
    {
        newJoint.SetParentJoint(&(this->jointArray[parentIndex]));
    }
    newJoint.SetPose(poseTranslate, poseRotate, poseScale);
    newJoint.SetName(name);
    this->jointArray[index] = newJoint;
}

//------------------------------------------------------------------------------
/**
    Finish defining joints.
*/
inline
void
nCharSkeleton::EndJoints()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharSkeleton::GetNumJoints() const
{
    return this->jointArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint&
nCharSkeleton::GetJointAt(int index) const
{
    return this->jointArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharSkeleton::GetJointIndexByName(const nString& name) const
{
    int index;
    for (index = 0; index < this->jointArray.Size(); index++)
    {
        if (this->jointArray[index].GetName() == name)
        {
            return index;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nCharSkeleton::SetChildJoint(int parentIndex, int childIndex)
{
    this->jointArray[parentIndex].SetChildJointIndex(childIndex);
    this->jointArray[parentIndex].SetChildJoint(&(this->jointArray[childIndex]));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::BeginJointGroups(int numGroups)
{
    n_assert(numGroups > 0);
    this->jointGroupArray.SetFixedSize(numGroups);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::SetGroupNumberJoints(int groupIndex, int numJoints)
{
    this->jointGroupArray.Set(groupIndex, nJointGroup(numJoints));
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::SetGroup(int groupIndex, int joint0, int joint1, int joint2, int joint3)
{
    nJointGroup& jointGroup = this->jointGroupArray[groupIndex];
    
    if (jointGroup.GetGroupSize() < jointGroup.GetGroupFinalSize())
    {
        jointGroup.AddJointIndex(joint0);
    }

    if (jointGroup.GetGroupSize() < jointGroup.GetGroupFinalSize())
    {
        jointGroup.AddJointIndex(joint1);
    }

    if (jointGroup.GetGroupSize() < jointGroup.GetGroupFinalSize())
    {
        jointGroup.AddJointIndex(joint2);
    }

    if (jointGroup.GetGroupSize() < jointGroup.GetGroupFinalSize())
    {
        jointGroup.AddJointIndex(joint3);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nCharSkeleton::EndJointGroups()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nJointGroup&
nCharSkeleton::GetJointGroupAt(int groupIndex)
{
    return this->jointGroupArray[groupIndex];    
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nCharSkeleton::GetNumberJointGroups()
{
    return this->jointGroupArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nCharSkeleton::GetNumberCurvesJointGroup(int groupIndex)
{
    // each joint defines 3 curves
    return this->jointGroupArray.At(groupIndex).GetGroupSize() * 3;
}

//------------------------------------------------------------------------------
#endif
