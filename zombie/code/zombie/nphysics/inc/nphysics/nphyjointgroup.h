#ifndef N_PHYJOINTGROUP_H
#define N_PHYJOINTGROUP_H

//-----------------------------------------------------------------------------
/**
    @class nPhyJointGroup
    @ingroup NebulaPhysicsSystem
    @brief A container for joints.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsconfig.h"
#include "util/narray.h"

//-----------------------------------------------------------------------------

class nPhysicsJoint;

//-----------------------------------------------------------------------------

class nPhyJointGroup {
public:
    /// constructor
    nPhyJointGroup();

    /// destructor
    ~nPhyJointGroup();

    /// returns the ID
    jointgroupid Id() const;

    /// creates the container
    void Create();

    /// set size of the group (b4 it's created)
    void SetSize( int size );

    /// empties the group
    void EmptyIt();

    /// adds a joint to the group
    void Add( nPhysicsJoint* joint );

    /// removes a joint of the group
    void Remove( nPhysicsJoint* joint );

    /// sets if the joints has to be destroyed
    void SetDestroyJoints( const bool destroy );

    /// sets if the joints has to be destroyed
    bool GetDestroyJoints() const;

    /// returns the number of joints in the group
    int GetNumJoints() const;

    /// returns a joint by index
    nPhysicsJoint* GetJoint( int index ) const;

private:
    /// stores the id
    jointgroupid jointgroupID;

    /// size of the contact group
    int jointGroupSize;

    /// default size of contact group
    static const int defaultJointGroupSize = 10;

    /// joints container
    nArray< nPhysicsJoint* > jointContainer;

    /// store if the joints has to be destroyed
    bool destroyJoints;
};

//-----------------------------------------------------------------------------
/**
    Returns the ID.

    @return id to the joint group

    history:
        - 25-Sep-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
jointgroupid nPhyJointGroup::Id() const
{
    return this->jointgroupID;
}

//-----------------------------------------------------------------------------
/**
    Adds a joint to the group.

    @param joint a joint

    history:
        - 26-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyJointGroup::Add( nPhysicsJoint* joint )
{
    n_assert2( joint, "Null pointer" );

    this->jointContainer.PushBack( joint );

}

//-----------------------------------------------------------------------------
/**
    Removes a joint of the group.

    @param joint a joint

    history:
        - 12-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyJointGroup::Remove( nPhysicsJoint* joint )
{
    n_assert2( joint, "Null pointer" );

    nArray< nPhysicsJoint* >::iterator it(this->jointContainer.Find( joint ));

    this->jointContainer.Erase(it);
}

//-----------------------------------------------------------------------------
/**
    Sets if the joints has to be destroyed.

    @param destroy true/false

    history:
        - 26-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyJointGroup::SetDestroyJoints( const bool destroy )
{
    this->destroyJoints = destroy;
}

//-----------------------------------------------------------------------------
/**
    Sets if the joints has to be destroyed.

    @return true/false

    history:
        - 26-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
bool nPhyJointGroup::GetDestroyJoints() const
{
    return this->destroyJoints;
}

#endif