#include "precompiled/pchn3dsmaxexport.h"

#pragma warning( push, 3 )
#include "IGame/IGame.h"
#include "IGAME/IGameObject.h"
#pragma warning( pop )
//------------------------------------------------------------------------------
// exporter includes

#include "n3dsexporters/n3dsskinshape.h"
#include "n3dsexporters/n3dstriangle.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsmeshshape.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsnebularuntime/n3dsnebularuntime.h"
#include "n3dsanimationexport/n3dsanimationexport.h"


#include "entity/nentityclassserver.h"
#include "n3dsexportsettings/n3dsExportSettings.h"
//------------------------------------------------------------------------------

// tools and scene includes
#include "tools/nskinpartitioner.h"
#include "nscene/nskinshapenode.h"
#include "kernel/nfileserver2.h"

#include "tools/nskinpartitioner.h"
#include "nscene/nattachmentnode.h"
#include "nasset/nentityassetbuilder.h"

//------------------------------------------------------------------------------
// constants

const int numjindices = 4;

//------------------------------------------------------------------------------
/**
*/
bool
n3dsSkinShape::RagdollData(int material, int uniqueId , int subMaterialId)
{
    if( this->skeleton->RagBonesArray.Size() != 0 )
    {
        n3dsExportServer* exportServer  = n3dsExportServer::Instance();
        nKernelServer*    ks      = exportServer->GetNebula()->GetKernelServer();

        ks->PushCwd( this->ragAssetInfo.node );

        nTransformNode*   trNode;
        // create node
        trNode = (nTransformNode* )ks->New("ntransformnode" , this->Name.Get());
        n_assert(trNode);
        trNode->AppendCommentFormat("Max name \"%s\"\n", iNode->GetName() );
        ks->PushCwd(trNode);

        // get transformation matrix and set node transformation
        GMatrix g=this->iNode->GetWorldTM();
        transform44 transform = exportServer->GetSystemCoordinates()->MaxToNebulaTransform(g);

        trNode->SetScale( transform.getscale() );
        trNode->SetQuat( transform.getquatrotation() );
        trNode->SetPosition(transform.gettranslation() );

        transMatrixInv=trNode->GetTransform();
        transMatrixInv.invert();

        this->box.begin_extend();
     
        this->CreateRagdollMesh();
        this->SaveRagdollSceneFile( material, uniqueId, subMaterialId);
        this->SaveRagdollMesh(material);

        trNode->SetLocalBox(this->box);
        trNode->SetPosition( vector3(0,0,0) );
        ks->PopCwd();

        ks->PopCwd();

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSkinShape::CreateRagdollMesh()
{
    n_assert(this->iNode);

    int numvertices = this->skinMesh->GetNumVertices();
    nMeshBuilder::Vertex vertex;

    this->ragMesh = n_new(nMeshBuilder(numvertices, numvertices/3 ,1));
    n_assert( this->ragMesh);

    this->skinRagShadowMesh = n_new(nMeshBuilder(numvertices, numvertices/3, 1));
    n_assert( this->skinRagShadowMesh);

    this->ragMesh->Copy( *this->skinMesh);

    vector4 jindices, jweights;

    //for each vertex
    for( int i=0; i < numvertices ; i++)
    {
        vertex = this->ragMesh->GetVertexAt(i);
        jindices = vertex.GetJointIndices();
        jweights = vertex.GetWeights();

        //fullskeletonindices
        nArray<int> indices;
        indices.Append(this->fragmentsArray[0].BonePaletteArray[(int)jindices.x]);
        indices.Append(this->fragmentsArray[0].BonePaletteArray[(int)jindices.y]);
        indices.Append(this->fragmentsArray[0].BonePaletteArray[(int)jindices.z]);
        indices.Append(this->fragmentsArray[0].BonePaletteArray[(int)jindices.w]);
        //correspondant weights
        nArray<float> weights;
        weights.Append( jweights.x );
        weights.Append( jweights.y );
        weights.Append( jweights.z );
        weights.Append( jweights.w );

        //idem for ragdoll skeleton
        nArray<int> ragIndices;
        nArray<float> ragWeights;

        //for each affected joint
        for( int j = 0; j< indices.Size() ; j++)
        {
            int ragIndex = skeleton->RagCorresp.FindIndex(indices[j]);
            
            //it isn't a ragjoint
            if( ragIndex == -1 )
            {
                //find ragbone idx
                n3dsBone theBone = skeleton->BonesArray[indices[j]];
                
                nString removedBip = theBone.strBoneName.ExtractRange(5, theBone.strBoneName.Length() - 5);
                
                // ugly hack -> in max, fore_twist bones parent is UpperArm instead of ForeArm
                // when creating ragdoll mesh that influences must affect forearm, not upperarm
                if( strstr(removedBip.Get(), "Twist" ))
                {
                    //if left side bone
                    if( strstr(removedBip.Get(), "_L_"))
                    {
                        ragIndex = skeleton->FindBoneThatContains( "L_Forearm" );
                        ragIndex = skeleton->RagCorresp.FindIndex(ragIndex);
                        n_assert(ragIndex != -1);
                    }
                    //if right side bone
                    else
                    {
                        ragIndex = skeleton->FindBoneThatContains( "R_Forearm" );
                        ragIndex = skeleton->RagCorresp.FindIndex(ragIndex);
                        n_assert(ragIndex != -1);
                    }
                }
                //usual case
                else
                {
                    ragIndex = skeleton->FindRagParent(theBone); //return the index of the ragskeleton, not the full one
                    n_assert(ragIndex != -1);
                }
            }
            //set index and weight
            ragIndices.Append( ragIndex );
            ragWeights.Append( weights[j] );
        }

        //fullfil arrays
        while (ragIndices.Size() < numjindices) //maximum of 4 affected joints
        {
            ragIndices.Append(0);
            ragWeights.Append(0);
        }

        jindices.set( (float)ragIndices[0], (float)ragIndices[1], (float)ragIndices[2], (float)ragIndices[3] ); 
        jweights.set( weights[0], weights[1], weights[2], weights[3] );

        //set data to vertex
        vertex.SetJointIndices(jindices);
        vertex.SetWeights(jweights);
        this->ragMesh->SetVertex( i, vertex);
    }

    if ( this->shadowEmitter )
    {
        this->skinRagShadowMesh->Copy( *this->ragMesh); // Copy before split
    }

    this->ConvertToShadowMesh( this->skinRagShadowMesh );
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSkinShape::SaveRagdollMesh(int material)
{
    nMatTypePropBuilder matProp;

    // set correct mesh location
    nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( this->ragAssetInfo.assetPath );
    meshName+= "ragdoll_";
    meshName+=this->CurrentName;

    bool needTangentUV0 = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(material).NeedTangentUV0();
    bool needTangentUV3 = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(material).NeedTangentUV3();
    bool optimizeMesh   = n3dsExportServer::Instance()->GetSettings().optimizeMesh;
    bool binaryFile     = n3dsExportServer::Instance()->GetSettings().binaryResource;

    if ( optimizeMesh)
    {
        this->ragMesh->Cleanup(0);
    }
    
    if (needTangentUV0)
    {
        this->ragMesh->BuildTriangleOnlyTangentBinormal();
    }
    
    if (needTangentUV3)
    {
        this->ragMesh->BuildTriangleOnlyTangentBinormal(3);
    }


    // Remove vertex after  build normals because thi method add new vertex
    int numVertexBeforeClean = skinMesh->GetNumVertices(); 
    bool triangleisDelete = skinMesh->CleanVertex(); // remove vertices not referenced and use only 65535 vertex

    if (optimizeMesh)
    {
         this->ragMesh->Optimize();
    }
        
    N3DSERRORCOND( /*name*/  meshexport , 
                /*COND*/ triangleisDelete ,
                /*LOG*/( 0, "ERROR: \"%s\" has has too many vertexes,  %i \n" , iNode->GetName() , numVertexBeforeClean) 
            );
    
      
    if (binaryFile)
    {
      this->ragMesh->SaveNvx2(nKernelServer::Instance()->GetFileServer(),meshName.Get());
    } else
    {
      this->ragMesh->SaveN3d2(nKernelServer::Instance()->GetFileServer(),meshName.Get());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSkinShape::SaveRagdollSceneFile(int material, int uniqueId , int subMaterialId)
{
    nKernelServer*  ks = nKernelServer::ks;
    nMatTypePropBuilder matProp;


    //set name
    nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( this->ragAssetInfo.assetPath );
    meshName+= "ragdoll_";
    meshName+=this->CurrentName;

    char buf[255];
    sprintf(buf,"shape.%i", uniqueId);

    nShapeNode *currentShape;
    
    currentShape = (nShapeNode* )ks->New("nshapenode" , buf );
    ks->PushCwd(currentShape);
    n_assert(currentShape);

    currentShape->SetMesh(meshName.Get());
    currentShape->SetGroupIndex(0);

    //a bounding box is needed, it's not used in scene
    this->box.set( vector3(0,1,0), vector3(0,0,0));
    currentShape->SetLocalBox(this->box);

    // skingeometrynode
    nSkinGeometryNode* skingeom;

    skingeom = static_cast < nSkinGeometryNode* > ( ks->New("nskingeometrynode", "skin") );

    skingeom->SetStateChannel("charState");

    skingeom->BeginJointPalette(10);
    for(int j=0;j<10;j++)
	{       
        skingeom->SetJointIndex(j,j);
	}
    skingeom->EndJointPalette();

    if ( n3dsExportServer::Instance()->GetMtlList()->IsShaderNode(material))
    {
        skingeom->SetInt(nShaderState::isSkinned, 1);
    }
    matProp.AddParam("deform", "skinned");
    
    // set material
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();  
    mtlList->SetMaterial(skingeom, material,&matProp);

    if (subMaterialId > 0) // If is a submaterial
    {
        N3DSERRORCOND( /*name*/  meshexport , 
                     /*COND*/ ! mtlList->Getn3dsMaterial( material ).IsValid() ,
                     /*LOG*/( 0, "ERROR: \"%s\" has a incorrect submaterial number \"%i\"\n" , iNode->GetName() , subMaterialId +1 )//UI Max begin by 1
                   );
        ///@todo remove this warning when the invalid shader will be a invalid material
        N3DSWARNCOND( /*name*/  meshexport , 
                    /*COND*/ mtlList->Getn3dsMaterial( material ).IsInValidShaderType() ,
                    /*LOG*/( 0, "WARNING: \"%s\" has a invalid shader number \"%i\"\n" , iNode->GetName() , subMaterialId + 1 ) //UI Max begin by 1
                   );
    } else
    {
        N3DSERRORCOND( /*name*/  meshexport , 
                     /*COND*/ ! mtlList->Getn3dsMaterial( material ).IsValid() ,
                     /*LOG*/( 0, "ERROR: \"%s\" has a incorrect material \"%s\"\n" , iNode->GetName() , mtlList->Getn3dsMaterial( material ).GetName() )
                   );
        //@todo remove this warning when the invalid shader will be a invalid material
        N3DSWARNCOND( /*name*/  meshexport , 
                    /*COND*/ mtlList->Getn3dsMaterial( material ).IsInValidShaderType() ,
                    /*LOG*/( 0, "WARNING: \"%s\" has a invalid shader \"%s\"\n" , iNode->GetName() , mtlList->Getn3dsMaterial( material ).GetName() )
                   );
    }


    ks->PushCwd(skingeom);

    // export attachements
    this->ExportRagdollAttachments();
    
    ks->PopCwd();//skinGeom

    ks->PopCwd();//currentShape
}

//------------------------------------------------------------------------------
/**
    export static attachments
*/
void
n3dsSkinShape::ExportRagdollAttachments()
{
    IGameScene* pIgame = n3dsExportServer::Instance()->GetIGameScene();
    nKernelServer*  ks = nKernelServer::ks;
    vector3 trans, scale;
    quaternion quat;

    char buf[255];
    // loop attachments
    for(int index =0; index < this->skeleton->AttachmentArray.Size(); index++)
    {
        // get node
        IGameNode* attachedNode = this->skeleton->AttachmentArray.At(index).iObject;
        int fullskBoneID = this->skeleton->AttachmentArray.At(index).iBoneID;

        //special code for ragdolls//
        n3dsBone fullSkBone = this->skeleton->BonesArray[fullskBoneID];
        int ragBoneID = this->skeleton->FindRagParent(fullSkBone);
        ////

        IGameObject::ObjectTypes type = attachedNode->GetIGameObject()->GetIGameType();
        if( (!attachedNode->IsNodeHidden()) && (type == IGameObject::ObjectTypes::IGAME_MESH) )
        {
            //create node
            sprintf(buf,"attach.%d",index);
            nAttachmentNode* attNode;
            attNode = static_cast < nAttachmentNode* >( ks->New("nattachmentnode", buf) );
            attNode->SetJointByIndex(ragBoneID);

            int fullskboneid = this->skeleton->RagCorresp[ragBoneID];
            
            IGameNode* parentNode = pIgame->GetIGameNode(this->skeleton->BonesArray[fullskboneid].iNodeId);

            this->PlaceAttachment( parentNode, trans, quat, scale);

            attNode->SetPosition(trans);
            attNode->SetQuat(quat);

            // export attached mesh
            IGameMesh * gM = static_cast < IGameMesh * > ( attachedNode->GetIGameObject() );
            if(gM->InitializeData())
            {
                ks->PushCwd(attNode);
                n3dsMeshShape *myShape= n_new(n3dsMeshShape(attachedNode, this->ragAssetInfo.assetPath ) );
                myShape->CreateMesh();
                n_delete(myShape);
                ks->PopCwd();
            }
        }
    }
    // clear attachments array
    //this->skeleton->AttachmentArray.Clear();
}
