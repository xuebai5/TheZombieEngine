#ifndef NC_SPATIAL_H
#define NC_SPATIAL_H
//------------------------------------------------------------------------------
/**
    @class ncSpatial
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial component for entities.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "mathlib/bbox.h"
#include "gfx2/ncamera2.h"

//------------------------------------------------------------------------------
class nSpatialSpace;
class ncSpatialSpace;
class nSpatialCell;
class nSpatialModel;
class nSpatialShadowModel;
class ncSpatialBatch;
class nOctElement;
class nVisibleFrustumVisitor;
class ncSpatialCell;
class ncSpatialLight;
class ncSpatialIndoor;

//------------------------------------------------------------------------------
class ncSpatial : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSpatial, nComponentObject);

public:

    enum DetermineSpaceFlag
    {
        DS_TRUE,
        DS_FALSE,
        DS_NOT_INIT
    };

    /// constructor
    ncSpatial();
    /// destructor
    virtual ~ncSpatial();

    /// destroy the spatial comp
    virtual void Destroy();
    /// clear all the scene links of this entity
    void ClearLinks();
    /// set axis-aligned bounding box
    void SetBBox(float, float, float, float, float, float);
    /// get axis-aligned bounding box
    void GetBBox(vector3&, vector3&);
    /// set original bbox (for an instance)
    void SetOriginalBBox(float, float, float, float, float, float);
    /// get original bbox (for an instance)
    void GetOriginalBBox(vector3&, vector3&);
    /// set the cell id the entity is in
    virtual void SetCellId(int);
    /// get the cell id the entity is in
    virtual int GetCellId() const;
    /// set the indoor entity id this entity is in
    virtual void SetSpaceId(nEntityObjectId);
    /// get the space id the entity is in
    virtual nEntityObjectId GetSpaceId() const;
    /// set the type of test model
    void SetModelType(int);
    /// get the type of test model
    int GetModelType() const;
    /// set the spatial category
    void SetSpatialCategory(int);
    /// get the spatial category
    int GetSpatialCategory() const;
    /// set the sphere test model's radius
    void SetTestSphereProperties(float);
    /// set the frustum perspective test model's properties
    void SetTestFrustumProperties(float aov, float aspect, float nearp, float farp);
    /// set the frustum orthogonal test model's properties
    void SetTestFrustumOrthoProperties(float width, float height, float nearp, float farp);
    /// set if it's a shadow caster
    void SetIsShadowCaster(bool);
    /// is shadow caster?
    bool GetIsShadowCaster() const;
    /// is batched?
    bool IsBatched() const;
    /// leave batch
    void LeaveBatch();
    /// send the entity to the global space
    void GoToGlobal();
    /// com back from global space
    void ComeBackFromGlobal();
    /// set as global
    void SetGlobal(bool gl);
    /// get if is a global
    bool GetGlobal() const;

#ifndef NGAME
    /// remove temporary from the cell this entity is in
    void RemoveTemporary();
    /// restore this entity from the temporary array of the cell is in
    void RestoreFromTempArray();
#endif // !NGAME

    /// save state of the component
    bool SaveCmds(nPersistServer *ps);

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    enum {
        // extend with your own element types in a subclass
        N_SPATIAL_ELEMENT,   /// common spatial element
        N_SPATIAL_PORTAL ,   /// points into another cell
        N_SPATIAL_LIGHT  ,   /// it's a light
        N_SPATIAL_CAMERA ,   /// it's a camera
        N_SPATIAL_MC,        /// it's a spatial element that can be in multiple cells at the same time
        N_SPATIAL_BATCH,     /// it's a spatial element that contains others
        N_SPATIAL_OCCLUDER,  /// it's a spatial element that occludes others

    };
    typedef int nSpatialComponentType;

    /// get component's axis aligned bounding box
    const bbox3 &GetBBox() const;
    /// set component's axis aligned bounding box
    void SetBBox(const bbox3 &bbox);
    /// set component's axis aligned bounding box
    void SetBBox(const vector3 &vmin, const vector3 &vmax);
    /// set instance's original bbox
    void SetOriginalBBox(const bbox3 &box);
    /// get instance's original bbox
    const bbox3& GetOriginalBBox();
    /// get bounding box's center
    const vector3& GetBBoxCenter() const;
    /// get bounding box's extents
    const vector3& GetBBoxExtents() const;
    /// get the camera's previous position
    const vector3 &GetPreviousPos() const;
    /// set the camera's previous position
    void SetPreviousPos(const vector3 &previousPos);
    /// get the cell the entity is in
    virtual ncSpatialCell *GetCell() const;
    /// set the cell the entity is in
    virtual void SetCell(ncSpatialCell *cell);
    /// get space the entity is in
    virtual ncSpatialSpace *GetSpace();
    /// remove the entity from the spaces
    virtual bool RemoveFromSpaces();
#ifndef NGAME
    /// remove the entity from the spaces without result check
    virtual void DoEnterLimbo();
    /// Update BBOX
    void ClassChanges(nObject* emitter);
#endif // !NGAME
    /// set the octree element pointer
    void SetOctElement(nOctElement *p);
    /// get the octree element pointer
    nOctElement *GetOctElement() const;
    /// set the indoor space pointer (for a facade)
    void SetIndoorSpace(ncSpatialIndoor *indoor);
    /// get the indoor space pointer (for a facade)
    ncSpatialIndoor *GetIndoorSpace() const;
    /// get type
    nSpatialComponentType GetType() const;
    /// set the 'determine space' flag
    void SetDetermineSpaceFlag(DetermineSpaceFlag flag);
    /// get the 'determine space' flag
    DetermineSpaceFlag GetDetermineSpaceFlag() const;
    /// determine space?
    bool DetermineSpace() const ;
    /// set the frame id that the entity was visible for the last time
    void SetFrameId(int id);
    /// get the frame id that the entity was visible for the last time
    int GetFrameId();
    /// set the id of the camera the entity was visible for the last time
    void SetLastCam(nEntityObjectId id);
    /// get the id of the camera the entity was visible for the last time
    nEntityObjectId GetLastCam();
    /// set the entity visible by the given camera
    void SetVisibleBy(nEntityObjectId currentCam);
    /// says if the entity was visible for the current camera in this frame
    bool IsVisibleBy(nEntityObjectId currentCam);
    /// set the entity as linked to the given light in this frame
    void SetLinkedTo(nEntityObjectId currentLight);
    /// says if the entity was linked to the given light in this frame
    bool IsLinkedTo(nEntityObjectId currentLight);
    /// recalculate the light links
    virtual void CalculateLightLinks();
    
    /// get test model
    nSpatialModel *GetTestModel() const;
    /// set test model
    void SetTestModel(nSpatialModel *testModel);

    /// find if there's a shadow generated for the given light id
    nSpatialShadowModel *FindShadowFor(nEntityObjectId lightId);
    /// add a shadow model
    void AddShadowModel(nSpatialShadowModel *model);
    /// remove a shadow model
    void RemoveShadowModel(nSpatialShadowModel *model);
    /// get the shadow models array
    const nArray<nSpatialShadowModel*> *GetShadowModels() const;
    /// remove all the shadow models
    void RemoveShadowModels();
    /// set a shadow (of this caster) visible
    void SetShadowVisible(unsigned short shadowId);
    /// set a shadow (of this caster) non visible
    void SetShadowNonVisible(unsigned short shadowId);
    /// check if a shadow is visible
    bool IsShadowVisible(unsigned short shadowId) const;
    /// says if there is any visible shadow
    bool NoVisibleShadows() const;

    /// update the shadow model for the given light
    nSpatialShadowModel *UpdateShadowModel(ncSpatialLight* light);

    /// get the occluders array (if any)
    const nArray<nEntityObject*> *GetOccluders();

    /// set the batch's spatial component this entity belongs to
    void SetBatch(ncSpatialBatch* batch);
    /// get batch
    ncSpatialBatch* GetBatch();
    
    /// copy the necessary information from another entity
    void operator=(const ncSpatial &sc);

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor, ncSpatialCell *cell);
    
    /// update element's transformation 
    virtual void Update(const matrix44 &worldMatrix);
    
    /// apply a transformation to the original bounding box
    /// and update the current component's bounding box
    virtual void UpdateBBox(const matrix44 &worldMatrix);

    /// set the transform indoor flag
    void SetTransformIndoor(bool flag);
    /// get the transform indoor flag
    bool GetTransformIndoor() const;

    struct VisibleEntity
    {
    public:
        /// constructor 1
        VisibleEntity();
        /// constructor 2
        VisibleEntity(nEntityObject*);
        /// constructor 3
        VisibleEntity(ncSpatial*);
        /// get entity object
        nEntityObject* GetEntity();
        ///// set level of detail
        //void SetCurrentLod(int lodIndex);
        ///// get level of detail
        //int GetCurrentLod();
        /// get if entry is valid
        bool IsValid();
        /// set if entry is valid
        void SetValid(bool isValid);
        /// set visibility flag
        void SetFlag(int flagMask, bool value);
        /// get visibility flag
        bool GetFlag(int flagMask);
        /// get all visibility flags
        int GetFlags();

    private:
        bool isValid;       ///< entry is valid
        int visibleFlags;   ///< combination of ncScene::Flag
        nRef<nEntityObject> refEntity;
    };

    /// set current detail level index
    void SetCurrentLod(int lodIndex);
    /// get current detail level index
    int GetCurrentLod();

    /// set if this entity is associated to a portal
    void SetAssociatedToPortal(bool flag);
    /// get if this entity is associated to a portal
    bool IsAssociatedToPortal() const;

protected:

    /// spatial component's type
    nSpatialComponentType m_spatialComponentType;
    /// spatial category
    int spatialCat;
    /// previous position.
    vector3 previousPos;
    /// entity's bounding box
    bbox3 m_bbox;
    /// original entity's bounding box
    bbox3 m_originalBBox;
    /// bbox center
    vector3 bboxCenter;
    /// bbox extents
    vector3 bboxExtents;

    /// true if it uses the class' original bounding box to calculate the instance's one
    bool useOriginalBBox;

    /// cell where this entity is in
    ncSpatialCell *m_cell;
    /// cell id this entity is in (used only when loading the entity)
    int m_cellId;
    /// space entity id this entity is in (used only when loading the entity)
    nEntityObjectId m_spaceId;

    /// flag to know if the space where is the entity has to be determined when the entity is moved
    DetermineSpaceFlag m_determineSpace;

    /// test model
    nSpatialModel *m_testModel;
    /// shadow models
    nArray<nSpatialShadowModel*> *shadowModels;
    /// true if it's a shadow caster
    bool m_shadowCaster;
    /// visible shadows
    unsigned short visibleShadows;

    /// batch entity (batch this entity belongs to)
    nRef<nEntityObject> refBatch;
    ncSpatialBatch* m_batch;

    /// occluders associated with this entity (it can be NULL, because the entity has no occluders)
    nArray<nEntityObject*> *m_occluders;

    /// only used by the octree spaces
    nOctElement *m_octElement;
    /// only used by a facade
    ncSpatialIndoor *m_indoorSpace;
    
    /// transform indoor flag
    bool m_transformIndoor;

    /// temporarily selected LOD- it is copied into VisibleEntity
    int currentLod;

    /// says if this entity is associated to a portal
    bool associatedToPortal;

    /// id of the frame that the element was visible for the current camera
    int m_frameId;
    /// id of the last camera that the element was visible for
    nEntityObjectId m_lastCam;

    /// id of the frame that the current light was linked to this entity
    int m_linkLightFrameId;
    /// id of the last light that was linked to this entity
    nEntityObjectId m_lastLinkLight;

};

//-----------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(ncSpatial::DetermineSpaceFlag, "i", (value = (ncSpatial::DetermineSpaceFlag) cmd->In()->GetI()), (cmd->Out()->SetI( int(value) ))  );

//-----------------------------------------------------------------------------
/**
*/
inline
ncSpatial::VisibleEntity::VisibleEntity() :
    isValid(true)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
inline
ncSpatial::VisibleEntity::VisibleEntity(nEntityObject* entity) :
    isValid(true)
{
    this->refEntity = entity;
}

//-----------------------------------------------------------------------------
/**
*/
inline
ncSpatial::VisibleEntity::VisibleEntity(ncSpatial* spatialComp) :
    isValid(true)
{
    this->refEntity = spatialComp->GetEntityObject();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
ncSpatial::VisibleEntity::SetValid(bool isValid)
{
    this->isValid = isValid;
    if (!isValid)
    {
        this->visibleFlags = 0;
    }
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
ncSpatial::VisibleEntity::SetFlag(int flagMask, bool value)
{
    if (value)
    {
        this->visibleFlags |= flagMask;
    }
    else
    {
        this->visibleFlags &= ~flagMask;
    }
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
ncSpatial::VisibleEntity::GetFlag(int flagMask)
{
    return (this->visibleFlags & flagMask) != 0;
}

//-----------------------------------------------------------------------------
/**
*/
inline
int
ncSpatial::VisibleEntity::GetFlags()
{
    return this->visibleFlags;
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
ncSpatial::VisibleEntity::IsValid()
{
    return this->refEntity.isvalid() && this->isValid;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
ncSpatial::VisibleEntity::GetEntity()
{
    return this->refEntity.isvalid() ? this->refEntity.get() : 0;
}

//-----------------------------------------------------------------------------
/**
    set the spatial category
*/
inline
void 
ncSpatial::SetSpatialCategory(int cat)
{
    this->spatialCat = cat;
}

//-----------------------------------------------------------------------------
/**
    get the spatial category
*/
inline
int 
ncSpatial::GetSpatialCategory() const
{
    return this->spatialCat;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
ncSpatial::SetCurrentLod(int lodIndex)
{
    this->currentLod = lodIndex;
}

//-----------------------------------------------------------------------------
/**
*/
inline
int
ncSpatial::GetCurrentLod()
{
    return this->currentLod;
}

//------------------------------------------------------------------------------
/**
    get the previous position
*/
inline
const vector3 &
ncSpatial::GetPreviousPos() const
{
    return this->previousPos;
}

//------------------------------------------------------------------------------
/**
    set the previous position
*/
inline
void 
ncSpatial::SetPreviousPos(const vector3 &previousPos)
{
    this->previousPos = previousPos;
}

//------------------------------------------------------------------------------
/**
    get entity's axis aligned bounding box in space coordinates
*/
inline
const bbox3 &
ncSpatial::GetBBox() const
{
    return this->m_bbox;
}

//------------------------------------------------------------------------------
/**
    set entity's axis aligned bounding box in space coordinates
*/
inline
void 
ncSpatial::SetBBox(const bbox3 &bbox)
{
    this->m_bbox = bbox;
    this->bboxCenter = bbox.center();
    this->bboxExtents = bbox.extents();
}

//------------------------------------------------------------------------------
/**
    set component's axis aligned bounding box
*/
inline
void 
ncSpatial::SetBBox(const vector3 &vmin, const vector3 &vmax)
{
    this->m_bbox.vmin = vmin;
    this->m_bbox.vmax = vmax;

    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
    set the cell id the entity is in (just while loading)
*/
inline
void 
ncSpatial::SetCellId(int cellId)
{
    this->m_cellId = cellId;
}

//------------------------------------------------------------------------------
/**
    set the space entity id this entity is in
*/
inline
void 
ncSpatial::SetSpaceId(nEntityObjectId spaceId)
{
    this->m_spaceId = spaceId;
}

//------------------------------------------------------------------------------
/**
    get test model
*/
inline
nSpatialModel*
ncSpatial::GetTestModel() const
{
    return this->m_testModel;
}

//------------------------------------------------------------------------------
/**
    get the cell the entity is in
*/
inline
ncSpatialCell*
ncSpatial::GetCell() const
{
    return this->m_cell;
}

//------------------------------------------------------------------------------
/**
    set the cell the entity is in
*/
inline
void
ncSpatial::SetCell(ncSpatialCell *cell)
{
    this->m_cell = cell;
}

//------------------------------------------------------------------------------
/**
    set the octree element pointer
*/
inline
void 
ncSpatial::SetOctElement(nOctElement *p)
{
    this->m_octElement = p;
}

//------------------------------------------------------------------------------
/**
    get the octree element pointer
*/
inline
nOctElement *
ncSpatial::GetOctElement() const
{
    return this->m_octElement;
}

//------------------------------------------------------------------------------
/**
    set the indoor space pointer (for a facade)
*/
inline
void 
ncSpatial::SetIndoorSpace(ncSpatialIndoor *indoor)
{
    this->m_indoorSpace = indoor;
}

//------------------------------------------------------------------------------
/**
    get the indoor space pointer (for a facade)
*/
inline
ncSpatialIndoor *
ncSpatial::GetIndoorSpace() const
{
    return this->m_indoorSpace;
}

//------------------------------------------------------------------------------
/**
    get spatial component's type
*/
inline
ncSpatial::nSpatialComponentType 
ncSpatial::GetType() const
{
    return this->m_spatialComponentType;
}
//------------------------------------------------------------------------------
/**
    set the 'determine space' flag
*/
inline
void 
ncSpatial::SetDetermineSpaceFlag(DetermineSpaceFlag flag)
{
    this->m_determineSpace = flag;
}

//------------------------------------------------------------------------------
/**
    get the 'determine space' flag
*/
inline
ncSpatial::DetermineSpaceFlag
ncSpatial::GetDetermineSpaceFlag() const
{
    return this->m_determineSpace;
}

//------------------------------------------------------------------------------
/**
    determine space?
*/
inline
bool 
ncSpatial::DetermineSpace() const
{
    return this->m_determineSpace == ncSpatial::DS_TRUE;
}

//------------------------------------------------------------------------------
/**
    set the bounding box given the center and extents
*/
inline
void
ncSpatial::SetBBox(float vcx, float vcy, float vcz, float vex, float vey, float vez)
{
    bbox3 bbox(vector3(vcx, vcy, vcz), vector3(vex, vey, vez));
    
    this->m_bbox = bbox;
    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
    returns the center and the extents of the bounding box
*/
inline
void
ncSpatial::GetBBox(vector3& v0, vector3& v1)
{
    bbox3 bbox = this->m_bbox;
    v0 = bbox.center();
    v1 = bbox.extents();
}

//------------------------------------------------------------------------------
/**
    set instance's original bbox
*/
inline
void 
ncSpatial::SetOriginalBBox(const bbox3 &box)
{
    this->m_originalBBox = box;
    this->useOriginalBBox = true;
}

//------------------------------------------------------------------------------
/**
    set instance's original bbox
*/
inline
void 
ncSpatial::SetOriginalBBox(float vcx, float vcy, float vcz, float vex, float vey, float vez)
{
    bbox3 bbox(vector3(vcx, vcy, vcz), vector3(vex, vey, vez));
    
    this->m_originalBBox = bbox;
    this->useOriginalBBox = true;
}

//------------------------------------------------------------------------------
/**
    get bounding box's center
*/
inline
const vector3& 
ncSpatial::GetBBoxCenter() const
{
    return this->bboxCenter;
}

//------------------------------------------------------------------------------
/**
    get bounding box's extents
*/
inline
const vector3& 
ncSpatial::GetBBoxExtents() const
{
    return this->bboxExtents;
}

//------------------------------------------------------------------------------
/**
    get the frame id that the entity was visible for the last time
*/
inline
int
ncSpatial::GetFrameId()
{
    return this->m_frameId;
}

//------------------------------------------------------------------------------
/**
    set the id of the camera the entity was visible for the last time
*/
inline
void 
ncSpatial::SetLastCam(nEntityObjectId id)
{
    this->m_lastCam = id;
}

//------------------------------------------------------------------------------
/**
    get the id of the camera the entity was visible for the last time
*/
inline
nEntityObjectId 
ncSpatial::GetLastCam()
{
    return this->m_lastCam;
}

//------------------------------------------------------------------------------
/**
    set the transform indoor flag
*/
inline
void 
ncSpatial::SetTransformIndoor(bool flag)
{
    this->m_transformIndoor = flag;
}

//------------------------------------------------------------------------------
/**
    get the transform indoor flag
*/
inline
bool 
ncSpatial::GetTransformIndoor() const
{
    return this->m_transformIndoor;
}

//------------------------------------------------------------------------------
/**
    is shadow caster?
*/
inline
bool 
ncSpatial::GetIsShadowCaster() const
{
    return this->m_shadowCaster;
}

//------------------------------------------------------------------------------
/**
    get the occluders array (if any)
*/
inline
const nArray<nEntityObject*> *
ncSpatial::GetOccluders()
{
    return this->m_occluders;
}

//------------------------------------------------------------------------------
/**
    is batched?
*/
inline
bool 
ncSpatial::IsBatched() const
{
    return this->refBatch.isvalid();
}

//------------------------------------------------------------------------------
/**
    get batch
*/
inline
ncSpatialBatch* 
ncSpatial::GetBatch()
{
    return this->refBatch.isvalid() ? this->m_batch : 0;
}

//------------------------------------------------------------------------------
/**
    set if this entity is associated to a portal
*/
inline
void 
ncSpatial::SetAssociatedToPortal(bool flag)
{
    this->associatedToPortal = flag;
}

//------------------------------------------------------------------------------
/**
    get if this entity is associated to a portal
*/
inline
bool 
ncSpatial::IsAssociatedToPortal() const
{
    return this->associatedToPortal;
}

//------------------------------------------------------------------------------
/**
    set a shadow (of this caster) visible
*/
inline
void 
ncSpatial::SetShadowVisible(unsigned short shadowId)
{
    this->visibleShadows |= shadowId;
}

//------------------------------------------------------------------------------
/**
    set a shadow (of this caster) non visible
*/
inline
void 
ncSpatial::SetShadowNonVisible(unsigned short shadowId)
{
    this->visibleShadows &= ~shadowId;
}

//------------------------------------------------------------------------------
/**
    check if a shadow is visible
*/
inline
bool 
ncSpatial::IsShadowVisible(unsigned short shadowId) const
{
    return (this->visibleShadows & shadowId) != 0;
}

//------------------------------------------------------------------------------
/**
    says if there is any visible shadow
*/
inline
bool 
ncSpatial::NoVisibleShadows() const
{
    return this->visibleShadows == 0;
}

//------------------------------------------------------------------------------
#endif // NC_SPATIAL_H
