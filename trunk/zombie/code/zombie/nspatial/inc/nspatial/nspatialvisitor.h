#ifndef N_SPATIALVISITOR_H
#define N_SPATIALVISITOR_H
//------------------------------------------------------------------------------
/**
    @class nSpatialVisitor
    @ingroup NebulaSpatialSystem
    @brief Visitor to collect all the entities in a given region of space or 
    update the spatial information

    (C) 2006 Conjurer Services, S.A.
*/

#include "util/narray.h"
#include "mathlib/matrix.h"

#define N_MIN_SEARCH_SPHERE_RADIUS 5.0f

class nSpatialServer;
class ncSpatialCell;
class nSpatialCollector;
class bbox3;
class sphere;
class ncSpatial;
class ncSpatialCamera;
class ncSpatialOccluder;
class ncSpatialPortal;
class nLightRegion;
class nFrustumClipper;

struct VisitorFlags;

class nSpatialVisitor 
{

public:
    
    /// constructor
    nSpatialVisitor();
    /// destructor
    ~nSpatialVisitor(){}

    /// set the spatial server reference
    void SetSpatialServer(nSpatialServer *spatialServer);

    /// move an entity from a cell to another one
    bool MoveEntity(nEntityObject *entity, ncSpatialCell *destCell);

    /// search the cell containing the point in all the spaces
    ncSpatialCell *SearchCell(const vector3 &point) const;
    /// process a spatial component
    void Visit(ncSpatial *visitee, const matrix44 &newTransform);
    /// process a spatial component in game mode
    void VisitGame(ncSpatial *visitee, const matrix44 &newTransform);
    /// process a spatial portal
    void Visit(ncSpatialPortal *portal, const matrix44 &newTransform);
    /// process a camera component
    void Visit(ncSpatialCamera *visitee, const vector3 &newPos);
    /// resituate the occluder in the corresponding cell only if is necessary
    void Visit(ncSpatialOccluder *visitee);
    /// resituate the light and create its new regions
    void Visit(ncSpatialLight *light, const matrix44 &newTransform);

    /// search all the entities in the sphere and give them to the collector
    void Visit(const sphere &sph, nSpatialCollector *collector);
    /// search all the entities in the bounding box and give them to the collector
    void Visit(const bbox3 &box, nSpatialCollector *collector);
 
//#ifndef NGAME
    /// process a spatial component in edition mode
    void VisitEdition(ncSpatial *visitee, const matrix44 &newTransform);
//#endif // !NGAME

    /// search all the entities whose positions are in the sphere and give them to the collector
    void GetEntitiesByPos(const sphere &sph, nSpatialCollector *collector);
    /// get the entities whose positions are in the sphere and add them to the given array
    void GetEntitiesByPos(const sphere &sph, nArray<nEntityObject*> *entities);
    
    /// get the entities of a determined category using the given flags and a collector
    bool GetEntitiesCategory(const bbox3 &box, 
                             int category, 
                             const int flags, 
                             nSpatialCollector *collector);
    /// get the entities of a determined category using the given flags and a collector
    bool GetEntitiesCategory(const sphere &sph, 
                             int category, 
                             const int flags, 
                             nSpatialCollector *collector);
    /// get the entities of a determined category using the given flags and a collector
    bool GetEntitiesCategory(const vector3 &point, 
                             int category, 
                             const int flags, 
                             nSpatialCollector *collector);

    /// get the entities of a determined category using the given flags
    bool GetEntitiesCategory(const bbox3 &box, 
                             int category, 
                             const int flags, 
                             nArray<nEntityObject*> &entities);
    /// get the entities of a determined category using the given flags
    bool GetEntitiesCategory(const sphere &sph, 
                             int category, 
                             const int flags, 
                             nArray<nEntityObject*> &entities);
    /// get the entities of a determined category using the given flags
    bool GetEntitiesCategory(const vector3 &point, 
                             int category, 
                             const int flags, 
                             nArray<nEntityObject*> &entities);

    /// get the entities of various categories using the given flags
    bool GetEntitiesCategories(const bbox3 &box, 
                               const nArray<int> &categories,
                               const int flags, 
                               nArray<nEntityObject*> &entities);
    /// get the entities of various categories using the given flags
    bool GetEntitiesCategories(const sphere &sph, 
                               const nArray<int> &categories,
                               const int flags, 
                               nArray<nEntityObject*> &entities);
    /// get the entities of various categories using the given flags
    bool GetEntitiesCategories(const vector3 &point, 
                               const nArray<int> &categories,
                               const int flags, 
                               nArray<nEntityObject*> &entities);


    
    /// get all the entities of a given category, using the box and the flags. It returns the intersecting cells
    bool GetEntitiesCellsCategory(const bbox3 &box,
                                  int category, 
                                  const int flags, 
                                  nArray<nEntityObject*> &entities,
                                  nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the sphere and the flags. It returns the intersecting cells
    bool GetEntitiesCellsCategory(const sphere &sph,
                                  int category, 
                                  const int flags, 
                                  nArray<nEntityObject*> &entities,
                                  nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the point and the flags. It returns the intersecting cells
    bool GetEntitiesCellsCategory(const vector3 &point,
                                  int category, 
                                  const int flags, 
                                  nArray<nEntityObject*> &entities,
                                  nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories using the given flags. It returns the intersecting cells
    bool GetEntitiesCellsCategories(const bbox3 &box, 
                                    const nArray<int> &categories,
                                    const int flags, 
                                    nArray<nEntityObject*> &entities,
                                    nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags. It returns the intersecting cells
    bool GetEntitiesCellsCategories(const sphere &sph, 
                                    const nArray<int> &categories,
                                    const int flags, 
                                    nArray<nEntityObject*> &entities,
                                    nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags. It returns the intersecting cells
    bool GetEntitiesCellsCategories(const vector3 &point, 
                                    const nArray<int> &categories,
                                    const int flags, 
                                    nArray<nEntityObject*> &entities,
                                    nArray<ncSpatialCell*> &cells) const;

    
    /// get all the entities of a given category, using the box and the flags and the given cells
    bool GetEntitiesUsingCellsCategory(const bbox3 &box,
                                       int category, 
                                       const int flags, 
                                       nArray<nEntityObject*> &entities,
                                       const nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the sphere and the flags and the given cells
    bool GetEntitiesUsingCellsCategory(const sphere &sph,
                                       int category, 
                                       const int flags, 
                                       nArray<nEntityObject*> &entities,
                                       const nArray<ncSpatialCell*> &cells) const;
    /// get all the entities of a given category, using the point and the flags and the given cells
    bool GetEntitiesUsingCellsCategory(const vector3 &point,
                                       int category, 
                                       const int flags, 
                                       nArray<nEntityObject*> &entities,
                                       const nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories using the given flags and the given cells
    bool GetEntitiesUsingCellsCategories(const bbox3 &box, 
                                         const nArray<int> &categories,
                                         const int flags, 
                                         nArray<nEntityObject*> &entities,
                                         const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags and the given cells
    bool GetEntitiesUsingCellsCategories(const sphere &sph, 
                                         const nArray<int> &categories,
                                         const int flags, 
                                         nArray<nEntityObject*> &entities,
                                         const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories using the given flags and the given cells
    bool GetEntitiesUsingCellsCategories(const vector3 &point, 
                                         const nArray<int> &categories,
                                         const int flags, 
                                         nArray<nEntityObject*> &entities,
                                         const nArray<ncSpatialCell*> &cells) const;

    /// get the nearest entity to a given position
    nEntityObject *GetNearestEntityTo(const vector3 &pos, nEntityObjectId id) const;
    /// get the nearest entity to the given sphere's center and into the given sphere
    nEntityObject *GetNearestEntityTo(const sphere &sph, nEntityObjectId id) const;

    /// Build a new frustum directly using the portal vertices and the given far plane
    nFrustumClipper BuildNewFrustumNoTest(ncSpatialPortal *portal, const vector3 &pos, 
        const plane *farPlane);

    /// Build a new frustum using the portal vertices, the old frustum and the given far plane
    nFrustumClipper BuildNewFrustumTest(ncSpatialPortal *portal, 
        const nFrustumClipper &oldFrustum,
        const plane *farPlane,
        const vector3 &pos,
        const struct ClipVertexInfo *vertsInfo);

protected:

    /// spatial server reference
    nSpatialServer *m_spatialServer;

    /// checks if the given spatial component has traversed any portal and resituates
    /// the entity if necessary
    bool CheckPortalTraversing(ncSpatial *spatialComp, ncSpatialCell *cell);

    /// Check if a segment (orig -> dest) intersects with a triangle (v0, v1, v2)
    bool IntersectsTriangle(vector3 const &orig, 
                            vector3 const &dest,
                            vector3 const &v0,
                            vector3 const &v1,
                            vector3 const &v2) const;

    /// Recalculate the light regions
    void CreateLightRegions(ncSpatialLight *light);

    /// Build the light regions for a point light
    void BuildRegionsPoint(nLightRegion *prevRegion, ncSpatialCell *cell, ncSpatialCell *prevCell, 
        nArray<ncSpatialPortal*> &traversedPortals);
    
    /// Build the light regions for a point light in a quadtree
    void BuildRegionsPointQuadtree(nLightRegion *prevRegion, ncSpatialQuadtreeCell *cell, 
        ncSpatialCell *prevCell, nArray<ncSpatialPortal*> &traversedPortals);

    /// Build the light regions for a spot light
    void BuildRegionsSpot(nLightRegion *prevRegion, ncSpatialCell *cell, ncSpatialCell *prevCell,
        const VisitorFlags &clipstatus, nArray<ncSpatialPortal*> &traversedPortals);
    
    /// Build the light regions for a spot light in a quadtree
    void BuildRegionsSpotQuadtree(nLightRegion *prevRegion, ncSpatialQuadtreeCell *cell, 
        ncSpatialCell *prevCell, const VisitorFlags &clipstatus, nArray<ncSpatialPortal*> &traversedPortals);

    /// Build the light regions for an omni light
    void BuildRegionsOmni(nLightRegion *prevRegion, ncSpatialCell *cell, ncSpatialCell *prevCell, 
        nArray<ncSpatialPortal*> &traversedPortals);

    /// Build the light regions for an omni light in a quadtree
    void BuildRegionsOmniQuadtree(nLightRegion *prevRegion, ncSpatialQuadtreeCell *cell, 
        ncSpatialCell *prevCell, nArray<ncSpatialPortal*> &traversedPortals);

    /// get the entities of various categories searching in the indoors
    bool GetEntitiesCategoriesIndoors(const bbox3 &box,
                                      const nArray<int> &categories, 
                                      const int flags, 
                                      nArray<nEntityObject*> &entities);
    /// get the entities of various categories searching in the indoors
    bool GetEntitiesCategoriesIndoors(const sphere &sph,
                                      const nArray<int> &categories, 
                                      const int flags, 
                                      nArray<nEntityObject*> &entities);
    /// get the entities of various categories searching in the indoors
    bool GetEntitiesCategoriesIndoors(const vector3 &point,
                                      const nArray<int> &categories, 
                                      const int flags, 
                                      nArray<nEntityObject*> &entities);

    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesCategoriesOutdoors(const bbox3 &box,
                                       const nArray<int> &categories, 
                                       const int flags, 
                                       nArray<nEntityObject*> &entities);
    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesCategoriesOutdoors(const sphere &sph,
                                       const nArray<int> &categories, 
                                       const int flags, 
                                       nArray<nEntityObject*> &entities);
    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesCategoriesOutdoors(const vector3 &point,
                                       const nArray<int> &categories, 
                                       const int flags, 
                                       nArray<nEntityObject*> &entities);



    /// get the entities of a given category searching in the indoors
    bool GetEntitiesCellsCategoryIndoors(const bbox3 &box,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the indoors
    bool GetEntitiesCellsCategoryIndoors(const sphere &sph,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the indoors
    bool GetEntitiesCellsCategoryIndoors(const vector3 &point,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories searching in the indoors
    bool GetEntitiesCellsCategoriesIndoors(const bbox3 &box,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the indoors
    bool GetEntitiesCellsCategoriesIndoors(const sphere &sph,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the indoors
    bool GetEntitiesCellsCategoriesIndoors(const vector3 &point,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;

    /// get the entities of a given category searching in the outdoors
    bool GetEntitiesCellsCategoryOutdoors(const bbox3 &box,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the outdoors
    bool GetEntitiesCellsCategoryOutdoors(const sphere &sph,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the outdoors
    bool GetEntitiesCellsCategoryOutdoors(const vector3 &point,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesCellsCategoriesOutdoors(const bbox3 &box,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesCellsCategoriesOutdoors(const sphere &sph,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesCellsCategoriesOutdoors(const vector3 &point,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        nArray<ncSpatialCell*> &cells) const;



    /// get the entities of a given category searching in the indoors
    bool GetEntitiesUsingCellsCategoryIndoors(const bbox3 &box,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the indoors
    bool GetEntitiesUsingCellsCategoryIndoors(const sphere &sph,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the indoors
    bool GetEntitiesUsingCellsCategoryIndoors(const vector3 &point,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories searching in the indoors
    bool GetEntitiesUsingCellsCategoriesIndoors(const bbox3 &box,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the indoors
    bool GetEntitiesUsingCellsCategoriesIndoors(const sphere &sph,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the indoors
    bool GetEntitiesUsingCellsCategoriesIndoors(const vector3 &point,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;

    /// get the entities of a given category searching in the outdoors
    bool GetEntitiesUsingCellsCategoryOutdoors(const bbox3 &box,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the outdoors
    bool GetEntitiesUsingCellsCategoryOutdoors(const sphere &sph,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of a given category searching in the outdoors
    bool GetEntitiesUsingCellsCategoryOutdoors(const vector3 &point,
        int category, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;

    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesUsingCellsCategoriesOutdoors(const bbox3 &box,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesUsingCellsCategoriesOutdoors(const sphere &sph,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;
    /// get the entities of various categories searching in the outdoors
    bool GetEntitiesUsingCellsCategoriesOutdoors(const vector3 &point,
        const nArray<int> &categories, 
        const int flags, 
        nArray<nEntityObject*> &entities,
        const nArray<ncSpatialCell*> &cells) const;





    /// get the nearest entity to the given initial sphere's center. This initial sphere can grow
    /// until it reaches the given maximun radius
    nEntityObject *GetNearestEntityTo(const sphere &initSph, nEntityObjectId id, float maxRadius) const;

#ifdef __NEBULA_STATS__
    /// profiler to know the time consumption of the common visit for the entities
    nProfiler profSpa_Visit;
    /// profiler for the get entities by category using box and collector
    mutable nProfiler profSpa_GetEntsCatBoxCol;
    /// profiler for the get entities by category using sphere and collector
    mutable nProfiler profSpa_GetEntsCatSphCol;
    /// profiler for the get entities by category using point and collector
    mutable nProfiler profSpa_GetEntsCatPointCol;
    
    /// profiler for the get entities by category using box
    mutable nProfiler profSpa_GetEntsCatBox;
    /// profiler for the get entities by category using sphere
    mutable nProfiler profSpa_GetEntsCatSph;
    /// profiler for the get entities by category using point
    mutable nProfiler profSpa_GetEntsCatPoint;
     
    /// profiler for the get entities by various categories using box
    mutable nProfiler profSpa_GetEntsCatsBox;
    /// profiler for the get entities by various categories using sphere
    mutable nProfiler profSpa_GetEntsCatsSph;
    /// profiler for the get entities by various categories using point
    mutable nProfiler profSpa_GetEntsCatsPoint;
#endif // __NEBULA_STATS__
};

//------------------------------------------------------------------------------
/**
    constructor
*/
inline
nSpatialVisitor::nSpatialVisitor()
#ifdef __NEBULA_STATS__
    :
    profSpa_Visit("profSpa_Visit", true),
    profSpa_GetEntsCatBoxCol("profSpa_GetEntsCatBoxCol", true),
    profSpa_GetEntsCatSphCol("profSpa_GetEntsCatSphCol", true),
    profSpa_GetEntsCatPointCol("profSpa_GetEntsCatPointCol", true),
    profSpa_GetEntsCatBox("profSpa_GetEntsCatBox", true),
    profSpa_GetEntsCatSph("profSpa_GetEntsCatSph", true),
    profSpa_GetEntsCatPoint("profSpa_GetEntsCatPoint", true),
    profSpa_GetEntsCatsBox("profSpa_GetEntsCatsBox", true),
    profSpa_GetEntsCatsSph("profSpa_GetEntsCatsSph", true),
    profSpa_GetEntsCatsPoint("profSpa_GetEntsCatPoint", true)
#endif // __NEBULA_STATS__
{
}

//------------------------------------------------------------------------------
/**
    Set the spatial server reference
*/
inline
void 
nSpatialVisitor::SetSpatialServer(nSpatialServer *spatialServer)
{
    this->m_spatialServer = spatialServer;
}

#endif
