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
#include "n3dsexporters/n3dsmeshshape.h"
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsanimationexport/n3dsanimationexport.h"
#include "n3dsnebularuntime/n3dsnebularuntime.h"
//------------------------------------------------------------------------------
// entity includes

#include "entity/nentityclassserver.h"
//------------------------------------------------------------------------------
#include "animcomp/ncskeletonclass.h"

// tools and scene includes
#include "tools/nskinpartitioner.h"
#include "nscene/nattachmentnode.h"
#include "nscene/nshapenode.h"
#include "nasset/nentityassetbuilder.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    constructor
*/
n3dsSkinShape::n3dsSkinShape(IGameNode* Node, nEntityClass* entityClass) :
    skinShadowMesh(0),
    skinRagShadowMesh(0),
    skinMesh(0),
    ragMesh(0)
{
    //this->assetPathMeshes = nEntityAssetBuilder::GetMeshesPath( entityClass );
    //this->assetPath = nEntityAssetBuilder::GetAssetPath( entityClass );

    this->entityClass = entityClass;
    this->Init( Node );
}

//------------------------------------------------------------------------------
/**
*/
n3dsSkinShape::n3dsSkinShape(IGameNode* Node, const n3dsAssetInfo& asset, const n3dsAssetInfo& ragAsset, nEntityClass* entityClass)  :
    skinShadowMesh(0),
    skinRagShadowMesh(0),
    skinMesh(0),
    ragMesh(0)
{
    this->assetInfo = asset;
    this->ragAssetInfo = ragAsset;

    //this->assetPathMeshes = nEntityAssetBuilder::GetMeshesPathForAsset( asset );
    //this->ragAssetPathMeshes = nEntityAssetBuilder::GetMeshesPathForAsset( ragAsset );
    this->entityClass = entityClass;
    this->Init(Node); 
}

//------------------------------------------------------------------------------
/**
    default constructor
*/
n3dsSkinShape::n3dsSkinShape():
    iNode(0),
    mesh(0),
	skinMesh(0),
    ragMesh(0),
    skeleton(0),
    Name(0),
    skinShadowMesh(0),
    skinRagShadowMesh(0),
    shadowEmitter(false),
    CurrentName(0)
{
    //empty
}
//------------------------------------------------------------------------------
/**
    default destructor
*/
n3dsSkinShape::~n3dsSkinShape()
{ 
    if (this->skeleton)
    {
        n_delete(this->skeleton);
        this->skeleton = 0;
    }

    if (this->skinShadowMesh)
    {
        n_delete(this->skinShadowMesh);
        this->skinShadowMesh = 0;
    }

    if (this->ragMesh)
    {
        n_delete(this->ragMesh);
        this->ragMesh = 0;
    }

    if (this->skinRagShadowMesh)
    {
        n_delete(this->skinRagShadowMesh);
        this->skinRagShadowMesh = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSkinShape::Init(IGameNode* Node) 
{
    n_assert(Node);
    n_assert(Node->GetIGameObject()->GetIGameType()==IGameObject::ObjectTypes::IGAME_MESH);

    // inialize max node and mesh
    this->iNode = Node;
    this->mesh = static_cast < IGameMesh* > ( this->iNode->GetIGameObject() );
    this->shadowEmitter = false;

    // get skeleton
    int lodLevel = n3dsExportServer::Instance()->GetSettings().lod.GetLODof(Node);
    this->skeleton = 0;
    this->skeleton = this->GetSkeleton(lodLevel);
    if( this->skeleton->BonesArray.Size() == 0 )
    {
        N3DSERROR( animexport , ( 0 , "ERROR: There's no skeleton in that scene. (n3dsSkinShape::Init)"));
        return;
    }    

    // initalize skinMesh
    this->skinMesh = 0;
    // get node name
    Name.Format("mesh_%.8X", Node->GetNodeID() ); 
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsSkinShape::CreateMesh()
{
    n_assert(this->iNode);

    n3dsExportServer* exportServer  = n3dsExportServer::Instance();
    nKernelServer*    ks      = exportServer->GetNebula()->GetKernelServer();
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();

    //check if loaded skeleton and scene skeleton have the same number of joints
    ncSkeletonClass *skeletonClass = this->entityClass->GetComponent<ncSkeletonClass>();
    if( ! skeletonClass )
    {
        //open lodLevel-th skeleton
        nString skeletonName = n3dsExportServer::Instance()->GetSettings().skeletonName;
        if(n3dsExportServer::Instance()->GetSettings().lod.GetCountLOD() != 0)
        {
            int lodLevel = n3dsExportServer::Instance()->GetSettings().lod.GetLODof(this->iNode);
            //skeleton name, if lod, add _x
            skeletonName += "_";
            skeletonName += lodLevel;
        }

        ncSkeletonClass *skeletonClass = this->entityClass->GetComponent<ncSkeletonClass>();
        nEntityClass * entityskelClass  = nEntityClassServer::Instance()->GetEntityClass( skeletonName.Get() );
        skeletonClass = entityskelClass->GetComponent<ncSkeletonClass>();
        n_assert( skeletonClass );

        if(entityskelClass && skeletonClass->GetCharacter().GetSkeleton().GetNumJoints() != skeleton->BonesArray.Size())
        {
            N3DSERROR( animexport , ( 0 , "ERROR: Wrong skeleton. Referred skeleton and scene skeleton have different number of joints. (n3dsSkinShape::CreateMesh)"));
            return false;
        }
    }

    nKernelServer::ks->PushCwd( this->assetInfo.node ); //NEW

    nTransformNode*   trNode;

    if (this->mesh->GetNumberOfFaces() > 0)
    {
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

        // begin bbox
        this->box.begin_extend();
        
        IGameMaterial* mtl = this->iNode->GetNodeMaterial();
        this->mesh->SetCreateOptimizedNormalList();

        // check if multimaterial, save mesh and asset
        if ((mtl) && (mtlList->IsMultitype(mtl)))
        {
            this->CreateSkinnedMeshMultiMtl();
        } else
        {
            int matId = mtlList->GetMaterialId(mtl);
            this->CreateSkinnedMeshOneMtl();
            this->SaveSceneFile(matId,0);
        }
    
        trNode->SetLocalBox(this->box);

        //if( exportServer->GetSettings().firstPerson && exportServer->GetCamera())
        //{
        //    // set pivot point to "head"
        //    GMatrix g = exportServer->GetCamera()->GetWorldTM();
        //    transform44 transform = exportServer->GetSystemCoordinates()->MaxToNebulaTransform(g);
        //    trNode->SetPosition( -transform.gettranslation() );
        //}
        //else
        //{
            // pivot point
            trNode->SetPosition( vector3(0,0,0) );
        //}
        nKernelServer::ks->PopCwd();

        return true;
    }

    nKernelServer::ks->PopCwd(); //NEW 

    return true;
}

//------------------------------------------------------------------------------
/**
    fill nmeshbuilder for one material objects
    save mesh and scene data
*/
void 
n3dsSkinShape::CreateSkinnedMeshOneMtl()
{
    n3dsMaterialList* mtlList = n3dsExportServer::Instance()->GetMtlList();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    n3dsTriangle triangle;
    nMeshBuilder::Vertex vertex[3];
    int idxFace;

    IGameMaterial* mtl = this->iNode->GetNodeMaterial();
    int matId = mtlList->GetMaterialId(mtl);

    const int numFaces = this->mesh->GetNumberOfFaces();

    // initialize mesh
    this->skinMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
    n_assert(this->skinMesh);
    this->skinShadowMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
    n_assert( this->skinShadowMesh);


    const int component = this->GetComponents(matId);
    // export faces
    for (idxFace=0; idxFace<numFaces; idxFace++)
    {
        triangle.Set(this->mesh,idxFace);
        triangle.GetVertex(vertex, component, this->iNode, this->skeleton);

        systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
        this->skinMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
    }

    if ( mtlList->Getn3dsMaterial(matId).IsEmitterShadow() )
    {
        this->skinShadowMesh->Copy( *this->skinMesh); // Copy before split
        this->shadowEmitter |= true;
    }
    //skinanimator - partition
    n3dsSkinShape::splitMesh( this->skinMesh , this->fragmentsArray );

    // transform and assign name
    this->TransformLocal(this->skinMesh);
    this->SetCurrentName(0);

    this->RagdollData(matId,0);

    //save mesh and asset
    this->SaveMesh(matId);
    
    this->TransformLocal(this->skinShadowMesh);
    this->ConvertToShadowMesh( this->skinShadowMesh );

    n_delete(this->skinMesh);

}

//------------------------------------------------------------------------------
/**
    fill nmeshbuilder for more than one material objects
    save meshes and scene data    
*/
void 
n3dsSkinShape::CreateSkinnedMeshMultiMtl()
{
    n3dsMaterialList* mtlList = n3dsExportServer::Instance()->GetMtlList();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    IGameMaterial* mtl = this->iNode->GetNodeMaterial();
    const int numFaces = mesh->GetNumberOfFaces();
    
    int i;

    // sort faces by material
    Map* map= n_new_array(Map,numFaces);
    for (i=0;i<numFaces;i++)
    {
        map[i].index = i;
        //get the real material
        map[i].material = this->mesh->GetFace(i)->matID;
    }
    qsort(map, numFaces, sizeof(Map), SortedByMatID);

    i=0;
    n3dsTriangle triangle;
    nMeshBuilder::Vertex vertex[3];
    int countMaterial = 0;

    this->skinShadowMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
    n_assert( this->skinShadowMesh);
    // export meshes and asset
    do
    {
        // initialize mesh
        this->skinMesh = n_new(nMeshBuilder(numFaces * 3, numFaces,1));
        n_assert(this->skinMesh);

        // export faces
        int mat=map[i].material;
        int matId=mtlList->GetSubMaterialId(mtl,mat);
        const int component = this->GetComponents(matId);
        while((i<numFaces)&&(mat==map[i].material))
        {
            triangle.Set(this->mesh,map[i].index);
            triangle.GetVertex(vertex,component,this->iNode, this->skeleton);
            systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
            this->skinMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
            i++;
        }

        if ( mtlList->Getn3dsMaterial(matId).IsEmitterShadow() )
        {
            this->skinShadowMesh->Append( *this->skinMesh );
            this->shadowEmitter |= true;
        }

        //skinanimator - partition
        n3dsSkinShape::splitMesh( this->skinMesh , this->fragmentsArray );

        //transform local and set name
        this->TransformLocal(this->skinMesh);
        
        this->SetCurrentName(countMaterial);

        this->RagdollData(matId, countMaterial, mat);

        //save mesh and asset
        this->SaveMesh(matId);
        this->SaveSceneFile(matId, countMaterial, mat );
        n_delete(this->skinMesh);
        countMaterial++;
   } while ( i<numFaces );
   this->TransformLocal(skinShadowMesh);

   this->ConvertToShadowMesh( this->skinShadowMesh );
   n_delete_array(map);
}

//------------------------------------------------------------------------------
/**
*/
int
__cdecl
n3dsSkinShape::SortedByMatID(const void* elm0, const void* elm1)
{
    Map* i0 = (Map*)elm0;
    Map* i1 = (Map*)elm1;

    if (i0->material != i1->material)
    { 
        return i0->material - i1->material;
    } else
    {
        return i0->index - i1->index;
    }
}

//------------------------------------------------------------------------------
/**
    add mesh extension
*/
void 
n3dsSkinShape::SetCurrentName(int uniqueId)
{
    bool binaryFile = n3dsExportServer::Instance()->GetSettings().binaryResource;
    nString buf;
    CurrentName = Name;
    buf.Format( ".%d", uniqueId);
    CurrentName += buf;

    if (binaryFile)
    {
       this->CurrentName += ".nvx2";
    } else
    {
       this->CurrentName += ".n3d2";
    }
}

//------------------------------------------------------------------------------
/**
    saves mesh with the appropiate format and data
*/
void
n3dsSkinShape::SaveMesh(int material)
{
    nMatTypePropBuilder matProp;

    // set correct mesh location
    nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( this->assetInfo.assetPath );
    meshName+=this->CurrentName;
    
    n3dsExportServer::Instance()->GetMtlList()->Preparen3dsMaterial(material); // Calculate the mesh components
    bool needTangentUV0 = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(material).NeedTangentUV0();
    bool needTangentUV3 = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(material).NeedTangentUV3();
    bool optimizeMesh   = n3dsExportServer::Instance()->GetSettings().optimizeMesh;
    bool binaryFile     = n3dsExportServer::Instance()->GetSettings().binaryResource;

    if ( optimizeMesh)
    {
        this->skinMesh->Cleanup(0);
    }
    
    if (needTangentUV0)
    {
        this->skinMesh->BuildTriangleOnlyTangentBinormal();
    }
    
    if (needTangentUV3)
    {
        this->skinMesh->BuildTriangleOnlyTangentBinormal(3);
    }

    // Remove vertex after  build normals because thi method add new vertex
    int numVertexBeforeClean = skinMesh->GetNumVertices(); 
    bool triangleisDelete = skinMesh->CleanVertex(); // remove vertices not referenced and use only 65535 vertex


    if ( optimizeMesh)
    {
         skinMesh->Optimize();
    }
        
    N3DSERRORCOND( /*name*/  meshexport , 
                /*COND*/ triangleisDelete ,
                /*LOG*/( 0, "ERROR: \"%s\" has has too many vertexes,  %i \n" , iNode->GetName() , numVertexBeforeClean) 
            );
      
    if (binaryFile)
    {
      this->skinMesh->SaveNvx2(nKernelServer::Instance()->GetFileServer(),meshName.Get());
    } else
    {
      this->skinMesh->SaveN3d2(nKernelServer::Instance()->GetFileServer(),meshName.Get());
    }
}

//------------------------------------------------------------------------------
/**
    save scene data
*/
void
n3dsSkinShape::SaveSceneFile(int material, int uniqueId , int subMaterialId)
{
    nKernelServer*  ks = nKernelServer::ks;
    nMatTypePropBuilder matProp;

    //set name
    nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( this->assetInfo.assetPath );
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
    skingeom->BeginJointPalette(this->fragmentsArray[0].BonePaletteArray.Size());
    for(int j=0;j<this->fragmentsArray[0].BonePaletteArray.Size();j++)
	{       
        skingeom->SetJointIndex(j,this->fragmentsArray[0].BonePaletteArray[j]);
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
                     /*LOG*/( 0 , "ERROR: \"%s\" has a incorrect material \"%s\"\n" , iNode->GetName() , mtlList->Getn3dsMaterial( material ).GetName() )
                   );
        //@todo remove this warning when the invalid shader will be a invalid material
        N3DSWARNCOND( /*name*/  meshexport , 
                    /*COND*/ mtlList->Getn3dsMaterial( material ).IsInValidShaderType() ,
                    /*LOG*/( 0, "WARNING: \"%s\" has a invalid shader \"%s\"\n" , iNode->GetName() , mtlList->Getn3dsMaterial( material ).GetName() )
                   );
    }

    ks->PushCwd(skingeom);

    // export attachements
    this->ExportAttachments();
    
    ks->PopCwd();//skinGeom

    ks->PopCwd();//currentShape
}

//------------------------------------------------------------------------------
/**
    transform mesh data to local coordinates
*/
void 
n3dsSkinShape::TransformLocal(nMeshBuilder *localMesh)
{
    localMesh->Transform(this->transMatrixInv);
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsSkinShape::GetComponents(int matId)
{    
    int matComponents = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(matId).GetMeshComponents();
    matComponents |= nMeshBuilder::Vertex::Component::WEIGHTS;
    matComponents |= nMeshBuilder::Vertex::Component::JINDICES;
    int components = matComponents & n3dsTriangle::GetMeshComponents(this->mesh);
    return components;
}

//------------------------------------------------------------------------------
/**
    @return skeleton
*/
n3dsSkeleton*
n3dsSkinShape::GetSkeleton(int lodLevel)
{
    if (this->skeleton == 0)
    {
        //create skeleton from scene
        this->skeleton = n_new(n3dsSkeleton(lodLevel)); 
        n_assert(this->skeleton);
    }
    return this->skeleton;
}

//------------------------------------------------------------------------------
/**
    export static attachments
*/
void
n3dsSkinShape::ExportAttachments()
{
    nKernelServer*  ks = nKernelServer::ks;
    vector3 trans, scale;
    quaternion quat;

    char buf[255];
    // loop attachments
    for(int index =0; index < this->skeleton->AttachmentArray.Size(); index++)
    {
        // get node
        IGameNode* attachedNode = this->skeleton->AttachmentArray.At(index).iObject;
        int boneID = this->skeleton->AttachmentArray.At(index).iBoneID;

        IGameObject::ObjectTypes type = attachedNode->GetIGameObject()->GetIGameType();
        if( (!attachedNode->IsNodeHidden()) && (type == IGameObject::ObjectTypes::IGAME_MESH) )
        {
            //create node
            sprintf(buf,"attach.%d",index);
            nAttachmentNode* attNode;
            attNode = static_cast < nAttachmentNode* >( ks->New("nattachmentnode", buf) );
            attNode->SetJointByIndex(boneID);

            IGameNode* parentNode = attachedNode->GetNodeParent();

            // aplicate bone transformation
            this->PlaceAttachment(parentNode, trans, quat, scale);

            attNode->SetPosition(trans);
            attNode->SetQuat(quat);

            // export attached mesh
            IGameMesh * gM = static_cast < IGameMesh * > ( attachedNode->GetIGameObject() );
            if(gM->InitializeData())
            {
                ks->PushCwd(attNode);
                n3dsMeshShape *myShape= n_new(n3dsMeshShape(attachedNode, this->assetInfo.assetPath ) );
                myShape->CreateMesh();
                n_delete(myShape);
                ks->PopCwd();
            }
        }
    }
    // clear attachments array
    this->skeleton->AttachmentArray.Clear();
}

//------------------------------------------------------------------------------
/**
    places static attachments. Computes "world" (character) transformations.
*/
void
n3dsSkinShape::PlaceAttachment(IGameNode* parentNode, vector3 &trans, quaternion &quater, vector3 &scale)
{
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    Matrix3 maxmatrix= Matrix3(true);
    //calculate inherited transform matrix
    while( parentNode )
    {	
        IGameControl *parentControl;
        IGameKeyTab Key;
        parentControl = parentNode->GetIGameControl();
        parentControl->GetFullSampledKeys(Key,n3dsAnimationExport::Instance()->GetAnimationSettings().iFramesPerSample,IGAME_TM );
		Matrix3 tm;

        tm = Key[0].sampleKey.gval.ExtractMatrix3();
        maxmatrix*=tm;
        parentNode = parentNode->GetNodeParent();
    }

    maxmatrix.Invert();
    maxmatrix.NoScale();
    //convert to nebula coordinates
    matrix44d nebMatrix = systemCoord->MaxToNebulaMatrix(maxmatrix);
    //get transformations
    nebMatrix.get(scale,quater,trans);

    if (scale.z<0)
    {
        scale.z=-scale.z;
    }
}

//------------------------------------------------------------------------------
/**
*/ void 
n3dsSkinShape::splitMesh( nMeshBuilder* mesh, nArray<n3dsMeshFragment>& fragmentsArray)
{
    //skinanimator - partition
    nMeshBuilder tmpMeshBuilder;
    nSkinPartitioner SkinPartitioner;
    fragmentsArray.Reallocate(5,5);
    if (SkinPartitioner.PartitionMesh(*mesh, tmpMeshBuilder, maxJointPaletteSize))
    {
	    const nArray<int>& GroupMapArray = SkinPartitioner.GetGroupMappingArray();
	    for(int i=0;i<GroupMapArray.Size();i++)
	    {
		    if (GroupMapArray[i]==0)
		    {
			    n3dsMeshFragment MeshFragment;
			    MeshFragment.iGroupMappingIndex = i;
			    MeshFragment.BonePaletteArray = SkinPartitioner.GetJointPalette(i);
			    fragmentsArray.Append(MeshFragment);
		    }
	    }
	    *mesh = tmpMeshBuilder;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsSkinShape::IsShadowEmitter()
{
    return this->shadowEmitter;
}