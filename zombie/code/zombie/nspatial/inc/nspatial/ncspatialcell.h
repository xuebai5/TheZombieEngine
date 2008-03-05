#ifndef NC_SPATIALCELL_H
#define NC_SPATIALCELL_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialCell
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial cell component for cell entities.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/ncomponentobject.h"
#include "nspatial/nspatialtypes.h"
#include "nspatial/nspatialserver.h"

//------------------------------------------------------------------------------
class nVisibleFrustumVisitor;
class nSpatialVisitor;
class nEntityObject;
class ncSpatialSpace;
class TiXmlElement;
class ncSpatialPortal;
class ncSpatialLight;
class ncSpatial;
class nLightRegion;
class nSpatialShadowModel;

//------------------------------------------------------------------------------
class ncSpatialCell : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSpatialCell, nComponentObject);

public:

    enum nSpatialCellType
    {
        // extend with your own cell types in a subclass
        N_SPATIAL_CELL,  /// base type (this one)
        N_INDOOR_CELL,   /// indoor cell
        N_QUADTREE_CELL, /// quadtree cell
        N_OCTREE_CELL,   /// octree cell
        N_GLOBAL_CELL    /// global cell
    };

    /// @struct CellAndEntityId an entry in a list of cell entities
    struct CellAndEntityId
    {
        CellAndEntityId();
        CellAndEntityId(int, nEntityObjectId);
        int cellId;
        nEntityObjectId objectId;
    };

    typedef nArray<nArray<CellAndEntityId> > WizardEntityArray;

    /// constructor
    ncSpatialCell();
    /// destructor
    virtual ~ncSpatialCell();

    /// flush all the lights in the cell
    virtual void FlushAllLights();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// save the entities contained into this cell
    virtual void Save(WizardEntityArray &wizards);
    /// load list of entities
    virtual bool Load(const char *spatialPath);
    /// destroy the cell and all its elements
    virtual void DestroyCell();

    /// destroy all the entities in this cell (not portals neither lights)
    void DestroyAllEntities();
    /// destroy all the portals in this cell
    void DestroyAllPortals();
    /// destroy all the lights in this cell
    void DestroyAllLights();

    /// load contained entities
    bool LoadEntities();

    /// set the space the cell belongs to
    void SetParentSpace(ncSpatialSpace *space);
    /// get the space the cell belongs to
    ncSpatialSpace *GetParentSpace() const;

    /// adds an entity id to the corresponding array
    virtual void AddEntityId(nEntityObjectId id);
    /// adds an entity to the cell
    virtual bool AddEntity(nEntityObject *entity);
    /// adds an entity to the cell in a category
    virtual bool AddEntity(nEntityObject *entity, int category);

    /// removes an entity from the cell without destroying it
    virtual bool RemoveEntity(nEntityObject *entity);
    /// removes an entity from the cell and a category without
    virtual bool RemoveEntity(nEntityObject *entity, int category);

    /// adds a light region to this cell
    void AddLightRegion(nLightRegion *lightRegion);
    /// remove a light region from this cell
    void RemoveLightRegion(nLightRegion *lightRegion);
    /// get the light regions array
    nArray<nLightRegion*> &GetLightRegions();
    /// destroy all the regions of this cell
    void DestroyLightRegions();
    /// recalculate the light regions of the lights in this cell
    void RefreshLights();

    /// adds a shadow model to this cell
    void AddShadowModel(nSpatialShadowModel *shadowModel);
    /// removes a shadow model from this cell
    void RemoveShadowModel(nSpatialShadowModel *shadowModel);
    /// get the shadow models array
    nArray<nSpatialShadowModel*> *GetShadowModels();
    /// destroy all the shadow models in this cell
    void DestroyShadowModels();
    
#ifndef NGAME
    /// removes an entity temporary from the cell
    bool RemoveTempEntity(nEntityObject *entity);
    /// restore an entity that was temporary removed
    bool RestoreTempEntity(nEntityObject *entity);
#endif // !NGAME

    /// determine if a given point belongs to this cell using the wrapper mesh
    virtual bool Contains(const vector3& point) const;
    /// determine if a given bbox belongs to this cell using the wrapper mesh
    virtual bool Contains(const bbox3 &box) const;
    /// determine if a given sphere belongs to this cell using the wrapper mesh
    virtual bool Contains(const sphere &sph) const;

    /// returns the portal in this cell that contains the given point
    ncSpatialPortal *IsInPortals(const vector3 &point);

    /// set the bounding box of the cell
    void SetBBox(const bbox3 &box);
    /// get the bounding box of the cell
    bbox3 &GetBBox();
    /// set the original bounding box of the cell
    void SetOriginalBBox(const bbox3 &box);
    /// get the original bounding box of the cell
    bbox3 &GetOriginalBBox();
    /// get bounding box's center
    const vector3& GetBBoxCenter() const;
    /// get bounding box's extents
    const vector3& GetBBoxExtents() const;
    /// set cell's transformation matrix
    void SetTransformMatrix(matrix44 &matrix);
    /// get cell's transformation matrix
    matrix44 *GetTransformMatrix();
    /// get number of entities
    int GetNumEntities() const;
    /// get the entities id array
    nArray<nEntityObjectId>& GetEntitiesIdArray();
    /// get a concrete category
    const nArray<nEntityObject*>& GetCategory(int catId) const;
    /// get the array of categories
    const nArray<nEntityObject*>* GetCategories() const;
    /// get the lights that only affect this cell
    void GetOnlyCellLights(nArray<ncSpatialLight*> &onlyCellLights);
    /// get visible entities array
    const nArray<nEntityObject*> &GetVisEntitiesArray();
    /// add an entity to the visible entities array
    void AppendVisEntity(nEntityObject *entity);
    /// reset the array of visible entities of this cell
    void ResetVisEntitiesArray();

    /// get neighbor cells
    void GetNeighbors(nArray<ncSpatialCell*> &neighbors);

    /// set cell's identifier
    void SetId(int id);
    /// get cell's identifier
    int GetId() const;

    /// read the static information from a TiXmlElement
    virtual bool ReadStaticInfo(const TiXmlElement *cellElem);

    /// write the static information of this cell to the TiXmlElement
    virtual bool WriteStaticInfo(TiXmlElement *cellElem);

    /// returns the type of cell
    virtual nSpatialCellType GetType() const;

    /// set visibility frame id
    virtual void SetVisibleFrameId(int id);
    /// get visibility frame id
    int GetVisibleFrameId() const;
    /// set visit frame id
    virtual void SetVisitFrameId(int id);
    /// get visit frame id
    int GetVisitFrameId() const;    
    /// set this cell as visible from the given camera id in the current frame
    virtual void SetVisibleBy(nEntityObjectId id);
    /// get the id of the camera the entity was visible for the last time
    nEntityObjectId GetLastVisibleCam() const;
    /// set the id of the camera the cell was visible for the last time
    void SetLastVisibleCam(nEntityObjectId cameraId);
    /// says if the entity was visible for the current camera in this frame
    bool IsVisibleBy(nEntityObjectId currentCam) const;
    /// says if the entity was visible by any camera in this frame
    bool IsVisible() const;

    /// set this cell as visited by the given camera id in the current frame
    virtual void SetVisitedBy(nEntityObjectId id);
    /// get the id of the camera the entity was visited by the last time
    nEntityObjectId GetLastVisitCam() const;
    /// says if the entity was visited by the current camera in this frame
    bool IsVisitedBy(nEntityObjectId currentCam) const;
    
    #ifndef NGAME
    /// set the layer id to all the entities into this cell
    virtual void SetLayerId(int layerId);
    #endif

    /// update the transformation of all the entities into the cell
    virtual void Update(vector3 &incPos, quaternion &incQuat, const matrix44 &matrix);

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);

protected:

    /// space that this cell belongs to
    ncSpatialSpace *m_parentSpace;

    /// array of entities ids of this cell
    nArray<nEntityObjectId> m_entitiesIdArray;

    /// visible entities determined in the last frame
    nArray<nEntityObject*> m_visEntitiesArray;

    /// light regions
    nArray<nLightRegion*> lightRegions;

    /// shadows in this cell
    nArray<nSpatialShadowModel*> *shadowModels;

    /// cell's transformation matrix
    matrix44 m_transformationMatrix;
    /// cell's bounding box
    bbox3 m_bbox;
    /// cell's original bounding box
    bbox3 m_originalBBox;
    /// bbox center
    vector3 bboxCenter;
    /// bbox extents
    vector3 bboxExtents;
    /// cell's id
    int m_cellId;
    /// cell's type
    nSpatialCellType m_cellType;

    /// categories
    nArray<nEntityObject*> m_categories[nSpatialTypes::NUM_SPATIAL_CATEGORIES];

    /// visibility frame id. Last frame that this cell was visible.
    int m_visibleFrameId;
    /// visit frame id. Last frame that this cell was visited.
    int m_visitFrameId;
    /// id of the last camera that the element was visible for
    nEntityObjectId m_lastVisibleCam;
    /// id of the last camera that the element was visited from
    nEntityObjectId m_lastVisitCam;

};

//------------------------------------------------------------------------------
/**
*/
inline
ncSpatialCell::CellAndEntityId::CellAndEntityId() :
    cellId(-1),
    objectId(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSpatialCell::CellAndEntityId::CellAndEntityId(int cid, nEntityObjectId eoid) :
    cellId(cid),
    objectId(eoid)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    returns the type of cell
*/
inline
ncSpatialCell::nSpatialCellType 
ncSpatialCell::GetType() const
{
    return this->m_cellType;
}

//------------------------------------------------------------------------------
/**
    set the space the cell belongs to
*/
inline
void 
ncSpatialCell::SetParentSpace(ncSpatialSpace *space)
{
    this->m_parentSpace = space;
}

//------------------------------------------------------------------------------
/**
    get the space the cell belongs to
*/
inline
ncSpatialSpace *
ncSpatialCell::GetParentSpace() const
{
    return this->m_parentSpace;
}

//------------------------------------------------------------------------------
/**
    adds an entity id to the corresponding array
*/
inline
void 
ncSpatialCell::AddEntityId(nEntityObjectId id)
{
    this->m_entitiesIdArray.Append(id);
}

//------------------------------------------------------------------------------
/**
    set the bounding box of the cell
*/
inline
void 
ncSpatialCell::SetBBox(const bbox3 &box)
{
    this->m_bbox = box;
    this->bboxCenter = box.center();
    this->bboxExtents = box.extents();
}

//------------------------------------------------------------------------------
/**
    set the original bounding box of the cell
*/
inline
void 
ncSpatialCell::SetOriginalBBox(const bbox3 &box)
{
    this->m_originalBBox = box;
}

//------------------------------------------------------------------------------
/**
    get the bounding box of the cell
*/
inline
bbox3 &
ncSpatialCell::GetBBox()
{
    return this->m_bbox;
}

//------------------------------------------------------------------------------
/**
    get the original bounding box of the cell
*/
inline
bbox3 &
ncSpatialCell::GetOriginalBBox()
{
    return this->m_originalBBox;
}

//------------------------------------------------------------------------------
/**
    get bounding box's center
*/
inline
const vector3& 
ncSpatialCell::GetBBoxCenter() const
{
    return this->bboxCenter;
}

//------------------------------------------------------------------------------
/**
    get bounding box's extents
*/
inline
const vector3& 
ncSpatialCell::GetBBoxExtents() const
{
    return this->bboxExtents;
}

//------------------------------------------------------------------------------
/**
    set cell's transformation matrix
*/
inline
void
ncSpatialCell::SetTransformMatrix(matrix44 &matrix)
{
    this->m_transformationMatrix = matrix;
}

//------------------------------------------------------------------------------
/**
    get cell's transformation matrix
*/
inline
matrix44 *
ncSpatialCell::GetTransformMatrix()
{
    return &this->m_transformationMatrix; 
}

//------------------------------------------------------------------------------
/**
    get the number of entities in the cell
*/
inline
int
ncSpatialCell::GetNumEntities() const
{
    int numEntities = 0;
    for ( int i = 0; i < nSpatialTypes::NUM_SPATIAL_CATEGORIES; i++)
    {
        numEntities += this->m_categories[i].Size();
    }

    return numEntities;
}

//------------------------------------------------------------------------------
/**
    get the entities id array

*/
inline
nArray<nEntityObjectId>&
ncSpatialCell::GetEntitiesIdArray()
{
    return this->m_entitiesIdArray;
}

//------------------------------------------------------------------------------
/**
    get a concrete category
*/
inline
const nArray<nEntityObject*>&
ncSpatialCell::GetCategory(int catId) const
{
    return this->m_categories[catId];
}

//------------------------------------------------------------------------------
/**
    get the array of categories
*/
inline
const nArray<nEntityObject*>*
ncSpatialCell::GetCategories() const
{
    return this->m_categories;
}

//------------------------------------------------------------------------------
/**
    get visible entities array
*/
inline
const nArray<nEntityObject*> &
ncSpatialCell::GetVisEntitiesArray()
{
    return this->m_visEntitiesArray;
}

//------------------------------------------------------------------------------
/**
    add an entity to the visible entities array
*/
inline
void 
ncSpatialCell::AppendVisEntity(nEntityObject *entity)
{
    this->m_visEntitiesArray.Append(entity);
}

//------------------------------------------------------------------------------
/**
    reset the array of visible entities of this cell
*/
inline
void 
ncSpatialCell::ResetVisEntitiesArray()
{
    this->m_visEntitiesArray.Reset();
}

//------------------------------------------------------------------------------
/**
    set cell's identifier
*/
inline
void 
ncSpatialCell::SetId(int id)
{
    this->m_cellId = id;
}

//------------------------------------------------------------------------------
/**
    get cell's identifier
*/
inline
int 
ncSpatialCell::GetId() const
{
    return this->m_cellId;
}

//------------------------------------------------------------------------------
/**
    set visibility frame id
*/
inline
void 
ncSpatialCell::SetVisibleFrameId(int id)
{
    this->m_visibleFrameId = id;
}

//------------------------------------------------------------------------------
/**
    get visibility frame id
*/
inline
int 
ncSpatialCell::GetVisibleFrameId() const
{
    return this->m_visibleFrameId;
}

//------------------------------------------------------------------------------
/**
    set visit frame id
*/
inline
void 
ncSpatialCell::SetVisitFrameId(int id)
{
    this->m_visitFrameId = id;
}

//------------------------------------------------------------------------------
/**
    get visit frame id
*/
inline
int 
ncSpatialCell::GetVisitFrameId() const
{
    return this->m_visitFrameId;
}

//------------------------------------------------------------------------------
/**
    set this cell as visible from the given camera id in the current frame
*/
inline
void 
ncSpatialCell::SetVisibleBy(nEntityObjectId cameraId)
{
    this->SetLastVisibleCam(cameraId);
    this->SetVisibleFrameId(nSpatialServer::Instance()->GetFrameId());
}

//------------------------------------------------------------------------------
/**
    set the id of the camera the cell was visible for the last time
*/
inline
void
ncSpatialCell::SetLastVisibleCam(nEntityObjectId cameraId)
{
    this->m_lastVisibleCam = cameraId;
}

//------------------------------------------------------------------------------
/**
    get the id of the camera the cell was visible for the last time
*/
inline
nEntityObjectId 
ncSpatialCell::GetLastVisibleCam() const
{
    return this->m_lastVisibleCam;
}

//------------------------------------------------------------------------------
/**
    says if the cell was visible for the current camera in this frame
*/
inline
bool 
ncSpatialCell::IsVisibleBy(nEntityObjectId currentCam) const
{
    return (this->GetVisibleFrameId()  == nSpatialServer::Instance()->GetFrameId() &&
            this->GetLastVisibleCam() == currentCam);
}

//------------------------------------------------------------------------------
/**
    return true if this cell has been determined visible in this frame
*/
inline
bool
ncSpatialCell::IsVisible() const
{
    return (this->GetVisibleFrameId() == nSpatialServer::Instance()->GetFrameId());
}

//------------------------------------------------------------------------------
/**
    set this cell as visited in this frame by the given camera
*/
inline
void 
ncSpatialCell::SetVisitedBy(nEntityObjectId cameraId)
{
    this->m_lastVisitCam = cameraId;
    this->SetVisitFrameId(nSpatialServer::Instance()->GetFrameId());
}

//------------------------------------------------------------------------------
/**
    get the id of the camera the entity was visited by the last time
*/
inline
nEntityObjectId 
ncSpatialCell::GetLastVisitCam() const
{
    return this->m_lastVisitCam;
}

//------------------------------------------------------------------------------
/**
    says if the cell was visited by the current camera in this frame
*/
inline
bool 
ncSpatialCell::IsVisitedBy(nEntityObjectId currentCam) const
{
    return (this->GetVisitFrameId() == nSpatialServer::Instance()->GetFrameId() &&
            this->GetLastVisitCam() == currentCam);
}

//------------------------------------------------------------------------------
/**
    get the light regions array
*/
inline
nArray<nLightRegion*> &
ncSpatialCell::GetLightRegions()
{
    return this->lightRegions;
}

#endif // NC_SPATIALCELL_H
