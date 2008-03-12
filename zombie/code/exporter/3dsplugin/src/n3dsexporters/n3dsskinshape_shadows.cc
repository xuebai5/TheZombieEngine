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
//------------------------------------------------------------------------------
// entity includes

#include "kernel/ndependencyserver.h"
//------------------------------------------------------------------------------
// tools and scene includes

#include "nscene/nskinshapenode.h"
#include "nscene/nattachmentnode.h"
#include "nasset/nentityassetbuilder.h"
#include "kernel/nfileserver2.h"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/**
*/
nMeshBuilder*
n3dsSkinShape::GetShadowMesh()
{
    n_assert(this->skinShadowMesh);
    return this->skinShadowMesh;
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder*
n3dsSkinShape::GetRagShadowMesh()
{
    if(this->skinRagShadowMesh)
    {
        return this->skinRagShadowMesh;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsSkinShape::ConvertToShadowMesh( nMeshBuilder* shadowMesh)
{
    //make a unique group
    int idx;
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
    shadowMesh->ForceVertexComponents( nMeshBuilder::Vertex::COORD | 
                                       nMeshBuilder::Vertex::NORMAL |
                                       nMeshBuilder::Vertex::WEIGHTS |
                                       nMeshBuilder::Vertex::JINDICES
                                      );
    shadowMesh->Cleanup(0); 
    
    /*closed = shadowMesh->CreateEdgeQuads(); //Create quads but not close the holes
    N3DSWARNCOND(  meshexport , 
              !closed,
             ( 2, "WARNING: \"%s\" invalid for a shadow volume\n" , iNode->GetName() )
        );
    */

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
n3dsSkinShape::SaveShadowMesh( nMeshBuilder* shadowMesh, const nString& assetPath)
{
    if ( 0 == shadowMesh->GetNumTriangles() ) 
    {
        // if not has any triangle then not exported it.
        return;
    }
    nMatTypePropBuilder matProp;
    nKernelServer*  ks = nKernelServer::ks;
    nSkinGeometryNode* skingeom;
    nShapeNode *currentShape;
    n3dsMaterialList *mtlList = n3dsExportServer::Instance()->GetMtlList();
    nString meshName = nEntityAssetBuilder::GetMeshesPathForAsset( assetPath );
    meshName+= "_shadow";
    bool binaryFile     = n3dsExportServer::Instance()->GetSettings().binaryResource;
    int closed;

    // First make a unique group
    int idx;
    for ( idx = 0; idx < shadowMesh->GetNumTriangles() ; ++idx )
    {
        nMeshBuilder::Triangle& triangle= shadowMesh->GetTriangleAt( idx);
        triangle.SetGroupId( 0 );
    }
    shadowMesh->ForceVertexComponents( nMeshBuilder::Vertex::COORD | 
                                       nMeshBuilder::Vertex::NORMAL |
                                       nMeshBuilder::Vertex::WEIGHTS |
                                       nMeshBuilder::Vertex::JINDICES
                                      );
    shadowMesh->Cleanup(0); //Cleanup
    closed = shadowMesh->CreateEdgeQuads2(false); //Create quads but not close the holes  
    N3DSWARNCOND( /*name*/  meshexport , 
              /*COND*/ 0 !=closed,
              /*LOG*/( 2, "WARNING: Invalid mesh \"%s\" for a shadow volume, number of open edges %d \n" , assetPath.Get(), closed )
            );
    nArray<n3dsMeshFragment> fragmentsArray;
    //skinanimator - partition
    n3dsSkinShape::splitMesh( shadowMesh, fragmentsArray );
    shadowMesh->Cleanup(0);
         
    if (binaryFile)
    {
        meshName+=".nvx2";
        shadowMesh->SaveNvx2(ks->GetFileServer(),meshName.Get());
    } else
    {
        meshName+=".n3d2";
        shadowMesh->SaveN3d2(ks->GetFileServer(),meshName.Get());
    }

    matProp.AddParam("deform", "skinned");
    for( int idx = 0; idx < fragmentsArray.Size() ; ++idx)
    {
        nString shapeNodeName("shape.shadow");
        shapeNodeName+=idx;
        currentShape = (nShapeNode* )ks->New("nshapenode" , shapeNodeName.Get());
        n_assert(currentShape);
        ks->PushCwd(currentShape);
        skingeom = static_cast < nSkinGeometryNode* > ( ks->New("nskingeometrynode", "skin") );
        n_assert(skingeom);
        ks->PopCwd();

        currentShape->SetMesh(meshName.Get());
        currentShape->SetGroupIndex( fragmentsArray[idx].iGroupMappingIndex );
        skingeom->SetStateChannel("charState");

        skingeom->BeginJointPalette(fragmentsArray[idx].BonePaletteArray.Size());
        for(int j=0; j<fragmentsArray[idx].BonePaletteArray.Size(); j++)
	    {       
            skingeom->SetJointIndex(j, fragmentsArray[idx].BonePaletteArray[j]);
	    }
        skingeom->EndJointPalette();
        mtlList->SetShadowMaterial(skingeom, &matProp);
    }
    
}
