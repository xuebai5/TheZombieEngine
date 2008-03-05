#include "precompiled/pchnspatial.h"
//--------------------------------------------------
// nvisiblefrustumvisitor.cc
// (C) 2004 Conjurer Services, S.A.
// @author Miquel Angel Rujula <>
//--------------------------------------------------

#include "nspatial/nvisiblefrustumvisitor.h"
#include "nspatial/nspatialserver.h"
#include "entity/nentityobjectserver.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialoccluder.h"
#include "nspatial/ncspatialbatch.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/nspatialoctreecell.h"
#include "nspatial/ncspatialoctree.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/nhorizonclipper.h"
#include "nspatial/nccellhorizon.h"
#include "nspatial/nchorizonclass.h"
#include "nspatial/nhorizonsegment.h"
#include "nspatial/nlightregion.h"

#include "nscene/ncscene.h"

#include "zombieentity/nctransform.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ngeomipmap/ncterraingmmcell.h"
#include "nspecialfx/nfxserver.h"
#endif

#include "kernel/nlogclass.h"

#include "nphysics/ncphyterraincell.h"

#define DEACT_HORIZON_ANGLE -0.65f

NCREATELOGLEVEL(visibilityVisitor, "Visibility Visitor", false, 1);

int spatialIndent = 0;

//------------------------------------------------------------------------------
/**
*/
nVisibleFrustumVisitor::nVisibleFrustumVisitor():
#ifndef NGAME
m_disableQuadtree(false),
disableLights(false),
m_doHorizonCulling(true),
#endif // !NGAME
spatialServer(0),
sceneGraph(0),
m_spatialCamera(0),
cameraTrComp(0),
m_currentCamId(nEntityObjectServer::IDINVALID),
m_currRecursionDepth(0),
profVisOcclusion("profVis_Occlusion", true),
profVisOcclusionFrus("profVis_BuildOcclFrus", true),
profVisHorizonTest("profVis_HorizonTest", true),
profVisHorizonInsert("profVis_HorizonInsert", true),
profVisQuadCell("profVis_QuadCellVis", true),
profVisQuadCellWhole("profVis_QuadCellWhole", true),
profVisTestEntities("profVis_TestEntities", true),
profVisTestBatches("profVis_TestBatches", true),
profVisHorizDebug("profVis_HorizDebug", true),
profVisCamera("profVis_VisitCamera", true),
profVisMakeTerrainVisible("profVis_MakeTerrainVisible", true),
profDetermineLOD("profVis_DetermineLod", true),

#ifndef NGAME
numTestedEntities(0),
watchNumTestedEntities("visTestedEntities", nArg::Int),
numTestedBatches(0),
watchNumTestedBatches("visTestedBatches", nArg::Int),
numTestedQuadCells(0),
watchNumTestedQuadCells("visTestedQuadCells", nArg::Int),
#endif // !NGAME
m_disableHorizon(false),
m_traversedAnyPortal(false),
outdoorVisible(false)
{
    // set occlusion flags
    this->m_occlusionFlags  = nSpatialTypes::SPF_OUTDOORS;
    this->m_occlusionFlags |= nSpatialTypes::SPF_CONTAINING;
    this->m_occlusionFlags |= nSpatialTypes::SPF_USE_POSITION;
}

//------------------------------------------------------------------------------
/**
*/
nVisibleFrustumVisitor::~nVisibleFrustumVisitor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    set pointer to spatial server
*/
void 
nVisibleFrustumVisitor::SetSpatialServer(nSpatialServer* spatialServer)
{
    this->spatialServer = spatialServer;
}

//------------------------------------------------------------------------------
/**
    set the visibility categories
*/
void
nVisibleFrustumVisitor::SetVisCategoriesMap()
{
    this->m_visCategoriesMap = this->spatialServer->GetVisCategoriesMap();
}

//------------------------------------------------------------------------------
/**
*/
void
nVisibleFrustumVisitor::SetCamera(ncSpatialCamera* cameraComp)
{
    this->m_spatialCamera = cameraComp;

    // reset the visible terrain cells array
    this->ResetVisibleTerrainCells();

    // set camera id
    this->m_currentCamId = cameraComp->GetEntityObject()->GetId();

    // set camera's transform component
    this->cameraTrComp = cameraComp->GetComponent<ncTransform>();

    // check if we have to disable the horizon culling
    ncSpatialQuadtree *quadtreeSpace = this->spatialServer->GetQuadtreeSpace();
    ncHorizonClass *hc = quadtreeSpace->GetClassComponentSafe<ncHorizonClass>();
    float cameraXAngle = this->cameraTrComp->GetEuler().x;
    this->m_disableHorizon = (cameraXAngle < DEACT_HORIZON_ANGLE) || !hc || !hc->IsValid() ;

    // initialize current recursion depth
    this->m_currRecursionDepth = 0;

    // select the occluders
    this->SelectOccluders();

    NLOG(visibilityVisitor, (1, " ----------------- START FRAME ------------------- "));
    
}

//------------------------------------------------------------------------------
/**
    select the best occluders
*/
void
nVisibleFrustumVisitor::SelectOccluders()
{
    // reset the array of selected occluders
    this->m_spatialCamera->ResetOccludersArray();
    // reset the occlusion frustums array
    this->m_spatialCamera->ResetOcclusionFrustums();

#ifndef NGAME
    // reset the array of discarded occluders (only for debug purposes)
    this->m_spatialCamera->ResetDiscardedOccludersArray();
#endif // !NGAME

    // get all the occluders around the camera in its occlusion radius
    sphere sph(this->cameraTrComp->GetPosition(), 
               this->m_spatialCamera->GetOcclusionRadius());

    static nArray<nEntityObject*> occluders;
    occluders.Reset();

    this->spatialServer->GetEntitiesCategory(sph, 
                                             nSpatialTypes::CAT_OCCLUDERS, 
                                             this->m_occlusionFlags, 
                                             occluders);
    if (!occluders.Empty())
    {
        // remove those occluders that are outside the view frustum
        VisitorFlags clipstatus(false, false);
        nCamera2 camera(this->m_spatialCamera->GetCamera());
        nFrustumClipper frustum(camera,
                                this->m_spatialCamera->GetViewMatrix());

        VisitorFlags vf;
        for (int i(0); i < occluders.Size(); ++i)
        {    
            struct ClipVertexInfo vertsInfo[4];
            vf = frustum.TestPortalVertices(occluders[i]->GetComponentSafe<ncSpatialOccluder>()->GetVertices(), 
                                            4,
                                            clipstatus,
                                            vertsInfo);
            if (!vf.AntiTestResult())
            {
                // the occluder is partially or totally visible.
                // Give it to the camera to consider it or not for occlusion
                this->m_spatialCamera->ConsiderOccluder(occluders[i]->GetComponentSafe<ncSpatialOccluder>());
            }
        }

        //profVisOcclusionFrus.StartAccum();

        // build occlusion frustums for the selected occluders
        this->m_spatialCamera->BuildOcclusionFrustums();

        //profVisOcclusionFrus.StopAccum();
    }
}

//------------------------------------------------------------------------------
/**
    get the array of visible lights
*/
nArray<nEntityObject*> *
nVisibleFrustumVisitor::GetVisibleLights()
{
    return &this->m_visibleLights;
}

//------------------------------------------------------------------------------
/**
    add a terrain cell to the visible array
*/
void 
nVisibleFrustumVisitor::AddVisibleTerrainCell(ncSpatialQuadtreeCell* cell)
{
    this->m_visibleTerrainCells.Append(cell);
}

//------------------------------------------------------------------------------
/**
    remove a terrain cell from the visible array
*/
void 
nVisibleFrustumVisitor::RemoveVisibleTerrainCell(ncSpatialQuadtreeCell* cell)
{
    int index = this->m_visibleTerrainCells.FindIndex(cell);
    if (index != -1)
    {
        this->m_visibleTerrainCells.EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
    reset the visible terrain cells array
*/
void 
nVisibleFrustumVisitor::ResetVisibleTerrainCells()
{
    this->m_visibleTerrainCells.Reset();
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(ncSpatialCamera *visitee)
{
    n_assert2(visitee, "miquelangel.rujula: NULL pointer to camera entity!");
    
    #ifdef __NEBULA_STATS__
    this->profVisCamera.StartAccum();
    #endif

    this->outdoorVisible = false;

    this->SetCamera(visitee);
    
#ifndef NGAME
    if ( this->spatialServer->IsInEditIndoorMode() )
    {
        nEntityObject* selectedEntity = this->spatialServer->GetSelectedEntity();
        if ( selectedEntity )
        {
            ncSpatial* spatComp = selectedEntity->GetComponent<ncSpatial>();
            if ( spatComp && 
                 spatComp->GetCell() && 
                 (spatComp->GetCell()->GetType() == ncSpatialCell::N_INDOOR_CELL) )
            {
                this->VisitCellAndNeighbors(static_cast<nSpatialIndoorCell*>(spatComp->GetCell()));
            }
            else
            {
                this->VisitCellAndNeighbors(static_cast<nSpatialIndoorCell*>(visitee->GetCell()));
            }
        }
        else
        {
            this->VisitCellAndNeighbors(static_cast<nSpatialIndoorCell*>(visitee->GetCell()));
        }
    }
    else if (this->spatialServer->GetSeeAllSelectedIndoors() && 
            !this->spatialServer->GetSelectedIndoors().Empty())
    {
        this->VisitAllSelectedIndoors();
    }
    else
    {
#endif // !NGAME
    // start the visibility determination from the camera's cell
    this->Visit(visitee->GetCell());
#ifndef NGAME
    }

    // set watches values
    this->watchNumTestedEntities->SetI(this->numTestedEntities);
    this->numTestedEntities = 0;
    this->watchNumTestedBatches->SetI(this->numTestedBatches);
    this->numTestedBatches = 0;
    this->watchNumTestedQuadCells->SetI(this->numTestedQuadCells);
    this->numTestedQuadCells = 0;
    
#endif // !NGAME

    // visibility was determined. Append the visible terrain cells.
    #ifdef __NEBULA_STATS__
    this->profVisMakeTerrainVisible.StartAccum();
    #endif

#ifndef NGAME
    if (this->m_disableQuadtree)
    {
        #ifdef __NEBULA_STATS__
        this->profVisCamera.StopAccum();
        this->profVisMakeTerrainVisible.StopAccum();
        #endif
        return;
    }

    if (this->m_spatialCamera->GetUseCameraCollector())
    {
        for ( int i(0); i < this->m_visibleTerrainCells.Size(); ++i )
        {
            this->m_spatialCamera->AppendVisibleEntity(this->m_visibleTerrainCells[i]->GetEntityObject());
        }
    }
#endif // !NGAME

    if (this->sceneGraph)
    {
        ncScene* sceneComponent = 0;
        double frameTime(nTimeServer::Instance()->GetFrameTime());
        int frameId(this->spatialServer->GetFrameId());
        for ( int i(0); i < this->m_visibleTerrainCells.Size(); ++i )
        {
            sceneComponent = this->m_visibleTerrainCells[i]->GetComponent<ncScene>();
            if (sceneComponent)
            {
                // HACK ma.garcias - check that the component has not been rendered this frame
                if (sceneComponent->GetFrameId() != (uint) frameId)
                {
                    sceneComponent->SetTime(frameTime);
                    sceneComponent->SetFrameId(frameId);
                    sceneComponent->Render(this->sceneGraph);
                }

                // make its links visibles
                ncSpatialLight *light;
                for ( int i(0); i < sceneComponent->GetNumLinks(); ++i )
                {
                    light = sceneComponent->GetLinkAt(i)->GetComponentSafe<ncSpatialLight>();
                    if ( !light->IsVisibleBy(this->m_currentCamId) )
                    {
                        light->Accept(*this);
                    }
                }
            }

        #ifndef __ZOMBIE_EXPORTER__
            //TODO- render decals on terrain cells
            //nFXServer::Instance()->RenderEffects(this->sceneGraph, this->m_visibleTerrainCells[i]->GetEntityObject());
        #endif
        }
    }

    #ifdef __NEBULA_STATS__
    this->profVisMakeTerrainVisible.StopAccum();
    this->profVisCamera.StopAccum();
    #endif
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::VisitCellAndNeighbors(nSpatialIndoorCell *visitee)
{
    n_assert2(visitee, "miquelangel.rujula");

    VisitorFlags clipstatus(false, false);
    nCamera2 camera = this->m_spatialCamera->GetCamera();
    nFrustumClipper frustum(camera,
                            this->m_spatialCamera->GetViewMatrix());
    nHorizonClipper horizon(this->m_spatialCamera->GetWidth(),
                            this->m_spatialCamera->GetHeight());
    horizon.SetViewProjection(this->m_spatialCamera->GetViewProjection());
    horizon.SetNearPlane(frustum.GetNearPlane());

    // visit the main cell
    this->Visit(visitee, 
                frustum,
                clipstatus,
                horizon);

    // visit its neighbors
    nArray<ncSpatialCell*> neighbors;
    visitee->GetNeighbors(neighbors);
    for ( int i(0); i < neighbors.Size(); ++i )
    {
        if ( neighbors[i]->GetType() == ncSpatialCell::N_INDOOR_CELL )
        {
            this->Visit(neighbors[i], 
                        frustum,
                        clipstatus,
                        horizon);
        }
    }
}

//------------------------------------------------------------------------------
/**
    make visibles all the entities in the selected indoors
*/
void
nVisibleFrustumVisitor::VisitAllSelectedIndoors()
{
    const nArray<nRefEntityObject> &selectedIndoors = this->spatialServer->GetSelectedIndoors();
    ncSpatialIndoor *indoor = 0;
    for (int i(0); i < selectedIndoors.Size(); ++i)
    {
        if (selectedIndoors[i].isvalid())
        {
            indoor = selectedIndoors[i].get()->GetComponentSafe<ncSpatialIndoor>();
            this->MakeVisibleAllEntities(indoor);
        }
    }
}

//------------------------------------------------------------------------------
/**
    make visibles all the entities in the given indoor
*/
void
nVisibleFrustumVisitor::MakeVisibleAllEntities(ncSpatialIndoor *indoor)
{
    n_assert(indoor);

    VisitorFlags clipstatus(false, false);
    nCamera2 camera = this->m_spatialCamera->GetCamera();
    nFrustumClipper frustum(camera,
                            this->m_spatialCamera->GetViewMatrix());
    nHorizonClipper horizon(this->m_spatialCamera->GetWidth(), 
                            this->m_spatialCamera->GetHeight());
    horizon.SetViewProjection(this->m_spatialCamera->GetViewProjection());
    horizon.SetNearPlane(frustum.GetNearPlane());

    const nArray<ncSpatialCell*>& cells = indoor->GetCellsArray();
    ncSpatialCell* cell = 0;

    for (int i(0); i < cells.Size(); ++i)
    {
        cell = cells[i];
        this->Visit(cell,frustum, clipstatus, horizon);
    }
}

#endif

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(ncSpatialCell *visitee)
{
    n_assert2(visitee, "miquelangel.rujula");

    VisitorFlags clipstatus(false, false);
    nCamera2 camera = this->m_spatialCamera->GetCamera();
    nFrustumClipper frustum(camera,
                            this->m_spatialCamera->GetViewMatrix());
    nHorizonClipper horizon(this->m_spatialCamera->GetWidth(), 
                            this->m_spatialCamera->GetHeight());
    horizon.SetViewProjection(this->m_spatialCamera->GetViewProjection());
    horizon.SetNearPlane(frustum.GetNearPlane());

    ncSpatialQuadtree *quadtreeSpace;
    ncSpatialCell::nSpatialCellType cellType = visitee->GetType();
    switch(cellType)
    {
        case ncSpatialCell::N_INDOOR_CELL: 
            this->Visit(static_cast<nSpatialIndoorCell*>(visitee), 
                        frustum,
                        clipstatus,
                        horizon); 
            break;

        case ncSpatialCell::N_QUADTREE_CELL: 
            quadtreeSpace = static_cast<ncSpatialQuadtree*>(visitee->GetParentSpace());
            this->Visit(quadtreeSpace,
                        frustum,
                        clipstatus,
                        horizon);
            break;

        case ncSpatialCell::N_OCTREE_CELL: 
            this->Visit(static_cast<ncSpatialOctree*>(visitee->GetParentSpace())); 
            break;
            
        default: 
            break;

    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(ncSpatialCell *visitee, 
                              const nFrustumClipper &frustum, 
                              const VisitorFlags &clipstatus,
                              nHorizonClipper &horizon)
{
    n_assert2(visitee, "miquelangel.rujula");

    ncSpatialQuadtree *quadtreeSpace;
    ncSpatialCell::nSpatialCellType cellType = visitee->GetType();
    switch(cellType)
    {
        case ncSpatialCell::N_INDOOR_CELL:     
            this->Visit(static_cast<nSpatialIndoorCell*>(visitee), 
                        frustum,
                        clipstatus,
                        horizon); 
            break;

        case ncSpatialCell::N_QUADTREE_CELL: 
            quadtreeSpace = static_cast<ncSpatialQuadtree*>(visitee->GetParentSpace());
            this->Visit(quadtreeSpace,
                        frustum,
                        clipstatus,
                        horizon);
            break;

        case ncSpatialCell::N_OCTREE_CELL: 
            this->Visit(static_cast<ncSpatialOctree*>(visitee->GetParentSpace())); 
            break;
            
        default: 
            break;

    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(nSpatialIndoorCell *visitee, 
                              const nFrustumClipper &frustum,
                              const VisitorFlags &clipstatus,
                              nHorizonClipper &horizon)
{
    n_assert2(visitee, "miquelangel.rujula");

    NLOG(visibilityVisitor, (1, "%*s - Visiting indoor cell (type: %d).",
                            ++spatialIndent * 4, " ",
                            visitee->GetType()));

    // if it's the first time that this cell is visited in the current frame by the
    // current camera, reset the array of visible entities
    if (!visitee->IsVisitedBy(this->m_currentCamId))
    {
        // reset the array of visible entities of this cell
        visitee->ResetVisEntitiesArray();
    }

    // set this cell visible by the current camera
    visitee->SetVisibleBy(this->m_currentCamId);

    // append the indoor cell to the visible indoor cells array of the camera
    this->m_spatialCamera->AddVisibleIndoorCell(visitee);

    // check if every entity in the cell is visible or not
    this->TestEntities(visitee, frustum, clipstatus, horizon);

    // check if the shadows in this cell are visibles or not
    this->TestShadows(visitee, frustum, clipstatus);

    // check if every portal in the cell is visible or not
    const nArray<nEntityObject*> &portalsArray = visitee->GetCategory(nSpatialTypes::CAT_PORTALS);
    this->TestPortals(portalsArray, frustum, clipstatus, horizon);

    NLOG(visibilityVisitor, (1, "%*s - LEAVING indoor cell (type: %d).",
                            --spatialIndent * 4, " ",
                            visitee->GetType()));
}

//------------------------------------------------------------------------------
/**
    Initialize the quadtree cell's visibility information
*/
void
nVisibleFrustumVisitor::InitQuadtreeCellVis(ncSpatialQuadtreeCell *cell)
{
    // set this cell as visited
    cell->SetVisitedBy(this->m_currentCamId);

    // append the cell to the visible quadtree cells array of the camera (CHECK THIS OUT NOW!!!!!!!)
    this->m_spatialCamera->AddVisibleQuadtreeCell(cell);

    // reset cell's visible entities array
    cell->ResetVisEntitiesArray();
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(ncSpatialQuadtree *visitee,
                              const nFrustumClipper &frustum,
                              const VisitorFlags &clipstatus,
                              nHorizonClipper &horizon)
{
    n_assert2(visitee, "miquelangel.rujula");
    
    #ifdef __NEBULA_STATS__
    this->profVisQuadCellWhole.StartAccum();
    #endif

    this->outdoorVisible = true;

    VisitorFlags terrainFlags(false, false);
    this->Visit(visitee->GetRootCell(), 
                frustum, 
                clipstatus, 
                terrainFlags, 
                horizon, 
                false);

    #ifdef __NEBULA_STATS__
    this->profVisQuadCellWhole.StopAccum();
    #endif

#ifndef NGAME
    #ifdef __NEBULA_STATS__
    this->profVisHorizDebug.StartAccum();
    #endif
    if ( this->m_spatialCamera->IsDebuggingHorizon() )
    {
        this->m_spatialCamera->SetHorizon(horizon);
    }
    #ifdef __NEBULA_STATS__
    this->profVisHorizDebug.StopAccum();
    #endif
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    Determines visibility for the given quadtree cell ('visitee')
*/
void 
nVisibleFrustumVisitor::Visit(ncSpatialQuadtreeCell *visitee,
                              const nFrustumClipper &frustum,
                              const VisitorFlags &clipstatus,
                              const VisitorFlags &terrainFlags,
                              nHorizonClipper &horizon,
                              bool fatherTerrainOccluded)
{
    n_assert2(visitee, "Null pointer to a quadtree cell!");

#ifndef NGAME
    ++this->numTestedQuadCells;
#endif // !NGAME

    VisitorFlags newCellFlags(clipstatus);
    if ( !visitee->IsVisitedBy(this->m_currentCamId) )
    {
        // this is the first time this cell is visited in this frame, 
        // initialize it
        this->InitQuadtreeCellVis(visitee);
    }

    // check if the whole cell is visible
    if (!this->IsQuadtreeCellVisible(visitee, frustum, clipstatus, newCellFlags, horizon))
    {
        return;
    }

    visitee->SetVisibleBy(this->m_currentCamId);

    // check if the cell's terrain is visible
    bool terrainOccluded(fatherTerrainOccluded);
    VisitorFlags newTerrainFlags(terrainFlags);
    if ( !terrainOccluded )
    {
        if ( visitee->GetEntityObject() )
        {
            terrainOccluded = this->IsTerrainCellVisible(visitee, frustum, terrainOccluded, clipstatus, 
                            terrainFlags, newTerrainFlags, horizon);
        }
        else
        {
            terrainOccluded = true;
        }
    }

    // test the entities in this cell if the cell is near enough or if the camera is in the cell
    const vector3 &camPos = this->cameraTrComp->GetPosition();
    if ( visitee->GetBBox().contains(camPos) || 
         (visitee->GetBBoxCenter() - camPos).lensquared() < 
         this->m_spatialCamera->GetMaxTestOutdoorSqDist() )
    {
        this->TestEntities(visitee, frustum, clipstatus, horizon);

        // check if the shadows in this cell are visibles or not
        this->TestShadows(visitee, frustum, clipstatus);

        ncPhyTerrainCell *phyComp(visitee->GetComponent<ncPhyTerrainCell>()); 

        if ( phyComp )
        {
            phyComp->SetVisible();
        }
    }

    // test the batches in this cell
    this->TestBatches(visitee, frustum, clipstatus, horizon);

    // test the portals in this cell, if any
    const nArray<nEntityObject*> &portalsArray = visitee->GetCategory(nSpatialTypes::CAT_PORTALS);
    if (portalsArray.Size() > 0)
    {
        this->TestPortals(portalsArray, frustum, clipstatus, horizon);
    }

    if (!visitee->IsLeaf())
    {
        this->ProcessSubcells(visitee, frustum, terrainOccluded, newCellFlags, newTerrainFlags, horizon);
    }
    else
    {
        if ( !terrainOccluded && 
             !visitee->IsTerrainVisibleBy(this->m_currentCamId) &&
             !visitee->GetParentCell()->IsTerrainVisibleBy(this->m_currentCamId) )
        {
            // the cell is also a leaf
            visitee->SetTerrainVisibleBy(this->m_currentCamId);

            // attach it
            visitee->Accept(*this);

            if ((this->m_doHorizonCulling) && !this->m_disableHorizon)
            {
                // update horizon
                profVisHorizonInsert.StartAccum();
                this->UpdateHorizon(visitee, horizon);
                profVisHorizonInsert.StopAccum();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check if the given quadtree cell is visible or not (just the cell, not what's
    inside it)
*/
bool
nVisibleFrustumVisitor::IsQuadtreeCellVisible(ncSpatialQuadtreeCell *cell, 
    const nFrustumClipper &frustum,
    const VisitorFlags &clipstatus,
    VisitorFlags &newCellFlags,
    nHorizonClipper &horizon)
{
    #ifdef __NEBULA_STATS__
    this->profVisQuadCell.StartAccum();
    #endif
    if (clipstatus.activeflags != 0 && !clipstatus.TestResult())
    {
        // check if the cell is visible or not
        frustum.TestBBox(cell->GetBBoxCenter(), cell->GetBBoxExtents(), clipstatus, newCellFlags);
                
        if (newCellFlags.AntiTestResult())
        {
            // the quadtree cell is clipped
            #ifdef __NEBULA_STATS__
            this->profVisQuadCell.StopAccum();
            #endif
            return false;
        }
    }

    const bbox3 &cellBox = cell->GetBBox();
    #ifdef __NEBULA_STATS__
    this->profVisHorizonTest.StartAccum();
    #endif
    // check if the cell is above or below the horizon
    if ( this->m_doHorizonCulling && !this->m_disableHorizon  &&
        (horizon.TestFast(cellBox) == nHorizonClipper::BELOW))
    {
        // the quadtree cell is below the horizon
        #ifdef __NEBULA_STATS__
        this->profVisQuadCell.StopAccum();
        this->profVisHorizonTest.StopAccum();
        #endif
        return false;
    }

    #ifdef __NEBULA_STATS__
    this->profVisHorizonTest.StopAccum();
    #endif

    // check if the cell is occluded or not
    if (this->IsOccluded(cell->GetBBoxCenter(), cell->GetBBoxExtents()))
    {
        #ifdef __NEBULA_STATS__
        this->profVisQuadCell.StopAccum();
        #endif
        return false;
    }

    // the cell is visible
    #ifdef __NEBULA_STATS__
    this->profVisQuadCell.StopAccum();
    #endif
    return true;
}

//------------------------------------------------------------------------------
/**
    Check if the terrain of the given quadtree cell is visible or not
*/
bool
nVisibleFrustumVisitor::IsTerrainCellVisible(ncSpatialQuadtreeCell *cell,
    const nFrustumClipper &frustum,
    bool currTerrOccluded,
    const VisitorFlags &clipstatus,
    const VisitorFlags &terrainFlags,
    VisitorFlags &newTerrainFlags, 
    nHorizonClipper &horizon)
{
#ifndef __ZOMBIE_EXPORTER__
    ncTerrainGMMCell* gmmCell = cell->GetComponentSafe<ncTerrainGMMCell>();
    const bbox3 &terrainBox = gmmCell->GetBBox();
#else
        // @todo fixme, fake code, just to compile with exporter and avoid extra dependency
    const bbox3 terrainBox;
#endif

    bool terrainOccluded(currTerrOccluded);
    if (terrainFlags.activeflags != 0 && !terrainFlags.TestResult())
    {
        frustum.TestBBox(terrainBox, clipstatus, newTerrainFlags);
        terrainOccluded = newTerrainFlags.AntiTestResult();
    }

    if (!terrainOccluded)
    {
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StartAccum();
        #endif
        if ( this->m_doHorizonCulling && !this->m_disableHorizon  &&
            (horizon.TestFast(terrainBox) == nHorizonClipper::BELOW))
        {
            // the terrain cell is below the horizon
            terrainOccluded = true;
        }
        else
        {
            // test occlusion for the terrain of this cell
            terrainOccluded = this->IsOccluded(terrainBox);
        }
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StopAccum();
        #endif
    }

    return terrainOccluded;
}

//------------------------------------------------------------------------------
/**
    Process quadtree cell's subcells
*/
void
nVisibleFrustumVisitor::ProcessSubcells(ncSpatialQuadtreeCell *cell, 
    const nFrustumClipper &frustum,
    bool terrainOccluded,
    const VisitorFlags &newCellFlags,
    const VisitorFlags &newTerrainFlags,
    nHorizonClipper &horizon)
{
    // recurse to subcells
    ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
    bool allChildsVisibles(true);

    // determine nearest child to the camera
    ncSpatialQuadtreeCell** sortedChilds = this->SortChilds(subcells);
    for (int i(0); i < 4; ++i)
    {
        if (sortedChilds[i]->IsActive())
        {
            this->Visit(static_cast<ncSpatialQuadtreeCell*>(sortedChilds[i]), 
                                                            frustum, 
                                                            newCellFlags,
                                                            newTerrainFlags,
                                                            horizon,
                                                            terrainOccluded);
            allChildsVisibles &= static_cast<ncSpatialQuadtreeCell*>(sortedChilds[i])->IsTerrainVisibleBy(this->m_currentCamId);
        }
    }

    n_delete_array(sortedChilds);
    
    if (!terrainOccluded && allChildsVisibles)
    {
        // if all childs are visibles attach this cell instead
        for (int i(0); i < 4; ++i)
        {
            this->RemoveVisibleTerrainCell(subcells[i]);
        }

        if (!cell->IsTerrainVisibleBy(this->m_currentCamId))
        {
            cell->SetTerrainVisibleBy(this->m_currentCamId);
            
            // attach it
            cell->Accept(*this);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Update the given horizon using the cell's horizon segments
*/
void
nVisibleFrustumVisitor::UpdateHorizon(const ncSpatialQuadtreeCell *cell, nHorizonClipper &horizon)
{
    const ncCellHorizon *horizCell = cell->GetComponentSafe<ncCellHorizon>();
    nHorizonSegment *segment = horizCell->GetHorizonSegment(ncCellHorizon::North);
    n_assert(segment);

    if (!segment->IsUsedBy(this->m_currentCamId))
    {
        horizon.Insert(segment->GetStart(), segment->GetEnd());
        segment->SetUsedBy(this->m_currentCamId);
    }

    segment = horizCell->GetHorizonSegment(ncCellHorizon::South);
    if (!segment->IsUsedBy(this->m_currentCamId))
    {
        horizon.Insert(segment->GetStart(), segment->GetEnd());
        segment->SetUsedBy(this->m_currentCamId);
    }

    segment = horizCell->GetHorizonSegment(ncCellHorizon::West);
    if (!segment->IsUsedBy(this->m_currentCamId))
    {
        horizon.Insert(segment->GetStart(), segment->GetEnd());
        segment->SetUsedBy(this->m_currentCamId);
    }

    segment = horizCell->GetHorizonSegment(ncCellHorizon::East);
    if (!segment->IsUsedBy(this->m_currentCamId))
    {
        horizon.Insert(segment->GetStart(), segment->GetEnd());
        segment->SetUsedBy(this->m_currentCamId);
    }
}

//------------------------------------------------------------------------------
/**
    sort the child cells of a quadtree cell for the horizon clipper algorithm
*/
ncSpatialQuadtreeCell **
nVisibleFrustumVisitor::SortChilds(ncSpatialQuadtreeCell **childs)
{
    ncSpatialQuadtreeCell **sortedChilds = n_new_array(ncSpatialQuadtreeCell*, 4);

    // calculate the nearest child to the camera
    const vector3 &campos = this->cameraTrComp->GetPosition();
    float minDist = FLT_MAX;
    float currDist = FLT_MIN;
    int nearest = -1;

    // the nearest cell is that one that contains the camera's position
    for (int i(0); i < 4; ++i)
    {
        if (childs[i]->GetBBox().contains(campos))
        {
            nearest = i;
            break;
        }
    }

    // if there's no cell containing the camera's position, calculate the nearest one by distance
    if (nearest == -1)
    {
        for (int i(0); i < 4; ++i)
        {
            currDist = (childs[i]->GetBBox().center() - campos).lensquared();
            if (currDist < minDist)
            {
                nearest = i;
                minDist = currDist;
            }
        }
    }

    n_assert(nearest > -1 && nearest < 4);

    sortedChilds[0] = childs[nearest];

    switch(nearest)
    {
        case 0:
            sortedChilds[1] = childs[1];
            sortedChilds[2] = childs[2];
            sortedChilds[3] = childs[3];
            break;
        case 1:
            sortedChilds[1] = childs[0];
            sortedChilds[2] = childs[3];
            sortedChilds[3] = childs[2];
            break;
        case 2:
            sortedChilds[1] = childs[3];
            sortedChilds[2] = childs[0];
            sortedChilds[3] = childs[1];
            break;
        case 3:
            sortedChilds[1] = childs[1];
            sortedChilds[2] = childs[2];
            sortedChilds[3] = childs[0];
            break;
        default:
            n_delete_array(sortedChilds);
            return 0;
            break;
    }

    return sortedChilds;
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(ncSpatialOctree *visitee)
{
    nOctNode *rootnode = visitee->m_octree->GetRoot();
    VisitorFlags clipinfo;
    nCamera2 camera = this->m_spatialCamera->GetCamera();
    nFrustumClipper frustum(camera,
                            this->m_spatialCamera->GetViewMatrix());

    // recursively descend the octree checking each node for clipping
    this->CheckOctNode(rootnode, frustum, clipinfo);
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::CheckOctNode(nOctNode *testnode, 
                                     nFrustumClipper &clipper, 
                                     VisitorFlags clipstatus)
{
    VisitorFlags newStatus(clipstatus);

    // if the node is totally enclosed, trivially accept all the children nodes.  otherwise, do a frustum clip test
    if (clipstatus.activeflags != 0)
    {
        bbox3 nodebbox ( (testnode->minCorner + testnode->maxCorner)*0.5, (testnode->maxCorner - testnode->minCorner)*0.5);
        clipper.TestBBox(nodebbox, clipstatus, newStatus);
    }

    // if the node is culled, then ignore this node and all nodes below it
    if (newStatus.AntiTestResult())
    {
        return;
    }

    // this node is not culled.  Test all the elements in this node, and then recurse to child nodes.
    nOctElement *oe;
    for (oe = (nOctElement *) testnode->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        nEntityObject *entity = static_cast<nEntityObject*>(oe->GetPtr());
        ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
        ncScene *sceneComp = entity->GetComponent<ncScene>();
        VisitorFlags vf;
        clipper.TestBBox(spatialComponent->GetBBoxCenter(), spatialComponent->GetBBoxExtents(), newStatus, vf);
        if (!vf.AntiTestResult())
        {
            this->DetermineLOD(spatialComponent);
            sceneComp->SetFlag(ncScene::ShapeVisible, true);
            spatialComponent->Accept(*this);
        }
    }

    // now check the children of this node
    if (testnode->c[0])
    for (int childix=0; childix < 8; ++childix)
    {
        this->CheckOctNode(testnode->c[childix], clipper, newStatus);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nVisibleFrustumVisitor::Visit(nEntityObject *visitee)
{
    n_assert2(visitee, "miquelangel.rujula: NULL pointer to entity!");

#ifndef NGAME
	if ((this->m_disableQuadtree && visitee->GetComponent<ncSpatialQuadtreeCell>()) ||
        (this->disableLights && visitee->GetComponent<ncSpatialLight>()))
	{
		return;
	}
#endif // !NGAME

    // if the visitee has a spatial component or a portal component, append it
    ncSpatial* spatialComp = visitee->GetComponent<ncSpatial>();
    if (spatialComp)
    {
#ifndef NGAME
        if (this->m_spatialCamera->GetUseCameraCollector())
        {
            this->m_spatialCamera->AppendVisibleEntity(visitee);
        }
#endif // !NGAME

        if (this->sceneGraph)
        {
            ncScene* sceneComp = visitee->GetComponent<ncScene>();
            if (sceneComp)
            {
                // HACK ma.garcias - check that the component has not been rendered this frame
                if (sceneComp->GetFrameId() != (uint) this->spatialServer->GetFrameId())
                {
                    sceneComp->SetTime(nTimeServer::Instance()->GetFrameTime());
                    sceneComp->SetFrameId(this->spatialServer->GetFrameId());
                    sceneComp->Render(this->sceneGraph);
                #ifndef __ZOMBIE_EXPORTER__
                    nFXServer::Instance()->RenderEffects(this->sceneGraph, visitee);
                #endif
                }

                // make its links visibles
                ncSpatialLight *light;
                for ( int i(0); i < sceneComp->GetNumLinks(); ++i )
                {
                    light = sceneComp->GetLinkAt(i)->GetComponentSafe<ncSpatialLight>();
                    if ( !light->IsVisibleBy(this->m_currentCamId) )
                    {
                        light->Accept(*this);
                    }
                }
            }
        }
        
        // the entity is visible
        spatialComp->SetVisibleBy(this->m_currentCamId);
    }
}

//------------------------------------------------------------------------------
/**
    process for global entities
*/
void 
nVisibleFrustumVisitor::VisitGlobalEntities(const nArray<nEntityObject*> &globalEntities)
{
    ncSpatial *spatialComp;
    nEntityObject *entity;
    VisitorFlags clipstatus(false, false);
    nCamera2 camera(this->m_spatialCamera->GetCamera());
    nFrustumClipper frustum(camera,
                            this->m_spatialCamera->GetViewMatrix());

    for ( int i(0); i < globalEntities.Size(); ++i )
    {
        entity = globalEntities[i];
        spatialComp = entity->GetComponent<ncSpatial>();
        if (!entity->GetComponent<ncSpatialCamera>() && spatialComp)
        {
            this->TestGlobalEntity(spatialComp, frustum, clipstatus);
        }
    }
}

//------------------------------------------------------------------------------
/**
    check if every entity in the cell is visible or not
*/
void 
nVisibleFrustumVisitor::TestEntities(ncSpatialCell *visitee, 
                                     const nFrustumClipper &frustum, 
                                     const VisitorFlags &clipstatus,
                                     nHorizonClipper &horizon)
{
    int cati = -1;
    // the cell is not completely visible, so make tests for the entities in it using the given clipstatus
    for (int i(0); i < this->m_visCategoriesMap->Size(); ++i)
    {
        cati = (*this->m_visCategoriesMap)[i];
        const nArray<nEntityObject*> &entitiesArray = visitee->GetCategory(cati);
        switch (cati)
        {
            case nSpatialTypes::CAT_GRAPHBATCHES:
                continue;
                
            case nSpatialTypes::CAT_PORTALS:
                for (int j(0); j < entitiesArray.Size(); ++j)
                {
                    this->TestPortalAndBrushes(entitiesArray[j]->GetComponentSafe<ncSpatialPortal>(), 
                                                frustum, clipstatus, horizon);
                }
                break;

            default:
                for (int j(0); j < entitiesArray.Size(); ++j)
                {
                    this->TestEntity(entitiesArray[j]->GetComponentSafe<ncSpatial>(), 
                                    frustum, clipstatus, horizon);
                }
                break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    check if the given entity is visible or not
*/
void 
nVisibleFrustumVisitor::TestEntity(ncSpatial *spatialComp,
                                   const nFrustumClipper &frustum, 
                                   const VisitorFlags &clipstatus,
                                   nHorizonClipper &horizon)
{
    n_assert2(spatialComp, 
              "miquelangel.rujula: trying to test visibility for an entity without spatial component!");
#ifndef NGAME
    ++this->numTestedEntities;
#endif // !NGAME

    #ifdef __NEBULA_STATS__
    this->profVisTestEntities.StartAccum();
    #endif

    // if the entity was already visible in this visibility query, for the current camera,
    // just continue testing the others
    if (spatialComp->IsVisibleBy(this->m_currentCamId))
    {
        #ifdef __NEBULA_STATS__
        this->profVisTestEntities.StopAccum();
        #endif
        return;
    }

    // reset scene links
    ncScene* sceneComp = spatialComp->GetComponent<ncScene>();

    if (!sceneComp)
    {
        #ifdef __NEBULA_STATS__
        this->profVisTestEntities.StopAccum();
        #endif
        return;
    }

    VisitorFlags vf(clipstatus);
    if (clipstatus.activeflags != 0 && !clipstatus.TestResult())
    {
        // make the frustum test for the entity
        frustum.TestBBox(spatialComp->GetBBoxCenter(), spatialComp->GetBBoxExtents(), clipstatus, vf);
    }

    if (!vf.AntiTestResult())
    {
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StartAccum();
        #endif
        if ( this->m_doHorizonCulling && !this->m_disableHorizon )
        {
            const bbox3 &box = spatialComp->GetBBox();
            if ((horizon.TestFast(box) == nHorizonClipper::BELOW) ||
                 this->IsOccluded(spatialComp->GetBBoxCenter(), spatialComp->GetBBoxExtents()))
            {
                #ifdef __NEBULA_STATS__
                this->profVisHorizonTest.StopAccum();
                this->profVisTestEntities.StopAccum();
                #endif
                return;
            }
        }
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StopAccum();
        #endif
        
        this->DetermineLOD(spatialComp);

        spatialComp->Accept(*this);

        sceneComp->SetFlag(ncScene::ShapeVisible, true);
    }

    #ifdef __NEBULA_STATS__
    this->profVisTestEntities.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    check if every batch in the cell is visible or not
*/
void 
nVisibleFrustumVisitor::TestBatches(ncSpatialCell *visitee, 
                                    const nFrustumClipper &frustum, 
                                    const VisitorFlags &clipstatus,
                                    nHorizonClipper &horizon)
{
    const nArray<nEntityObject*> &batches = visitee->GetCategory(nSpatialTypes::CAT_GRAPHBATCHES);
        
    for (int j(0); j < batches.Size(); ++j)
    {
        this->TestBatch(batches[j]->GetComponentSafe<ncSpatialBatch>(), 
                        frustum, clipstatus, horizon);
    }
}

//------------------------------------------------------------------------------
/**
    check if the given global entity is visible or not
*/
void 
nVisibleFrustumVisitor::TestGlobalEntity(ncSpatial *spatialComp,
    const nFrustumClipper &frustum, 
    const VisitorFlags &clipstatus)
{
    n_assert2(spatialComp, 
              "miquelangel.rujula: trying to test visibility for an entity without spatial component!");
#ifndef NGAME
    ++this->numTestedEntities;
#endif // !NGAME

    ncScene* sceneComp = spatialComp->GetComponent<ncScene>();
    if ( !sceneComp )
    {
        return;
    }

    #ifdef __NEBULA_STATS__
    this->profVisTestEntities.StartAccum();
    #endif

    // if the entity was already visible in this visibility query, for the current camera,
    // just continue testing the others
    if (spatialComp->IsVisibleBy(this->m_currentCamId))
    {
        #ifdef __NEBULA_STATS__
        this->profVisTestEntities.StopAccum();
        #endif
        return;
    }
    else
    {
        sceneComp->SetFlag(ncScene::ShapeVisible, false);
    }

    // reset scene links
    spatialComp->ClearLinks();
    sceneComp->SetAttachFlags(0);

    VisitorFlags vf(clipstatus);
    if (clipstatus.activeflags != 0 && !clipstatus.TestResult())
    {
        // make the frustum test for the entity
        frustum.TestBBox(spatialComp->GetBBoxCenter(), spatialComp->GetBBoxExtents(), clipstatus, vf);
    }

    if (!vf.AntiTestResult())
    {
        this->DetermineLOD(spatialComp);
        spatialComp->Accept(*this);
        sceneComp->SetFlag(ncScene::ShapeVisible, true);
    }

    #ifdef __NEBULA_STATS__
    this->profVisTestEntities.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    check if the batch is visible or not
*/
void 
nVisibleFrustumVisitor::TestBatch(ncSpatialBatch *spatialBatchComp,
                                  const nFrustumClipper &frustum, 
                                  const VisitorFlags &clipstatus,
                                  nHorizonClipper &horizon)
{

#ifndef NGAME
    ++this->numTestedBatches;
#endif // !NGAME

    #ifdef __NEBULA_STATS__
    this->profVisTestBatches.StartAccum();
    #endif

    n_assert2(spatialBatchComp, 
              "miquelangel.rujula: trying to test visibility for a batch entity without spatial component!");

    // if the batch was already visible in this visibility query, for the current camera,
    // just continue testing the other entities
    if (spatialBatchComp->IsVisibleBy(this->m_currentCamId))
    {
        #ifdef __NEBULA_STATS__
        this->profVisTestBatches.StopAccum();
        #endif
        return;
    }

    VisitorFlags vf(clipstatus);
    if (clipstatus.activeflags != 0 && !clipstatus.TestResult())
    {
        // make the frustum test for the entity
        frustum.TestBBox(spatialBatchComp->GetBBoxCenter(), spatialBatchComp->GetBBoxExtents(), clipstatus, vf);
    }

    if (!vf.AntiTestResult())
    {
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StartAccum();
        #endif
        if ( this->m_doHorizonCulling && !this->m_disableHorizon )
        {
            const bbox3 &box = spatialBatchComp->GetBBox();
            if ((horizon.TestFast(box) == nHorizonClipper::BELOW) ||
                this->IsOccluded(spatialBatchComp->GetBBoxCenter(), spatialBatchComp->GetBBoxExtents()))
            {
                // all the batch is occluded
                #ifdef __NEBULA_STATS__
                this->profVisHorizonTest.StopAccum();
                this->profVisTestBatches.StopAccum();
                #endif
                return;
            }
        }
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StopAccum();
        #endif

        // the batch is partially or fully visible
        const vector3 &campos = this->cameraTrComp->GetPosition();
        const vector3 &batchCenter = spatialBatchComp->GetBBox().center();

        // if subentities are not loaded, keep the batch visible
        float distanceFactor = this->m_spatialCamera->GetXScale() / (campos - batchCenter).lensquared();
        if (spatialBatchComp->GetSubentities().Empty() ||
            distanceFactor < spatialBatchComp->GetMaxDistanceFactor())
        {
            ncScene *sceneComp = spatialBatchComp->GetComponent<ncScene>();
            if (sceneComp)
            {
                // attach the batch instead of its subentities
                sceneComp->SetFlag(ncScene::ShapeVisible, true);
                spatialBatchComp->Accept(*this);
            }
            //else
            //{
            //    attach subentities???
            //}
        }
        else
        {
            // we're too close to the batch, so, test its subentities
            const nArray<nEntityObject*> &subentities = spatialBatchComp->GetSubentities();
            ncSpatialBatch *subBatch = 0;
            for (int i(0); i < subentities.Size(); ++i)
            {
                subBatch = subentities[i]->GetComponent<ncSpatialBatch>();
                if ( subBatch )
                {
                    // the subentity is also a batch
                    this->TestBatch(subBatch, 
                                    frustum, vf, horizon);
                }
                else
                {
                    // the subentity is not a batch
                    this->TestEntity(subentities[i]->GetComponentSafe<ncSpatial>(), 
                                     frustum, vf, horizon);
                }
            }
        }    
    }
    #ifdef __NEBULA_STATS__
    this->profVisTestBatches.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    check if the given portal and its associated brushes are visibles or not
*/
void 
nVisibleFrustumVisitor::TestPortalAndBrushes(ncSpatialPortal *portal,
                                             const nFrustumClipper &frustum, 
                                             const VisitorFlags &clipstatus,
                                             nHorizonClipper &horizon)
{
    n_assert2(portal, 
              "miquelangel.rujula: trying to test visibility for a portal without spatial component!");

    // first test visibility for the portal as a common entity
    this->TestEntity(portal, frustum, clipstatus, horizon);

    // test visibility for the brushes associated to the portal
    const nArray<nEntityObject*> *brushes = portal->GetBrushes();
    if ( brushes )
    {
        ncSpatial *spatialComp;
        for (int i(0); i < brushes->Size(); ++i)
        {
            spatialComp = (*brushes)[i]->GetComponentSafe<ncSpatial>();

            // if the entity was already visible in this visibility query, for the current camera,
            // just continue testing the others
            if (spatialComp->IsVisibleBy(this->m_currentCamId))
            {
                return;
            }

            const bbox3 &box = spatialComp->GetBBox();
            VisitorFlags vf(clipstatus);
            if (clipstatus.activeflags != 0 && !clipstatus.TestResult())
            {
                // make the frustum test for the entity
                frustum.TestBBox(spatialComp->GetBBoxCenter(), spatialComp->GetBBoxExtents(), clipstatus, vf);
            }

            // reset scene links
            ncScene* sceneComp = spatialComp->GetComponent<ncScene>();

            if (!vf.AntiTestResult())
            {
                #ifdef __NEBULA_STATS__
                this->profVisHorizonTest.StartAccum();
                #endif
                if ( this->m_doHorizonCulling && !this->m_disableHorizon )
                {
                    if ((horizon.TestFast(box) == nHorizonClipper::BELOW) ||
                        this->IsOccluded(spatialComp->GetBBoxCenter(), spatialComp->GetBBoxExtents()))
                    {
                        #ifdef __NEBULA_STATS__
                        this->profVisHorizonTest.StopAccum();
                        #endif
                        return;
                    }
                }
                #ifdef __NEBULA_STATS__
                this->profVisHorizonTest.StopAccum();
                #endif
                
                this->DetermineLOD(spatialComp);
                spatialComp->Accept(*this, portal->GetCell());

                if (sceneComp)
                {
                    sceneComp->SetFlag(ncScene::ShapeVisible, true);
                }
            }
            else
            {
                sceneComp->SetFlag(ncScene::ShapeVisible, false);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check if the given box is occluded by the selected occluders of the current
    camera.

    Returns true if its occluded, false otherwise.
*/
bool
nVisibleFrustumVisitor::IsOccluded(const bbox3 &box)
{
    //profVisOcclusion.StartAccum();
    
    const nArray<nFrustumClipper*> &ocFrustums = this->m_spatialCamera->GetOcclusionFrustums();
    int numFrustums = ocFrustums.Size();
    for (int i(0); i < numFrustums; ++i)
    {
        VisitorFlags of;
        VisitorFlags initOf(false, false);
        
        // test if the bounding box is occluded by the current occluder
        ocFrustums[i]->TestBBox(box, initOf, of);
        
        if (of.TestResult())
        {
            //profVisOcclusion.StopAccum();
            return true;
        }
    }
    
    //profVisOcclusion.StopAccum();

    return false;
}

//------------------------------------------------------------------------------
/**
    Check if the given box (center and extents) is occluded by the selected 
    occluders of the current camera.

    Returns true if its occluded, false otherwise.
*/
bool
nVisibleFrustumVisitor::IsOccluded(const vector3& bboxCenter, const vector3& bboxExtents)
{
    //profVisOcclusion.StartAccum();
    
    const nArray<nFrustumClipper*> &ocFrustums = this->m_spatialCamera->GetOcclusionFrustums();
    int numFrustums = ocFrustums.Size();
    for (int i = 0; i < numFrustums; ++i)
    {
        VisitorFlags of;
        VisitorFlags initOf(false, false);
        
        // test if the bounding box is occluded by the current occluder
        ocFrustums[i]->TestBBox(bboxCenter, bboxExtents, initOf, of);
        
        if (of.TestResult())
        {
            //profVisOcclusion.StopAccum();
            return true;
        }
    }
    
    //profVisOcclusion.StopAccum();

    return false;
}

//------------------------------------------------------------------------------
/**
    Check if the given portal is occluded by the selected occluders of the current
    camera.

    Returns true if its occluded, false otherwise.
*/
bool
nVisibleFrustumVisitor::IsOccluded(ncSpatialPortal *portal)
{
    const nArray<nFrustumClipper*> &ocFrustums = this->m_spatialCamera->GetOcclusionFrustums();
    const vector3 *portalVerts = portal->GetVertices();
    struct ClipVertexInfo vertsInfo[4];
    for (int i(0); i < ocFrustums.Size(); ++i)
    {
        VisitorFlags of;
        VisitorFlags initOf(false, false);
        
        // test if the portal is occluded by the current occluder
        of = ocFrustums[i]->TestPortalVertices(portalVerts, 4, initOf, vertsInfo);

        if (of.TestResult())
        {
            return true;
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    check if every portal in the cell is visible or not
*/
void 
nVisibleFrustumVisitor::TestPortals(const nArray<nEntityObject*> &portalsArray, 
                                    const nFrustumClipper &frustum, 
                                    const VisitorFlags &clipstatus,
                                    nHorizonClipper &horizon)
{
    NLOG(visibilityVisitor, (1, "%*s - Test Portals. Number of portals: %d.",
                             ++spatialIndent * 4, " ",
                             portalsArray.Size()));

    ncSpatialPortal* portal = 0;
    for (int i(0); i < portalsArray.Size(); ++i)
    {
        portal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();
        if (portal->IsActive())
        {
            this->CheckPortal(portal, 
                            frustum,
                            clipstatus,
                            horizon);
        }
        else
        {
            NLOG(visibilityVisitor, (1, "%*s - Portal %d, %x is not active.",
                            --spatialIndent * 4, " ",
                            portal->GetEntityObject()->GetId(),
                            portal->GetEntityObject()->GetId()));
        }
    }

    NLOG(visibilityVisitor, (1, "%*s - Finished Test Portals. Number of portals: %d.",
                            --spatialIndent * 4, " ",
                            portalsArray.Size()));
}

//------------------------------------------------------------------------------
/**
    check if the portal has to be traversed by the visibility algorithm 
    and also if the camera has traversed through it
*/
void 
nVisibleFrustumVisitor::CheckPortal(ncSpatialPortal* portal,
                                    const nFrustumClipper &frustum, 
                                    const VisitorFlags &clipstatus,
                                    nHorizonClipper &horizon)
{
#ifndef NGAME
    nEntityObjectId twinId = nEntityObjectServer::IDINVALID;
    if (portal->GetTwinPortal())
    {
        twinId = portal->GetTwinPortal()->GetEntityObject()->GetId();
    }
    
    NLOG(visibilityVisitor, (1, "%*s - Checking portal: %d, %x in cell type: %d. TWIN: %d", 
                                ++spatialIndent * 4, " ",
                                portal->GetEntityObject()->GetId(), 
                                portal->GetEntityObject()->GetId(),
                                portal->GetCell()->GetType(),
                                twinId));
#endif // !NGAME

    if (this->m_currRecursionDepth > MAX_VIS_RECURSION_DEPTH)
    {
        NLOG(visibilityVisitor, (1, "%*s - MAXIMUM RECURSION DEPTH REACHED! Aborting portals traversal.", 
                                ++spatialIndent * 4, " "));
        return;
    }

    // check if we are in front of the portal
    const plane &portalPlane = portal->GetPlane();
    const vector3 &campos = this->cameraTrComp->GetPosition();
    float dist = portalPlane.distance(campos);
    
    if (dist >= 0.0f)
    {
        // if the portal is farther than its deactivation distance to the camera, deactivate it
        vector3 distVect(portal->GetClipCenter() - campos);
        if ( distVect.lensquared() >= portal->GetSqDeactivationDistance() )
        {
            return;
        }

        const vector3 *portalVerts = portal->GetVertices();
        
        // test the portal vertices
        VisitorFlags vertsClip;
        struct ClipVertexInfo vertsInfo[4];
        if (this->m_traversedAnyPortal)
        {
            vertsClip = frustum.TestPortalVertices(portalVerts, 4, clipstatus, vertsInfo);
        }
        else
        {
            nFrustumClipper frustumTest = frustum;
            plane p = frustum.GetNearPlane();
            p.calculate_d(campos);
            frustumTest.SetNearPlane(p);
            vertsClip = frustumTest.TestPortalVertices(portalVerts, 4, clipstatus, vertsInfo);
        }

        if (vertsClip.AntiTestResult())
        {
            NLOG(visibilityVisitor, (1, "%*s - Leaving Check Portal of portal: %d, %x. CLIPPED",
                        --spatialIndent * 4, " ",
                        portal->GetEntityObject()->GetId(), 
                        portal->GetEntityObject()->GetId()));

            // portal is totally clipped
            return;
        }

        ////// @TODO: MAKE A HORIZON TEST JUST FOR THE PORTALS!!!
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StartAccum();
        #endif
        if ( this->m_doHorizonCulling && !this->m_disableHorizon )
        {
            if ((horizon.TestFast(portal->GetBBox()) == nHorizonClipper::BELOW) || 
                this->IsOccluded(portal))
            {
                // portal is totally clipped
                #ifdef __NEBULA_STATS__
                this->profVisHorizonTest.StopAccum();
                #endif
                return;
            }
        }
        #ifdef __NEBULA_STATS__
        this->profVisHorizonTest.StopAccum();
        #endif

        this->m_spatialCamera->AddVisiblePortal(portal);

#ifndef NGAME
        if ( this->spatialServer->IsInEditIndoorMode() || 
             ( this->spatialServer->GetSeeAllSelectedIndoors() && 
              !this->spatialServer->GetSelectedIndoors().Empty()
              )
           )
        {
            return;
        }
#endif // !NGAME

        // visit next cell
        if (portal->GetTwinPortal())
        {
            nFrustumClipper newFrustum;
            VisitorFlags newFlags(false, false);
            
            // recalculate the new frustum
            newFrustum = this->BuildNewFrustum(portal, 
                                               frustum, 
                                               vertsInfo, 
                                               vertsClip.TestResult());
            
            NLOG(visibilityVisitor, (1, "%*s - Traversing portal: %d, %x. Built new frustum! Is in cell: %d (OtherSideCell: %d type: %d, TwinPortalCell: %d type: %d.", 
                        spatialIndent * 4, " ",
                        portal->GetEntityObject()->GetId(), 
                        portal->GetEntityObject()->GetId(),
                        portal->GetCell(),
                        portal->GetOtherSideCell(),
                        portal->GetOtherSideCell()->GetType(),
                        portal->GetTwinPortal()->GetCell(),
                        portal->GetTwinPortal()->GetCell()->GetType()
                        ));
#ifndef NGAME
            if (portal->GetOtherSideCell() == portal->GetCell())
            {
                NLOG(visibilityVisitor, (1, "%*s - !!!! WARNING: Portal %d points to the same cell is in !!!!!",
                                            spatialIndent * 4, " ",
                                            portal->GetEntityObject()->GetId()));
            }
#endif // !NGAME
            ++this->m_currRecursionDepth;
            bool currTraversedFlag = this->m_traversedAnyPortal;
            this->m_traversedAnyPortal = true;
            // visit the next cell with the new frustum and flags
            this->Visit(portal->GetOtherSideCell(),
                        newFrustum,
                        newFlags,
                        horizon);
            this->m_traversedAnyPortal = currTraversedFlag;
            this->m_currRecursionDepth--;
        }
    }

    NLOG(visibilityVisitor, (1, "%*s - Leaving Check Portal of portal: %d, %x.",
                            --spatialIndent * 4, " ",
                            portal->GetEntityObject()->GetId(), 
                            portal->GetEntityObject()->GetId()));
}

//------------------------------------------------------------------------------
/**
    Builds a new frustum using the previous one and the vertices of the portal's 
    clip rectangle.
*/
nFrustumClipper 
nVisibleFrustumVisitor::BuildNewFrustum(ncSpatialPortal *portal, 
                                        const nFrustumClipper &oldFrustum,
                                        const struct ClipVertexInfo *vertsInfo,
                                        bool completelyInside)
{
    n_assert(portal);

    // if all the portal vertices are completely inside, there's no need to check for clipping,
    // thus, just build the new frustum using these vertices
    if (completelyInside)
    {
        NLOG(visibilityVisitor, (1, "%*s - Build frustum NO TEST",
            spatialIndent * 4, " "));
        return this->BuildNewFrustumNoTest(portal, oldFrustum);
    }
    else
    {
        NLOG(visibilityVisitor, (1, "%*s - Build frustum TEST",
            spatialIndent * 4, " "));
        return this->BuildNewFrustumTest(portal, oldFrustum, vertsInfo);
    }
}

//------------------------------------------------------------------------------
/**
    Builds a new frustum using the previous one and the vertices of the portal's 
    clip rectangle, without determining the intersection points between the portal and 
    the given frustum. This method is called when the portal rectangle (vertices)
    is completely inside the frustum.
*/
nFrustumClipper 
nVisibleFrustumVisitor::BuildNewFrustumNoTest(ncSpatialPortal *portal, 
                                              const nFrustumClipper &oldFrustum)
{
    n_assert(portal);

    const vector3 *vertices = portal->GetVertices();
    n_assert2(vertices, "miquelangel.rujula: NULL pointer to portal vertices!");

    const vector3 &camPos = this->cameraTrComp->GetPosition();
    plane newClipPlanes[8];
    int numNewClipPlanes = 0;
    
    // set the near plane
    float dist = 0;
    float minDist = FLT_MAX;
    int nearVertex = -1;

    for (int i(0); i < 4; ++i)
    {
        dist = (vertices[i] - camPos).lensquared();
        if (dist < minDist)
        {
            minDist = dist;
            nearVertex = i;
        }
    }

    n_assert( 0 <= nearVertex && nearVertex < 4 );
    n_assert(numNewClipPlanes == 0);
    plane newNear = oldFrustum.GetNearPlane();
    newNear.calculate_d(vertices[nearVertex]);
    newClipPlanes[numNewClipPlanes] = newNear;
    int nearPlaneIdx = numNewClipPlanes;
    ++numNewClipPlanes;
    
    // set the second near plane
    n_assert(numNewClipPlanes == 1);
    newClipPlanes[numNewClipPlanes] = portal->GetTwinPortal()->GetPlane();
    ++numNewClipPlanes;

    int i2;
    // initialize vertex index 1 to the first vertex (in the loop)
    for (int i1(0); i1 < 4; ++i1)
    {
        i2 = (i1+1) % 4;
        n_assert(numNewClipPlanes < 6);
        newClipPlanes[numNewClipPlanes].set(camPos, vertices[i1], vertices[i2]);
        ++numNewClipPlanes;
    }

    // set the far plane
    n_assert(numNewClipPlanes == 6);
    n_assert(oldFrustum.GetFarPlaneIdx() != -1);
    newClipPlanes[numNewClipPlanes] = oldFrustum.GetFarPlane();
    int farPlaneIdx = numNewClipPlanes;
    ++numNewClipPlanes;

    n_assert(numNewClipPlanes == 7);
    nFrustumClipper newFrustum(newClipPlanes, numNewClipPlanes);
    newFrustum.SetNearPlaneIdx(nearPlaneIdx);
    newFrustum.SetFarPlaneIdx(farPlaneIdx);

    return newFrustum;
}

//------------------------------------------------------------------------------
/**
    Builds a new frustum using the previous one and the vertices of the portal's 
    clip rectangle, determining the intersection points between the portal and 
    the given frustum. This method is called when the portal rectangle (vertices)
    is partially inside the frustum.
*/
nFrustumClipper 
nVisibleFrustumVisitor::BuildNewFrustumTest(ncSpatialPortal *portal, 
                                            const nFrustumClipper &oldFrustum,
                                            const struct ClipVertexInfo *vertsInfo)
{
    n_assert(portal);
    
    const vector3 *vertices = portal->GetVertices();
    n_assert2(vertices, "miquelangel.rujula: NULL pointer to portal vertices!");
    const plane *planes = oldFrustum.GetPlanes();
    n_assert2(planes, "miquelangel.rujula: frustum has no planes!");
    const vector3 &camPos = this->cameraTrComp->GetPosition();

    plane newClipPlanes[N_PLANECLIPPER_MAXPLANES];
    int numNewClipPlanes = 0;
    unsigned short usedPlanes = 0; // which planes from the original frustum are reused

    // set the near plane
    float dist = 0;
    float minDist = FLT_MAX;
    int nearVertex = -1;
    int nearPlaneIdx = oldFrustum.GetNearPlaneIdx();
    int nearMask = (1<<nearPlaneIdx);
    bool aNearClip = false;

    for (int i(0); i < 4; ++i)
    {
        aNearClip = (nearMask & vertsInfo[i].planesMask) != 0;
        if (aNearClip)
        {
            break;
        }

        dist = (vertices[i] - camPos).lensquared();
        if (dist < minDist)
        {
            minDist = dist;
            nearVertex = i;
        }
    }

    if (!aNearClip)
    {
        n_assert( 0 <= nearVertex && nearVertex < 4 );
        n_assert(numNewClipPlanes == 0);
        plane newNear = oldFrustum.GetNearPlane();
        newNear.calculate_d(vertices[nearVertex]);
        newClipPlanes[numNewClipPlanes] = newNear;
        nearPlaneIdx = numNewClipPlanes;
        ++numNewClipPlanes;
    }

    // set the second near plane
    newClipPlanes[numNewClipPlanes] = portal->GetTwinPortal()->GetPlane();
    ++numNewClipPlanes;

    // we have the vertices classified (clipped or not) to build up the new frustum. Let's do it
    int i2;
    // initialize vertex index 1 to the first vertex (in the loop)
    for (int i1(0); i1 < 4; ++i1)
    {
        if (numNewClipPlanes >= N_PLANECLIPPER_MAXPLANES)
        {
            return oldFrustum;
        }

        i2 = (i1+1) % 4;

        if (vertsInfo[i1].clipped)
        {
            if (!vertsInfo[i2].clipped)
            {
                // v1 clipped and v2 not clipped
                newClipPlanes[numNewClipPlanes].set(camPos, vertices[i1], vertices[i2]);
                ++numNewClipPlanes;

                // add the planes that clipped 'vertices[i1]' to the new frustum, 
                // if they aren't used yet
                unsigned short planeMask;
                for (int i(0); i < vertsInfo[i1].numPlanes; ++i)
                {
                    planeMask = static_cast<unsigned short>(1<<vertsInfo[i1].planes[i]);
                    if ( (usedPlanes & planeMask) == 0 )
                    {
                        // the plane is not used yet, let's do it
                        newClipPlanes[numNewClipPlanes] = planes[vertsInfo[i1].planes[i]];
                        ++numNewClipPlanes;

                        // mark the plane like used
                        usedPlanes |= planeMask;
                    }
                }
            }
            else
            {
                // v1 and v2 clipped
                unsigned short planeMask = vertsInfo[i1].planesMask & vertsInfo[i2].planesMask;
                if ( planeMask != 0 )
                {
                    // both vertices are clipped by common planes
                    if ( (usedPlanes & planeMask) == 0 )
                    {
                        // the planes are stil not used, let's do it
                        int bit = 1;
                        for (int i(0); i < oldFrustum.GetNumPlanes(); ++i, bit <<= 1)
                        {
                            if ((bit & planeMask) != 0)
                            {
                                newClipPlanes[numNewClipPlanes] = planes[i];
                                ++numNewClipPlanes;
                            }
                        }

                        // mark the planes like used
                        usedPlanes |= planeMask;
                    }
                }
                else
                {
                    // v1 and v2 are clipped by different planes (they haven't a common clipping plane)
                    newClipPlanes[numNewClipPlanes].set(camPos, vertices[i1], vertices[i2]);
                    ++numNewClipPlanes;
                }
            }
        }
        else
        {
            if (!vertsInfo[i2].clipped)
            {
                // v1 and v2 not clipped
                newClipPlanes[numNewClipPlanes].set(camPos, vertices[i1], vertices[i2]);
                ++numNewClipPlanes;
            }
            else
            {
                // v1 not clipped and v2 clipped
                newClipPlanes[numNewClipPlanes].set(camPos, vertices[i1], vertices[i2]);
                ++numNewClipPlanes;

                // add the planes that clipped 'vertices[i2]' to the new frustum, 
                // if they aren't used yet
                unsigned short planeMask;
                for (int i(0); i < vertsInfo[i2].numPlanes; ++i)
                {
                    planeMask = static_cast<unsigned short>(1<<vertsInfo[i2].planes[i]);
                    if ( (usedPlanes & planeMask) == 0 )
                    {
                        // the plane is not used yet, let's do it
                        newClipPlanes[numNewClipPlanes] = planes[vertsInfo[i2].planes[i]];
                        ++numNewClipPlanes;

                        // mark the plane like used
                        usedPlanes |= planeMask;
                    }
                }
            }
        }
    }

    if (numNewClipPlanes >= N_PLANECLIPPER_MAXPLANES)
    {
        return oldFrustum;
    }

    // set the far plane
    n_assert(oldFrustum.GetFarPlaneIdx() != -1);
    newClipPlanes[numNewClipPlanes] = oldFrustum.GetFarPlane();
    int farPlaneIdx = numNewClipPlanes;
    ++numNewClipPlanes;

    if (numNewClipPlanes >= N_PLANECLIPPER_MAXPLANES)
    {
        return oldFrustum;
    }
    nFrustumClipper newFrustum(newClipPlanes, numNewClipPlanes);
    newFrustum.SetNearPlaneIdx(nearPlaneIdx);
    newFrustum.SetFarPlaneIdx(farPlaneIdx);

    return newFrustum;
}

//------------------------------------------------------------------------------
/**
    test if the shadows in the given cell are visibles or not
*/
void
nVisibleFrustumVisitor::TestShadows(ncSpatialCell *visitee, const nFrustumClipper &frustum, 
                                    const VisitorFlags &clipstatus)
{
    nArray<nSpatialShadowModel*> *shadows = visitee->GetShadowModels();
    if ( !shadows )
    {
        return;
    }

    nSpatialShadowModel *shadow;
    ncSpatial *caster;
    ncScene *sceneComp;
    for ( int i(0); i < shadows->Size(); ++i )
    {
        shadow = (*shadows)[i];
        caster = shadow->GetCaster();
        n_assert(caster);
        sceneComp = caster->GetComponentSafe<ncScene>();
        if ( !frustum.TestCylinderQuick(shadow->GetCylinder(), clipstatus).AntiTestResult() )
        {
            caster->SetShadowVisible(shadow->GetShadowId());
            sceneComp->SetFlag(ncScene::ShadowVisible, true);
            if ( !caster->IsVisibleBy(this->m_currentCamId) )
            {
                caster->Accept(*this);
            }
        }
        else
        {
            caster->SetShadowNonVisible(shadow->GetShadowId());
            if ( caster->NoVisibleShadows() )
            {
                sceneComp->SetFlag(ncScene::ShadowVisible, false);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nVisibleFrustumVisitor::DetermineLOD(ncSpatial *spatialComponent)
{
    n_assert2(spatialComponent, "miquelangel.rujula");

    ncSpatialClass *spatialCompClass = spatialComponent->GetEntityClass()->GetComponent<ncSpatialClass>();
    n_assert2(spatialCompClass, "miquelangel.rujula: spatial component has no component class!");

    ncScene* sceneComp = spatialComponent->GetComponent<ncScene>();
    if (!sceneComp)
    {
        return;
    }

    if (spatialCompClass->ranges.Size() == 0)
    {
        return;
    }

    if (spatialCompClass->GetLevelLocked() != -1)
    {
        spatialComponent->SetCurrentLod(spatialCompClass->GetLevelLocked());
        sceneComp->SetAttachIndex(spatialCompClass->GetLevelLocked());
        return;
    }

    #if __NEBULA_STATS__
    this->profDetermineLOD.StartAccum();
    #endif

    // get current lod
    int currentLod = spatialComponent->GetCurrentLod();

    // get the entity position
    const vector3 &entityPos = spatialComponent->GetBBoxCenter();

    // get the camera transform
    const matrix44 &cameraTransform = this->m_spatialCamera->GetViewMatrix();

    // get the entity distance from the camera
    const vector3& cameraPos = cameraTransform.pos_component();
    float distanceSq = vector3(cameraPos - entityPos).lensquared();

    // project camera-space position to account for field of view:
    float factor = this->m_spatialCamera->GetXScale() / distanceSq;

    // determine level of detail
    if (currentLod >= 0 && currentLod < spatialCompClass->ranges.Size())
    {
        // the entity is in a predefined range
        // first, check if the entity has left its current LOD range or not
        ncSpatialClass::LODRangeType &currentRange = spatialCompClass->ranges[currentLod];

        if (factor > currentRange.GetLowerFactor() && factor < currentRange.GetUpperFactor())
        {   
            // the entity hasn't left its current LOD, keep it and return
            #ifdef __NEBULA_STATS__
            this->profDetermineLOD.StopAccum();
            #endif
            return;
        }
    }

    // the entity has changed its current LOD, determine the new LOD (range)
    for (int i(0); i < spatialCompClass->ranges.Size(); ++i)
    {
        if (i != currentLod)
        {
            ncSpatialClass::LODRangeType& range = spatialCompClass->ranges[i];

            if (factor > range.GetLowerFactor() && factor < range.GetUpperFactor())
            {
                spatialComponent->SetCurrentLod(i);
                sceneComp->SetAttachIndex(i);
                #ifdef __NEBULA_STATS__
                this->profDetermineLOD.StopAccum();
                #endif
                return;
            }
        }
    }

    // if no range contains the entity find if it is behind the last ...
    if (spatialCompClass->ranges.Back().GetLowerFactor() > factor)
    {
        spatialComponent->SetCurrentLod(ncSpatialClass::N_FAR_LOD);
        sceneComp->SetAttachIndex(ncSpatialClass::N_FAR_LOD);
        profDetermineLOD.StopAccum();
        return;
    }

    // ... or in front of the first one
    if (spatialCompClass->ranges.Front().GetUpperFactor() < factor)
    {
        spatialComponent->SetCurrentLod(ncSpatialClass::N_NEAR_LOD);
        sceneComp->SetAttachIndex(ncSpatialClass::N_NEAR_LOD);
        profDetermineLOD.StopAccum();
        return;
    }
}

//------------------------------------------------------------------------------
/**
    calculate the links for all the entities and the terrain of a cell 
    given a light region
*/
void 
nVisibleFrustumVisitor::CalculateLightLinks(ncSpatialCell *cell, const nLightRegion *region)
{
    n_assert(cell);
    n_assert(region);

    ncSpatialLight *light = region->GetParentLight();
    n_assert(light);

    if ( cell->GetType() == ncSpatialCell::N_QUADTREE_CELL && 
        !light->GetComponent<ncSpatialLightEnv>())
    {
        nEntityObject *cellEnt = cell->GetEntityObject();
        if ( cellEnt )
        {
            ncScene *cellSceneComp = cellEnt->GetComponentSafe<ncScene>();
            NLOG(visibilityVisitor, (0, "AddLink(%s, %s)", cellEnt->GetEntityClass()->GetName(), light->GetEntityClass()->GetName()))
            if (cellSceneComp->AddLink(light->GetEntityObject()))
            {
                light->AddLinkedEntity(cellEnt);
            }
        }
    }

    int cati = -1;
    for (int i(0); i < this->m_visCategoriesMap->Size(); ++i)
    {
        cati = (*this->m_visCategoriesMap)[i];
        const nArray<nEntityObject*> &entitiesArray = cell->GetCategory(cati);
        switch (cati)
        {
            case nSpatialTypes::CAT_GRAPHBATCHES:
                for (int j(0); j < entitiesArray.Size(); ++j)
                {
                    this->CalculateLightLinks(entitiesArray[j]->GetComponentSafe<ncSpatialBatch>(), 
                        region);
                }
                break;

            case nSpatialTypes::CAT_PORTALS:
                for (int j(0); j < entitiesArray.Size(); ++j)
                {
                    this->CalculateLightLinks(entitiesArray[j]->GetComponentSafe<ncSpatialPortal>(), 
                        region);
                }
                break;

            default:
                for (int j(0); j < entitiesArray.Size(); ++j)
                {
                    this->CalculateLightLinks(entitiesArray[j]->GetComponentSafe<ncSpatial>(), 
                        region);
                }
                break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    calculate the links for the global entities given a light region
*/
void 
nVisibleFrustumVisitor::CalculateLightLinksGlobals(const nArray<nEntityObject*> &globalEntities, 
    const nLightRegion *region)
{
    n_assert(region);

    ncSpatial *spatialComp;
    for ( int i(0); i < globalEntities.Size(); ++i )
    {
        spatialComp = globalEntities[i]->GetComponent<ncSpatial>();
        if ( spatialComp )
        {
            this->CalculateLightLinks(spatialComp, region);
        }
    }
}

//------------------------------------------------------------------------------
/**
    calculate the links for a common entity given a light region
*/
void 
nVisibleFrustumVisitor::CalculateLightLinks(ncSpatial *spatialComp, const nLightRegion *region)
{
    n_assert(spatialComp);
    n_assert(region);

    nFrustumClipper *clipper;
    ncScene *sceneComp;
    ncSpatialLight *light;
    VisitorFlags inFlags(false, false);
    VisitorFlags vf(false, false);

    clipper = region->GetClipper();
    light = region->GetParentLight();
    n_assert(light);

    sceneComp = spatialComp->GetComponent<ncScene>();
    if ( !sceneComp )
    {
        return;
    }

    bool linked(false);

    if ( clipper )
    {
        clipper->TestBBox(spatialComp->GetBBoxCenter(), spatialComp->GetBBoxExtents(), inFlags, vf);
        if ( !vf.AntiTestResult() )
        {
            // the entity is touched by the light, so link it
            NLOG(visibilityVisitor, (0, "AddLink(%s, %s)", sceneComp->GetEntityClass()->GetName(), light->GetEntityClass()->GetName()))
            if ( sceneComp->AddLink(light->GetEntityObject()) )
            {
                light->AddLinkedEntity(spatialComp->GetEntityObject());
                linked = true;
            }
        }
    }
    else
    {
        nSpatialModel *model = light->GetTestModel();
        if ( !model || model->Intersects(spatialComp->GetBBox()) )
        {
            // the entity is touched by the light, so link it
            NLOG(visibilityVisitor, (0, "AddLink(%s, %s)", sceneComp->GetEntityClass()->GetName(), light->GetEntityClass()->GetName()))
            if ( sceneComp->AddLink(light->GetEntityObject()) )
            {
                light->AddLinkedEntity(spatialComp->GetEntityObject());
                linked = true;
            }
        }
    }

    if (linked)
    {
        if ( spatialComp->GetIsShadowCaster() && light->GetCastShadows() )
        {
            nSpatialShadowModel *shadowModel = spatialComp->UpdateShadowModel(light);
            n_assert(spatialComp->GetCell());
            // array of traversed portals
            static nArray<ncSpatialPortal*> traversedPortals(4, 2);
            n_assert(traversedPortals.Empty());
            this->InsertShadow(spatialComp->GetCell(), 0, shadowModel, traversedPortals);
        }
    }
}

//------------------------------------------------------------------------------
/**
    calculate the links for a batch entity given a light region
*/
void 
nVisibleFrustumVisitor::CalculateLightLinks(ncSpatialBatch *batchComp, const nLightRegion *region)
{
    n_assert(batchComp);
    n_assert(region);

    // calculate links for the batch itself
    this->CalculateLightLinks(static_cast<ncSpatial*>(batchComp), region);

    // calculate links for the subentities
    const nArray<nEntityObject*> &subentities = batchComp->GetSubentities();
    ncSpatialBatch *subBatch = 0;
    for (int i(0); i < subentities.Size(); ++i)
    {
        subBatch = subentities[i]->GetComponent<ncSpatialBatch>();
        if ( subBatch )
        {
            // the subentity is also a batch
            this->CalculateLightLinks(subBatch, region);
        }
        else
        {
            // the subentity is not a batch
            this->CalculateLightLinks(subentities[i]->GetComponentSafe<ncSpatial>(), region);
        }
    }
}

//------------------------------------------------------------------------------
/**
    calculate the links for a portal given a light region
*/
void 
nVisibleFrustumVisitor::CalculateLightLinks(ncSpatialPortal *portal, const nLightRegion *region)
{
    // calculate links for the portal itself
    this->CalculateLightLinks(static_cast<ncSpatial*>(portal), region);

    // test visibility for the brushes associated to the portal
    const nArray<nEntityObject*> *brushes = portal->GetBrushes();
    if ( brushes )
    {
        ncSpatial *spatialComp;
        for (int i(0); i < brushes->Size(); ++i)
        {
            spatialComp = (*brushes)[i]->GetComponentSafe<ncSpatial>();
            this->CalculateLightLinks(spatialComp, region);
        }
    }
}

//------------------------------------------------------------------------------
/**
    calculate the light links for the given spatial component
*/
void 
nVisibleFrustumVisitor::CalculateLightLinks(ncSpatial *spatialComp)
{
    n_assert(spatialComp);

    ncSpatialCell *cell = spatialComp->GetCell();
    if ( !cell )
    {
        return;
    }

    // get the cell's light regions
    nArray<nLightRegion*> &regions = cell->GetLightRegions();
    ncScene *sceneComp = spatialComp->GetComponent<ncScene>();
    if ( regions.Empty() || !sceneComp )
    {
        return;
    }

    // clear scene links
    spatialComp->ClearLinks();

    // clear shadow visible flag
    sceneComp->SetFlag(ncScene::ShadowVisible, false);

    // destroy all its shadows
    spatialComp->RemoveShadowModels();
    
    // link the lights to the entity
    for ( int i(0); i < regions.Size(); ++i )
    {
        this->CalculateLightLinks(spatialComp, regions[i]);
    }
}

//------------------------------------------------------------------------------
/**
    calculate the light links for the given spatial batch component
*/
void 
nVisibleFrustumVisitor::CalculateLightLinks(ncSpatialBatch *spatialBatchComp)
{
    n_assert(spatialBatchComp);

    ncSpatialCell *cell = spatialBatchComp->GetCell();
    if ( !cell )
    {
        return;
    }

    // get the cell's light regions
    nArray<nLightRegion*> &regions = cell->GetLightRegions();
    ncScene *sceneComp = spatialBatchComp->GetComponent<ncScene>();
    if ( regions.Empty() || !sceneComp )
    {
        return;
    }

    // clear scene links
    spatialBatchComp->ClearLinks();

    // link the lights to the entity
    for ( int i(0); i < regions.Size(); ++i )
    {
        this->CalculateLightLinks(spatialBatchComp, regions[i]);
    }
}

//------------------------------------------------------------------------------
/**
    inserts the shadow model in the given cell and checks if it intersects
    with any portal and recurses to the cell on the other side
*/
void
nVisibleFrustumVisitor::InsertShadow(ncSpatialCell *cell, ncSpatialCell *prevCell, 
    nSpatialShadowModel *shadowModel, nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(cell);
    n_assert(shadowModel);

    if ( cell->GetType() == ncSpatialCell::N_INDOOR_CELL )
    {
        // add the shadow to the cell
        cell->AddShadowModel(shadowModel);
    }
    else
    {
        if ( cell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
        {
            n_assert(shadowModel->GetCaster());
            shadowModel->GetCaster()->RemoveShadowModel(shadowModel);
            return;
        }

        bbox3 cylBox;
        cylBox.begin_extend();
        cylBox.extend(shadowModel->GetCylinder());
        ncSpatialCell *contCell = static_cast<ncSpatialQuadtree*>(cell->GetParentSpace())->SearchCellContaining2D(cylBox);
        if ( contCell )
        {
            contCell->AddShadowModel(shadowModel);
        }
        else
        {
            n_assert(shadowModel->GetCaster());
            shadowModel->GetCaster()->RemoveShadowModel(shadowModel);
            return;
        }
    }

    // check portal intersections
    ncSpatialPortal *portal;
    const nArray<nEntityObject*> &portalsArray = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
    for ( int i(0); i < portalsArray.Size(); ++i )
    {
        portal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();

        // check if we already have passed through this portal
        if ( traversedPortals.FindIndex(portal) != -1 )
        {
            continue;
        }
        else
        {
            traversedPortals.Append(portal);
        }

        n_assert(portal->GetOtherSideCell());

        if ( (portal->GetOtherSideCell() != prevCell) && shadowModel->TouchesPortal(portal) )
        {
            this->InsertShadow(portal->GetOtherSideCell(), cell, shadowModel, traversedPortals);
        }

        traversedPortals.EraseQuick(traversedPortals.Size() - 1);
    }
}
