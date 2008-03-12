#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsanimationexport/n3dsskeleton.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dssystemcoordinates.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    constructor
*/
n3dsSkeleton::n3dsSkeleton(int lodLevel)
    :BonesArray(0,30)
    ,AttachmentArray(0,10)
    ,bSkeletonCreated(false)
    ,SeparationArray(0,2)
    ,HelperArray(0,2)
    ,lodLevel(lodLevel)
    ,rootLocalMatrix()
    ,rootWorldMatrix()
{
    this->humanRagBones.Append(nString("L_Thigh"));
    this->humanRagBones.Append(nString("R_Thigh"));
    this->humanRagBones.Append(nString("Head"));
    this->humanRagBones.Append(nString("L_UpperArm"));
    this->humanRagBones.Append(nString("R_UpperArm"));
    this->humanRagBones.Append(nString("L_Calf"));
    this->humanRagBones.Append(nString("R_Calf"));
    this->humanRagBones.Append(nString("L_Forearm"));
    this->humanRagBones.Append(nString("R_Forearm"));

    this->scavengerRagBones.Append(nString("L_Leg_B2"));
    this->scavengerRagBones.Append(nString("R_Leg_B2"));
    this->scavengerRagBones.Append(nString("L_Leg_A2"));
    this->scavengerRagBones.Append(nString("R_Leg_A2"));
    this->scavengerRagBones.Append(nString("Head"));
    this->scavengerRagBones.Append(nString("L_Leg_B3"));
    this->scavengerRagBones.Append(nString("R_Leg_B3"));
    this->scavengerRagBones.Append(nString("L_Leg_A3"));
    this->scavengerRagBones.Append(nString("R_Leg_A3"));

    this->striderRagBones.Append(nString("L_Leg1"));
    this->striderRagBones.Append(nString("R_Leg1"));
    this->striderRagBones.Append(nString("L_Arm1"));
    this->striderRagBones.Append(nString("R_Arm1"));
    this->striderRagBones.Append(nString("Head"));
    this->striderRagBones.Append(nString("L_Leg2"));
    this->striderRagBones.Append(nString("R_Leg2"));
    this->striderRagBones.Append(nString("L_Arm2"));
    this->striderRagBones.Append(nString("R_Arm2"));

    this->bSkeletonCreated = this->CreateSkeleton();
}

//------------------------------------------------------------------------------
/**
    default destructor
*/
n3dsSkeleton::~n3dsSkeleton()
{
    //empty
}

//------------------------------------------------------------------------------
/**
    check if it is a bone or not
    notice that all bones do start with "Bip"

*/
bool n3dsSkeleton::IsBoneNode(IGameNode *pGameNode)
{
	IGameObject *pGameObject = pGameNode->GetIGameObject();
	IGameControl *pGameControl = pGameNode->GetIGameControl();

	if (pGameObject->GetIGameType() == IGameObject::IGAME_BONE)
		return true;

	Object *pMaxObject = pGameObject->GetMaxObject();
	if (pMaxObject->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;

	if (pGameControl->GetControlType(IGAME_POS) == IGameControl::IGAME_BIPED ||
		pGameControl->GetControlType(IGAME_ROT) == IGameControl::IGAME_BIPED)
		return true;

    //bone attachment - all the attached bones' names are Bip*
    TCHAR* attachedbone = "Bip";
    int stringcomparison;
    if ((stringcomparison = strncmp(pGameNode->GetName(),attachedbone,3)) == 0)
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    check if it is a dummy or not
*/
bool n3dsSkeleton::IsDummyNode(IGameNode *pGameNode)
{
	IGameObject *pGameObject = pGameNode->GetIGameObject();

	Object *pMaxObject = pGameObject->GetMaxObject();
	if (pMaxObject->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
    {
		return true;
    }

	return false;
}

//------------------------------------------------------------------------------
/**
    fills the skeleton
*/
bool n3dsSkeleton::CreateSkeleton()
{
    IGameScene *pIgame=n3dsExportServer::Instance()->GetIGameScene();

	for (int loop = 0; loop < pIgame->GetTopLevelNodeCount(); loop++)
	{
		IGameNode *pGameRootNode = pIgame->GetTopLevelNode(loop);
        if(( this->lodLevel == -1) || (n3dsExportServer::Instance()->GetSettings().lod.GetLODof(pGameRootNode) == lodLevel))
        {
		    this->RecruseFillBonesArray(-1, pGameRootNode, matrix44d());
        }
	}

    this->FillRagSkeleton();

    //for ( int i=0; i< this->BonesArray.Size(); i++)
    //{
    //    N3DSERROR( animexport , ( 0 , "ERROR: Bone %d parent %d name %s", i, this->BonesArray[i].iParentBoneId, this->BonesArray[i].strBoneName.Get()));
    //}

    //N3DSERROR( animexport , ( 0 , "ERROR: "));

	return this->BonesArray.Size() >0;
}

//------------------------------------------------------------------------------
/**
    fills skeleton arrays (bones, attachments, and separationjoints)
*/
void n3dsSkeleton::RecruseFillBonesArray(int iParentID, IGameNode *pGameNode, matrix44d parentMatrix)
{
    char* dummy = "Dummy"; // dummies
    char* tgr = "tgr"; // trigger attachment helper
    char* prt = "prt"; // porter attachment helper
    char* rem = "rem"; // remove bone for low lod skeletons

	n_assert(pGameNode != NULL);
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

	Matrix3 localTM;
	Matrix3 worldTM;

	BOOL bIsBone = this->IsBoneNode(pGameNode);
	BOOL bIsDummy = this->IsDummyNode(pGameNode);

    nString strBoneName = pGameNode->GetName();
    strBoneName.ConvertSpace();

    // is attach weapon helper
    BOOL bIsAttachHelper = bIsDummy && (strstr(strBoneName.Get(), tgr) || strstr(strBoneName.Get(), prt));
    if( bIsAttachHelper )
    {
        localTM = pGameNode->GetLocalTM().ExtractMatrix3();
        int iGNodeId = pGameNode->GetNodeID();
        this->HelperArray.Append(n3dsDynAttach(strBoneName.Get(), iParentID, localTM, iGNodeId));
    }

    //is bone but not final joint
	if (bIsBone &&  (!bIsAttachHelper) && (!strstr(strBoneName.Get(), dummy)))
	{
		localTM = pGameNode->GetLocalTM().ExtractMatrix3();
		worldTM = pGameNode->GetWorldTM().ExtractMatrix3();

        matrix44d localMatrix = systemCoord->MaxToNebulaMatrix(localTM);
        matrix44d worldMatrix = systemCoord->MaxToNebulaMatrix(worldTM);

        localMatrix.mult_simple( parentMatrix );

        int iBonesID = this->BonesArray.Size();

        if( !strstr(strBoneName.Get(), rem) )
        {
            this->FindJointSeparations( strBoneName );
            int parent = iParentID;
            if( this->IsJointGroupLeader( strBoneName ) )
            {
                parent = -1;
                localMatrix.mult_simple( this->BonesArray[0].localTr );
                //worldMatrix.mult_simple( this->BonesArray[0].worldTr );
            }

            this->BonesArray.Append(n3dsBone(parent, strBoneName, localMatrix, worldMatrix, pGameNode->GetNodeID(), bIsDummy));
        }

		for (int i = 0; i < pGameNode->GetChildCount(); i++)
		{
			IGameNode *pGameChildNode = pGameNode->GetNodeChild(i);
            if( !strstr(strBoneName.Get(), rem) )
            {
			    this->RecruseFillBonesArray(iBonesID, pGameChildNode, matrix44d() );
            }
            else
            {
			    this->RecruseFillBonesArray(iParentID, pGameChildNode, localMatrix );
            }

		}
	}
    else if( ! bIsAttachHelper && iParentID != -1) //is attachment (not attached bone)
    {
        this->AttachmentArray.Append( n3dsAttachment(iParentID, pGameNode));
    }
}

//------------------------------------------------------------------------------
/**
    fills ragdoll skeleton
*/
void
n3dsSkeleton::FillRagSkeleton()
{
    int parentid = -1;
    int fullparentid = -1;
    matrix44d localmatrix, worldmatrix;

    for (int i = 0; i < BonesArray.Size(); i++)
	{
        int bone = -1;
        if( i != 0)
        {
            bone = this->IsRagdollBone(BonesArray[i].strBoneName);
        }

        if( bone != -1  || i == 0 )
		{
            localmatrix.ident();
            worldmatrix.ident();

            this->RagCorresp.Append(i);

            if( bone < 5  && i!=0 )
            {
                fullparentid = parentid = 0;
                this->CalculateRagJointMatrix(i, fullparentid,localmatrix);
            }
            else
            {
                // bip00 for lod, bip01 when they're exporting only one skeleton
                if( i==0)
                {
                    localmatrix = this->BonesArray[i].localTr;
                    worldmatrix = this->BonesArray[i].worldTr;
                    fullparentid = parentid = -1;
                }
                else
                {
                    parentid = this->FindRagParent( BonesArray[i] );
                    int fullparentid = this->RagCorresp[parentid];
                    this->CalculateRagJointMatrix(i, fullparentid, localmatrix);
                }
            }

            //add the bone to the ragskeleton
            this->RagBonesArray.Append(n3dsBone(parentid,this->BonesArray[i].strBoneName.Get(),localmatrix,worldmatrix,i));
        }
    }
}


//------------------------------------------------------------------------------
/**
    find child bones that belong to ragdoll skeleton
*/
nArray<int> n3dsSkeleton::FindRagChildren(int parentindex)
{
    nArray<int> children;
    for(int i=0; i< this->BonesArray.Size(); i++)
    {
        if( (this->BonesArray[i].iParentBoneId == parentindex)&&(this->RagCorresp.FindIndex(i) != -1) )
        {
            //appnd the index of the sonbone
            children.Append(i);
        }
    }
    return children;
}

//------------------------------------------------------------------------------
/**
    find parent bone given a child bone
*/
int n3dsSkeleton::FindRagParent(n3dsBone child)
{
    bool isragbone = false;
    n3dsBone& bone = child;
    int iparentid;

    while ( !isragbone )
    {
        iparentid = bone.iParentBoneId;
        
        if( iparentid != -1)
        {
            bone = this->BonesArray[bone.iParentBoneId];
            isragbone = (this->RagCorresp.FindIndex(iparentid) != -1);
        }
        else
        {
            //group leaders cannot be ragboness
            iparentid = 0;
            isragbone = true;
        }
    }

    return this->RagCorresp.FindIndex(iparentid);
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSkeleton::CalculateRagJointMatrix(int boneindex,int parentid, matrix44d& localmatrix)
{
    n3dsBone bone = this->BonesArray[boneindex];
    int boneId = boneindex;
    while( boneId != parentid)
    {
        localmatrix.mult_simple( bone.localTr );

        if( this->IsJointGroupLeader( BonesArray[boneId].strBoneName.Get() ))
        {
            matrix44d localTrBip00 = this->BonesArray[0].localTr;
            localTrBip00.invert_simple();
            localmatrix.mult_simple( localTrBip00 );
        }

        //get bone
        boneId = bone.iParentBoneId;
        if( boneId != -1 )
        {
            bone = this->BonesArray[boneId];
        }
        else
        {
            boneId = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsSkeleton::FindBoneThatContains( const nString& jointName )
{
    for (int i = 0; i < this->BonesArray.Size(); i++)
	{
        nString removedBip = this->BonesArray[i].strBoneName.ExtractRange(5, this->BonesArray[i].strBoneName.Length() - 5);
        if( strstr(removedBip.Get(), jointName.Get() ))
		{
			return i;
		}
	}

	return -1;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSkeleton::FindJointSeparations( const nString& boneName )
{
    //find separation group joints
    n3dsExportSettings::CritterNameType exportCritter = n3dsExportServer::Instance()->GetSettings().critterName;
    switch ( exportCritter )
    {
    case n3dsExportSettings::Human:
        this->FindHumanJointSeparations( boneName );
        break;

    case n3dsExportSettings::Scavenger:
        this->FindScavengerJointSeparations( boneName );
        break;

    case n3dsExportSettings::Strider:
        //this->FindHumanJointSeparations( boneName );
        this->FindStriderJointSeparations( boneName );
        break;
                
    default:
        n_assert_always();
    }
}

//------------------------------------------------------------------------------
/**
    find human joint separations: will be tighs, spines and weapon bone
*/
void
n3dsSkeleton::FindHumanJointSeparations( const nString& boneName )
{
    nString boneNameLower = boneName;
    boneNameLower.ToLower();

    //find inferior joint for humans
    if ((boneNameLower.IndexOf("pelvis", 0) != -1) ||
        (boneNameLower.IndexOf("thigh", 0) != -1))
    {
        this->SeparationArray.Append( SeparationJoint( this->BonesArray.Size(), 2));
    }
    //find superior joint for humans
    if ((boneNameLower.IndexOf("spine", 0) != -1) ||
        (boneNameLower.IndexOf("weapon", 0) != -1) || 
        (boneNameLower.IndexOf("acc", 0) != -1))
    {
        this->SeparationArray.Append( SeparationJoint( this->BonesArray.Size(), 1));
    }
}

//------------------------------------------------------------------------------
/**
    find strider joint separations: will be legs and spines
*/
void
n3dsSkeleton::FindScavengerJointSeparations( const nString& /*boneName*/ )
{
    //nString boneNameLower = boneName;
    //boneNameLower.ToLower();
}

//------------------------------------------------------------------------------
/**
    find strider joint separations: will be legs and spines
*/
void
n3dsSkeleton::FindStriderJointSeparations( const nString& boneName )
{
    nString boneNameLower = boneName;
    boneNameLower.ToLower();

    //find inferior joint for strider
    if (boneNameLower.IndexOf("leg", 0) != -1)
    {
        this->SeparationArray.Append( SeparationJoint( this->BonesArray.Size(), 2));
    }
    //find superior joint for humans
    if (boneNameLower.IndexOf("spine", 0) != -1)
    {
        this->SeparationArray.Append( SeparationJoint( this->BonesArray.Size(), 1));
    }
}

//------------------------------------------------------------------------------
/**
    humans: lod0 -> pelvis  spine   weapon
            lod1 -> pelvis  spine   weapon
            lod2 -> thigh   spine   weapon
            lower train won't be tested because bip01 bone is used in that train
*/
bool
n3dsSkeleton::IsJointGroupLeader( const nString& boneName )
{
    nString boneNameLower = boneName;
    boneNameLower.ToLower();

    n3dsExportSettings::CritterNameType exportCritter = n3dsExportServer::Instance()->GetSettings().critterName;
    switch ( exportCritter )
    {
    case n3dsExportSettings::Human:
        return ( (boneNameLower.IndexOf("weapon", 0) != -1) ||
                 ((boneNameLower.IndexOf("spine", 0) != -1) && (boneNameLower.Length() == 11)) 
               );

    case n3dsExportSettings::Scavenger:
        return false;

    case n3dsExportSettings::Strider:
        return ( (boneNameLower.IndexOf("spine1", 0) != -1) && (boneNameLower.Length() == 12) ) ||
               ( (this->lodLevel == 2) && (boneNameLower.IndexOf("spine3", 0) != -1) && (boneNameLower.Length() == 12) );
        
    default:
        n_assert_always();
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
n3dsSkeleton::IsRagdollBone(const nString& boneName)
{
    nString boneNameShort = boneName.ExtractRange(6, boneName.Length() - 6);
    //boneNameLower.ToLower();


    n3dsExportSettings::CritterNameType exportCritter = n3dsExportServer::Instance()->GetSettings().critterName;
    switch ( exportCritter )
    {
    case n3dsExportSettings::Human:
        return this->humanRagBones.FindIndex(boneNameShort);

    case n3dsExportSettings::Scavenger:
        return this->scavengerRagBones.FindIndex(boneNameShort);

    case n3dsExportSettings::Strider:
        return this->striderRagBones.FindIndex(boneNameShort);
        
    default:
        n_assert_always();
        return -1;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
n3dsSkeleton::GetNumberOfRootBones() const
{
    int rootBones = 0;
    for( int i=0; i< this->BonesArray.Size(); i++)
    {
        if( this->BonesArray[i].iParentBoneId == -1 )
        {
            rootBones++;
        }
    }

    return rootBones;
}
