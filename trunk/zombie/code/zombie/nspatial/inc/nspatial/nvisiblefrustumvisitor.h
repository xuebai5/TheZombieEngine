#ifndef N_VISIBLEFRUSTUMVISITOR_H
#define N_VISIBLEFRUSTUMVISITOR_H
//------------------------------------------------------------------------------
/**
    @class nVisibleFrustumVisitor
    @ingroup NebulaSpatialSystem
    @brief Visits all the entities visible in a view frustum, useful for
    rendering to a camera.

    This visitor finds all the entities possibly visible from a given
    viewpoint within a view frustum.  This is useful for determining
    objects that appear on screen with a given projective camera.

    The visitor will call Visit() for every entitiy determined possibly
    visible.
*/

#include "nspatial/nfrustumclipper.h"
#include "entity/nentitytypes.h"
#include "octree/noctree.h"
#ifndef NGAME
#include "misc/nwatched.h"
#endif // !NGAME

#define MAX_VIS_RECURSION_DEPTH 16

//------------------------------------------------------------------------------
class rectangle;
class nSpatialServer;
class ncSpatial;
class ncSpatialSpace;
class ncSpatialCell;
class ncSpatialOctree;
class ncSpatialQuadtreeCell;
class nSpatialIndoorCell;
class nSpatialOctreeCell;
class nSpatialQuadNode;
class ncSpatialCamera;
class ncSpatialLight;
class ncSpatialIndoor;
class ncSpatialQuadtree;
class ncSpatialPortal;
class ncSpatialBatch;
class nSpatialShadowModel;
class nHorizonClipper;
class nSceneGraph;
class ncTransform;

//------------------------------------------------------------------------------
class nVisibleFrustumVisitor 
{
    friend class nSpatialVisitor;

public:
    /// constructor 1
    nVisibleFrustumVisitor();
    /// destructor
    ~nVisibleFrustumVisitor();

    /// set pointer to spatial server
    void SetSpatialServer(nSpatialServer* spatialServer);
    /// set scene graph
    void SetSceneGraph(nSceneGraph *sceneGraph);
    /// set the visibility categories
    void SetVisCategoriesMap();
    /// visibility queries will be determined for this camera
    void SetCamera(ncSpatialCamera *camera);
    /// get the current camera we're determining visibility for
    const ncSpatialCamera* GetCamera() const;
    /// get the current camera's entity id
    nEntityObjectId GetCurrentCamId() const;
        
    /// get the array of visible lights
    nArray<nEntityObject*> *GetVisibleLights();
    
    void Visit(ncSpatialCell *visitee);
    void Visit(ncSpatialCell *visitee, 
                       const nFrustumClipper &frustum, 
                       const VisitorFlags &clipstatus,
                       nHorizonClipper &horizon);

    void Visit(nEntityObject *visitee);

    void Visit(ncSpatialLight *visitee, nSpatialIndoorCell* cell);
    void LinkOmniSpaceLight(ncSpatialLight *visitee, ncSpatialIndoor* indoor);
    void LinkQuadtreeLights(ncSpatialLight *visitee);
    void Visit(ncSpatialCamera *visitee);

    /// process for octree spaces and octree cells
    void Visit(ncSpatialOctree *visitee);

    /// process for quadtree spaces and quadtree cells
    void Visit(ncSpatialQuadtree *visitee,
               const nFrustumClipper &frustum,
               const VisitorFlags &clipstatus,
               nHorizonClipper &horizon);

    void Visit(ncSpatialQuadtreeCell  *visitee,
               const nFrustumClipper &frustum,
               const VisitorFlags &clipstatus,
               const VisitorFlags &terrainFlags,
               nHorizonClipper &horizon,
               bool fatherTerrainOccluded);

    /// process for indoor cells
    void Visit(nSpatialIndoorCell *visitee, 
               const nFrustumClipper &frustum, 
               const VisitorFlags &clipstatus,
               nHorizonClipper &horizon);

    /// process for global entities
    void VisitGlobalEntities(const nArray<nEntityObject*> &globalEntities);

    /// determine an entity's LOD
    void DetermineLOD(ncSpatial *spatialComponent);

    /// add a terrain cell to the visible array
    void AddVisibleTerrainCell(ncSpatialQuadtreeCell* cell);
    /// remove a terrain cell from the visible array
    void RemoveVisibleTerrainCell(ncSpatialQuadtreeCell* cell);
    /// reset the visible terrain cells array
    void ResetVisibleTerrainCells();

    /// calculate the links for all the entities and the terrain of a cell given a light region
    void CalculateLightLinks(ncSpatialCell *cell, const nLightRegion *region);
    /// calculate the links for the global entities given a light region
    void CalculateLightLinksGlobals(const nArray<nEntityObject*> &globalEntities, const nLightRegion *region);

    /// Calculate the links for a common entity given a light region
    void CalculateLightLinks(ncSpatial *spatialComp, const nLightRegion *region);
    /// Calculate the light links for the given spatial component
    void CalculateLightLinks(ncSpatial *spatialComp);
    /// Calculate the light links for the given spatial batch component
    void CalculateLightLinks(ncSpatialBatch *spatialBatchComp);

    /// says if the outdoor is visible
    bool IsOutdoorVisible() const;

    /// enable or disable horizon culling
    void SetDoHorizonCulling(bool flag);
    /// get if it's doing horizon culling or not
    bool IsDoingHorizonCulling() const;

#ifndef NGAME
    /// enable or disable quadtree cells painting
    void SetTerrainVisible(bool flag);
    bool IsTerrainVisible() const;

    /// enable or disable lights
    void SetLightsDisabled(bool flag);
    bool AreLightsDisabled() const;
#endif // !NGAME

protected:

    /// spatial server
    nSpatialServer* spatialServer;

    /// scene graph
    nSceneGraph* sceneGraph;

    /// camera we determine visibility for
    ncSpatialCamera* m_spatialCamera;

    /// camera's transform component
    ncTransform* cameraTrComp;

    /// camera id
    nEntityObjectId m_currentCamId;

    /// map of categories that we have to determine visibility for
    nArray<int>* m_visCategoriesMap;

    /// current recursion detph
    int m_currRecursionDepth;
    
    /// visible terrain cells
    nArray<ncSpatialQuadtreeCell*> m_visibleTerrainCells;

    /// occlusion flags to make a query to the spatial server
    int m_occlusionFlags;

#ifndef NGAME
    /// enable or disable quadtree cells painting
    bool m_disableQuadtree;
    /// enable or disable lights
    bool disableLights;
#endif // !NGAME

    /// doing horizon culling or not
    bool m_doHorizonCulling;

    /// Disable horizon culling, because the camera is looking at the ground and the horizon 
    /// culling it's not necessary
    bool m_disableHorizon;

    /// select the best occluders
    void SelectOccluders();

    /// True if the algorithm has traversed any portal
    bool m_traversedAnyPortal;

    /// Says if any outdoor cell is visible
    bool outdoorVisible;

    /// Inserts the shadow model in the given cell and checks if it intersects
    /// with any portal and recurses to the cell on the other side
    void InsertShadow(ncSpatialCell *cell, ncSpatialCell *prevCell, 
        nSpatialShadowModel *shadowModel,
        nArray<ncSpatialPortal*> &traversedPortals);

    /// Initialize the quadtree cell's visibility information
    void InitQuadtreeCellVis(ncSpatialQuadtreeCell *cell);

    /// Sort the child cells of a quadtree cell for the horizon clipper algorithm
    ncSpatialQuadtreeCell **SortChilds(ncSpatialQuadtreeCell **childs);

    /// Check if the given quadtree cell is visible or not
    bool IsQuadtreeCellVisible(ncSpatialQuadtreeCell *cell, 
        const nFrustumClipper &frustum,
        const VisitorFlags &clipstatus,
        VisitorFlags &newCellFlags,
        nHorizonClipper &horizon);
    
    /// Check if the terrain of the given quadtree cell is visible or not
    bool IsTerrainCellVisible(ncSpatialQuadtreeCell *cell,
        const nFrustumClipper &frustum,
        bool terrainOccluded,
        const VisitorFlags &terrainFlags,
        const VisitorFlags &clipstatus,
        VisitorFlags &newTerrainFlags, 
        nHorizonClipper &horizon);

    /// Process quadtree cell's subcells
    void ProcessSubcells(ncSpatialQuadtreeCell *cell,
        const nFrustumClipper &frustum,
        bool terrainOccluded,
        const VisitorFlags &newCellFlags,
        const VisitorFlags &newTerrainFlags,
        nHorizonClipper &horizon);

    /// Update the given horizon using the cell's horizon segments
    void UpdateHorizon(const ncSpatialQuadtreeCell *cell, nHorizonClipper &horizon);

    /// Recursive descent of the octree embedded inside an octree space
    void CheckOctNode(nOctNode *testnode, 
                      nFrustumClipper &clipper, 
                      VisitorFlags clipstatus);

    /// Array that contains all the lights that were determined visibles
    nArray<nEntityObject*> m_visibleLights;

    /// test if the shadows in the given cell are visibles or not
    void TestShadows(ncSpatialCell *visitee, const nFrustumClipper &frustum, 
                     const VisitorFlags &clipstatus);

    /// Check if the given entity is visible or not
    void TestEntity(ncSpatial *spatialComp,
                    const nFrustumClipper &frustum, 
                    const VisitorFlags &clipstatus,
                    nHorizonClipper &horizon);
        
    /// check if the given global entity is visible or not
    void TestGlobalEntity(ncSpatial *spatialComp, const nFrustumClipper &frustum, 
        const VisitorFlags &clipstatus);

    /// Check if the batch is visible or not
    void TestBatch(ncSpatialBatch *spatialBatchComp,
                   const nFrustumClipper &frustum, 
                   const VisitorFlags &clipstatus,
                   nHorizonClipper &horizon);

    /// Check if the given portal and its associated brushes are visibles or not
    void TestPortalAndBrushes(ncSpatialPortal *portal,
                              const nFrustumClipper &frustum, 
                              const VisitorFlags &clipstatus,
                              nHorizonClipper &horizon);

    /// Check if the given box is occluded by the selected occluders of the current camera
    bool IsOccluded(const bbox3 &box);

    /// Check if the given box (center and extents) is occluded by the selected occluders of 
    /// the current camera
    bool IsOccluded(const vector3 &bboxCenter, const vector3 &bboxExtents);

    /// Check if the given portal is occluded by the selected occluders of the current camera
    bool IsOccluded(ncSpatialPortal *portal);

    /// check if every entity in the cell is visible or not
    void TestEntities(ncSpatialCell *visitee, 
                      const nFrustumClipper &frustum, 
                      const VisitorFlags &clipstatus,
                      nHorizonClipper &horizon);

    /// check if every batch in the cell is visible or not
    void TestBatches(ncSpatialCell *visitee, 
                     const nFrustumClipper &frustum, 
                     const VisitorFlags &clipstatus,
                     nHorizonClipper &horizon);

    /// Traverse every portal in the quadtree portals list using its own frustum and flags information
    void TraversePortals();

    /// Check if every portal in the cell is visible or not
    void TestPortals(const nArray<nEntityObject*> &portalsArray, 
                     const nFrustumClipper &frustum, 
                     const VisitorFlags &clipstatus,
                     nHorizonClipper &horizon);

    void CheckPortal(ncSpatialPortal* portal,
                     const nFrustumClipper &frustum, 
                     const VisitorFlags &clipstatus,
                     nHorizonClipper &horizon);

    /// Builds a new frustum using the previous one and the vertices of the portal's clip rectangle.
    nFrustumClipper BuildNewFrustum(ncSpatialPortal *portal, 
                                    const nFrustumClipper &oldFrustum,
                                    const struct ClipVertexInfo *vertsInfo,
                                    bool completelyInside);

    /// Builds a new frustum using the previous one and the vertices of the portal's clip rectangle,
    /// without determining the intersection points between the portal and the frustum
    nFrustumClipper BuildNewFrustumNoTest(ncSpatialPortal *portal, 
                                          const nFrustumClipper &oldFrustum);

    /// Builds a new frustum using the previous one and the vertices of the portal's clip rectangle,
    /// determining the intersection points between the portal and the frustum
    nFrustumClipper BuildNewFrustumTest(ncSpatialPortal *portal, 
                                        const nFrustumClipper &oldFrustum,
                                        const struct ClipVertexInfo *vertsInfo);

    /// Calculate the links for a batch entity given a light region
    void CalculateLightLinks(ncSpatialBatch *batchComp, const nLightRegion *region);
    /// Calculate the links for a portal given a light region
    void CalculateLightLinks(ncSpatialPortal *portal, const nLightRegion *region);
    

#ifndef NGAME
    void VisitCellAndNeighbors(nSpatialIndoorCell *visitee);
    /// Make visibles all the entities in the selected indoors
    void VisitAllSelectedIndoors();
    /// Make visibles all the entities in the given indoor
    void MakeVisibleAllEntities(ncSpatialIndoor *indoor);
#endif // !NGAME

    /// Occlusion time consumption
    nProfiler profVisOcclusion;
    /// Occlusion, build occlusion frustum time consumption
    nProfiler profVisOcclusionFrus;
    /// Horizon culling, test
    nProfiler profVisHorizonTest;
    /// Horizon culling, insert
    nProfiler profVisHorizonInsert;
    /// Test entities profiler
    nProfiler profVisTestEntities;
    /// Test batches profiler
    nProfiler profVisTestBatches;
    /// Quadtree cell's visibility determination profiler
    nProfiler profVisQuadCell;
    /// Whole quadtree cell's process profiler
    nProfiler profVisQuadCellWhole;
    /// Horizon debug profiler
    nProfiler profVisHorizDebug;
    /// Visit camera profiler
    nProfiler profVisCamera;
    /// Make terrain visibles time consumption
    nProfiler profVisMakeTerrainVisible;
    /// Determine level of detail
    nProfiler profDetermineLOD;

#ifndef NGAME
    /// Number of tested entities
    int numTestedEntities;
    /// Watch for the number of tested entities
    nWatched watchNumTestedEntities;
    /// Number of tested batches
    int numTestedBatches;
    /// Watch for the number of tested batches
    nWatched watchNumTestedBatches;
    /// Number of tested quadtree cells
    int numTestedQuadCells;
    /// Watch for the number of tested quadtree cells
    nWatched watchNumTestedQuadCells;
#endif // !NGAME
};

//------------------------------------------------------------------------------
/**
    set scene graph
*/
inline
void 
nVisibleFrustumVisitor::SetSceneGraph(nSceneGraph *sceneGraph)
{
    this->sceneGraph = sceneGraph;
}

//------------------------------------------------------------------------------
/**
    get the current camera we're determining visibility for
*/
inline
const ncSpatialCamera* 
nVisibleFrustumVisitor::GetCamera() const
{
    return this->m_spatialCamera;
}

//------------------------------------------------------------------------------
/**
    get the current camera's entity id
*/
inline
nEntityObjectId 
nVisibleFrustumVisitor::GetCurrentCamId() const
{
    return this->m_currentCamId;
}

//------------------------------------------------------------------------------
/**
    enable or disable horizon culling
*/
inline
void 
nVisibleFrustumVisitor::SetDoHorizonCulling(bool flag)
{
    this->m_doHorizonCulling = flag;
}

//------------------------------------------------------------------------------
/**
    get if it's doing horizon culling or not
*/
inline
bool 
nVisibleFrustumVisitor::IsDoingHorizonCulling() const
{
    return this->m_doHorizonCulling;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    enable or disable quadtree cells painting
*/
inline
void 
nVisibleFrustumVisitor::SetTerrainVisible(bool flag)
{
    this->m_disableQuadtree = !flag;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVisibleFrustumVisitor::IsTerrainVisible() const
{
    return !this->m_disableQuadtree;
}

//------------------------------------------------------------------------------
/**
    enable or disable lights
*/
inline
void 
nVisibleFrustumVisitor::SetLightsDisabled(bool flag)
{
    this->disableLights = flag;
}

//------------------------------------------------------------------------------
/**
    are lights enabled
*/
inline
bool 
nVisibleFrustumVisitor::AreLightsDisabled() const
{
    return this->disableLights;
}

#endif // !NGAME

//------------------------------------------------------------------------------
/**
    says if the outdoor is visible
*/
inline
bool 
nVisibleFrustumVisitor::IsOutdoorVisible() const
{
    return this->outdoorVisible;
}

//------------------------------------------------------------------------------
#endif // N_VISIBLEFRUSTUMVISITOR_H

