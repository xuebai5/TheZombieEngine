#include "precompiled/pchn3dsmaxexport.h"
//------------------------------------------------------------------------------
#include "n3dsexporters/n3dsblendskinshape.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dstriangle.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsnebularuntime/n3dsnebularuntime.h"
#include "n3dsmaterial/n3dsmateriallist.h"
//------------------------------------------------------------------------------
#include "nscene/nblendshapenode.h"
#include "nscene/nskinshapenode.h"
#include "nscene/nskingeometrynode.h"
#include "nscene/ntransformnode.h"
#include "tools/nskinpartitioner.h"
#include "nasset/nentityassetbuilder.h"
//------------------------------------------------------------------------------

const int nameBufferSize = 255;
const int maxmorphtargets = 5;

//------------------------------------------------------------------------------
/**
    constructor
*/
n3dsBlendSkinShape::n3dsBlendSkinShape(IGameNode* Node, nEntityClass* entityClass):
    n3dsBlendShape(Node, entityClass),
    n3dsSkinShape(Node, entityClass)
{
    ;
}

//------------------------------------------------------------------------------
/**
    constructor
*/
n3dsBlendSkinShape::n3dsBlendSkinShape(IGameNode* Node, const n3dsAssetInfo& asset, const n3dsAssetInfo& ragAsset, nEntityClass* entityClass):
    n3dsBlendShape(Node, asset, entityClass),
    n3dsSkinShape(Node, asset, ragAsset, entityClass)
{
    ;
}

//------------------------------------------------------------------------------
/**
    fill scene file, called for blended-skinned nodes
*/
void
n3dsBlendSkinShape::SaveSceneFile(int material, int uniqueId, int subMaterialId )
{
    nKernelServer*  ks = nKernelServer::ks;
    nMatTypePropBuilder matProp;

    nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( this->n3dsSkinShape::assetInfo.assetPath );
    meshName+=this->CurrentName;

    //nString meshName = nEntityAssetBuilder::GetMeshesPath( n3dsSkinShape::entityClass );
    //meshName+=this->CurrentName.Get();

    char buf[255];
    sprintf(buf,"shape.%i",uniqueId);

    nShapeNode *currentShape;
    
    currentShape = (nShapeNode* )ks->New("nblendshapenode" , buf);
    ks->PushCwd(currentShape);
     
    n_assert(currentShape);

    nBlendShapeNode* currentBlendShape = (nBlendShapeNode*) currentShape;

    //the skinned target (initial)
    currentBlendShape->SetMeshAt(0, meshName.Get());
    currentBlendShape->SetWeightAt(0, 0.00);

    //morph targets
    if ( ( Names.Size() - 1 ) > maxmorphtargets )
    {
        n_message("More targets than the allowed, meshes will be saved correctly, animation won't");
    }

    for(int i = 1 ; (i < (Names.Size()-1))&&(i < maxmorphtargets); i++) //the last target is the neutral one
    {
        nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( this->n3dsSkinShape::assetInfo.assetPath );

        //nString meshName = nEntityAssetBuilder::GetMeshesPath( n3dsSkinShape::entityClass );
        meshName+=Names.At(i).Get();

        currentBlendShape->SetMeshAt(i, meshName.Get());
        currentBlendShape->SetWeightAt(i, 0.00);
    }

    //add animators and other properties
    currentBlendShape->AddAnimator("/usr/animators/anims/blendanimator");
    currentBlendShape->SetGroupIndex(0);

    //skingeometrynode
    sprintf(buf,"skin.%i",material);
    nSkinGeometryNode* skingeom;

    skingeom = static_cast< nSkinGeometryNode* >( ks->New("nskingeometrynode", buf) );

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

    matProp.AddParam("deform", "morpher");
 

    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();  
    mtlList->SetMaterial(skingeom, material,&matProp);

    if (subMaterialId > 0) // If is a submaterial
    {
        N3DSERRORCOND( /*name*/  meshexport , 
                     /*COND*/ ! mtlList->Getn3dsMaterial( material ).IsValid() ,
                     /*LOG*/( 0, "ERROR: \"%s\" has a incorrect submaterial number \"%i\"\n" , iNode->GetName() , subMaterialId + 1 ) //UI Max begin by 1
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

    ks->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsBlendSkinShape::CreateMeshMultiMtl(int matmorpherId, int nummorphvertexs)
{
    int i;
    const int numFaces = mesh->GetNumberOfFaces();
    IGameMaterial* mtl = this->iNode->GetNodeMaterial();

    //singleton material list
    n3dsMaterialList* mtlList = n3dsExportServer::Instance()->GetMtlList();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    n3dsSkinShape::Map* map= n_new_array(n3dsSkinShape::Map,numFaces);
    for (i=0;i<numFaces;i++)
    {
        map[i].index = i;
        //get the real material
        map[i].material = n3dsSkinShape::mesh->GetFace(i)->matID;
    }

    qsort(map, numFaces, sizeof(n3dsSkinShape::Map), n3dsSkinShape::SortedByMatID);

    i=0;
    n3dsTriangle triangle;
    nMeshBuilder::Vertex vertex[3];
    int countMaterial = 0;

    do
    {
        n3dsSkinShape::skinMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
        n_assert(n3dsSkinShape::skinMesh);

        int mat=map[i].material;
        int matId=mtlList->GetSubMaterialId(mtl,mat);
        const int component = n3dsSkinShape::GetComponents(matId);

        while((i<numFaces)&&(mat==map[i].material))
        {
            triangle.Set(n3dsSkinShape::mesh,map[i].index);
            triangle.GetVertex(vertex,component,this->iNode, skeleton);
            systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
            n3dsSkinShape::skinMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
            i++;
        }
		nMeshBuilder TmpMeshBuilder;
		nSkinPartitioner SkinPartitioner;
        this->fragmentsArray.Reallocate(5,5);
        if (SkinPartitioner.PartitionMesh(*n3dsSkinShape::skinMesh,TmpMeshBuilder,maxJointPaletteSize))
		{
			const nArray<int>& GroupMapArray = SkinPartitioner.GetGroupMappingArray();
			for(int i=0;i<GroupMapArray.Size();i++)
			{
				if (GroupMapArray[i]==0)
				{
					n3dsMeshFragment MeshFragment;
					MeshFragment.iGroupMappingIndex = i;
					MeshFragment.BonePaletteArray = SkinPartitioner.GetJointPalette(i);
					this->fragmentsArray.Append(MeshFragment);
				}
			}
            *n3dsSkinShape::skinMesh = TmpMeshBuilder;
		}

        this->n3dsSkinShape::TransformLocal(n3dsSkinShape::skinMesh);

        this->SetCurrentName(matId);

        this->SaveMesh(matId);
        
        if(matmorpherId == matId)
        {
            if ( n3dsSkinShape::skinMesh->GetNumVertices() == nummorphvertexs )
            {
                this->SaveSceneFile(matId, countMaterial, mat);
            }
            else
            {
                n_message("Different number of vertexs blendmesh and skinmesh");
            }

        }
        else
        {   
            n3dsSkinShape::SaveSceneFile(matId, countMaterial, mat);
        }
        n_delete(n3dsSkinShape::skinMesh);
        countMaterial++;

   } while ( i<numFaces );
   n_delete_array(map);
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsBlendSkinShape::CreateMesh()
{
    nTransformNode*   trNode;

    n3dsExportServer* Server  = n3dsExportServer::Instance();
    nKernelServer*    ks      = Server->GetNebula()->GetKernelServer();
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();
    IGameMaterial* mtl;
    int matmorpherId = 0; //initialized to a non-used value

    //do not optimize mesh any mesh if there's skinning and blending
    n3dsExportServer::Instance()->SetOptimizeMeshData( false );

    n_assert(this->targets.At(0));
    n_assert(this->iNode);

    GMatrix g=this->targets.At(0)->GetWorldTM(); // PIVOT POINT 
    transform44 transform = Server->GetSystemCoordinates()->MaxToNebulaTransform(g);
    localTransMatrix= transform.getmatrix();   

    //create targets (not the initial one, ie, last one array)
    int nummorphvertexs = 0;

    mtl = this->targets.At(0)->GetNodeMaterial();

    if ((mtl) && (mtlList->IsMultitype(mtl)))
    {
        //int numFaces = this->targetmesh[1]->GetNumberOfFaces();
        int mat = this->targetmesh[1]->GetFace(0)->matID;
        matmorpherId = mtlList->GetSubMaterialId(mtl,mat);
    }
    else
    {
        matmorpherId = mtlList->GetMaterialId(mtl);
    }


    for(int i=1; i< (targets.Size()-1); i++)
    {
        this->CreateTarget(i, nMeshBuilder::Vertex::Component::COORD | nMeshBuilder::Vertex::Component::NORMAL );
        this->SetCurrentTargetName(i, matmorpherId);
        this->SaveTarget(i, matmorpherId);
        nummorphvertexs = n3dsBlendShape::blendMesh->GetNumVertices();
        n_delete(n3dsBlendShape::blendMesh);
    }

    if (this->mesh->GetNumberOfFaces() > 0)
    {
        trNode = (nTransformNode* )ks->New("ntransformnode" , this->Name.Get());
        n_assert(trNode);
        ks->PushCwd(trNode);

        trNode->SetScale( transform.getscale() );
        trNode->SetQuat( transform.getquatrotation() );
        trNode->SetPosition(transform.gettranslation() );

        this->n3dsSkinShape::box.begin_extend();

        mtl = this->iNode->GetNodeMaterial();
        this->mesh->SetCreateOptimizedNormalList();

        if ((mtl) && (mtlList->IsMultitype(mtl)))
        {
            this->CreateMeshMultiMtl( matmorpherId , nummorphvertexs);
        } else
        {
            int matId = mtlList->GetMaterialId(mtl);
            n3dsSkinShape::CreateSkinnedMeshOneMtl();
            this->SaveSceneFile(matId, 0);
        }
    
        nKernelServer::ks->PopCwd();
        return true;
    } else
    {
        return false;
    }

}

//------------------------------------------------------------------------------
/**
*/
bbox3 & 
n3dsBlendSkinShape::GetBBox()
{
    return this->n3dsSkinShape::GetBBox();
}