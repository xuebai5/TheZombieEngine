#include "precompiled/pchngeomipmap.h"
/*-----------------------------------------------------------------------------
    @file ngeomipmapviewport.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief Calculate some geomipmap parameters specific per viewport/camera

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngeomipmapviewport.h"
#include "gfx2/ngfxserver2.h"
#include "nscene/nviewportnode.h"
#include "nscene/nscenegraph.h"

//------------------------------------------------------------------------------
/**
*/
nGeoMipMapViewport::nGeoMipMapViewport() :
    sceneGraph(0),
    attachStarted(false),
    renderStarted(false),
    numAttached(0),
    numAttachedCells(0),
    numRendered(0),
    numRenderedCells(0),
    C2(0.0f),
    minLOD(0),
    maxLOD(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nGeoMipMapViewport::~nGeoMipMapViewport()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Calculate C squared factor used in the LOD checks.
    For detailed information check geomipmapping paper.
*/
float 
nGeoMipMapViewport::CalculateC2Factor(nCamera2 & camera, nViewport & viewport)
{
    // get camera position from View matrix
    this->cameraPos = this->sceneGraph->GetViewTransform().pos_component();
    this->CalculateCameraBlockPos(this->cameraPos);

    // get view volume camera parameters
    float minx, maxx, miny, maxy, minz, maxz;
    camera.GetViewVolume(minx, maxx, miny, maxy, minz, maxz);

    // get viewport height
    float height = viewport.height;

    // calculate, store and return C2 value
    float A = minz / fabsf(miny);
    float T = 2 * terrainGMMClass->GetError() / float(height);
    float C = (A / T);

    this->C2 = C * C;
    // protect division by zero
    if (this->C2 > 0)
    {
        this->C2Inv = 1.0f / this->C2;
    }
    else
    {
        this->C2Inv = 1.0e9;
    }
    return this->C2;
}

//------------------------------------------------------------------------------
void
nGeoMipMapViewport::CalculateCameraBlockPos(const vector3 & cameraPos)
{
    int numBlocks = this->terrainGMMClass->GetNumBlocks();
    float bsscaled = this->terrainGMMClass->GetBlockSideSizeScaled();
    if (bsscaled)
    {
        bsscaled = 1.0f / bsscaled;
    }

    vector3 scaledPos = cameraPos * bsscaled;
    cameraBlockPos[0] = static_cast<int> (scaledPos.x);
    cameraBlockPos[0] = max(0, min(numBlocks, cameraBlockPos[0]));
    cameraBlockPos[1] = static_cast<int> (scaledPos.y);
    cameraBlockPos[1] = max(0, min(numBlocks, cameraBlockPos[1]));
    cameraBlockPos[2] = static_cast<int> (scaledPos.z);
    cameraBlockPos[2] = max(0, min(numBlocks, cameraBlockPos[2]));
}

//------------------------------------------------------------------------------
const int *
nGeoMipMapViewport::GetCameraBlockPos() const
{
    return this->cameraBlockPos;
}

//------------------------------------------------------------------------------
void 
nGeoMipMapViewport::SetSceneGraph(nSceneGraph * sg, nEntityObject * entityObject)
{
    n_assert(entityObject);
    n_assert(entityObject->IsA("neoutdoorcell"));
    ncTerrainGMMCell * tgc = entityObject->GetComponentSafe<ncTerrainGMMCell>();
    n_assert(tgc);
    ncTerrainGMM * tgmm = tgc->GetOutdoor();
    n_assert(tgmm);
    nEntityClass * entityClass = tgmm->GetEntityClass();    
    n_assert(entityClass);
    this->terrainGMMClass = entityClass->GetComponentSafe<ncTerrainGMMClass>();
    n_assert(this->terrainGMMClass);

    this->sceneGraph = sg;
    this->attachStarted = false;
    this->renderStarted = false;
    this->numAttached = 0;
    this->numAttachedCells = 0;
    this->numRendered = 0;
    this->numRenderedCells = 0;
}

//------------------------------------------------------------------------------
nSceneGraph * 
nGeoMipMapViewport::GetSceneGraph()
{
    return this->sceneGraph;
}

//------------------------------------------------------------------------------
void
nGeoMipMapViewport::UpdateViewport()
{
    nGfxServer2 * gfx = nGfxServer2::Instance();
    nCamera2 & camera = gfx->GetCamera();
    nViewport & viewport = gfx->GetViewport();
    this->CalculateC2Factor(camera, viewport);
}

//------------------------------------------------------------------------------
bool 
nGeoMipMapViewport::IsFirstAttach()
{
    if (!this->attachStarted)
    {
        this->UpdateViewport();
        this->attachStarted = true;
        this->renderStarted = false;
        this->numAttached = 0;
        this->numAttachedCells = 0;
        this->numRendered = 0;
        this->numRenderedCells = 0;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
void
nGeoMipMapViewport::Attach(int sumMaterialLOD)
{
    //if (!this->attachStarted)
    //{
    //    this->UpdateViewport();
    //    this->attachStarted = true;
    //    this->renderStarted = false;
    //    this->numAttached = sumMaterialLOD > 1 ? 2 : 1;
    //    this->numAttachedCells = 0;
    //    this->numRendered = 0;
    //    this->numRenderedCells = 0;
    //    return true;
    //}

    this->numAttached += sumMaterialLOD;

    //NLOG(terrain, (nTerrainLog::NLOG_RENDER | 1, "ViewportAttach: numAttached=%d sceneGraph=0x%x", this->numAttached, sceneGraph));
}

//------------------------------------------------------------------------------
void 
nGeoMipMapViewport::AttachCell()
{
    this->numAttachedCells++;
}

//------------------------------------------------------------------------------
bool
nGeoMipMapViewport::Apply()
{
    if (!this->renderStarted)
    {
        this->attachStarted = false;
        this->renderStarted = false;
        this->numRendered = 0;
        this->numRenderedCells = 0;
        return true;
    }
    return false;
#if 0
    /// apply not called for all viewports in conjurer
    int i = 0;
    while( i < N_ARRAY_SIZEOF(this->viewport) && this->viewport[i].sceneGraph )
    {
        this->viewport[i].attachStarted = false;
        this->viewport[i].numRendered = 0;
        i++;
    }
#endif
}

//------------------------------------------------------------------------------
void 
nGeoMipMapViewport::Render()
{
    if (!this->renderStarted)
    {
        n_assert(0 == this->numRendered);
        //this->UpdateViewport();
        this->renderStarted = true;
    }
    this->numRendered++;
}

//------------------------------------------------------------------------------
void 
nGeoMipMapViewport::RenderCell()
{
    this->numRenderedCells++;
}

//------------------------------------------------------------------------------
bool 
nGeoMipMapViewport::IsFirstRender()
{
    return !this->renderStarted;
}

//------------------------------------------------------------------------------
bool 
nGeoMipMapViewport::IsLastRender()
{
    // the 2 is because we have two scene passes gmm0, gmm1
    //return ((2 * this->numAttached) == this->numRendered);
    return (this->numAttached == this->numRendered);
}
