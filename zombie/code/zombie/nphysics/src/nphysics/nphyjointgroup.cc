//-----------------------------------------------------------------------------
//  nphyjointgroup_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyjointgroup.h"
#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 25-Sep-2004   David Reyes    created
*/
nPhyJointGroup::nPhyJointGroup() :
    jointgroupID(NoValidID),
    jointGroupSize(defaultJointGroupSize),
    jointContainer( defaultJointGroupSize, defaultJointGroupSize ),
    destroyJoints(false)
{
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 25-Sep-2004   David Reyes    created
        - 26-Jan-2005   David Reyes    added code to destroy the joints
*/
nPhyJointGroup::~nPhyJointGroup()
{
    if( NoValidID != this->Id() )
    {
        if( this->destroyJoints )
        {
            // destroy the joints first
            for( nArray< nPhysicsJoint* >::iterator it( this->jointContainer.Begin() );
                it != this->jointContainer.End(); it = this->jointContainer.Begin() )
            {
                    (*it)->Release();
            }
        }

        this->jointContainer.Clear();

        phyDestroyJointGroup( this->Id() );
        this->jointgroupID = NoValidID;
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the container.

    history:
        - 25-Sep-2004   David Reyes    created
*/
void nPhyJointGroup::Create()
{
    n_assert2( this->Id() == NoValidID, "It has been already created" );
    
    this->jointgroupID = phyCreateJointGroup( this->jointGroupSize );

    n_assert2( this->jointgroupID != NoValidID, "Failed to create a joint group" );
}

//-----------------------------------------------------------------------------
/**
    Set size of the group (b4 it's created).

    history:
        - 25-Sep-2004   David Reyes    created
*/
void nPhyJointGroup::SetSize( int size )
{
    n_assert2( this->Id() == NoValidID, "It has been already created" );

    n_assert2( size >= 0, "The size cannot be zero or less" );

    this->jointGroupSize = size;
}

//-----------------------------------------------------------------------------
/**
    Empties the group.

    history:
        - 26-Sep-2004   David Reyes    created
*/
void nPhyJointGroup::EmptyIt()
{
    n_assert2( this->Id() != NoValidID, "It has been already created" );

    phyEmptyJointGroup( this->Id() );

    int Size(this->jointContainer.Size());

    for( int i(0); i < Size; ++i )
    {
        this->jointContainer.At(0)->Release();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the number of joints in the group.

    @return number of joints

    history:
        - 18-Aug-2005   David Reyes    created
*/
int nPhyJointGroup::GetNumJoints() const
{
    return this->jointContainer.Size();
}

//-----------------------------------------------------------------------------
/**
    Returns a joint by index.

    @param index index to a joint in the group

    @return a joint

    history:
        - 18-Aug-2005   David Reyes    created
*/
nPhysicsJoint* nPhyJointGroup::GetJoint( int index ) const
{
    n_assert2( index >= 0, "Error index out of bounds." );
    n_assert2( index < this->GetNumJoints(), "Error index out of bounds." );

    return this->jointContainer[ index ];
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
