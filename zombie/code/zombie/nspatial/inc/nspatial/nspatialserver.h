#ifndef N_SPATIALSERVER_H
#define N_SPATIALSERVER_H
//------------------------------------------------------------------------------
/**
    @class nSpatialServer
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialServer manages the spatial distribution of the world.
    
    (C) 2004  Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/narray.h"
#include "kernel/nroot.h"
#include "kernel/nprofiler.h"
#include "entity/nentity.h"
#include "entity/nrefentityobject.h"
#include "nspatial/nspatialtypes.h"
#include "mathlib/sphere.h"

//------------------------------------------------------------------------------
class ncSpatialCell;
class nSpatialCollector;
class ncSpatialGlobalSpace;
class ncSpatialIndoor;
class ncSpatialQuadtree;
class ncSpatialOctree;
class ncSpatialSpace;
class ncSpatialPortal;
class nVisibleFrustumVisitor;
class nSpatialVisitor;
class nSceneGraph;
class nCamera2;

//------------------------------------------------------------------------------
class nSpatialServer : public nRoot
{
    friend class nSpatialVisitor;
    friend class ncSpatialCamera;

public:

#ifndef NGAME
    enum ExecutionMode
    {
        EDITOR,
        GAME
    };
#endif // !NGAME

    /// constructor
    nSpatialServer();
    /// destructor
    ~nSpatialServer();
    /// get instance pointer
    static nSpatialServer* Instance();

#ifndef NGAME
    /// set the spatial execution mode
    void SetExecutionMode(ExecutionMode mode);
    /// get the spatial execution mode
    ExecutionMode GetExecutionMode() const;
#endif // !NGAME

    /// callback for the level loaded
    void OnLevelLoaded();
    /// flush all the lights in the world
    void FlushAllLights();

    /// insert the entity into the correponding space and cell
    bool InsertEntity(nEntityObject *entity);
    /// insert a global entity
    bool InsertGlobalEntity(nEntityObject *entity, bool alwaysVisible = false);
    /// remove a global entity
    bool RemoveGlobalEntity(nEntityObject *entity);
    /// get the global entities array
    void GetAllGlobalEntities(nArray<nEntityObject*> &globalEntities) const;
    /// get the global common entities array
    void GetCommonGlobalEntities(nArray<nEntityObject*> &globalEntities) const;
    /// get the global always visible entities array
    const nArray<nEntityObject*> &GetGlobalAlwaysVisibleEntities() const;
    /// insert the entity into the correponding space, cell and category, 
    /// depending on the flags
    bool InsertEntity(nEntityObject *entity, const int flags);
           
    /// insert the entity in the first indoor space that contains or 
    /// intersects with the entity, depending on the given flags
    bool InsertEntityOneIndoor(nEntityObject *entity, 
                               const int flags);
    /// insert the entity in all the indoor spaces that contain or intersect with
    /// the given entity, depending on the given flags
    bool InsertEntityAllIndoors(nEntityObject *entity, 
                                const int flags);
    /// insert the given entity in the outdoors (if it's registered) depending on 
    /// the given flags
    bool InsertEntityOutdoors(nEntityObject *entity, 
                              const int flags);
    /// insert the given entity into the octree space (if it's registered) depending
    /// on the flags
    bool InsertEntityOctree(nEntityObject *entity, 
                            const int flags);

    /// move an entity from a cell to another one. The entity's spatial component contains the origin cell
    bool MoveEntity(nEntityObject *entity, ncSpatialCell *destCell);

    /// register an indoor space in the spatial server to manage it
    void RegisterSpace(ncSpatialIndoor* indoorSpace);
    /// register a quadtree space in the spatial server to manage it
    void RegisterSpace(ncSpatialQuadtree* outdoorSpace);
    /// register an octree space in the spatial server to manage it
    void RegisterSpace(ncSpatialOctree* octreeSpace);
    /// register a space in the spatial server to manage it
    void RegisterSpace(ncSpatialSpace *space);
    /// connect the indoor spaces to the outdoor space
    bool ConnectSpaces();
    /// connect the parameter indoor space to the outdoor space
    bool ConnectSpace(ncSpatialIndoor* indoor);
    /// connect indoors that have a common portal
    void ConnectIndoors();
    /// release all owned spaces
    void ReleaseSpaces();
    /// release the octree space
    void ReleaseOctreeSpace();
    /// release all the indoor spaces
    void ReleaseIndoorSpaces();
    /// release an indoor space
    void ReleaseIndoorSpace(ncSpatialIndoor* indoor);
    /// release the quadtree space
    void ReleaseOutdoorSpace();
    /// release the global space
    void ReleaseGlobalSpace();
    /// remove an indoor space from the array (but it doesn't destroy it)
    void RemoveIndoorSpace(ncSpatialIndoor* indoor);
    /// return the number of registered indoor spaces
    int NumIndoorSpaces() const;
    /// return true if there is a registered octree space
    bool HasOctreeSpace() const;
    /// return true if there is a registered quadtree space
    bool HasQuadtreeSpace() const;

    /// create a new spatial camera and introduce it in its corresponding space and cell
    nEntityObject* CreateCamera(const nCamera2 &camera, const matrix44 &viewMatrix, bool on);
    /// destroy a spatial camera
    bool DestroyCamera(ncSpatialCamera *camera);
    /// get access to all spatial cameras
    const nArray<ncSpatialCamera*>& GetCamerasArray();

    /// get the total number of entities in all the spaces controled by the spatial server
    int GetTotalNumEntities() const;
    /// get the disconnected portals of all the indoor spaces
    void GetDisconnectedPortals(nArray<ncSpatialPortal*> *portalsArray) const;
    /// search the cell containing the point in all the spaces
    ncSpatialCell *SearchCell(const vector3 &point) const;

    /// get the entities in the bounding box using the spatial collector
    void GetEntities(const bbox3 &box, nSpatialCollector *collector);
    /// get the entities in the sphere using the spatial collector
    void GetEntities(const sphere &sph, nSpatialCollector *collector);

    /// get the entities whose positions are in the sphere using the spatial collector
    void GetEntitiesByPos(const sphere &sph, nSpatialCollector *collector);
    /// get the entities whose positions are in the sphere and add them to the given array
    void GetEntitiesByPos(const sphere &sph, nArray<nEntityObject*> *entities);

    /// get the entities of a determined category using the given flags and a collector
    void GetEntitiesCategory(const bbox3 &box, 
                             int category, 
                             const int flags, 
                             nSpatialCollector *collector);
    /// get the entities of a determined category using the given flags and a collector
    void GetEntitiesCategory(const sphere &sph, 
                             int category, 
                             const int flags, 
                             nSpatialCollector *collector);
    /// get the entities of a determined category using the given flags and a collector
    void GetEntitiesCategory(const vector3 &point, 
                             int category, 
                             const int flags, 
                             nSpatialCollector *collector);

    /// get the entities of a determined category using the given flags
    void GetEntitiesCategory(const bbox3 &box, 
                             int category, 
                             const int flags, 
                             nArray<nEntityObject*> &entities);
    /// get the entities of a determined category using the given flags
    void GetEntitiesCategory(const sphere &sph, 
                             int category, 
                             const int flags, 
                             nArray<nEntityObject*> &entities);
    /// get the entities of a determined category using the given flags
    void GetEntitiesCategory(const vector3 &point, 
                             int category, 
                             const int flags, 
                             nArray<nEntityObject*> &entities);

    /// get the entities of various categories using the given flags
    void GetEntitiesCategories(const bbox3 &box, 
                               const nArray<int> &categories, 
                               const int flags, 
                               nArray<nEntityObject*> &entities);
    /// get the entities of various categories using the given flags
    void GetEntitiesCategories(const sphere &sph, 
                               const nArray<int> &categories, 
                               const int flags, 
                               nArray<nEntityObject*> &entities);
    /// get the entities of various categories using the given flags
    void GetEntitiesCategories(const vector3 &point, 
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
    /// get the nearest entity to a given position, inside the given sphere
    nEntityObject *GetNearestEntityTo(const sphere &sph, nEntityObjectId id) const;

    /// tell if a point is approximately inside some indoor
    bool IsInsideIndoorApprox( const vector3& point ) const;

    /// get the frame id
    int GetFrameId() const;
    /// set the frame id
    void SetFrameId(int frameId);
    /// set the use octree visibility flag
    void SetUseOctrees(bool flag);
    /// is using octree visibility?
    bool GetUseOctrees();
    /// toggle the octree visibility flag
    void ToggleUseOctrees();
    /// set the recalculate camera position flag
    void SetRecalculateCamPos(bool flag);
    /// get the recalculate camera position flag
    bool GetRecalculateCamPos() const;

    /// adjust quadtree's cells' bounding boxes with the given offset
    void AdjustQuadtreeBBoxes(float offset);

    /// enable or disable horizon culling
    void SetDoHorizonCulling(bool flag);
    /// get if it's doing horizon culling or not
    bool IsDoingHorizonCulling() const;

    /// get the spatial visitor
    nSpatialVisitor &GetSpatialVisitor();
    /// get the visibility visitor
    nVisibleFrustumVisitor &GetVisibilityVisitor();
    /// get the outdoor space
    ncSpatialQuadtree* GetQuadtreeSpace();
    /// get the outdoor space
    nEntityObject* GetOutdoorEntity();
    /// get the indoor spaces
    nArray<ncSpatialIndoor*>& GetIndoorSpaces();
    /// get the octree space
    ncSpatialOctree* GetOctreeSpace();
    /// get a space given its id
    ncSpatialSpace *GetSpaceById(nEntityObjectId id) const;

    /// get the category the class belongs to
    int GetCategory(nEntityObject *entity);
    /// get the map of categories that we have to determine visibility for
    nArray<int> *GetVisCategoriesMap();
    /// add a category to take into account for visibility
    void AddVisCategory(int category);
    /// remove a category from the map of visibility categories
    void RemoveVisCategory(int category);

    /// get the world bbox
    const bbox3 &GetWorldBBox() const;

    /// determine the visible objects for the given camera
    void DetermineVisibility(ncSpatialCamera* cameraComp, nSceneGraph* sceneGraph);
    
    /// remove the entity with the given id from all the spaces
    void EntityDeleted( int id );

#ifndef NGAME
    /// enable or disable quadtree cells painting
    void SetTerrainVisible(bool flag);
    /// says if the terrain is visible
    bool IsTerrainVisible();

    /// set indoor edition mode true or false
    void SetEditIndoorMode(bool flag);
    /// get indoor edition mode true or false
    bool IsInEditIndoorMode() const;
    /// toggle indoor edition mode on/off
    void ToggleEditIndoor();
    /// set the selected entity in the editor
    void SetSelectedEntity(nEntityObject* entity);
    /// get the selected entity in the editor
    nEntityObject* GetSelectedEntity();

    /// set the see all selected indoors mode true or false
    void SetSeeAllSelectedIndoors(bool flag);
    /// get the see all selected indoors mode true or false
    bool GetSeeAllSelectedIndoors() const;
    /// toggle all selected indoors edition mode on/off
    void ToggleEditAllSelectedIndoors();
    /// add a selected indoor
    void AddSelectedIndoor(nEntityObject* indoor);
    /// remove a selected indoor
    bool RemoveSelectedIndoor(nEntityObject* indoor);
    /// get selected indoors
    const nArray<nRefEntityObject> &GetSelectedIndoors() const;
    /// reset the selected indoors array
    void ResetSelectedIndoors();
#endif // !NGAME

private:

    static nSpatialServer *Singleton;
#ifndef NGAME
    /// spatial execution mode
    ExecutionMode execMode;
#endif // !NGAME
    /// current frame id
    int m_frameId;
    /// use only indoor visibility flag
    bool m_onlyIndoorVisibility;
    /// use octrees flag
    bool m_useOctrees;
    /// total number of entities in all the registered spaces
    int m_totalNumEntities;
    /// world box. Contains all the indoors and the outdoor box (but not the octree space!)
    bbox3 m_worldBox;

    /// indoor spaces controlled by the spatial server
    nArray<ncSpatialIndoor*> m_indoorSpaces;
    /// outdoor space controlled by the spatial server. We will have only one.
    nRef<nEntityObject> m_outdoorSpace;
    /// octree space controlled by the spatial server. We will have only one.
    nRef<nEntityObject> m_octreeSpace;

    /// registered spatial cameras
    nArray<ncSpatialCamera*> m_spatialCameras;

    /// map of the categories that we have to determine visibility for
    nArray<int> m_visCategoriesMap;

    /// global space. Contains objects that are always visibles and that aren't in any space
    ncSpatialGlobalSpace *m_globalSpace;

    /// spatial visitor
    nSpatialVisitor *spatialVisitor;
    /// visibility visitor
    nVisibleFrustumVisitor *visibilityVisitor;
    /// recalculate camera's position flag
    bool m_recalculateCamPos;
    
    /// insert the spatial element into only one indoor space depending on the category and flags
    bool InsertElementOneIndoor(const bbox3 &box, int flags);
    /// insert the spatial element into all the spaces that complain with the flags
    bool InsertElementAllIndoors(const bbox3 &box, int flags);
    /// insert the spatial element into the outdoors depending on the category and flags
    bool InsertElementOutdoors(const bbox3 &box, int flags);
    /// insert the spatial element into the octree space depending on the category and flags
    bool InsertElementOctree(const bbox3 &box, int flags);

    /// visibility time consumption
    nProfiler profVis;
    /// make visibles global entities
    nProfiler profVisGlobalEnts;

#ifndef NGAME
    bool m_editIndoorMode;
    nRefEntityObject m_selectedEntity;

    bool seeAllSelectedIndoors;
    nArray<nRefEntityObject> selectedIndoors;
#endif // !NGAME
};

//------------------------------------------------------------------------------
/**
*/
inline
nSpatialServer*
nSpatialServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set the spatial execution mode
*/
inline
void 
nSpatialServer::SetExecutionMode(ExecutionMode mode)
{
    this->execMode = mode;
}

//------------------------------------------------------------------------------
/**
    get the spatial execution mode
*/
inline
nSpatialServer::ExecutionMode 
nSpatialServer::GetExecutionMode() const
{
    return this->execMode;
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    get the total number of entities in all the spaces controled by the spatial server
*/
inline
int 
nSpatialServer::GetTotalNumEntities() const
{
    return this->m_totalNumEntities;
}

//------------------------------------------------------------------------------
/**
    return true if there is an octree space registered
*/
inline
bool 
nSpatialServer::HasOctreeSpace() const
{
    return this->m_octreeSpace.isvalid();
}

//------------------------------------------------------------------------------
/**
    return the number of registered indoor spaces
*/
inline
int 
nSpatialServer::NumIndoorSpaces() const
{
    return this->m_indoorSpaces.Size();
}

//------------------------------------------------------------------------------
/**
    return true if there is a registered quadtree space
*/
inline 
bool 
nSpatialServer::HasQuadtreeSpace() const
{
    return this->m_outdoorSpace.isvalid();
}

//------------------------------------------------------------------------------
/**
    get the frame id
*/
inline
int 
nSpatialServer::GetFrameId() const
{
    return this->m_frameId;
}

//------------------------------------------------------------------------------
/**
    set the frame id
*/
inline
void 
nSpatialServer::SetFrameId(int frameId)
{
    this->m_frameId = frameId;
}

//------------------------------------------------------------------------------
/**
    is using octree visibility?
*/
inline
bool 
nSpatialServer::GetUseOctrees()
{
    return this->m_useOctrees;
}

//------------------------------------------------------------------------------
/**
    set the recalculate camera position flag
*/
inline
void 
nSpatialServer::SetRecalculateCamPos(bool flag)
{
    this->m_recalculateCamPos = flag;
}

//------------------------------------------------------------------------------
/**
    get the recalculate camera position flag
*/
inline
bool 
nSpatialServer::GetRecalculateCamPos() const
{
    return this->m_recalculateCamPos;
}

//------------------------------------------------------------------------------
/**
    get the spatial visitor
*/
inline
nSpatialVisitor &
nSpatialServer::GetSpatialVisitor()
{
    n_assert(this->spatialVisitor);
    return *this->spatialVisitor;
}

//------------------------------------------------------------------------------
/**
    get the visibility visitor
*/
inline
nVisibleFrustumVisitor &
nSpatialServer::GetVisibilityVisitor()
{
    n_assert(this->visibilityVisitor);
    return *this->visibilityVisitor;
}

//------------------------------------------------------------------------------
/**
    get the outdoor space
*/
inline
nEntityObject* 
nSpatialServer::GetOutdoorEntity()
{
    return this->m_outdoorSpace.isvalid() ? this->m_outdoorSpace.get() : 0;
}

//------------------------------------------------------------------------------
/**
    get the indoor spaces
*/
inline
nArray<ncSpatialIndoor*>& 
nSpatialServer::GetIndoorSpaces()
{
    return this->m_indoorSpaces;
}

//------------------------------------------------------------------------------
/**
    get the map of categories that we have to determine visibility for
*/
inline
nArray<int> *
nSpatialServer::GetVisCategoriesMap()
{
    return &this->m_visCategoriesMap;
}

//------------------------------------------------------------------------------
/**
    add a category to take into account for visibility
*/
inline
void 
nSpatialServer::AddVisCategory(int category)
{
    this->m_visCategoriesMap.Append(category);
}

//------------------------------------------------------------------------------
/**
    remove a category from the map of visibility categories
*/
inline
void 
nSpatialServer::RemoveVisCategory(int category)
{
    int index = this->m_visCategoriesMap.FindIndex(category);
    if ( index != -1 )
    {
        this->m_visCategoriesMap.EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
    get the world bbox
*/
inline
const bbox3 &
nSpatialServer::GetWorldBBox() const
{
    return this->m_worldBox;
}

//------------------------------------------------------------------------------
/**
    enable or disable quadtree cells painting
*/
inline
const nArray<ncSpatialCamera*>&
nSpatialServer::GetCamerasArray()
{
    return this->m_spatialCameras;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set indoor edition mode true or false
*/
inline
void 
nSpatialServer::SetEditIndoorMode(bool flag)
{
    this->m_editIndoorMode = flag;
    if (this->m_editIndoorMode)
    {
        this->seeAllSelectedIndoors = false;
    }
}

//------------------------------------------------------------------------------
/**
    get indoor edition mode flag
*/
inline
bool 
nSpatialServer::IsInEditIndoorMode() const
{
    return this->m_editIndoorMode;
}

//------------------------------------------------------------------------------
/**
    toggle indoor edition mode on/off
*/
inline
void 
nSpatialServer::ToggleEditIndoor()
{
    this->m_editIndoorMode = !this->m_editIndoorMode;
    if (this->m_editIndoorMode)
    {
        this->seeAllSelectedIndoors = false;
    }
}

//------------------------------------------------------------------------------
/**
    set the selected entity in the editor
*/
inline
void 
nSpatialServer::SetSelectedEntity(nEntityObject* entity)
{
    if ( entity )
    {
        this->m_selectedEntity = entity;
    }
    else
    {
        this->m_selectedEntity.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    get the selected entity in the editor
*/
inline
nEntityObject* 
nSpatialServer::GetSelectedEntity()
{
    if ( this->m_selectedEntity.isvalid() )
    {
        return this->m_selectedEntity;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    set the see all selected indoors mode true or false
*/
inline
void 
nSpatialServer::SetSeeAllSelectedIndoors(bool flag)
{
    this->seeAllSelectedIndoors = flag;
    if (this->seeAllSelectedIndoors)
    {
        this->m_editIndoorMode = false;
    }
}

//------------------------------------------------------------------------------
/**
    get the see all selected indoors mode true or false
*/
inline
bool 
nSpatialServer::GetSeeAllSelectedIndoors() const
{
    return this->seeAllSelectedIndoors;
}

//------------------------------------------------------------------------------
/**
    toggle all selected indoors edition mode on/off
*/
inline
void 
nSpatialServer::ToggleEditAllSelectedIndoors()
{
    this->seeAllSelectedIndoors = !this->seeAllSelectedIndoors;
    if (this->seeAllSelectedIndoors)
    {
        this->m_editIndoorMode = false;
    }
}

//------------------------------------------------------------------------------
/**
    add a selected indoor
*/
inline
void 
nSpatialServer::AddSelectedIndoor(nEntityObject* indoor)
{
    if (!this->seeAllSelectedIndoors)
    {
        this->selectedIndoors.Append(indoor->GetId());
    }
}

//------------------------------------------------------------------------------
/**
    remove a selected indoor
*/
inline
bool 
nSpatialServer::RemoveSelectedIndoor(nEntityObject* indoor)
{
    if (!this->seeAllSelectedIndoors)
    {
        int index = this->selectedIndoors.FindIndex(indoor);
        if (index != -1)
        {
            this->selectedIndoors.Erase(index);
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    get selected indoors
*/
inline
const nArray<nRefEntityObject> &
nSpatialServer::GetSelectedIndoors() const
{
    return this->selectedIndoors;
}

//------------------------------------------------------------------------------
/**
    reset the selected indoors array
*/
inline
void 
nSpatialServer::ResetSelectedIndoors()
{
    if (!this->seeAllSelectedIndoors)
    {
        this->selectedIndoors.Reset();
    }
}

#endif // !NGAME

#endif
