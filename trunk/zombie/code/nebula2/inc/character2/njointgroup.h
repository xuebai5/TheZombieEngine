#ifndef N_JOINTGROUP_H
#define N_JOINTGROUP_H
//------------------------------------------------------------------------------
/**
    @class nJointGroup
    @ingroup NebulaAnimationSystem
    
    @brief Group of joints that will be animated with the same anim file. Used to split character in groups.

    (C) 2005 Conjurer Services, S.A.
*/

class nJointGroup
{
public:
    /// default constructor
    nJointGroup();
    /// constructor
    nJointGroup(int numJoints);
    /// destructor
    ~nJointGroup();

    /// add joint index
    void AddJointIndex(int jointIndex);
    /// get joint indices array
    nArray<int> GetJointIndices();
    /// get joint index
    int GetJointIndexAt(int index);
    /// get group size
    int GetGroupSize();
    /// get group allocated size
    int GetGroupFinalSize();
    /// find jointIndex
    int FindJoint(int jointIndex);

private:
    /// list of ordered joint indices
    nArray<int> orderedJointIndices;
};

//------------------------------------------------------------------------------
/**
*/
inline
nJointGroup::nJointGroup() :
    orderedJointIndices(0,2)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nJointGroup::nJointGroup(int numJoints):
    orderedJointIndices(numJoints, 2)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nJointGroup::~nJointGroup()
{
    //empty
}

//------------------------------------------------------------------------------
/**
    inserts jointIndex to the appropiate place, ordered jointIndices
*/
inline
void
nJointGroup::AddJointIndex(int jointIndex)
{
    // find where to insert new jointIndex
    int index=0;

    for (; ((index<this->orderedJointIndices.Size()) && (this->orderedJointIndices[index]< jointIndex)); index++);

    // insert
    this->orderedJointIndices.Insert(index, jointIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<int>
nJointGroup::GetJointIndices()
{
    return this->orderedJointIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nJointGroup::GetJointIndexAt(int index)
{
    return this->orderedJointIndices[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nJointGroup::GetGroupSize()
{
    return this->orderedJointIndices.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nJointGroup::GetGroupFinalSize()
{
    return this->orderedJointIndices.AllocSize();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nJointGroup::FindJoint(int jointIndex)
{
    return this->orderedJointIndices.FindIndex(jointIndex);
}

//------------------------------------------------------------------------------
#endif
