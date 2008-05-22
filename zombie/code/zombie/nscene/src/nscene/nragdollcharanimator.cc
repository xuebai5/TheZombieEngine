#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nragdollcharanimator_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nragdollcharanimator.h"
#include "variable/nvariableserver.h"
#include "ncharacter/ncharacter2.h"
#include "ncragdoll/ncragdollclass.h"
#include "nphysics/ncphyfourleggedragdoll.h"
#include "nphysics/ncphyhumragdoll.h"
#include "nphysics/ncphyragdoll.h"
#include "nphysics/nphysicsjoint.h"
#include "nphysics/nphyhingejoint.h"
#include "nphysics/nphyuniversaljoint.h"
#include "kernel/nlogclass.h"

#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"

//------------------------------------------------------------------------------
/**
*/
nRagdollCharAnimator::nRagdollCharAnimator() :
    ragdollJoints(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRagdollCharAnimator::~nRagdollCharAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nRagdollCharAnimator::Animate(nCharacter2* character, nVariableContext* /*varContext*/)
{
    n_assert(this->humragdoll.isvalid() || this->fourleggedRagdoll.isvalid());

    if( this->humragdoll.isvalid())
    {
       return this->AnimateHuman(character);
    }
    if( this->fourleggedRagdoll.isvalid())
    {
        return this->Animate4Legged(character);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Joint and ragdoll corrections are specific for the initial skeleton

*/
bool
nRagdollCharAnimator::AnimateHuman(nCharacter2* character )
{
    quaternion jointcorrection( 0, 0, -0.707f, 0.707f);
    jointcorrection = quaternion(0,-0.707f,0,0.707f) * jointcorrection;

    n_assert(this->ragdollJoints->Size() > 0);
    for  ( int i = 0; i < this->ragdollJoints->Size(); i++ )
    {
        int jointIndex = this->ragdollJoints->GetKeyAt( i );
        int ragdollIndex = this->ragdollJoints->GetElementAt( i );

        //get character joint
        nCharJoint& charjoint = character->GetSkeleton().GetJointAt( jointIndex );

        if (ragdollIndex != -1)
        {
            vector3 pos, scale;
            quaternion rot;
            this->GetPhysicsWorldTransformations( ragdollIndex, pos, rot, scale );
            
            rot = rot * jointcorrection;

            charjoint.SetTranslate( pos );
            charjoint.SetRotate( rot);
            charjoint.SetScale( scale );
        }
        else //the ragdoll
        {

            ncPhyRagDoll* ragdoll = this->humragdoll.get()->GetComponentSafe< ncPhyRagDoll>();

            matrix33 orientationMatrix;
            vector3 position;

            ragdoll->GetPosition(position);
            ragdoll->GetOrientation( orientationMatrix);

            //correct body-box position bip01 is not the center of the box
            vector3 offset;


            switch( this->humragdoll->GetClassComponent<ncRagDollClass>()->GetRagDollType() )
            {
            case ncRagDollClass::Human:
                offset.set(0,0,float(-.25)); // <--- Hardcoded correction
                break;
            }

            offset = orientationMatrix * offset;
            position += offset;

            quaternion rotation = orientationMatrix.get_quaternion();
            rotation = rotation * jointcorrection;

            charjoint.SetTranslate( position );
            charjoint.SetRotate( rotation );
            charjoint.SetScale(vector3(1,1,1));
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Joint and ragdoll corrections are specific for the initial skeleton

*/
bool
nRagdollCharAnimator::Animate4Legged(nCharacter2* character )
{
    quaternion ragdollcorrection(0, 0, 0.707f, 0.707f);
    ragdollcorrection = ragdollcorrection *  quaternion( 0.707f, 0, 0, 0.707f);
    quaternion jointcorrection( 0, 0, -0.707f, 0.707f);
    jointcorrection = quaternion(0,-0.707f,0,0.707f) * jointcorrection;

    n_assert(this->ragdollJoints->Size() > 0);
    for  ( int i = 0; i < this->ragdollJoints->Size(); i++ )
    {
        int jointIndex = this->ragdollJoints->GetKeyAt( i );
        int ragdollIndex = this->ragdollJoints->GetElementAt( i );

        //get character joint
        nCharJoint& charjoint = character->GetSkeleton().GetJointAt( jointIndex );

        if (ragdollIndex != -1)
        {
            vector3 pos, scale;
            quaternion rot;
            this->GetPhysicsWorldTransformations( ragdollIndex, pos, rot, scale );
            
            rot = rot * jointcorrection;

            charjoint.SetTranslate( pos );
            charjoint.SetRotate( rot);
            charjoint.SetScale( scale );
        }
        else //the ragdoll
        {

            ncPhyRagDoll* ragdoll = this->fourleggedRagdoll.get()->GetComponentSafe< ncPhyRagDoll>();

            matrix33 orientationMatrix;
            vector3 position;

            ragdoll->GetPosition(position);
            ragdoll->GetOrientation( orientationMatrix);

            vector3 offset(0,float(-.1), 0); // <--- Hardcoded correction (Antonia)
            offset = orientationMatrix * offset;
            position += offset;

            quaternion rotation = orientationMatrix.get_quaternion();
            
            rotation = rotation * ragdollcorrection;// * quaternion(-0.707,-0.707,0,0);

            charjoint.SetTranslate( position );
            charjoint.SetRotate( rotation );
            charjoint.SetScale(vector3(1,1,1));
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nRagdollCharAnimator::GetPhysicsWorldPosition( nPhysicsJoint* ragjoint )
{
    vector3 position;
    nPhysicsJoint::JointTypes type = ragjoint->Type();
    switch (type)
    {
    case nPhysicsJoint::Hinge:
        {
            nPhyHingeJoint *hinge = static_cast<nPhyHingeJoint *>(ragjoint);

            hinge->GetAnchor( position );
            return position;
        }
    case nPhysicsJoint::Universal:
        {
            nPhyUniversalJoint *universaljoint = static_cast<nPhyUniversalJoint *>(ragjoint);

            universaljoint->GetAnchor(position);
            return position;
        }
    }
    return position;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nRagdollCharAnimator::GetPhysicsScale( nPhysicsJoint* /*ragjoint*/)
{
    return vector3(1,1,1);
}


//------------------------------------------------------------------------------
/**
*/
void
nRagdollCharAnimator::GetPhysicsWorldTransformations( int ragdollIndex, vector3 & position, quaternion & rotation, vector3 & scale )
{
    matrix33 orientation;
    nPhysicsJoint *ragjoint = 0;

    if( this->humragdoll.isvalid())
    {
        ncPhyHumRagDoll* ragdoll = this->humragdoll.get()->GetComponentSafe< ncPhyHumRagDoll>();
        ragjoint = ragdoll->GetJointsContainer()[ragdollIndex];

        switch ( ragdollIndex )
        {
        case 0:
            {
                ragdoll->GetHeadObj()->GetOrientation( orientation );
                break;
            }
        case 1:
            {
                ragdoll->GetLeftArm()->GetOrientation( orientation );
                break;
            }
        case 2:
            {
                ragdoll->GetRightArm()->GetOrientation( orientation );
                break;
            }
        case 3:
            {
                ragdoll->GetLeftForeArm()->GetOrientation( orientation );
                break;
            }
        case 4:
            {
                ragdoll->GetRightForeArm()->GetOrientation( orientation );
                break;
            }
        case 5:
            {
                ragdoll->GetLeftLeg()->GetOrientation( orientation );
                break;
            }
        case 6:
            {
                ragdoll->GetRightLeg()->GetOrientation( orientation );
                break;
            }
        case 7:
            {
                ragdoll->GetForeLeftLeg()->GetOrientation( orientation );
                break;
            }
        case 8:
            {
                ragdoll->GetForeRightLeg()->GetOrientation( orientation );
                break;
            }
        case 9:
            {
                ragdoll->GetLeftArm()->GetOrientation( orientation );
                break;
            }
        }
    }

    //separate like that because head, arm, .... won't have that names in a future
    if( this->fourleggedRagdoll.isvalid() )
    {
        ncPhyFourleggedRagDoll* ragdoll = this->fourleggedRagdoll.get()->GetComponentSafe<ncPhyFourleggedRagDoll>();
        ragjoint = ragdoll->GetJointsContainer()[ragdollIndex];

        switch ( ragdollIndex )
        {
        case 0:
            {
                ragdoll->GetHeadObj()->GetOrientation( orientation );
                break;
            }
        case 1:
            {
                ragdoll->GetLeftArm()->GetOrientation( orientation );
                break;
            }
        case 2:
            {
                ragdoll->GetRightArm()->GetOrientation( orientation );
                break;
            }
        case 3:
            {
                ragdoll->GetLeftForeArm()->GetOrientation( orientation );
                break;
            }
        case 4:
            {
                ragdoll->GetRightForeArm()->GetOrientation( orientation );
                break;
            }
        case 5:
            {
                ragdoll->GetLeftLeg()->GetOrientation( orientation );
                break;
            }
        case 6:
            {
                ragdoll->GetRightLeg()->GetOrientation( orientation );
                break;
            }
        case 7:
            {
                ragdoll->GetForeLeftLeg()->GetOrientation( orientation );
                break;
            }
        case 8:
            {
                ragdoll->GetForeRightLeg()->GetOrientation( orientation );
                break;
            }
        case 9:
            {
                ragdoll->GetLeftArm()->GetOrientation( orientation );
                break;
            }
        }
    }

    rotation = orientation.get_quaternion();
    position = this->GetPhysicsWorldPosition( ragjoint );
    scale = vector3(1,1,1);
}
