#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsanimationexport/n3dsanimationexport.h"
#include "n3dsanimationexport/n3dsskeleton.h"
//------------------------------------------------------------------------------
#include "n3dsexporters/n3dssystemcoordinates.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsgraphicsexport/n3dsgraphicsexport.h"
#include "n3dsexporters/n3dsscenelist.h"
//------------------------------------------------------------------------------
#include "entity/nentityclassserver.h"
#include "animcomp/ncskeletonclass.h"
#include "zombieentity/ncloaderclass.h"
#include "kernel/nfileserver2.h"
//------------------------------------------------------------------------------
#include "tools/nanimbuilder.h"
#include "nasset/nentityassetbuilder.h"

#include "n3dsexporters/n3dslog.h"
//------------------------------------------------------------------------------
/**
*/
void
n3dsAnimationExport::ExportSkinAnimation()
{
    // export animation for first lod
    // create others from the first one
    n3dsExportServer* exportServer  = n3dsExportServer::Instance();

    nArray<int> numRootBones;

    for(int lodLevel = 0; lodLevel< exportServer->GetSettings().lod.GetCountLOD() ; lodLevel++)
    {
        // get skeleton
        this->skeletons.Append( n_new(n3dsSkeleton(lodLevel)) );
        if( this->skeletons[lodLevel]->BonesArray.Size() == 0 )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: There's no skeleton for %i lod. (n3dsAnimationExport::ExportSkinAnimation)", lodLevel));
            return;
        }
        numRootBones.Append( this->skeletons[lodLevel]->GetNumberOfRootBones());
    }

    if( exportServer->GetSettings().lod.GetCountLOD() == 0 )
    {
        // get skeleton
        this->skeletons.Append( n_new(n3dsSkeleton()) );
        if( this->skeletons[0]->BonesArray.Size() == 0 )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: There's no skeleton in that scene (n3dsAnimationExport::ExportSkinAnimation)"));
            return;
        }
    }

    // a root bone has been removed
    for(int lodLevel = 0; lodLevel< exportServer->GetSettings().lod.GetCountLOD() ; lodLevel++)
    {
        if (numRootBones[lodLevel] != numRootBones[0] )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: Attention: You've removed a root bone at lod %i! Animations won't be exported.", lodLevel));
            n_message( "Attention: You've removed a root bone at lod %i! Animations won't be exported.", lodLevel);
            return;
        }
    }

    for( int lodLevel=0; lodLevel < skeletons.Size(); lodLevel++)
    {
        nAnimBuilder AnimBuilder;

        // load skeleton
        nString lodSk = exportServer->GetSettings().skeletonName;
        if( skeletons.Size() != 1 )
        {
            lodSk += "_";
            lodSk += lodLevel;
        }

        nEntityClass * entityClass  = nEntityClassServer::Instance()->GetEntityClass( lodSk.Get() );
        n_assert( entityClass );
        ncSkeletonClass *skeletonClass = entityClass->GetComponent<ncSkeletonClass>( );
        n_assert( skeletonClass );
        
        //check if loaded skeleton and scene skeleton have the same number of joints
        int charNumJoints = skeletonClass->GetCharacter().GetSkeleton().GetNumJoints();
        if ( charNumJoints != this->skeletons[lodLevel]->BonesArray.Size() )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: %s and scene skeleton have different number of joints. (n3dsAnimationExport::ExportSkinAnimation)", lodSk.Get() ));
            return;
        }

        //set paths
        nString animPath = nEntityAssetBuilder::GetAnimsPath( entityClass );
        animPath.Append( exportServer->GetSettings().resourceName );

        nString motionPath = animPath;
        motionPath += "_motion";

        if(exportServer->GetSettings().binaryResource)
        {
            animPath.Append( ".nax2" );
            motionPath.Append( ".nax2" );
        }
        else
        {
            animPath.Append( ".nanim2" );
            motionPath.Append( ".nanim2" );
        }

        if( lodLevel == 0 )
        {
            // i need it before creating animation
            //find jointGroup HUMAN CASE!!
            nString fileName =animPath.ExtractFileName();
            fileName.TerminateAtIndex(2);
            //"inferior"
            if( fileName.FindChar('i',1) ==1)
            {
                this->jointGroupIndex = 2;
            }
            //"superior"
            if( fileName.FindChar('s',1) ==1)
            {
                this->jointGroupIndex = 1;
            }
            
            // get max data
            this->canExportAnim = this->CreateSkinAnimation(entityClass, this->completeAnimBuilder, lodLevel);
            if( this->canExportAnim )
            {
                this->canExportMotion = this->CreateMotionBuilder();
            }
        }
        else
        {
            this->canExportAnim = this->CreateLowLodSkinAnimation(entityClass, AnimBuilder, lodLevel);
        }

        if (this->canExportAnim)
	    {
            // save animation and motion
            nFileServer2::Instance()->MakePath( animPath.ExtractDirName().Get() );
            nFileServer2::Instance()->MakePath( motionPath.ExtractDirName().Get() );

            bool saveCompleteAnim = (lodLevel ==0 ) && this->completeAnimBuilder.Save(nKernelServer::Instance()->GetFileServer(), animPath.Get());
            bool saveLowLevelAnim = (lodLevel !=0 ) && AnimBuilder.Save(nKernelServer::Instance()->GetFileServer(), animPath.Get());
            
            if( lodLevel == 0 && this->canExportMotion )
            {
                this->motionBuilder.Save(nKernelServer::Instance()->GetFileServer(), motionPath.Get());
            }

            if( saveCompleteAnim || saveLowLevelAnim )
		    {
                this->SetSkeletonStateData(entityClass, animPath, motionPath, lodLevel);
		    }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsAnimationExport::CreateSkinAnimation( nEntityClass * entityClass, nAnimBuilder &AnimBuilder, int lodLevel)
{
    n3dsExportServer* exportServer  = n3dsExportServer::Instance();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    // load skeleton
    n_assert( entityClass );
    ncSkeletonClass *skeletonClass = entityClass->GetComponent<ncSkeletonClass>();
    n_assert( skeletonClass );

    nJointGroup jointGroup = skeletonClass->GetCharacter().GetSkeleton().GetJointGroupAt(this->jointGroupIndex);

    bool motion  = n3dsExportServer::Instance()->GetSettings().exportMotion;
    bool motionX = n3dsExportServer::Instance()->GetSettings().motionX;
    bool motionY = n3dsExportServer::Instance()->GetSettings().motionY;
    bool motionZ = n3dsExportServer::Instance()->GetSettings().motionZ;

    //animation data
    IGameScene* pIgame = exportServer->GetIGameScene();

    this->animationSettings.fKeyDuration = 
            (float)this->animationSettings.iFramesPerSample / GetFrameRate();

    // start animation collect data
	nAnimBuilder::Group AnimGroup;

    int iFirstKey = 0;
	//int iNumStateKeys = this->animationSettings.GetNumFrames()/this->animationSettings.iFramesPerSample;
    int iNumClipKeys  = 0;

    //looptype, keytime and numkeys
    AnimGroup.SetLoopType(nAnimBuilder::Group::CLAMP);
	AnimGroup.SetKeyTime(this->animationSettings.fKeyDuration);

	// loop skeleton bones
	for(int iBoneIdx=0; iBoneIdx< this->skeletons[lodLevel]->BonesArray.Size(); iBoneIdx++)
	{
		n3dsBone &Bone = this->skeletons[lodLevel]->BonesArray[iBoneIdx];

		IGameNode *pGameNode = pIgame->GetIGameNode(Bone.iNodeId);
		IGameControl *pGameControl = pGameNode->GetIGameControl();

        if (!pGameControl)
		{
            N3DSERROR( animexport , ( 0 , "ERROR: No gamecontrol. (n3dsAnimationExport::CreateSkinAnimation)"));
			return false;
		} 

        // start collecting keys
		IGameKeyTab Key;
		if (pGameControl->GetFullSampledKeys(Key, this->animationSettings.iFramesPerSample, IGAME_TM))
		{
            if(jointGroup.FindJoint( iBoneIdx) != -1 )
            {
			    nAnimBuilder::Curve AnimCurveTrans;
			    nAnimBuilder::Curve AnimCurveRot;
			    nAnimBuilder::Curve AnimCurveScale;
                                    
                AnimCurveTrans.SetIpolType(nAnimBuilder::Curve::LINEAR);
			    AnimCurveRot.SetIpolType(nAnimBuilder::Curve::QUAT);
			    AnimCurveScale.SetIpolType(nAnimBuilder::Curve::LINEAR);

                // extract correct key pose, rot, scale
			    Matrix3 tm;

                if(iNumClipKeys == 0)
                {
                    iNumClipKeys = Key.Count();
                }

                vector4 initialRootPose;

                for(int j=0; j<Key.Count(); j++)
			    {
				    nAnimBuilder::Key AnimKeyTrans;
				    nAnimBuilder::Key AnimKeyRot;
				    nAnimBuilder::Key AnimKeyScale;

				    tm = Key[iFirstKey+0*iNumClipKeys+j].sampleKey.gval.ExtractMatrix3();
				    tm.NoScale();

                    // convert to nebula system coordinates
                    matrix44d myMatrix = systemCoord->MaxToNebulaMatrix(tm);

                    vector3 scale, pos;
                    quaternion quat;

                    quaternion oldquat;
                    myMatrix.get(scale,oldquat,pos);

                    pGameNode = pIgame->GetIGameNode(Bone.iNodeId);

                    //in human case: if it's spine or weapon, ie, first joint of jointgroup diferent of bip01
                    if( Bone.iParentBoneId == -1 && pGameNode->GetNodeParent())
                    {
                        pGameNode = pGameNode->GetNodeParent();
                        IGameKeyTab parentKeys;
                        pGameControl = pGameNode->GetIGameControl();
                        pGameControl->GetFullSampledKeys(parentKeys, this->animationSettings.iFramesPerSample, IGAME_TM);

                        Matrix3 pm = parentKeys[iFirstKey+0*iNumClipKeys+j].sampleKey.gval.ExtractMatrix3();
    		            pm.NoScale();

                        matrix44d parentMatrix = systemCoord->MaxToNebulaMatrix(pm);

                        if( motion )
                        {
                            vector3 parentPos = parentMatrix.pos_component();
                            
                            if( j == 0 )
                            {
                                initialRootPose = parentPos;
                            }

                            if( motionX )
                            {
                                parentPos.x = initialRootPose.x;
                            }
                            if( motionY )
                            {
                                parentPos.y = initialRootPose.y;
                            }
                            if( motionZ )
                            {
                                parentPos.z = initialRootPose.z;
                            }
                            parentMatrix.set_translation( parentPos );
                        }
                        myMatrix.mult_simple( parentMatrix );
                    }

                    // get trans, rot, scale
                    myMatrix.get(scale,quat,pos);

                    // apply correction
                    if (scale.z<0)
                    {
                        scale.z=-scale.z;
                    }

                    if( iBoneIdx == 0 && exportServer->GetSettings().firstPerson && exportServer->GetCamera())
                    {
                        GMatrix g = exportServer->GetCamera()->GetWorldTM();
                        transform44 transform = exportServer->GetSystemCoordinates()->MaxToNebulaTransform(g);
                        pos -= vector3(0,transform.gettranslation().y,0);
                    }

                    AnimKeyTrans.Set(vector4(  pos.x, pos.y, pos.z, 0));                                      
                    AnimCurveTrans.SetKey(j,AnimKeyTrans);

                    AnimKeyRot.Set(vector4(quat.x, quat.y, quat.z, quat.w));
				    AnimCurveRot.SetKey(j,AnimKeyRot);

                    AnimKeyScale.Set(vector4(scale.x, scale.y, scale.z, 0));
				    AnimCurveScale.SetKey(j,AnimKeyScale);
			    }

                if( iBoneIdx == 0 )
                {
                    this->SetMotionCurve( AnimCurveTrans );
                }

                // add curves to state
			    AnimGroup.AddCurve(AnimCurveTrans);
			    AnimGroup.AddCurve(AnimCurveRot);
			    AnimGroup.AddCurve(AnimCurveScale);
            }
		}
		else
		{
			//n_printf("No animation keys for node %s",pGameNode->GetName());
            N3DSERROR( animexport , ( 0 , "ERROR: No animation keys for node %s (n3dsAnimationExport::CreateSkinAnimation)",pGameNode->GetName()));
            return false;
        }
    }

    AnimGroup.SetNumKeys(iNumClipKeys);

    // add state
    AnimBuilder.AddGroup(AnimGroup);
    AnimBuilder.Optimize();
    AnimBuilder.FixKeyOffsets();

	return true;
}

//------------------------------------------------------------------------------
/**
    For each joint of the current low level skeleton, if parent is the same as in complete skeleton, copy keys; else joint_matrix*parent_matrix till we arrive to correct parent (the one that corresponds with low level skeleton)
    Example:
        FullSkeleton        bone00 -> bone01 -> bone02 -> bone03
        LowLevelSkeleton    bone10 -----------> bone11 -> bone12
        bone 10 -> copy curves of bone00
        bone 11 -> for each key do bone02_keymatrix*bone01_keymatrix, set key
        bone 12 -> copy curves of bone03    
*/
bool 
n3dsAnimationExport::CreateLowLodSkinAnimation( nEntityClass * entityClass, nAnimBuilder &AnimBuilder, int lodLevel )
{
    // start animation collect data
	nAnimBuilder::Group AnimGroup;
    nAnimBuilder::Group completeSkAnimGroup = this->completeAnimBuilder.GetGroupAt(0);
    
    n_assert( entityClass );
    ncSkeletonClass *skeletonClass = entityClass->GetComponent<ncSkeletonClass>();
    n_assert( skeletonClass );

    //current skeleton joint group
    nJointGroup jointGroup = skeletonClass->GetCharacter().GetSkeleton().GetJointGroupAt(this->jointGroupIndex);

    //full skeleton joint group
    nString fullSkName = skeletonClass->GetEntityClass()->GetName();
    fullSkName = fullSkName.ExtractRange( 0, fullSkName.Length() - 1 );
    fullSkName += "0";
    nEntityClass* fullSkEntity = nEntityClassServer::Instance()->GetEntityClass( fullSkName.Get() );
    n_assert(fullSkEntity);
    ncSkeletonClass* fullSkClass = fullSkEntity->GetComponentSafe<ncSkeletonClass>();
    nJointGroup fullSkJointGroup = fullSkClass->GetCharacter().GetSkeleton().GetJointGroupAt(this->jointGroupIndex);

    int numKeysPerCurve = completeSkAnimGroup.GetNumKeys();

    //set looptype, keytime
    AnimGroup.SetLoopType( completeSkAnimGroup.GetLoopType() );
    AnimGroup.SetKeyTime( completeSkAnimGroup.GetKeyTime() );

    // for each low lod skeleton bone
    for(int boneIdx = 0; boneIdx < this->skeletons[lodLevel]->BonesArray.Size(); boneIdx++)
    {
        n3dsBone bone = this->skeletons[lodLevel]->BonesArray[boneIdx];

        //find index of the same bone in the complete skeleton
        nString boneNameNoBip = bone.strBoneName.ExtractRange(5, bone.strBoneName.Length() - 5);
        int completeSkBoneIndex = this->skeletons[0]->FindBoneThatContains( boneNameNoBip );
        n_assert(completeSkBoneIndex != -1);

        // non root bones
        if (bone.iParentBoneId != -1)
        {
            //if it's in the group
            if(jointGroup.FindJoint(boneIdx) != -1 )
            {
                nString parentNameNoBip = this->skeletons[lodLevel]->BonesArray[bone.iParentBoneId].strBoneName.ExtractRange(5, this->skeletons[lodLevel]->BonesArray[bone.iParentBoneId].strBoneName.Length() -5);
                int completeSkParentBoneIdx = this->skeletons[0]->FindBoneThatContains( parentNameNoBip );

                this->ComputeLowLevelCurves( AnimGroup, completeSkParentBoneIdx, fullSkJointGroup, completeSkBoneIndex );
            }
        }
        else //Bip01, and first joint jointgroup curves have to be inherited
        {
            //if it's in the group
            if(jointGroup.FindJoint(boneIdx) != -1 )
            {
                this->ComputeLowLevelCurves( AnimGroup, -1, fullSkJointGroup, completeSkBoneIndex );
            }
        }
    }

    AnimGroup.SetNumKeys(numKeysPerCurve);
    AnimBuilder.AddGroup(AnimGroup);
	AnimBuilder.Optimize(); 
	AnimBuilder.FixKeyOffsets();

    return true;
}

//------------------------------------------------------------------------------
/**

*/
void 
n3dsAnimationExport::ComputeLowLevelCurves( nAnimBuilder::Group& AnimGroup, int completeSkParentBoneIdx, nJointGroup fullSkJointGroup, int completeSkBoneIndex)
{
    int completeSkParentIdxFromCompleteSk = this->skeletons[0]->BonesArray[completeSkBoneIndex].iParentBoneId;
    nAnimBuilder::Group completeSkAnimGroup = this->completeAnimBuilder.GetGroupAt(0);
    int numKeysPerCurve = completeSkAnimGroup.GetNumKeys();

    //if parents are the same copy curves (pos, rot, scale)
    if( completeSkParentBoneIdx == completeSkParentIdxFromCompleteSk)
    {
        int jointGroupIdx = fullSkJointGroup.FindJoint(completeSkBoneIndex);
        int curveIdx = jointGroupIdx * 3;
        nAnimBuilder::Curve completeSkAnimCurveTrans = completeSkAnimGroup.GetCurveAt( curveIdx++ );
		nAnimBuilder::Curve completeSkAnimCurveRot   = completeSkAnimGroup.GetCurveAt( curveIdx++ );
		nAnimBuilder::Curve completeSkAnimCurveScale = completeSkAnimGroup.GetCurveAt( curveIdx++ );

		AnimGroup.AddCurve(completeSkAnimCurveTrans);
		AnimGroup.AddCurve(completeSkAnimCurveRot);
		AnimGroup.AddCurve(completeSkAnimCurveScale);
    }
    //else find matrix conversion and multiply all keys
    else
    {
        //define curves for new animation
        nAnimBuilder::Curve AnimCurveTrans;
		nAnimBuilder::Curve AnimCurveRot;
        nAnimBuilder::Curve AnimCurveScale;

        //set interpolation type
        AnimCurveTrans.SetIpolType(nAnimBuilder::Curve::LINEAR);
		AnimCurveRot.SetIpolType(nAnimBuilder::Curve::QUAT);
		AnimCurveScale.SetIpolType(nAnimBuilder::Curve::LINEAR);

        //for each key, modify it
        for( int keyIdx = 0; keyIdx < numKeysPerCurve; keyIdx++)
        {
            //new keys
			nAnimBuilder::Key AnimKeyTrans;
			nAnimBuilder::Key AnimKeyRot;
			nAnimBuilder::Key AnimKeyScale;

            vector3 pos, scale;
            quaternion quat;

            // find transformation, looking for correct parent
            matrix44d matrix;

            int completeBoneIndex = completeSkBoneIndex;
            n3dsBone completeParentSkBone = this->skeletons[0]->BonesArray[completeBoneIndex];

            int boneId = completeBoneIndex;
            while( boneId != completeSkParentBoneIdx)
            {
                int jointGroupIdx = fullSkJointGroup.FindJoint(boneId);
                int curvIdx = jointGroupIdx * 3;

                //get correct keys and put them in correct structures
                vector4 aux;
                aux = completeSkAnimGroup.GetCurveAt( curvIdx++ ).GetKeyAt(keyIdx).Get();
                pos = vector3(aux.x, aux.y, aux.z);

                aux = completeSkAnimGroup.GetCurveAt( curvIdx++ ).GetKeyAt(keyIdx).Get();
                quat = quaternion(aux.x, aux.y, aux.z, aux.w);

                aux = completeSkAnimGroup.GetCurveAt( curvIdx++ ).GetKeyAt(keyIdx).Get();
                scale = vector3(aux.x, aux.y, aux.z);

                //compute parent matrix
                matrix44d parentMatrix;
                parentMatrix.scale(scale);
                parentMatrix.mult_simple(matrix44d(quat));
                parentMatrix.translate(pos);

                matrix.mult_simple( parentMatrix );

                //get bone
                boneId = completeParentSkBone.iParentBoneId;
                
                if( boneId != -1 )
                {
                    completeParentSkBone = this->skeletons[0]->BonesArray[boneId];
                }
            }

            // get trans, rot, scale
            matrix.get(scale,quat,pos);

            AnimKeyTrans.Set(vector4( pos.x, pos.y, pos.z, 0));                                      
            AnimCurveTrans.SetKey(keyIdx,AnimKeyTrans);

            AnimKeyRot.Set(vector4(quat.x, quat.y, quat.z, quat.w));
			AnimCurveRot.SetKey(keyIdx,AnimKeyRot);

            AnimKeyScale.Set(vector4(scale.x, scale.y, scale.z, 0));
			AnimCurveScale.SetKey(keyIdx,AnimKeyScale);
        }

        //add curves to groups
		AnimGroup.AddCurve(AnimCurveTrans);
		AnimGroup.AddCurve(AnimCurveRot);
		AnimGroup.AddCurve(AnimCurveScale);
    }
}

//------------------------------------------------------------------------------
/**

*/
void
n3dsAnimationExport::SetSkeletonStateData( nEntityClass* entityClass, const nString& animPath, const nString& motionPath, int lodLevel)
{
    n_assert(entityClass);
    ncSkeletonClass *skeletonClass = entityClass->GetComponent<ncSkeletonClass>();
    n_assert( skeletonClass );

    n3dsExportServer* exportServer  = n3dsExportServer::Instance();

    int numState = skeletonClass->GetNumStates();
    int group = 0;

    float fadeInTime = 0.1f;

    int oldAnim = -1;
    if ( this->SkinAnimationExists(entityClass, oldAnim) )
    {
        // change state
        numState = oldAnim;
    }
    else
    {
        // add state
        skeletonClass->AddState( group, this->jointGroupIndex, fadeInTime );
    }
    // state data, anim data and state name to skeleton 

    // add state data
    skeletonClass->SetStateAnim( numState, animPath.Get() );
    skeletonClass->SetStateName( numState, exportServer->GetSettings().resourceName.Get());

    if( lodLevel == 0 && this->canExportMotion ) 
    {
        skeletonClass->SetStateMotion( numState, motionPath.Get() );
    }

    // find bounding box 
    this->CreateBoundingBox();
    vector3 center, extents;
    if( this->boundingBox.IsValid() )
    {
        center = this->boundingBox.center();
        extents = this->boundingBox.extents();
    }
    else
    {
        center = vector3(0,0,0);
        extents = vector3(1,1,1);
        N3DSERROR( animexport , ( 0 , "ERROR: Have you unhidden the mesh? Reexport this animation to have a correct bounding box. (n3dsGraphicsExport::ExportCharacterClass)"));
    }
    skeletonClass->SetStateBBox( numState, center , extents );

    this->FindPerceptionOffset();
    skeletonClass->SetPerceptionOffset( numState, this->perceptionOffset);

    // add clip
    int numClips = 1;
    skeletonClass->BeginClips( numState, numClips ); 
    skeletonClass->SetClip( numState, numClips - 1 , "one" );
    skeletonClass->EndClips( numState );

    if( lodLevel == 0 )
    {
        skeletonClass->SaveChunk();
    }

    // dirty entity, has to be persisted
    nEntityClassServer::Instance()->SetEntityClassDirty( entityClass, true );
}

//------------------------------------------------------------------------------
/**
    create bounding box for an specified animation
*/
void
n3dsAnimationExport::CreateBoundingBox()
{
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    Object* obj;
    Box3 boundingBox;
    bbox3 localBox = bbox3();
    this->boundingBox.begin_extend();

    IGameScene* pIgame = n3dsExportServer::Instance()->GetIGameScene();
    float anim_end_time = (float)pIgame->GetSceneEndTime();
    int ticks_per_frame = pIgame->GetSceneTicks();

    n3dsSceneList list = n3dsExportServer::Instance()->GetPostOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::graphics ) );

    for ( ; index != list.End() ; ++index)
    {
        IGameNode* skinnedNode = (*index).GetNode();
        IGameMesh* mesh = static_cast< IGameMesh* > ( skinnedNode->GetIGameObject() );
        if (  (!(skinnedNode->IsNodeHidden() ) && (mesh->IsObjectSkinned())) )
        {
            if( n3dsGraphicsExport::IsBlendedNode(skinnedNode))
            {
                n_message( "Blending not yet, sorry");
            }
            else
            {
                //node->GetIGameObject()->InitializeData();
                Matrix3 worldmatrix = skinnedNode->GetWorldTM(0).ExtractMatrix3();

                for(int i=0; i< anim_end_time; i=i+ticks_per_frame)
                {
                    obj = skinnedNode->GetMaxNode()->EvalWorldState(i).obj;
                    obj->GetDeformBBox(i, boundingBox, &worldmatrix);

                    //correct max and min with respect to nebula system coordinates
                    vector3 vmin = systemCoord->MaxtoNebulaVertex(boundingBox.Min());
                    vector3 vmax = systemCoord->MaxtoNebulaVertex(boundingBox.Max());

                    if(vmin.y > vmax.y)
                    {
                        float aux = vmin.y;
                        vmin.y = vmax.y;
                        vmax.y = aux;
                    }
                    if(vmin.z > vmax.z)
                    {
                        float aux = vmin.z;
                        vmin.z = vmax.z;
                        vmax.z = aux;
                    }

                    //get center and extend for nebula system coordinates
                    vector3 centerneb = (vmin+vmax)/2;
                    vector3 extendsneb = (vmax-vmin)*0.5f;

                    localBox.set(centerneb,extendsneb);
                    this->boundingBox.extend(localBox);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    true if it exists, false if do not
    oldAnim will have the animation index, if it doesn't exist will be -1
*/
bool
n3dsAnimationExport::SkinAnimationExists( nEntityClass* entityClass, int& oldAnim )
{
    n_assert( entityClass );
    n3dsExportServer* exportServer = n3dsExportServer::Instance();

    ncSkeletonClass *skeletonClass = entityClass->GetComponent<ncSkeletonClass>();
    n_assert( skeletonClass );

    for(int i=0; i < skeletonClass->GetNumStates(); i++)
    {
        if( 0 == strcmp(skeletonClass->GetStateAt(i).GetName().Get(), exportServer->GetSettings().resourceName.Get() ))
        {
            oldAnim = i;
            return true;
        }
    }

    // wrong animState
    oldAnim = -1;
    return false;
}


//------------------------------------------------------------------------------
/**
    perception offset is 90% of bounding box (last frame) data
    change % with proportion variable
*/
void
n3dsAnimationExport::FindPerceptionOffset()
{
    float proportion = 0.9f;
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();
    IGameScene* pIgame = n3dsExportServer::Instance()->GetIGameScene();
    TimeValue anim_end_time = pIgame->GetSceneEndTime();
    Object* obj;
    Box3 boundingBox;
    bbox3 localBox = bbox3();
    bbox3 auxBoundingBox;
    auxBoundingBox.begin_extend();

    n3dsSceneList list = n3dsExportServer::Instance()->GetPostOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::graphics ) );

    for ( ; index != list.End() ; ++index)
    {
        IGameNode* skinnedNode = (*index).GetNode();
        IGameMesh* mesh = static_cast< IGameMesh* > ( skinnedNode->GetIGameObject() );
        if (  (!(skinnedNode->IsNodeHidden() ) && (mesh->IsObjectSkinned())) )
        {
            Matrix3 worldmatrix = skinnedNode->GetWorldTM(0).ExtractMatrix3();
            // get object in last animation frame
            obj = skinnedNode->GetMaxNode()->EvalWorldState(anim_end_time).obj;
            // get bounding box of last animation frame
            obj->GetDeformBBox(anim_end_time, boundingBox, &worldmatrix);

            //correct max and min with respect to nebula system coordinates
            vector3 vmin = systemCoord->MaxtoNebulaVertex(boundingBox.Min());
            vector3 vmax = systemCoord->MaxtoNebulaVertex(boundingBox.Max());

            if(vmin.y > vmax.y)
            {
                float aux = vmin.y;
                vmin.y = vmax.y;
                vmax.y = aux;
            }
            if(vmin.z > vmax.z)
            {
                float aux = vmin.z;
                vmin.z = vmax.z;
                vmax.z = aux;
            }

            //get center and extend for nebula system coordinates
            vector3 centerneb = (vmin+vmax)/2;
            vector3 extendsneb = (vmax-vmin)*0.5f;

            localBox.set(centerneb,extendsneb);
            auxBoundingBox.extend(localBox);
        }
    }
    // get proportion to find perception offset 
    this->perceptionOffset = auxBoundingBox.center() + auxBoundingBox.extents()*proportion;   
}


//------------------------------------------------------------------------------
/**
*/
bool
n3dsAnimationExport::CreateMotionBuilder()
{
    if( this->motionCurve.GetNumKeys() )
    {
	    nAnimBuilder::Group animGroup;
        animGroup.AddCurve( this->motionCurve );
        animGroup.SetNumKeys(this->motionCurve.GetNumKeys());
        animGroup.SetLoopType(nAnimBuilder::Group::CLAMP);
	    animGroup.SetKeyTime(this->animationSettings.fKeyDuration);

        motionBuilder.AddGroup(animGroup);
        motionBuilder.Optimize();
        motionBuilder.FixKeyOffsets();
        
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsAnimationExport::SetMotionCurve( nAnimBuilder::Curve& AnimCurveTrans )
{
    this->motionCurve.SetIpolType(nAnimBuilder::Curve::LINEAR);

    bool motion  = n3dsExportServer::Instance()->GetSettings().exportMotion;
    bool motionX = n3dsExportServer::Instance()->GetSettings().motionX;
    bool motionY = n3dsExportServer::Instance()->GetSettings().motionY;
    bool motionZ = n3dsExportServer::Instance()->GetSettings().motionZ;

    vector4 initialRootPose;
    if( AnimCurveTrans.GetNumKeys() )
    {
        initialRootPose = AnimCurveTrans.GetKeyAt(0).Get();
    }

    for( int keyIdx = 0; keyIdx < AnimCurveTrans.GetNumKeys() && motion  ; keyIdx++)
    {
        nAnimBuilder::Key key;
        key = AnimCurveTrans.GetKeyAt(keyIdx);

        vector4 vector;

        if( motionX )
        {
            vector.x = key.value.x - initialRootPose.x;
            key.value.x = initialRootPose.x;
        }
        if( motionY )
        {
            vector.y = key.value.y - initialRootPose.y;
            key.value.y = initialRootPose.y;
        }
        if( motionZ )
        {
            vector.z = key.value.z - initialRootPose.z;
            key.value.z = initialRootPose.z;
        }
        
        AnimCurveTrans.SetKey(keyIdx, key);
        this->motionCurve.SetKey(keyIdx, nAnimBuilder::Key(vector));
    }
}

