//-----------------------------------------------------------------------------
//  ncphyragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyragdoll.h"
#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyRagDoll,ncPhyCompositeObj);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 28-Feb-2005   David Reyes    created
*/
ncPhyRagDoll::ncPhyRagDoll() :
    containerLimbs( NumInitialLimbs, NumGrowthLimbs ),
    containerJoints( NumInitialJointLimbs, NumGrowthJointLimbs ),
    parentJoints( NumInitialJointLimbs, NumGrowthJointLimbs ),
    space(0)
{
    this->SetType( RagDoll );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 28-Feb-2005   David Reyes    created
*/
ncPhyRagDoll::~ncPhyRagDoll()
{
    this->MoveToSpace(NoValidID);

    if( this->GetSpace() )
    {
        this->GetSpace()->Release();
    }

    // removes all the joints and limbs
    for( int index(0); index < containerJoints.Size(); ++index )
    {
        containerJoints[index]->Release();
    }

    /*for( tContainerLimbs::iterator it(containerLimbs.begin());
        it != containerLimbs.end(); ++it )
    {
        /// ENTITIES: TODO
        //(*it)->Release();
    } 
    */
}

//-----------------------------------------------------------------------------
/**
    Sets the position of the rigid body

    @param newposition ragdoll's new position

    history:
        - 28-Feb-2005   David Reyes    created
*/
void ncPhyRagDoll::SetPosition( const vector3& newposition )
{
    // gets the position offset
    vector3 oldposition;

    this->GetPosition( oldposition );

    vector3 offsetPosition( newposition - oldposition );

    // updates all the limbs
    for( int index(0); index < containerLimbs.Size(); ++index )        
    {
        containerLimbs.GetElementAt(index)->GetPosition( oldposition );
        containerLimbs.GetElementAt(index)->SetPosition( oldposition + offsetPosition );
    }
    
    // updates itself position
    ncPhyCompositeObj::SetPosition( newposition );
}

//-----------------------------------------------------------------------------
/**
    Sets the rotation of the rag-doll.

    @param newrotation ragdoll's new orientation

    history:
        - 16-Mar-2005   David Reyes    created
*/
void ncPhyRagDoll::SetRotation( const matrix33& newrotation )
{
    // updates itself position
    ncPhyCompositeObj::SetRotation( newrotation );
}

//-----------------------------------------------------------------------------
/**
    Sets the rotation of the rag-doll.

    @param ax ragdoll's new angle in the x-axis
    @param ay ragdoll's new angle in the y-axis
    @param az ragdoll's new angle in the z-axis

    history:
        - 16-Mar-2005   David Reyes    created
*/
void ncPhyRagDoll::SetRotation( phyreal ax, phyreal ay, phyreal az )
{
    // updates itself position
    ncPhyCompositeObj::SetRotation( ax, ay, az );
    
}

//-----------------------------------------------------------------------------
/**
    Process of creating of the object.

    @param world it's the world where the object will be come to existance

    history:
        - 28-Feb-2005   David Reyes    created
*/
void ncPhyRagDoll::Create( nPhysicsWorld* world )
{
    // creating the object in the world
    ncPhysicsObj::Create( world );
}

//-----------------------------------------------------------------------------
/**
    Add geometry to the body.

    @param geometry geometry composing the main body

    history:
        - 28-Feb-2005   David Reyes    created
*/
void ncPhyRagDoll::AddGeometryToMainBody( nPhysicsGeom* geometry ) 
{
    n_assert2( geometry, "Null pointer" );

    // compositeobject call
    this->Add( geometry );
}

//-----------------------------------------------------------------------------
/**
    Create limb joint.

    @param limbjoint joint
    @param parent parent joint

    history:
        - 28-Feb-2005   David Reyes    created
*/
void ncPhyRagDoll::AddLimbJoint( nPhysicsJoint* limbjoint, nPhysicsJoint* parent )
{
    n_assert2( limbjoint, "Null pointer" );

    this->containerJoints.PushBack( limbjoint );
    this->parentJoints.Add( int(reinterpret_cast<size_t>(limbjoint)), parent );
}

//-----------------------------------------------------------------------------
/**
    Adds limb to the hierarchy.

    @param limb ragdoll limb

    history:
        - 28-Feb-2005   David Reyes    created
*/
void ncPhyRagDoll::AddLimb( ncPhyRagDollLimb* limb )
{
    n_assert2( limb, "Null pointer" );

    containerLimbs.Add( limb->GetLimbId(), limb );
}

//-----------------------------------------------------------------------------
/**
    Breaks a limb joint.

    @param limb limb id

    history:
        - 28-Feb-2005   David Reyes    created
*/
void ncPhyRagDoll::BreakLimbJoint( const ncPhyRagDollLimb::rdId limb )
{
    nPhysicsJoint* it(*containerJoints.Find( reinterpret_cast<nPhysicsJoint*>(size_t(limb)) ));

    n_assert2( it, "Limb Joint id not found" );

    it->ForceBreak();
}

//-----------------------------------------------------------------------------
/**
    Enables the physic object.

    history:
        - 14-Mar-2005   David Reyes    created
*/
void ncPhyRagDoll::Enable()
{
    /// enables all the limbs an itself
    for( int index(0); index < containerLimbs.Size(); ++index )
    {
        containerLimbs.GetElementAt(index)->Enable();
    }

    ncPhyCompositeObj::Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the physic object.

    history:
        - 14-Mar-2005   David Reyes    created
*/
void ncPhyRagDoll::Disable()
{
    /// enables all the limbs an itself
    for( int index(0); index < containerLimbs.Size(); ++index )
    {
        containerLimbs.GetElementAt(index)->Disable();
    }

    ncPhyCompositeObj::Disable();
}


//-----------------------------------------------------------------------------
/**
    Returns the rag-doll space.

    @return space

    history:
        - 09-Mar-2005   David Reyes    created
*/
nPhySpace* ncPhyRagDoll::GetSpace() const
{
    return this->space;
}

//-----------------------------------------------------------------------------
/**
    Creates the rag-doll space.

    history:
        - 09-Mar-2005   David Reyes    created
*/
void ncPhyRagDoll::CreateSpace()
{
    this->space = static_cast<nPhyGeomSpace*>(nKernelServer::Instance()->New("nphygeomhashspace"));

    n_assert2( space, "Failed to create an space for the rag-doll" );
}

//-----------------------------------------------------------------------------
/**
    Removes a limb to the hierarchy.

    @param limb ragdoll limb

    history:
        - 30-Mar-2005   David Reyes    created
*/
void ncPhyRagDoll::RemoveLimb( ncPhyRagDollLimb* limb )
{
    this->containerLimbs.Rem( limb->GetLimbId() );
}


//-----------------------------------------------------------------------------
/**
    Accessor to the joints container.

*/
const ncPhyRagDoll::tContainerJoints& ncPhyRagDoll::GetJointsContainer() const
{
    return containerJoints;
}

//-----------------------------------------------------------------------------
/**
    Returns the parent joint.

    @param joint a joint

    history:
        - 14-Apr-2005   David Reyes    created
*/
nPhysicsJoint* ncPhyRagDoll::GetParentJoint( const nPhysicsJoint* joint )
{
    n_assert2( joint, "Null pointer." );

    nPhysicsJoint *it;
    
    if( !this->parentJoints.Find( int(reinterpret_cast<size_t>(joint)), it ) )
        return 0;

    return it;
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyRagDoll::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhyCompositeObj::InitInstance(initType);
}

//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   David Reyes    created
*/
void ncPhyRagDoll::Scale( const phyreal factor )
{
    /// scales main body
    ncPhyCompositeObj::Scale( factor );

    /// scales limbs
    for( int index(0); index < containerLimbs.Size(); ++index )
    {
        containerLimbs.GetElementAt(index)->Scale( factor );
    }

    /// scales anchor points
    for( int index(0); index < containerJoints.Size(); ++index )
    {
        containerJoints[index]->Scale( factor );
    }
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
	Moves the object to limbo.

    history:
     - 24-Oct-2005   David Reyes    created
*/
void ncPhyRagDoll::YouShallDwellIntoTheLimbo()
{
    // sending limbs to the limbo
    for( int index(0); index < containerLimbs.Size(); ++index )
    {
        containerLimbs.GetElementAt(index)->GetEntityObject()->SendToLimbo( containerLimbs.GetElementAt(index)->GetEntityObject() );
    }

    ncPhyCompositeObj::YouShallDwellIntoTheLimbo();

}

//------------------------------------------------------------------------------
/**
	Recovers an object from the limbo.

    history:
     - 24-Oct-2005   David Reyes    created
*/
void ncPhyRagDoll::YourSoulMayComeBackFromLimbo()
{
    // sending back limbs from limbo
    for( int index(0); index < containerLimbs.Size(); ++index )
    {
        containerLimbs.GetElementAt(index)->GetEntityObject()->ReturnFromLimbo( containerLimbs.GetElementAt(index)->GetEntityObject() );
    }

    ncPhyCompositeObj::YourSoulMayComeBackFromLimbo();
}

#endif // !NGAME

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
