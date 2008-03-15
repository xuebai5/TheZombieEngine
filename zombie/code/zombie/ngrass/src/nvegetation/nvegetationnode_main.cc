#include "precompiled/pchngrass.h"
//------------------------------------------------------------------------------
//  nvegetationnode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nvegetation/nvegetationnode.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "zombieentity/ncdictionary.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "ngeomipmap/ncterraingmm.h"
#include "gfx2/nmesh2.h"
#include "nscene/nscenegraph.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatiallightenv.h"
#include "nscene/ncscenelight.h"
#include "ngeomipmap/ncterrainmaterialclass.h"

//#include "zombieentity/ncdictionary.h"

//---------------------------------------------------------------------------
nNebulaScriptClass(nVegetationNode, "ngeometrynode");


//------------------------------------------------------------------------------
/**
*/
nVegetationNode::nVegetationNode() :
    vegClass(0),
    profRender("profGrassRender", true),
    profRenderDraw("profGrassRenderDraw", true)
{

}

//------------------------------------------------------------------------------
/**
*/
nVegetationNode::~nVegetationNode()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationNode::InitInstance(nObject::InitInstanceMsg)
{
    this->SetWorldCoord(true);
    //SetPassEnabled( FOURCC('lght'), false );
    SetPassEnabled( FOURCC('lght'), true );
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationNode::LoadResources()
{
    return nGeometryNode::LoadResources();
}
//------------------------------------------------------------------------------
/**
*/
void 
nVegetationNode::UnloadResources()
{
    return nGeometryNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationNode::Attach(nSceneGraph * sceneGraph , nEntityObject * entityObject)
{
    nGeometryNode::Attach(sceneGraph , entityObject );
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationNode::SetVegetaionClass( ncTerrainVegetationClass* vegClass)
{
    this->vegClass = vegClass;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationNode::Apply(nSceneGraph* sceneGraph)
{

    if ( nGeometryNode::Apply(sceneGraph))
    {
        n_assert( this->vegClass );
        // set the model transform to identity to avoid setting it at Render()
        nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, matrix44());
        nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();
        n_assert_return(shader,false);

        if ( shader->IsParameterUsed( nShaderState::TerrainGrassMinDist ) )
        {
            float blockSize = this->vegClass->GetSubBlockSideSizeScaled();
            float distance = blockSize * vegClass->GetNumSubBlockClip(); 
            shader->SetFloat(nShaderState::TerrainGrassMinDist, distance * vegClass->GetGrasBeginFadeFactor() );
            shader->SetFloat(nShaderState::TerrainGrassMaxDist, distance );
        }

        // The modulation map is not use in material - editor 
        if ( shader->IsParameterUsed(nShaderState::TerrainModulationMap) )
        {
            ncTerrainMaterialClass * tlm = this->vegClass->GetComponent<ncTerrainMaterialClass>();
            n_assert_return(tlm,false);
            ncTerrainGMMClass * tgmm = this->vegClass->GetComponent<ncTerrainGMMClass>();
            shader->SetFloat(nShaderState::TerrainSideSizeInv, 1.0f / tgmm->GetBlockSideSizeScaled());
            shader->SetFloat(nShaderState::TerrainGlobalMapScale, 1.0f / tgmm->GetTotalSideSizeScaled());
            shader->SetTexture(nShaderState::TerrainModulationMap, tlm->GetModulationTexture());
            shader->SetFloat(nShaderState::TerrainModulationFactor, tlm->GetModulationFactor());
        }


        /*
        //nEntityObject *outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
        //n_assert(outdoor );
        //ncDictionary* dic = outdoor->GetComponentSafe< ncDictionary>();
        //nEntityObject* lightMap = static_cast<nEntityObject*>(dic->GetLocalObjectVariable("outlight"));
        nEntityObject* lightMap = this->vegClass->GetComponentSafe< ncTerrainMaterialClass>()->GetLightMap();

        if ( lightMap)
        {
            nEntityObject* originalLight = lightMap->GetComponentSafe<ncSpatialLightEnv>()->GetLightEntity();
            //originalLight->GetComponentSafe<ncSceneLight>()->Render( sceneGraph);
            ncScene *sceneComp = originalLight->GetComponentSafe<ncScene>();
            nAbstractShaderNode* rootNode = static_cast<nAbstractShaderNode*>(sceneComp->GetRootNode());
            rootNode->Apply(  sceneGraph );
            rootNode->Render( sceneGraph ,  originalLight  );
        }
        */
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationNode::Render(nSceneGraph* sceneGraph, nEntityObject * entityObject)
{
    nGeometryNode::Render(sceneGraph , entityObject );
    n_assert( this->vegClass );

    this->profRender.StartAccum();

    /// Get the meshes for subcells, in the future getmeshes need a shader pass or other info.
    const ncTerrainVegetationClass::MeshList&  meshList =  this->vegClass->GetMeshes( );
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // set the model transform to identity to avoid setting it at Render()
    nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, matrix44());
    nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();



    // Draw all meshes
    nEntityObject* previousCell = 0;

    for ( int idx=0 ; idx < meshList.Size()  ; ++idx )
    {
        nMesh2* mesh = meshList[idx].mesh; 
        if ( mesh  && (meshList[idx].numIndices != 0) /* && mesh->IsLoaded() */) // check the mesh is not empty
        {
            if ( previousCell != meshList[idx].cell )
            {
                previousCell = meshList[idx].cell;
                this->SetShaderParams(shader , meshList[idx].cell );
            }

            ///@TODO: todo pass necesary lights
          
            
            #if  ( !defined (NGAME) ) && ( !defined( __ZOMBIE_EXPORTER__) )
            const bool draw (!ncTerrainVegetationClass::debugDisableDrawMesh);
            if(draw)
            #endif
            {
                this->profRenderDraw.StartAccum();
                // set mesh, vertex and index range
                gfxServer->SetMesh( mesh , mesh );
                const nMeshGroup& curGroup = mesh->Group( 0 );
                gfxServer->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
                gfxServer->SetIndexRange(curGroup.GetFirstIndex(), meshList[idx].numIndices);
                gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
                this->profRenderDraw.StopAccum();
            }
        }
    }

    this->profRender.StopAccum();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationNode::SetShaderParams( nShader2* shader, nEntityObject* cell)
{
    if ( ! cell )
    {
        return;
    }

    ncTerrainGMMCell*  gmmCell  = cell->GetComponentSafe< ncTerrainGMMCell>();
    ncTerrainGMMClass* gmmClass = this->vegClass->GetComponent< ncTerrainGMMClass>();
    ncTerrainMaterialClass* gmmMatClass = this->vegClass->GetComponent< ncTerrainMaterialClass>();


    this->shaderParams.SetArg( nShaderState::TerrainSideSizeInv, 1.0f / gmmClass->GetBlockSideSizeScaled() );

    // cell min position: base x, y, z
    float sidesize = gmmClass->GetBlockSideSizeScaled();
    vector4 cellminpos;
    cellminpos.x = gmmCell->GetCellX() * sidesize;
    cellminpos.y = gmmClass->GetHeightMap()->GetHeightOffset();
    cellminpos.z = gmmCell->GetCellZ() * sidesize;
    cellminpos.w = 1.0f ;
    shaderParams.SetArg(nShaderState::TerrainCellPosition, cellminpos);

    //ncScene* cellSceneComp = cell->GetComponent<ncScene>();

    /*
    nShaderParams& cellShaderOverrides = cellSceneComp->GetShaderOverrides();
    if ( cellShaderOverrides.IsParameterValid( nShaderState::lightMap  ) )
    {
        const nShaderArg& lightMap = cellShaderOverrides.GetArg( nShaderState::lightMap );
        this->shaderParams.SetArg( nShaderState::lightMap , lightMap );
    }
    */

    nTexture2 * lightMap = gmmMatClass->GetLightMapTexture( gmmCell->GetCellX() ,  gmmCell->GetCellZ() );
    if ( lightMap )
    {
        this->shaderParams.SetArg( nShaderState::lightMap , lightMap );
    }

    
    shader->SetParams( this->shaderParams );
}



//------------------------------------------------------------------------------
/**
*/
void 
nVegetationNode::RestoreOnLostDevice(void)
{
}
