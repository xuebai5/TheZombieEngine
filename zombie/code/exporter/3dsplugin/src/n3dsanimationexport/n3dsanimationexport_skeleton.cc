#include "precompiled/pchn3dsmaxexport.h"
//------------------------------------------------------------------------------
#include "n3dsanimationexport/n3dsanimationexport.h"
#include "n3dsanimationexport/n3dsskeleton.h"
#include "n3dsexporters/n3dssystemcoordinates.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dslog.h"
//------------------------------------------------------------------------------
#include "animcomp/ncskeletonclass.h"
#include "ncragdoll/ncragdollclass.h"
#include "nphysics/ncphyhumragdollclass.h"
#include "nphysics/ncphyfourleggedragdollclass.h"
//------------------------------------------------------------------------------
#include "zombieentity/ncdictionaryclass.h"
#include "nspatial/ncspatialclass.h"

//------------------------------------------------------------------------------
/**
    only create neskeleton class
    object is not created, not needed
*/
void
n3dsAnimationExport::ExportSkeleton( nEntityClass * entityClass, int lodLevel, const nArray<n3dsSkeleton*>& skeletons )
{
    //set skeleton data
    n_assert(entityClass);
    ncSkeletonClass *skeletonClass = entityClass->GetComponent<ncSkeletonClass>();
    skeletonClass->CleanData();

    // set joints
    if (skeletons[lodLevel]->BonesArray.Size()>0)
	{
        skeletonClass->BeginJoints(skeletons[lodLevel]->BonesArray.Size());

        int numJoints = skeletons[lodLevel]->BonesArray.Size();
        for (int iBoneIdx= 0; iBoneIdx < numJoints; iBoneIdx++)
		{
            vector3 scale, pos;
            quaternion quat;

            // get scale, quat and pose
            skeletons[lodLevel]->BonesArray[iBoneIdx].localTr.get(scale,quat,pos);

            // update scale
            // i dunno why ( but otherwise do not work )
            if (scale.z<0)
            {
                scale.z=-scale.z;
            }      

            skeletonClass->SetJoint(iBoneIdx,
   				                    skeletons[lodLevel]->BonesArray[iBoneIdx].iParentBoneId, 
				                    pos,
                                    quat,
                                    scale );
            skeletonClass->AddJointName(iBoneIdx,skeletons[lodLevel]->BonesArray[iBoneIdx].strBoneName.Get());
		}
		skeletonClass->EndJoints();
    }

    this->CreateJointGroups(skeletonClass, lodLevel, skeletons[lodLevel]);

    // add dynamic attachment helpers
    skeletonClass->BeginAttachments(skeletons[0]->HelperArray.Size());

    for( int i=0; i< skeletons[0]->HelperArray.Size(); i++)
    {
        n3dsDynAttach helper = skeletons[0]->HelperArray[i];
        
        vector3 scale, pos;
        quaternion quat;

        // get lod 0 bone name
        nString maxLodJointName = skeletons[0]->BonesArray[helper.GetJointIndex()].strBoneName;
        // remove Bip0x from name
        nString maxJointName = maxLodJointName.ExtractRange(5, maxLodJointName.Length() - 5);
        
        int lodLevelJointIndex = skeletons[lodLevel]->FindBoneThatContains( maxJointName );
        n_assert(lodLevelJointIndex != -1 );

        this->GetPosRotScale( helper.GetTransformation(), helper.GetJointIndex(), pos, quat, scale);
        skeletonClass->SetAttachmentHelper(i, helper.GetAtName().Get(), lodLevelJointIndex, pos, quat);
    }
    skeletonClass->EndAttachments();

    //for default character visualization
    ncDictionaryClass *dictionaryClass = entityClass->GetComponent<ncDictionaryClass>();
    dictionaryClass->SetFloatVariable("one", 1);
}

//------------------------------------------------------------------------------
/**
    only create nehumragdoll class
    object is not created, not needed
*/
void
n3dsAnimationExport::ExportRagdollSkeleton( nEntityClass * entityClass, const nArray<n3dsSkeleton*>& skeletons )
{
    //set skeleton data
    n_assert(entityClass);
    ncRagDollClass *ragdollSkeletonClass = entityClass->GetComponent<ncRagDollClass>();
    ragdollSkeletonClass->CleanData();

    // set joints
    ragdollSkeletonClass->BeginJoints(skeletons[0]->RagBonesArray.Size());
    int numJoints = skeletons[0]->RagBonesArray.Size();
    for (int iBoneIdx= 0; iBoneIdx < numJoints; iBoneIdx++)
	{
        vector3 scale, pos;
        quaternion quat;

        // get scale, quat and pose
        skeletons[0]->RagBonesArray[iBoneIdx].localTr.get(scale,quat,pos);

        // update scale
        // i dunno why ( but otherwise do not work )
        if (scale.z<0)
        {
            scale.z=-scale.z;
        }      

        ragdollSkeletonClass->SetJoint(iBoneIdx,
   				                skeletons[0]->RagBonesArray[iBoneIdx].iParentBoneId, 
				                pos,
                                quat,
                                scale );
        ragdollSkeletonClass->AddJointName(iBoneIdx,skeletons[0]->RagBonesArray[iBoneIdx].strBoneName.Get());
	}
	ragdollSkeletonClass->EndJoints();

    // joint correspondence between lod skeletons and that one
    for( int ragJointIdx=0; ragJointIdx< skeletons[0]->RagCorresp.Size(); ragJointIdx++)
    {
        int maxSkeletonGfxJoint = skeletons[0]->RagCorresp[ragJointIdx];

        for( int lodLevel =0; lodLevel< skeletons.Size(); lodLevel++)
        {
            int lodJointIdx;
            nString jointName = skeletons[0]->BonesArray[maxSkeletonGfxJoint].strBoneName;
            jointName = jointName.ExtractRange(5, jointName.Length() - 5);
            jointName.Strip( "." );

            if( lodLevel == 0)
            {
                lodJointIdx = maxSkeletonGfxJoint;
            }
            else
            {
                lodJointIdx = skeletons[lodLevel]->FindBoneThatContains( jointName );
            }

            ragdollSkeletonClass->SetJointCorrespondence( lodLevel, lodJointIdx, ragJointIdx);
        }
    }


    ncPhyHumRagDollClass* phyclass = 0;
    ncPhyFourleggedRagDollClass* phy4class = 0;

    n3dsExportSettings::CritterNameType exportCritter = n3dsExportServer::Instance()->GetSettings().critterName;
    switch ( exportCritter )
    {
    case n3dsExportSettings::Human:
        phyclass = entityClass->GetComponentSafe<ncPhyHumRagDollClass>();
        phyclass->SetHumanAngles();
        ragdollSkeletonClass->SetRagType( ncRagDollClass::TypeToString(ncRagDollClass::Human) );
        this->ExportHumanRagdollCorrespondence( ragdollSkeletonClass, skeletons );
        break;
/*    case n3dsExportSettings::Scavenger:
        phy4class = entityClass->GetComponentSafe<ncPhyFourleggedRagDollClass>();
        phy4class->SetScavengerAngles();
        ragdollSkeletonClass->SetRagType( ncRagDollClass::TypeToString(ncRagDollClass::Scavenger) );
        this->ExportScvRagdollCorrespondence( ragdollSkeletonClass, skeletons );
        break;
    case n3dsExportSettings::Strider:
        phyclass = entityClass->GetComponentSafe<ncPhyHumRagDollClass>();
        phyclass->SetStriderAngles();
        ragdollSkeletonClass->SetRagType( ncRagDollClass::TypeToString(ncRagDollClass::Strider) );
        this->ExportStriderRagdollCorrespondence( ragdollSkeletonClass, skeletons );
        break;*/
    default:
        n_assert_always();
    }

}

//------------------------------------------------------------------------------
/**
*/
void
n3dsAnimationExport::ExportHumanRagdollCorrespondence( ncRagDollClass * ragdollSkeletonClass, const nArray<n3dsSkeleton*>& skeletons )
{
    // set ragdoll joints (correspondence between physics ragdoll and gfx ragdoll)
    // order them in the same way physic joints are created
    int phyRagdollJointIdx = -1;

    int fullSkJointIdx = 0;
    int gfxRagdollJointIdx = 0;
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "Head" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_UpperArm" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_UpperArm" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Forearm" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Forearm" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Thigh" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Thigh" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Calf" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Calf" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsAnimationExport::ExportScvRagdollCorrespondence( ncRagDollClass * ragdollSkeletonClass, const nArray<n3dsSkeleton*>& skeletons )
{
    // set ragdoll joints (correspondence between physics ragdoll and gfx ragdoll)
    // order them in the same way physic joints are created
    int phyRagdollJointIdx = -1;

    int fullSkJointIdx = 0;
    int gfxRagdollJointIdx = 0;
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "Head" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Leg_B2" ); //upperarm
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Leg_B2" ); //upperarm
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Leg_B3" ); 
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Leg_B3" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Leg_A2" ); //thigh
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Leg_A2" ); //thigh
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Leg_A3" ); 
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Leg_A3" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsAnimationExport::ExportStriderRagdollCorrespondence( ncRagDollClass * ragdollSkeletonClass, const nArray<n3dsSkeleton*>& skeletons )
{
    // set ragdoll joints (correspondence between physics ragdoll and gfx ragdoll)
    // order them in the same way physic joints are created
    int phyRagdollJointIdx = -1;

    int fullSkJointIdx = 0;
    int gfxRagdollJointIdx = 0;
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "Head" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Arm1" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Arm1" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Arm2" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Arm2" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Leg1" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Leg1" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "L_Leg2" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);

    fullSkJointIdx = skeletons[0]->FindBoneThatContains( "R_Leg2" );
    gfxRagdollJointIdx = skeletons[0]->RagCorresp.FindIndex( fullSkJointIdx );
    ragdollSkeletonClass->SetRagdollJoint( gfxRagdollJointIdx , phyRagdollJointIdx++);
}

//------------------------------------------------------------------------------
/**
    gets bone position, rotation and scale from 3dsMax
*/
void
n3dsAnimationExport::GetPosRotScale( Matrix3 localMatrix, int parentID, vector3& pos, quaternion& quat, vector3& scale)
{
    // aux data
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    // get right joint pose, rot, scale
    Matrix3 trans(true);
    if (parentID == -1)
    {
        trans = this->GetSkinPivotMatrix();
        trans.Invert();
    }

    Matrix3 tm = localMatrix;
    tm = tm*trans;
	tm.NoScale();

    // convert to nebula
    matrix44d auxMatrix = systemCoord->MaxToNebulaMatrix(tm);

    // get scale, quat and pose
    auxMatrix.get(scale,quat,pos);

    // update scale
    // i dunno why ( but otherwise do not work )
    if (scale.z<0)
    {
        scale.z=-scale.z;
    }      
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsAnimationExport::CreateJointGroups(ncSkeletonClass *skeletonClass, int /*lodLevel*/, const n3dsSkeleton* skeleton )
{
    ////////////////////////
    int numGroups = 0;

    n3dsExportSettings::CritterNameType exportCritter = n3dsExportServer::Instance()->GetSettings().critterName;
    switch ( exportCritter )
    {
    case n3dsExportSettings::Human:
        numGroups = 3;
        break;

    case n3dsExportSettings::Scavenger:
        numGroups = 1;
        break;
        
    case n3dsExportSettings::Strider:
        numGroups = 3;
        break;

    default:
        n_assert_always();
    }
    ////////////////////////


    int jointGroupIndex = 0;

    //int humanNumGroups = 3;

    //can be used always, not only for humans
    //********************************************
    skeletonClass->BeginJointGroups(numGroups);

    for( ; jointGroupIndex< numGroups; jointGroupIndex++)
    {
        //all joints for all lodLevels
        if( jointGroupIndex == 0)
        {
            skeletonClass->SetGroupNumberJoints( jointGroupIndex, skeleton->BonesArray.Size());
            for( int jointIndex=0; jointIndex< skeleton->BonesArray.Size();)
            {
                int jointIndices[4];

                //set jointIndices in groups of 4
                for(int j=0; j<4 ; j++, jointIndex++)
                {
                    if( jointIndex < skeleton->BonesArray.Size())
                    {
                        jointIndices[j] = jointIndex;
                    }
                    else
                    {
                        jointIndices[j] = 0;
                    }
                }
                skeletonClass->SetGroup( jointGroupIndex, jointIndices[0], jointIndices[1], jointIndices[2], jointIndices[3]);
            }
        }
        else
        {
            // create jointIndices array, to use easily SetGroup method
            nArray<int> jointIndices;

            // add from 0 to jointseparation only for lower train (before it was added for all joint groups)
            // because in separationArray not has bip 01
            if( jointGroupIndex +1 == numGroups)
            {
                for(int jointIndex=0; ((jointIndex < skeleton->SeparationArray[0].GetJointIndex()) && (jointIndex<skeleton->BonesArray.Size())); jointIndex++)
                {
                    jointIndices.Append( jointIndex );
                }
            }
            // for other joint separations
            for(int j=0; j< skeleton->SeparationArray.Size(); j++)
            {
                //if joint group index i'm working with
                if( skeleton->SeparationArray[j].GetGroupIndex() == jointGroupIndex)
                {
                    // add joints till next joint separation
                    if( j < skeleton->SeparationArray.Size() - 1 )
                    {
                        for(int jointIndex = skeleton->SeparationArray[j].GetJointIndex(); jointIndex < skeleton->SeparationArray[j+1].GetJointIndex(); jointIndex++)
                        {
                            jointIndices.Append( jointIndex );
                        }
                    }
                    else
                    {
                        for(int jointIndex = skeleton->SeparationArray[j].GetJointIndex(); jointIndex < skeleton->BonesArray.Size(); jointIndex++)
                        {
                            jointIndices.Append( jointIndex );
                        }
                    }
                }
            }

            skeletonClass->SetGroupNumberJoints( jointGroupIndex, jointIndices.Size());

            // easy use of setgroup
            int jIndices[4];
            int jI = 0;
            for( jI=0; jI< jointIndices.Size();)
            {
                //set jointIndices in groups of 4
                for(int j=0; j<4 ; j++, jI++)
                {
                    if( jI < jointIndices.Size())
                    {
                        jIndices[j] = jointIndices[jI];
                    }
                    else
                    {
                        jIndices[j] = 0;
                    }
                }
                skeletonClass->SetGroup( jointGroupIndex, jIndices[0], jIndices[1], jIndices[2], jIndices[3]);
            }

        }
    }

    skeletonClass->EndJointGroups();
    //********************************************

    // always these common joints
    //skeletonClass->SetCommonJoint(0);
    //skeletonClass->SetCommonJoint(1);
}
