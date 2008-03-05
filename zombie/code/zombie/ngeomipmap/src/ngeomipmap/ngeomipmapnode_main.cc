/*-----------------------------------------------------------------------------
    @file ngeomipmapnode_main.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief Terrain rendering methods

    (C) 2004 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"

#include "nscene/nscenegraph.h"
#include "gfx2/nshader2.h"
#include "kernel/nlogclass.h"
#include "napplication/napplication.h"
#include "nscene/nsurfacenode.h"
#include "signals/nsignalserver.h"
#include "ngeomipmap/nterrainweightmapbuilder.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "ngeomipmap/ngmmdynamicmeshstorage.h"
#include "ngeomipmap/ngmmindexbuilder.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
#include "ngeomipmap/ngmmindexcache.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "ngeomipmap/ngmmlodsystem.h"

//---------------------------------------------------------------------------
nNebulaScriptClass(nGeoMipMapNode, "ngeometrynode");

//------------------------------------------------------------------------------
extern const char * errorTexturePath;

//------------------------------------------------------------------------------
/**
    constructor
*/
inline 
nGeoMipMapNode::nGeoMipMapNode() :
    terrainGMMClass(0),
    terrainGMM(0),
    dynMeshStorage(0),
    profAttach("profGMMAttach", true),
    profRender("profGMMRender", true),
    profRenderFill("profGMMRenderFill", true),
    profRenderMaterial("profGMMRenderMaterial", true),
    profRenderLOD("profGMMRenderLOD", true),
    ncTerrainGMMCellIndex(-1),
    ncSpatialQuadtreeCellIndex(-1),
    frameId(-1)
#ifndef NGAME
    ,renderGrassEdition(false)
#endif
{
    this->SetSaveModeFlags(N_FLAG_SAVESHALLOW);
    this->ResetStatistics();
}

//------------------------------------------------------------------------------
/**
    destructor
*/
inline 
nGeoMipMapNode::~nGeoMipMapNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
void 
nGeoMipMapNode::InitInstance(nObject::InitInstanceMsg)
{
    // bind for getting lost signal
    nGfxServer2::Instance()->BindSignal( NSIGNAL_OBJECT(nGfxServer2, RestoreOnLost), this, &nGeoMipMapNode::RestoreOnLostDevice, 0);

    this->SetWorldCoord(true);
}

//------------------------------------------------------------------------------
/**
*/
bool 
nGeoMipMapNode::LoadResources()
{
    // get pass enabled flags
    this->passFlagGMM0 = 1 << nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmm0'));
    this->passFlagGMM1 = 1 << nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmm1'));
    #ifndef NGAME
    this->passFlagGMLM = 1 << nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmlm'));
    this->passFlagGMGE = 1 << nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmge'));
    this->passFlagGMSD = 1 << nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmsd'));
    #endif

    if (! nGeometryNode::LoadResources())
    {
        return false;
    }

    if (! this->dynMesh.IsValid() ) 
    {
        this->dynMesh.Initialize(nGfxServer2::TriangleList, nMesh2::Coord | nMesh2::Normal, nMesh2::NeedsVertexShader, true, false);
    }

    if (! this->dynMeshStorage)
    {
        this->dynMeshStorage = n_new(nGMMDynamicMeshStorage);
        n_assert(this->dynMeshStorage);
        this->dynMeshStorage->AddRef();
        this->dynMeshStorage->SetDynamicMesh(&this->dynMesh);
    }

    if ( ! this->refErrorTexture.isvalid() )
    {
        this->refErrorTexture = nGfxServer2::Instance()->NewTexture(errorTexturePath);
        n_assert(this->refErrorTexture.get());
        this->refErrorTexture->SetFilename(errorTexturePath);
        this->refErrorTexture->Load();
    }

    if ( terrainGMMClass )
    {
        return terrainGMMClass->GetComponentSafe<ncTerrainVegetationClass>()->LoadSceneResources();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGeoMipMapNode::UnloadResources()
{
    nGeometryNode::UnloadResources();

    if ( terrainGMMClass )
    {
        terrainGMMClass->GetComponentSafe<ncTerrainVegetationClass>()->UnloadSceneResources();
    }

    if (this->refErrorTexture.isvalid())
    {
        this->refErrorTexture->Release();
        this->refErrorTexture.invalidate();
    }

    if (this->dynMeshStorage)
    {
        this->dynMeshStorage->Release();
        this->dynMeshStorage = 0;
    }

    if (this->dynMesh.IsValid()) 
    {
        this->dynMesh.Deinitialize();
    }
}

//---------------------------------------------------------------------------
void 
nGeoMipMapNode::SetTerrainGMM(ncTerrainGMM * terrainGMM)
{
    n_assert(terrainGMM);

    this->terrainGMM = terrainGMM;

    this->terrainGMMClass = terrainGMM->GetClassComponent<ncTerrainGMMClass>();
    n_assert(this->terrainGMMClass);

    // initialize max lod level for all viewports
    for(int i = 0;i < N_ARRAY_SIZEOF(this->vp);i++)
    {
        this->vp[i].SetMaxLODLevel(terrainGMMClass->GetNumLODLevels() - 1);
    }
}

//---------------------------------------------------------------------------
/**
    Called by nSceneGraph when object is attached to scene. 
    Called by the visibility processor when a terrain cell becomes visible.
*/
void
nGeoMipMapNode::Attach(nSceneGraph *sceneGraph, nEntityObject *entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    nGeoMipMapViewport * gmmvp = this->FindViewport(sceneGraph, entityObject);
    if (gmmvp->IsFirstAttach())
    {
        NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "Attach(sceneGraph=0x%x) started", sceneGraph));

        // attach the grass vegetation
        terrainGMMClass->GetComponentSafe<ncTerrainVegetationClass>()->Attach( sceneGraph ,  entityObject );

        // initialize the indices
        this->ncTerrainGMMCellIndex = entityObject->IndexOfComponent( ncTerrainGMMCell::GetComponentIdStatic() );
        n_assert(this->ncTerrainGMMCellIndex != nComponentIdList::InvalidIndex);
        this->ncSpatialQuadtreeCellIndex = entityObject->IndexOfComponent( ncSpatialQuadtreeCell::GetComponentIdStatic() );
        n_assert(this->ncSpatialQuadtreeCellIndex != nComponentIdList::InvalidIndex);
        this->frameId = nApplication::Instance()->GetFrameId();
    }

    this->profAttach.StartAccum();

    // attach recursively until getting to the leafes
    ncTerrainGMMCell * cell = entityObject->GetComponent<ncTerrainGMMCell>();
    n_assert(cell);
    int passes = this->AttachCell(sceneGraph, cell, gmmvp);

    // attach to the proper passes of the scene graph 
    this->AttachSceneGraph(sceneGraph, entityObject, gmmvp, passes);

    this->profAttach.StopAccum();
}

//---------------------------------------------------------------------------
/**
    It goes down through the quadtree recursively. It calculates the LOD for 
    each cell. Attach the terrain cell to the scenegraph to the proper pass.
    @return an int value identifying the passes to render for the cell and 
            children
*/
int
nGeoMipMapNode::AttachCell(nSceneGraph *sceneGraph, ncTerrainGMMCell* cell, nGeoMipMapViewport * gmmvp)
{
    n_assert(sceneGraph);
    n_assert(cell);
    n_assert(gmmvp);

    ncSpatialQuadtreeCell * qtCell = static_cast<ncSpatialQuadtreeCell *> (cell->GetComponentByIndex(this->ncSpatialQuadtreeCellIndex));
    int numChildren = qtCell->GetNumSubcells();
    if (numChildren > 0)
    {
        // Render all children cells if any (non-leaf cell)
        ncSpatialQuadtreeCell ** children = qtCell->GetSubcells();
        ncTerrainGMMCell * child;

        int ret = 0;
        for(int i = 0;i < numChildren;i++)
        {
            child = static_cast<ncTerrainGMMCell *> (children[i]->GetComponentByIndex(this->ncTerrainGMMCellIndex));
            if (child)
            {
                ret |= this->AttachCell( sceneGraph, child, gmmvp );
            }
        }

        return ret;
    }
    else
    {
        bool visible = true;
        if (terrainGMM->GetUnpaintedCellsInvisible())
        {
            ncSpatialQuadtreeCell * qtcell = static_cast<ncSpatialQuadtreeCell *> (cell->GetComponentSafeByIndex(this->ncSpatialQuadtreeCellIndex));
            visible = qtcell->IsActive();
        }

        if (visible)
        {
            gmmvp->AttachCell();
            // get the terrain cell component of the entity object
            cell->SetVisible( this->frameId );
            terrainGMMClass->GetLODSystem()->SelectCellLOD(cell, gmmvp);

            //// update shader parameters if material has changed
            if (cell->IsMaterialDirty())
            {
                // get scene context and shader params
                ncScene *sceneContext = cell->GetComponent<ncScene>();
                n_assert(sceneContext);
                nShaderParams & shaderParams = sceneContext->GetShaderOverrides();

                this->SetShaderParamsGMM0(shaderParams, cell);
            }

            return cell->GetMaterialLODLevel() ? this->passFlagGMM1 : this->passFlagGMM0;
        }

        return 0;
    }
}

//---------------------------------------------------------------------------
void
nGeoMipMapNode::AttachSceneGraph(nSceneGraph *sceneGraph, nEntityObject *entityObject, nGeoMipMapViewport * gmmvp, int passes)
{
    ncScene* renderContext = entityObject->GetComponent<ncScene>();
    n_assert(renderContext);

    int flags = renderContext->GetPassEnabledFlags();

    flags &= ~(this->passFlagGMM0 | this->passFlagGMM1);
    flags |= passes;

    #ifndef NGAME
    flags |= this->passFlagGMLM;
    flags |= this->passFlagGMSD;

    if ( this->renderGrassEdition && (passes != this->passFlagGMM1) )
    {
        flags |= this->passFlagGMGE;
    }  
    else
    {
        flags &= ~this->passFlagGMGE;
    }
    #endif

    renderContext->SetPassEnabledFlags( flags );
    nGeometryNode::Attach(sceneGraph, entityObject);

    int sumMaterialLOD = 0;
    if (passes & this->passFlagGMM0)
    {
        sumMaterialLOD++;
    }
    if (passes & this->passFlagGMM1)
    {
        sumMaterialLOD++;
    }
    gmmvp->Attach(sumMaterialLOD);
}

//---------------------------------------------------------------------------
/**
    Use pre-instancing geometry rendering phase for calculating LODs.
*/
bool
nGeoMipMapNode::Apply(nSceneGraph* sceneGraph)
{
    n_assert(sceneGraph);

    if (!nGeometryNode::Apply(sceneGraph))
    {
        return false;
    }

    // set the model transform to identity to avoid setting it at Render()
    nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, matrix44());

    nGeoMipMapViewport * vp = this->FindViewport(sceneGraph, 0);
    n_assert(vp);
    if (vp->Apply())
    {
        this->terrainGMMClass->ResetStats();
        this->PreloadCache(vp);
        this->ResetStatistics();

        this->profRenderLOD.StartAccum();
        this->terrainGMMClass->GetLODSystem()->LevelLOD(this->terrainGMM);
        this->profRenderLOD.StopAccum();
    }

    // set shader shared params 
    static nShaderParams params;
    params.Clear();
    this->SetShaderParamsShared(params);
    nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();
    shader->SetParams(params);

    this->dynMeshStorage->BeginTerrainMesh();

    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "Apply(sceneGraph=0x%x) attaches=%d-%d renders=%d-%d", sceneGraph, vp->GetNumAttached(), vp->GetNumAttachedCells(), vp->GetNumRendered(), vp->GetNumRenderedCells()));

    return true;
}

//---------------------------------------------------------------------------
/**
    Rendering.

    Note we only need a single ngeomipmapnode scene node for the rendering,
    and we use different entity objects for each cell of the terrain.
*/
bool 
nGeoMipMapNode::Render(nSceneGraph* sceneGraph, nEntityObject *entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    // get cell
    ncTerrainGMMCell * cell = static_cast<ncTerrainGMMCell *> (entityObject->GetComponentByIndex(ncTerrainGMMCellIndex));
    n_assert(cell);

    // get viewport info container
    nGeoMipMapViewport * vp = this->FindViewport(sceneGraph, entityObject);
    n_assert(vp);

    // only done in first render call for this viewport
    if (vp->IsFirstRender())
    {
        NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "Render(sceneGraph=0x%x) started numAttached=%d-%d numRendered=%d-%d", sceneGraph, vp->GetNumAttached(), vp->GetNumAttachedCells(), vp->GetNumRendered(), vp->GetNumRenderedCells()));
    }
    this->profRender.StartAccum();

    vp->Render();

    this->RenderCell(sceneGraph, cell, vp, true);

    this->profRender.StopAccum();

    return true;
}

//---------------------------------------------------------------------------
bool 
nGeoMipMapNode::Flush(nSceneGraph* sceneGraph)
{
    this->dynMeshStorage->EndTerrainMesh();
    this->ShowStatistics();
    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "Render(sceneGraph=0x%x) finished attached=%d-%d numRendered= %d-%d", sceneGraph, vp->GetNumAttached(), vp->GetNumAttachedCells(), vp->GetNumRendered(), vp->GetNumRenderedCells()));

    return true;
}

//---------------------------------------------------------------------------
/**
    Rendering call from visibility.
    Visibility provides the higher quadtree level completely visible.
*/
void
nGeoMipMapNode::RenderCell(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell, nGeoMipMapViewport * gmmvp, bool /*firstlevel*/)
{
    n_assert(sceneGraph);
    n_assert(cell);

    ncSpatialQuadtreeCell * qtCell = static_cast<ncSpatialQuadtreeCell *> (cell->GetComponentByIndex(this->ncSpatialQuadtreeCellIndex));
    int numChildren = qtCell->GetNumSubcells();
    if (numChildren > 0)
    {
        // Render all children cells if any (non-leaf cell)
        ncSpatialQuadtreeCell ** children = qtCell->GetSubcells();
        ncTerrainGMMCell * child;

        for(int i = 0;i < numChildren;i++)
        {
            child = static_cast<ncTerrainGMMCell *> (children[i]->GetComponentByIndex(this->ncTerrainGMMCellIndex));
            if (child)
            {
                this->RenderCell( sceneGraph, child, gmmvp, false );
            }
        }
    }
    else
    {
        bool visible = true;
        if (terrainGMM->GetUnpaintedCellsInvisible())
        {
            ncSpatialQuadtreeCell * qtCell = static_cast<ncSpatialQuadtreeCell *> (cell->GetComponentByIndex(this->ncSpatialQuadtreeCellIndex));
            visible = qtCell->IsActive();
            if (!visible)
            {
                return;
            }
        }

        // ignore the pass not related with the entity object material lod
        if (cell->GetMaterialLODFourCC() != sceneGraph->GetCurrentPass())
        {
            #ifndef NGAME
            if ( cell->GetMaterialLODLevel() != 0 ||
                 !( (this->renderGrassEdition && FOURCC('gmge') == sceneGraph->GetCurrentPass()  )  ||
                    (FOURCC('gmlm') == sceneGraph->GetCurrentPass() ) ||
                    (FOURCC('gmsd') == sceneGraph->GetCurrentPass() )
                  )
               )
            {
            #endif //!NGAME

            NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "render SKIP bx=%d bz=%d glod=%d mlod=%d", cell->GetCellX(), cell->GetCellZ(), cell->GetGeometryLODLevel(), cell->GetMaterialLODLevel()));
            return;

            #ifndef NGAME
            }
            #endif
        }

        #ifndef NGAME
        // check if wireframe rendering
        if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
        {
            if (cell->IsVisible( this->frameId ))
            {
                if ( this->terrainGMMClass->IsLowResMeshLOD( cell->GetGeometryLODLevel() ) )
                {
                    this->RenderCellDynamicMesh(cell);
                }
                else
                {
                    // render the cell using primitive and cache it
                    this->RenderCellAndCache(sceneGraph, cell);
                }
            }
            return;
        }
        #endif

        if( !nGeometryNode::Render(sceneGraph, cell->GetEntityObject()) )
        {
            return;
        }

        // Render a leaf cell
        gmmvp->RenderCell();

        #ifndef NGAME
        if ( cell->IsVisible( this->frameId ) &&
             sceneGraph->GetCurrentPass() == FOURCC('gmge')
            )
        {
            this->RenderGrassEditionMaterial(sceneGraph, cell);
            // render the cell using primitive and cache it
            this->RenderCellAndCache(sceneGraph, cell);
            return;
        }

        if ((sceneGraph->GetCurrentPass() == FOURCC('gmlm')) ||
            (sceneGraph->GetCurrentPass() == FOURCC('gmsd'))
            )
        {
            // render the cell using primitive and cache it
            this->RenderMaterial(sceneGraph, cell);
            this->RenderCellAndCache(sceneGraph, cell);
            return;
        }
        #endif

        // Check if the cell is visible and it has not been already rendered (due to cache)
        if (cell->IsVisible( this->frameId ) && cell->NotRendered(sceneGraph))
        {
            // Render the material first
            if (sceneGraph->GetCurrentPass() == FOURCC('gmm0'))
            {
                this->RenderMaterial(sceneGraph, cell);

                // render the cell using primitive and cache it
                this->RenderCellAndCache(sceneGraph, cell);
            }
            else
            {
                // Render with dynamic mesh if the lod level is higher than lowMeshLOD
                if ( this->terrainGMMClass->IsLowResMeshLOD( cell->GetGeometryLODLevel() ) )
                {
                    this->RenderCellDynamicMesh(cell);
                }
                else
                {
                    // render the cell using primitive and cache it
                    this->RenderCellAndCache(sceneGraph, cell);
                    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "render MAT1 MESH bx=%d bz=%d glod=%d mlod=%d", cell->GetCellX(), cell->GetCellZ(), cell->GetGeometryLODLevel(), cell->GetMaterialLODLevel()));
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
/**
    Rendering call from visibility.
    Visibility provides the higher quadtree level completely visible.
*/
void
nGeoMipMapNode::RenderMesh(ncTerrainGMMCell* cell, nMesh2 * vmesh, nMesh2 * imesh, int group)
{
    n_assert(vmesh);
    n_assert(imesh);

    nGfxServer2 * gfx = nGfxServer2::Instance();
    n_assert(gfx);

    //NLOG(gfx, (NLOG_SHADERPARAMS | 0, "node->GetShaderParams()"));
    //nShaderParams & paramsNode = this->GetShaderParams();
    //paramsNode.Log();
    //NLOG(gfx, (NLOG_SHADERPARAMS | 0, "node->GetShaderOverrides()"));
    //ncScene *sceneContext = cell->GetComponent<ncScene>();
    //n_assert(sceneContext);
    //nShaderParams & paramsOverrides = sceneContext->GetShaderOverrides();
    //paramsOverrides.Log();

    gfx->SetMesh( vmesh, imesh );
    nMeshGroup & vmeshGroup = vmesh->Group( 0 );
    gfx->SetVertexRange( vmeshGroup.GetFirstVertex(), vmeshGroup.GetNumVertices() );
    nMeshGroup & imeshGroup = imesh->Group( group );
    gfx->SetIndexRange( imeshGroup.GetFirstIndex(), imeshGroup.GetNumIndices() );
    gfx->DrawIndexedNS( nGfxServer2::TriangleList );

    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "Render MESH bx=%d bz=%d glod=%d mlod=%d numIndices=%d numVertices=%d", cell->GetCellX(), cell->GetCellZ(), cell->GetGeometryLODLevel(), cell->GetMaterialLODLevel(), imeshGroup.GetNumIndices(), vmeshGroup.GetNumVertices()));

    this->AddStatisticsCell(1, imeshGroup.GetNumIndices() / 3, cell->GetGeometryLODLevel());
}

//---------------------------------------------------------------------------
/**
    Terrain material Rendering.
*/
bool
nGeoMipMapNode::RenderMaterial(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell)
{
    this->profRenderMaterial.StartAccum();

    // check if texture is available for this cell
    ncTerrainMaterialClass * tlm = this->terrainGMMClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    nTexture2 * tex = tlm->GetWeightMapTexture(cell->GetCellX(), cell->GetCellZ(), true);
    nTexture2 * lightMap = tlm->GetLightMapTexture(cell->GetCellX(), cell->GetCellZ());

    //Not put texture in shaderParams because this method is called every frame.
    //If the texture is in shaderParams then it has problem when texture cache unload the texture.
    // The resource is valid but it use by other cell or is unload.

    if (tex || lightMap)
    {
        // get scene context
        //ncScene *sceneContext = cell->GetComponent<ncScene>();
        //n_assert(sceneContext);
        //nShaderParams & shaderParams = sceneContext->GetShaderOverrides();

        // get current shader
        nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();

        if (tex)
        {
            // set mini weight texture
            //shaderParams.SetArg(nShaderState::TerrainCellWeightMap, tex);
            // apply this in current render too
            shader->ClearParameter(nShaderState::TerrainCellWeightMap);
            shader->SetTexture(nShaderState::TerrainCellWeightMap, tex);
        }

        if (lightMap)
        {
            // set mini weight texture
            //shaderParams.SetArg(nShaderState::lightMap , lightMap );
            // apply this in current render too
            shader->ClearParameter(nShaderState::lightMap);
            shader->SetTexture(nShaderState::lightMap, lightMap);
        }

    }

    this->profRenderMaterial.StopAccum();

    return true;
}

//---------------------------------------------------------------------------
/**
    Called when entity created. Setup all the parameters in the render context.
*/
void 
nGeoMipMapNode::EntityCreated(nEntityObject* entityObject)
{
    nGeometryNode::EntityCreated(entityObject);

    // get cell component
    ncTerrainGMMCell * cell = entityObject->GetComponent<ncTerrainGMMCell>();
    n_assert(cell);

    // if cell has children don't finish setup
    ncSpatialQuadtreeCell * qtCell = cell->GetComponent<ncSpatialQuadtreeCell>();
    if (qtCell->GetNumSubcells() > 0)
    {
        return;
    }

    // get scene context and shader params
    ncScene *sceneContext = entityObject->GetComponent<ncScene>();
    n_assert(sceneContext);
    nShaderParams & shaderParams = sceneContext->GetShaderOverrides();

    this->SetShaderParamsGMM0(shaderParams, cell);
}

//---------------------------------------------------------------------------
/**
Shared shader parameters
*/
void
nGeoMipMapNode::SetShaderParamsShared(nShaderParams & shaderParams) const
{
    // get terrain material class
    ncTerrainMaterialClass * tlm = this->terrainGMMClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    shaderParams.SetArg(nShaderState::TerrainSideSizeInv, 1.0f / this->terrainGMMClass->GetBlockSideSizeScaled());
    shaderParams.SetArg(nShaderState::TerrainGlobalMap, tlm->GetGlobalTexture());
    shaderParams.SetArg(nShaderState::TerrainGlobalMapScale, 1.0f / this->terrainGMMClass->GetTotalSideSizeScaled());
    shaderParams.SetArg(nShaderState::TerrainModulationMap, tlm->GetModulationTexture());
    shaderParams.SetArg(nShaderState::TerrainModulationFactor, tlm->GetModulationFactor());
    shaderParams.SetArg(nShaderState::GlobalLightMap, tlm->GetGlobalLighMapTexture() );

    // set material lod distance parameter
    // only needed for GMM0, but common among all the cells
    float sidesize = this->terrainGMMClass->GetBlockSideSizeScaled();
    float dist = tlm->GetMaterialLODDistance() * sidesize;
    float distinv = 1.0f / (dist * dist);
    shaderParams.SetArg(nShaderState::TerrainCellDistSquaredInv, distinv);

    // GlobalLightMap is set up through the light rendering path

    // TODO: use a different shader
    // trick the shader not use the diffuse ilumination when it has a lightmap
    nEntityObject* lightMapEntity = tlm->GetLightMap();

    if ( lightMapEntity)
    {
        shaderParams.SetArg(nShaderState::matDiffuse, nShaderArg(vector4(0.0f, 0.0f, 0.0f, 0.0f)));
    } else
    {
        shaderParams.ClearArg(nShaderState::matDiffuse);
    }
}

//---------------------------------------------------------------------------
/**
    Set shader parameters for maximum level of detail of terrain cell.
*/
void 
nGeoMipMapNode::SetShaderParamsGMM0(nShaderParams & shaderParams, ncTerrainGMMCell* cell)
{
    n_assert(cell);
    n_assert2(!cell->GetComponent<ncSpatialQuadtreeCell>()->GetNumSubcells(), "cell cannot have children");

    if (cell->GetMaterialLODLevel() != 0)
    {
        shaderParams.ClearArg(nShaderState::TerrainCellPosition);
        shaderParams.ClearArg(nShaderState::TerrainCellWeightMap);
        for(int i = 0;i < 5;i++)
        {
            shaderParams.ClearArg(nShaderState::Param(nShaderState::TerrainCellMaterial0 + i));
            shaderParams.ClearArg(nShaderState::Param(nShaderState::TerrainCellTransform0U + i));
            shaderParams.ClearArg(nShaderState::Param(nShaderState::TerrainCellTransform0V + i));
        }
        return;
    }

    // get terrain material class
    ncTerrainMaterialClass * tlm = this->terrainGMMClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    // cell min position: base x, y, z
    float sidesize = this->terrainGMMClass->GetBlockSideSizeScaled();
    vector4 cellminpos;
    cellminpos.x = cell->GetCellX() * sidesize;
    cellminpos.y = terrainGMMClass->GetHeightMap()->GetHeightOffset();
    cellminpos.z = cell->GetCellZ() * sidesize;
    cellminpos.w = 1.0f ;
    shaderParams.SetArg(nShaderState::TerrainCellPosition, cellminpos);

    nTerrainCellInfo * cellInfo = tlm->GetTerrainCellInfo( cell->GetCellX(), cell->GetCellZ() );
    n_assert(cellInfo);

    // set mini weight texture
    // check if texture is available for this cell
    //nTexture2 * texWeightMap = tlm->GetWeightMapTexture(cell->GetCellX(), cell->GetCellZ(), false);
    //shaderParams.SetArg(nShaderState::TerrainCellWeightMap, texWeightMap);
    shaderParams.ClearArg(nShaderState::TerrainCellWeightMap);

    // Get terrain material indices
    const nuint8* idxp = cellInfo->GetIndices();
    n_assert(idxp);

    // Set material params per layer
    for(int i = 0;i < 5;i++)
    {
        if ( idxp[i] != 255 )
        {
            nTerrainMaterial::LayerHandle layerHandle = cellInfo->GetLayerHandle( idxp[i] );
            nTerrainMaterial* layer = tlm->GetLayerByHandle( layerHandle );

            nTexture2 * tex = 0;
            vector4 transformU;
            vector4 transformV;

            if ( layer && layer->GetTexture() )
            {
                tex = layer->GetTexture();
                layer->GetTerrainCellTransformU(cellminpos, transformU);
                layer->GetTerrainCellTransformV(cellminpos, transformV);
            }
            else
            {
                tex = this->refErrorTexture.get();
                transformU = vector4( 1.0f, 1.0f, 1.0f, 1.0f );
                transformV = vector4( 1.0f, 1.0f, 1.0f, 1.0f );
            }

            NLOG(terrain, (nTerrainLog::NLOG_RENDER | 0, "Index %d - %d (0x%x) %s", i, layerHandle, layerHandle, tex->GetFilename().Get()));

            n_assert(tex);
            shaderParams.SetArg(nShaderState::Param(nShaderState::TerrainCellMaterial0 + i), tex);
            shaderParams.SetArg(nShaderState::Param(nShaderState::TerrainCellTransform0U + i), transformU);
            shaderParams.SetArg(nShaderState::Param(nShaderState::TerrainCellTransform0V + i), transformV);
        }
        else
        {
            shaderParams.ClearArg(nShaderState::Param(nShaderState::TerrainCellMaterial0 + i));
            shaderParams.ClearArg(nShaderState::Param(nShaderState::TerrainCellTransform0U + i));
            shaderParams.ClearArg(nShaderState::Param(nShaderState::TerrainCellTransform0V + i));
        }
    }
}

//---------------------------------------------------------------------------
/**
*/
void 
nGeoMipMapNode::RenderCellAndCache(nSceneGraph* /*sceneGraph*/, ncTerrainGMMCell* cell)
{
    this->profRenderFill.StartAccum();

    nGMMMeshCacheEntry * entry = this->terrainGMMClass->GetMeshCache()->Lookup( 
        nGMMMeshCacheEntry::ToKey( cell->GetCellX(), cell->GetCellZ() ), 
        cell );

    this->profRenderFill.StopAccum();

    nMesh2 * imesh = 0;
    int group = 0;
    
    if (cell->HasHoles())
    {
        n_assert_return(this->terrainGMMClass && this->terrainGMMClass->GetHoleIndicesCache(), );
        nGMMHoleIndicesCacheEntry * holeEntry = this->terrainGMMClass->GetHoleIndicesCache()->Lookup(
            nGMMHoleIndicesCacheEntry::ToKey( cell->GetCellX(), cell->GetCellZ(), cell->GetIndexKey() ), 
            cell );
        if (holeEntry)
        {
            imesh = holeEntry->GetMesh();
        }
    }

    // for cells without holes or as fallback for holes without indices found 
    if (!imesh)
    {
        imesh = this->terrainGMMClass->GetIndexCache()->GetIndexMesh(
            cell->GetGeometryLODLevel(),
            cell->GetNeighborLOD( ncTerrainGMMCell::North ),
            cell->GetNeighborLOD( ncTerrainGMMCell::East ),
            cell->GetNeighborLOD( ncTerrainGMMCell::South ),
            cell->GetNeighborLOD( ncTerrainGMMCell::West ) );

        group = this->terrainGMMClass->GetIndexBuilder()->CalcIndexGroup(
            cell->GetGeometryLODLevel(),
            cell->GetNeighborLOD( ncTerrainGMMCell::North ),
            cell->GetNeighborLOD( ncTerrainGMMCell::East ),
            cell->GetNeighborLOD( ncTerrainGMMCell::South ),
            cell->GetNeighborLOD( ncTerrainGMMCell::West ) );
    }

    //NLOG(gfx, (NLOG_SHADERPARAMS | 0, "shader->GetShaderParams()"));
    //nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();
    //shader->LogShaderParams();

    // draw mesh
    this->RenderMesh(cell, entry->GetMesh(), imesh, group);
}

//---------------------------------------------------------------------------
/**
Render a leaf cell with a dynamic mesh.
*/
void
nGeoMipMapNode::RenderCellDynamicMesh(ncTerrainGMMCell* cell)
{
    this->profRenderFill.StartAccum();

    nGMMVertexBuilder * vertexBuilder = this->terrainGMMClass->GetVertexBuilder();
    vertexBuilder->SetupStorage(this->dynMeshStorage);

    nGMMIndexBuilder * indexBuilder = this->terrainGMMClass->GetIndexBuilder();
    indexBuilder->SetupStorage(this->dynMeshStorage);

    this->dynMeshStorage->BeginTerrainGroup( 
        vertexBuilder->CountVertices( cell->GetGeometryLODLevel() ), 
        indexBuilder->CountIndices( cell->GetGeometryLODLevel() ) );

    int numVerticesPrev = this->dynMeshStorage->GetNumVertices();
#if 1
    nGMMLowMeshCacheEntry * entry = this->terrainGMMClass->GetLowResMeshCache()->Lookup(
        nGMMLowMeshCacheEntry::ToKey( cell->GetCellX(), cell->GetCellZ() ), 
        cell );

    this->dynMeshStorage->AppendVertices(
        entry->GetVertices(), 
        vertexBuilder->CountVertices( cell->GetGeometryLODLevel() ));
#elif 0

    nGMMMeshCacheEntry * entry = this->meshCache.Lookup( 
        nGMMMeshCacheEntry::ToKey( cell->GetCellX(), cell->GetCellZ() ), 
        cell );

    nMesh2 * mesh = entry->GetMesh();

    float * vbsrc = mesh->LockVertices();
    // copy vertex data from cache
    int numVertices = vertexBuilder->CountVertices( cell->GetGeometryLODLevel() );
    this->dynMeshStorage->AppendVertices(vbsrc, numVertices);

    mesh->UnlockVertices();
#else
    // old code (generated vertices block)
    vertexBuilder->FillVerticesBlock( cell->GetCellX(), cell->GetCellZ(), cell->GetGeometryLODLevel() );
#endif

#if 1
    // copy index data from cache
    int numIndices;

    nGMMIndexCache * indexCache = this->terrainGMMClass->GetIndexCache();

    nuint16 * ib = indexCache->GetIndexBuffer(
        cell->GetGeometryLODLevel(),
        cell->GetNeighborLOD( ncTerrainGMMCell::North ),
        cell->GetNeighborLOD( ncTerrainGMMCell::East ),
        cell->GetNeighborLOD( ncTerrainGMMCell::South ),
        cell->GetNeighborLOD( ncTerrainGMMCell::West ), 
        numIndices);

    this->dynMeshStorage->AppendIndices(ib, numIndices);

    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "Render DYNMESH bx=%d bz=%d glod=%d mlod=%d numIndices=%d numVertices=%d", cell->GetCellX(), cell->GetCellZ(), cell->GetGeometryLODLevel(), cell->GetMaterialLODLevel(), numIndices, this->dynMeshStorage->GetNumVertices() - numVerticesPrev));
#else
    // old code (generated indices block)
    indexBuilder->FillIndicesBlock( 
        cell->GetGeometryLODLevel(),
        cell->GetNeighborLOD( ncTerrainGMMCell::North ),
        cell->GetNeighborLOD( ncTerrainGMMCell::East ),
        cell->GetNeighborLOD( ncTerrainGMMCell::South ),
        cell->GetNeighborLOD( ncTerrainGMMCell::West ) );
#endif

    this->dynMeshStorage->EndTerrainGroup();

    vertexBuilder->SetupStorage(0);
    indexBuilder->SetupStorage(0);

    this->profRenderFill.StopAccum();
}

//------------------------------------------------------------------------------
/**
    Reset statistics
*/
void 
nGeoMipMapNode::ResetStatistics()
{
    this->numPrimitives = 0;
    this->numCalls = 0;
    for(int i = 0;i < MaxLODLevels;i++)
    {
        this->infoLOD[i].numCells = 0;
        this->infoLOD[i].numPrimitives = 0;
    }

    if (this->terrainGMMClass)
    {
        this->terrainGMMClass->ResetStats();
    }
}

//------------------------------------------------------------------------------
/**
    Add statistics information for one terrain cell
*/
void 
nGeoMipMapNode::AddStatisticsCell(int numCalls, int numPrimitives, int lod)
{
    n_assert(lod >= 0 && lod < MaxLODLevels);

    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 3, "AddStatisticsCell(numCalls=%d, numPrimitives=%d, lod=%d)", numCalls, numPrimitives, lod));

    this->numCalls += numCalls;
    this->numPrimitives += numPrimitives;
    this->infoLOD[lod].numCells++;
    this->infoLOD[lod].numPrimitives += numPrimitives;
}

//------------------------------------------------------------------------------
/**
    Add statistics information for one terrain cell
*/
void 
nGeoMipMapNode::ShowStatistics()
{
    for(int i = 0;i < MaxLODLevels;i++)
    {
        if (this->infoLOD[i].numCells > 0 || this->infoLOD[i].numPrimitives > 0)
        {
            NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "LOD=%d  numCells=%d  numPrimitives=%d", i, this->infoLOD[i].numCells, this->infoLOD[i].numPrimitives));
        }
    }

    NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "TOTAL numCalls=%d  numPrimitives=%d  numHits=%d", this->numCalls, this->numPrimitives, this->terrainGMMClass->GetMeshCache()->GetNumHits()));
}

//------------------------------------------------------------------------------
/**
*/
nGeoMipMapViewport * 
nGeoMipMapNode::FindViewport(nSceneGraph* sceneGraph, nEntityObject * entityObject)
{
    int i = 0;

    while( i < N_ARRAY_SIZEOF(this->vp) && this->vp[i].GetSceneGraph())
    {
        if (this->vp[i].GetSceneGraph() == sceneGraph) 
        {
            return &this->vp[i];
        }
        i++;
    }

    n_assert(i < N_ARRAY_SIZEOF(this->vp));

    this->vp[i].SetSceneGraph(sceneGraph, entityObject);

    return &this->vp[i];
}

//---------------------------------------------------------------------------
/**
    On lost device, we must basically invalidate all the caches, so they are
    recalculated.
*/
void 
nGeoMipMapNode::RestoreOnLostDevice(void)
{
    //  update geometry
    int maxsize = this->terrainGMMClass->GetHeightMap()->GetSize() - 1;
    this->terrainGMM->UpdateGeometry(0, 0, maxsize, maxsize);

    // call to update weightmap
    #ifndef NGAME
    ncTerrainMaterialClass * tlm = this->terrainGMMClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);
    tlm->RefreshWeightTexture();

    // refresh global texture
    tlm->LoadGlobalTexture();
    #endif

    // call UpdateMaterials async on beginning next frame (outside begin/end scene)   
    nCmdProto * cmdp = this->terrainGMMClass->GetEntityClass()->GetClass()->FindCmdByName("updatematerialall");
    n_assert_return(cmdp,);
    nCmd * cmd = cmdp->NewCmd();
    nSignalServer::Instance()->PostCmd(0.0f, this->terrainGMMClass->GetEntityClass(), cmd);

    // delete the whole hole cache
    nGMMHoleIndicesCache * holeIndicesCache = 0;
    n_assert_return(this->terrainGMMClass, );
    holeIndicesCache = this->terrainGMMClass->GetHoleIndicesCache();
    if (holeIndicesCache)
    {
        holeIndicesCache->DiscardAll();
    }
}

//---------------------------------------------------------------------------
/**
    Preload the cache asynchronously
*/
void
nGeoMipMapNode::PreloadCache(const nGeoMipMapViewport * vpSettings)
{
    // Get distance from camera to terrain block centre
    const int * cameraBlockPos = vpSettings->GetCameraBlockPos();

    // get terrain material class
    ncTerrainMaterialClass * matc = this->terrainGMM->GetClassComponent<ncTerrainMaterialClass>();;
    n_assert(matc);

    // preload radius
    const int preloadRadius = matc->GetMaterialLODDistance() + matc->GetNumBlockPreload();

    nGMMTextureCacheEntry::SetAsync(false);

    int numLoaded = 0;
    int maxLoad = 1;
    int numBlocks = this->terrainGMMClass->GetNumBlocks();
    // preload block bx, bz
    for(int bz = max(0, cameraBlockPos[2] - preloadRadius);
        numLoaded < maxLoad &&
        bz < min(numBlocks, cameraBlockPos[2] + preloadRadius);
        bz++)
    {
        for(int bx = max(0, cameraBlockPos[0] - preloadRadius);
            numLoaded < maxLoad &&
            bx < min(numBlocks, cameraBlockPos[0] + preloadRadius);
            bx++)
        {
            if (! matc->IsWeightMapCached(bx, bz))
            {
                matc->GetWeightMapTexture(bx, bz, true);
                numLoaded++;
            }
        }
    }

    nGMMTextureCacheEntry::SetAsync(false);
}

#if 0
//---------------------------------------------------------------------------
/**
    Old test code, probably not working.
*/
//---------------------------------------------------------------------------
/**
    Render all terrain blocks (used when no visibility available).
    Same idea as RenderAllWithManyMeshes but using rendering with 2 streams.
    One with (x,z) which are fixed, and the other with just height (y) info.
    To render multiple streams a MeshArray is used.
    It saves memory and some processing, but it performed slower.
*/
bool 
nGeoMipMapNode::RenderAllWithManyMeshes2Streams(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    // super class render    
    if( !nGeometryNode::Render(sceneGraph, entityObject) )
    {
        return false;
    }

    // get graphics server and set the mesh on it
    nGfxServer2 * gfx = nGfxServer2::Instance();

    // get the heightmap from the geomipmap node
    nFloatMap * hm = this->refGeoMipMapTerrain->GetHeightMap();
    int size = hm->GetSize();
    int numblocks = (size-1) / (this->blockSize - 1);

    // get shader object
    nShader2 * shader = sceneGraph->GetShaderObject();

    // set mesh group (x,0,z) shared by all other mesh patches
    this->refMeshArray->SetMeshAt(0, this->refMeshXZ.get());
    nMeshGroup & meshGroup = this->refMeshXZ->GetGroup( 0 );

    for(int bz = 0;bz < numblocks;bz++) 
    {
        shader->SetFloat(nShaderState::Intensity1, static_cast<float> (bz * (blockSize - 1) * hm->GetGridScale()));
        for(int bx = 0;bx < numblocks;bx++) 
        {
            this->refMeshArray->SetMeshAt(1, this->refMeshPatches[bx][bz].get() );
            gfx->SetMeshArray( this->refMeshArray.get() );
            shader->SetFloat(nShaderState::Intensity0, static_cast<float> (bx * (blockSize - 1) * hm->GetGridScale()));

            // draw mesh
            gfx->SetVertexRange( meshGroup.GetFirstVertex(), meshGroup.GetNumVertices() );
            gfx->SetIndexRange( meshGroup.GetFirstIndex(), meshGroup.GetNumIndices() );
            gfx->DrawIndexedNS( nGfxServer2::TriangleList );
        }
    }

    return true;
}
#endif


#ifndef NGAME

//---------------------------------------------------------------------------
/**
    Draw the grass edition pass
*/
void
nGeoMipMapNode::SetRenderGrassEdition( bool render)
{
    this->renderGrassEdition = render;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGeoMipMapNode::GetRenderGrassEdition()
{
    return this->renderGrassEdition;
}
//---------------------------------------------------------------------------
/**
    Terrain grass edition material Rendering.
*/
#ifdef __ZOMBIE_EXPORTER__
bool
nGeoMipMapNode::RenderGrassEditionMaterial(nSceneGraph* , ncTerrainGMMCell* )
{
    return true;
}
#else
bool
nGeoMipMapNode::RenderGrassEditionMaterial(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell)
{

    // check if texture is available for this cell
    ncTerrainVegetationClass * vegClass = this->terrainGMMClass->GetComponentSafe<ncTerrainVegetationClass>();
    nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();

    nTexture2 * tex = vegClass->GetGrowthTexture( cell->GetCellX(), cell->GetCellZ() );
    if (tex)
    {
        shader->SetTexture(nShaderState::TerrainGrassEditionMap, tex);
    }

    nArray<vector4>  grassPalette;
    vegClass->GetTerrainGrassPalette(grassPalette);
    shader->SetVector4Array(nShaderState::TerrainGrassPalette, &grassPalette[0], grassPalette.Size() );

    float scale = static_cast<float>(vegClass->GetGrowthMapSizeByCell());

    shader->SetFloat(nShaderState::Scale, scale );


    return true;
}
#endif
#endif //!NGAME
