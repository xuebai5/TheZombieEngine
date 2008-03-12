#include "precompiled/pchn3dsmaxexport.h"
#pragma warning( push, 3 )
#include "IGAME/IGameObject.h"
#pragma warning( pop )

#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsexporters/n3dsmeshshape.h"
#include "n3dsexporters/n3dstriangle.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsnebularuntime/n3dsnebularuntime.h"
//------------------------------------------------------------------------------
#include "nscene/nshapenode.h"
#include "nscene/ntransformnode.h"
//------------------------------------------------------------------------------
#include "nasset/nentityassetbuilder.h"

#define MR3_NUM_CHANNELS	100

const int nameBufferSize = 255;


//------------------------------------------------------------------------------
/**
*/
n3dsMeshShape::n3dsMeshShape(IGameNode* Node, const nString& assetPath) 
{
    this->assetPathMeshes = nEntityAssetBuilder::GetMeshesPathForAsset( assetPath );
    this->Init(Node); 
}

//------------------------------------------------------------------------------
/**
*/
n3dsMeshShape::n3dsMeshShape(IGameNode* Node, nEntityClass* entityClass) 
{
    this->assetPathMeshes = nEntityAssetBuilder::GetMeshesPath( entityClass );
    this->Init( Node );
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsMeshShape::Init(IGameNode* Node) 
{
    n_assert(Node);
    n_assert(Node->GetIGameObject()->GetIGameType()==IGameObject::ObjectTypes::IGAME_MESH);

    iNode=Node;
    mesh=(IGameMesh *)this->iNode->GetIGameObject();

    myMesh = 0;
    shadowEmitter = false;

    //Name.Format("mesh_%.8X.%s" , Node->GetNodeID(), Node->GetName() );
    //Name.ConvertSpace();
    Name.Format("mesh_%.8X", Node->GetNodeID() );
 
}

//------------------------------------------------------------------------------
/**
    Export a mesh
    Create a shapenode in scnee ( the current kernel server) 
*/
bool
n3dsMeshShape::CreateMesh()
{
    n_assert( nKernelServer::Instance()->GetCwd()->IsA("nscenenode") );
    nTransformNode*   trNode;
    vector3 pos;
    vector3 scale;
    quaternion quat;

    n_assert(this->iNode);

    n3dsExportServer* Server  = n3dsExportServer::Instance();
    nKernelServer*    ks      = Server->GetNebula()->GetKernelServer();
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();

    if (mesh->GetNumberOfFaces() > 0)
    {
        GMatrix g=this->iNode->GetWorldTM(); // PIVOT POINT TRANSFORM
        matrix44d worldMatrix = Server->GetSystemCoordinates()->MaxToNebulaMatrix(g);


        trNode = static_cast<nTransformNode*>(ks->New( "ntransformnode" , Name.Get() ) );
        n_assert(trNode);
        trNode->AppendCommentFormat("Max name \"%s\"\n", iNode->GetName() );
        ks->PushCwd(trNode);

        matrix44 myMatrix;//= Server->GetSystemCoordinates()->MaxToNebulaMatrix(g);

        // export in world coordinates
        //if(Server->GetSettings().exportMode != n3dsExportSettings::Indoor )
        //{
        //    //GMatrix g = this->mesh->GetIGameObjectTM(); // OBECT COORDINATES
        //    GMatrix g=this->iNode->GetWorldTM(); // PIVOT POINT TRANSFORM
        //    transform44 transform = Server->GetSystemCoordinates()->MaxToNebulaTransform(g);

        //    trNode->SetScale( transform.getscale() );
        //    trNode->SetQuat( transform.getquatrotation() );
        //    trNode->SetPosition(transform.gettranslation() );

        //    myMatrixInv=trNode->GetTransform();
        //    myMatrixInv.invert();       
        //}

        this->box.begin_extend();
        
        IGameMaterial* mtl = this->iNode->GetNodeMaterial();
        this->mesh->SetCreateOptimizedNormalList();
        //n_assert(mtl);

        if ((mtl) && (mtlList->IsMultitype(mtl)))
        {
            this->CreateMesh_multiMtl();
        } else
        {
            this->CreateMesh_oneMtl();
        }
    
        trNode->SetLocalBox(this->box);
    
        nKernelServer::ks->PopCwd();
        return true;
    } else
    {
        return false;
    }
}

void 
n3dsMeshShape::CreateMesh_oneMtl()
{
    const int numFaces = mesh->GetNumberOfFaces();
    n3dsMaterialList* mtlList = n3dsExportServer::Instance()->GetMtlList();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    n3dsTriangle triangle;
    nMeshBuilder::Vertex vertex[3];
    int idxFace;
    IGameMaterial* mtl = this->iNode->GetNodeMaterial();
    int matId = mtlList->GetMaterialId(mtl);
    // int mat=this->iNode->GetMaterialIndex();

    myMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
    n_assert(myMesh);

    const int component = GetComponents(matId);
    for (idxFace=0; idxFace<numFaces; idxFace++)
    {
          triangle.Set(mesh,idxFace);
          triangle.GetVertex(vertex, component,this->iNode);

          systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
          myMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
    }

    this->TransformLocal(myMesh);
    nMeshBuilder* shadowMesh = n_new(nMeshBuilder(*myMesh)); // Create copy before save "myMesh"
    this->SetCurrentName(0);
    this->Save(matId,0);
    n_delete(myMesh);
    if ( mtlList->Getn3dsMaterial(matId).IsEmitterShadow() &&
         shadowMesh->GetNumVertices() > 0 &&
         n3dsExportServer::Instance()->GetSettings().exportMode != n3dsExportSettings::Indoor)
    {
        shadowEmitter |= true;
        this->ConvertToShadowMesh( shadowMesh ); // Convert the same mesh
        this->SaveShadowMesh( shadowMesh);
    }
    n_delete(shadowMesh);
}

void 
n3dsMeshShape::CreateMesh_multiMtl()
{
    int i;
    const int numFaces = mesh->GetNumberOfFaces();
    IGameMaterial* mtl = this->iNode->GetNodeMaterial();
    //singleton material list
    n3dsMaterialList* mtlList = n3dsExportServer::Instance()->GetMtlList();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    Map* map= n_new_array(Map,numFaces);
    for (i=0;i<numFaces;i++)
    {
        map[i].index = i;
        //get the real material
        map[i].material = mesh->GetFace(i)->matID;
    }

    qsort(map, numFaces, sizeof(Map), SortedByMatID);

    //centinell

    i=0;
    n3dsTriangle triangle;
    nMeshBuilder::Vertex vertex[3];
    int countMaterial = 0;

    nMeshBuilder* shadowMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
    do
    {
        myMesh = n_new(nMeshBuilder(numFaces * 3, numFaces, 1));
        n_assert(myMesh);
        int mat=map[i].material;
        int matId=mtlList->GetSubMaterialId(mtl,mat);
        const int component = GetComponents(matId);

        while((i<numFaces)&&(mat==map[i].material))
        {
            triangle.Set(mesh,map[i].index);
            triangle.GetVertex(vertex,component,this->iNode);
            systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
            myMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
            i++;
        }

        this->TransformLocal(myMesh);
        this->SetCurrentName(countMaterial);

        this->Save(matId , countMaterial, mat);

        if ( mtlList->Getn3dsMaterial(matId).IsEmitterShadow() )
        {
            shadowEmitter |= true;
            shadowMesh->Append( *myMesh );
        }

        n_delete(myMesh);
        countMaterial++;
   } while ( i<numFaces );
   n_delete_array(map);

   if ( n3dsExportServer::Instance()->GetSettings().exportMode != n3dsExportSettings::Indoor&&
        shadowMesh->GetNumVertices() > 0
        )
   {
       ConvertToShadowMesh( shadowMesh );
       SaveShadowMesh( shadowMesh );
   }
   n_delete( shadowMesh );
}


int
__cdecl
n3dsMeshShape::SortedByMatID(const void* elm0, const void* elm1)
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


void 
n3dsMeshShape::SetCurrentName(int uniqueId)
{
    
    nString buf;
    CurrentName = Name;
    buf.Format( ".%d", uniqueId);
    CurrentName += buf;
}

void
n3dsMeshShape::Save(int material , int uniqueId, int subMaterialId)
{
    nKernelServer*  ks = nKernelServer::ks;
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();
    nMatTypePropBuilder matProp;
    nString meshName = this->assetPathMeshes;
    //meshName+="/meshes/";
    meshName+=this->CurrentName;
    mtlList->Preparen3dsMaterial(material); // Calculate the mesh components

    bool isSphereNormal = mtlList->Getn3dsMaterial(material).IsSphereNormal();
    bool isBillBoard    = mtlList->Getn3dsMaterial(material).IsBillBoard();
    bool needTangentUV0 = mtlList->Getn3dsMaterial(material).NeedTangentUV0();
    bool needTangentUV3 = mtlList->Getn3dsMaterial(material).NeedTangentUV3();
    bool optimizeMesh   = n3dsExportServer::Instance()->GetSettings().optimizeMesh;
    bool binaryFile     = n3dsExportServer::Instance()->GetSettings().binaryResource;
    bool isAnimation    = n3dsExportServer::Instance()->GetSettings().exportMode == n3dsExportSettings::ExportModeType::Character ;
                            //&&(n3dsExportServer::Instance()->GetSettings().animMode == n3dsExportSettings::CharacterModeType::SkinAnimation || n3dsExportServer::Instance()->GetSettings().animMode == n3dsExportSettings::CharacterModeType::MorpherAnimation);


    if (( isSphereNormal || isBillBoard || optimizeMesh )  && !( isAnimation))
    {
        myMesh->Cleanup(0); // remove redundant vertices
    }


    if (isSphereNormal)
    {
        myMesh->TransformSphericalNormals();
    }
        
    if (isBillBoard)
    {
        
        myMesh->TransformBillBoard();
    }

    if (needTangentUV0)
    {
        myMesh->BuildTriangleOnlyTangentBinormal();
    }

    if (needTangentUV3)
    {
        myMesh->BuildTriangleOnlyTangentBinormal(3);
    }

    // Remove vertex after  build normals because thi method add new vertex
    int numVertexBeforeClean = myMesh->GetNumVertices(); 
    bool triangleisDelete = myMesh->CleanVertex(); // remove vertices not referenced and use only 65535 vertex


    if (( optimizeMesh )  && !( isAnimation))
    {
         myMesh->Optimize();
    }
        
    N3DSERRORCOND( /*name*/  meshexport , 
                /*COND*/ triangleisDelete ,
                /*LOG*/( 0, "ERROR: \"%s\" has has too many vertexes,  %i \n" , iNode->GetName() , numVertexBeforeClean) 
            );
    

      
    if (binaryFile)
    {
      meshName+=".nvx2";
      this->myMesh->SaveNvx2(ks->GetFileServer(),meshName.Get());
    } else
    {
      meshName+=".n3d2";
      this->myMesh->SaveN3d2(ks->GetFileServer(),meshName.Get());
    }

    char buf[255];
    sprintf(buf,"shape.%i", uniqueId);

    nShapeNode *currentShape;
    
    currentShape = (nShapeNode* )ks->New("nshapenode" , buf );
     
    n_assert(currentShape);

    currentShape->SetMesh(meshName.Get());
    currentShape->SetGroupIndex(0);
    
    bbox3 localBox = this->myMesh->GetGroupBBoxTestBillboard(0);
    currentShape->SetLocalBox(localBox);
    this->box.extend(localBox);

    // SET MATERIAL 
    mtlList->SetMaterial(currentShape, material,&matProp);

    if (subMaterialId > 0) // If is a submaterial
    {
        N3DSERRORCOND( /*name*/  meshexport , 
                     /*COND*/ ! mtlList->Getn3dsMaterial( material ).IsValid() ,
                     /*LOG*/( 0, "ERROR: \"%s\" has a incorrect submaterial number \"%i\"\n" , iNode->GetName() , subMaterialId + 1) //UI Max begin by 1
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

}

void 
n3dsMeshShape::TransformLocal(nMeshBuilder *localMesh)
{
    if (n3dsExportServer::Instance()->GetSettings().exportMode != n3dsExportSettings::Indoor)
    {
        localMesh->Transform(myMatrixInv);
    }
}


/// return components for export
int 
n3dsMeshShape::GetComponents(int matId)
{
    int matComponents = n3dsExportServer::Instance()->GetMtlList()->Getn3dsMaterial(matId).GetMeshComponents();
    int components = matComponents & n3dsTriangle::GetMeshComponents(this->mesh);
    return components;
}


//------------------------------------------------------------------------------
/**
*/
void 
n3dsMeshShape::ConvertToShadowMesh( nMeshBuilder* shadowMesh)
{
    //make a unique group
    int idx;
    int closed;
    for ( idx = 0; idx < shadowMesh->GetNumTriangles() ; ++idx )
    {
        nMeshBuilder::Triangle& triangle= shadowMesh->GetTriangleAt( idx);
        triangle.SetGroupId( 0 );
    }

    /*
       delete unnecesary components
       Use the same normal of max for extrude the vertex
       The max know if a mesh is smoth or not.
    */
    shadowMesh->ForceVertexComponents( (nMeshBuilder::Vertex::COORD | nMeshBuilder::Vertex::NORMAL ) );
    shadowMesh->Cleanup(0);
    closed = shadowMesh->CreateEdgeQuads2(false); //Create quads but not close the holes  
    N3DSWARNCOND( /*name*/  meshexport , 
              /*COND*/ 0 != closed,
              /*LOG*/( 2, "WARNING: \"%s\" invalid for a shadow volume, number of open edges %d \n" , iNode->GetName(), closed )
            );

    // Remove vertex after  build normals because thi method add new vertex
    int numVertexBeforeClean = shadowMesh->GetNumVertices(); 
    bool triangleisDelete = shadowMesh->CleanVertex(); // remove vertices not referenced and use only 65535 vertex

    N3DSERRORCOND( /*name*/  meshexport , 
                /*COND*/ triangleisDelete ,
                /*LOG*/( 0, "ERROR: \"%s\" has has too many vertexes,  %i \n" , iNode->GetName() , numVertexBeforeClean) 
            );

}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsMeshShape::SaveShadowMesh( nMeshBuilder* shadowMesh)
{
    if ( 0 == shadowMesh->GetNumTriangles() ) 
    {
        // if not has any triangle then not exported it.
        return;
    }
    nKernelServer*  ks = nKernelServer::ks;
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();
    nString meshName = this->assetPathMeshes;
    meshName+= Name;
    meshName+= "_shadow";
    bool binaryFile     = n3dsExportServer::Instance()->GetSettings().binaryResource;
         
    if (binaryFile)
    {
        meshName+=".nvx2";
        shadowMesh->SaveNvx2(ks->GetFileServer(),meshName.Get());
    } else
    {
        meshName+=".n3d2";
        shadowMesh->SaveN3d2(ks->GetFileServer(),meshName.Get());
    }

    nShapeNode *currentShape;
    currentShape = (nShapeNode* )ks->New("nshapenode" , "shape.shadow");
    n_assert(currentShape);

    currentShape->SetMesh(meshName.Get());
    currentShape->SetGroupIndex(0);

    mtlList->SetShadowMaterial(currentShape);
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsMeshShape::IsShadowEmitter()
{
    return this->shadowEmitter;
}