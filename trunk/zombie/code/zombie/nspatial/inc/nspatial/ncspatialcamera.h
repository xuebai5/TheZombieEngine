#ifndef NC_SPATIALCAMERA_H
#define NC_SPATIALCAMERA_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialCamera
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial camera component for camera entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/ncspatial.h"
#include "nspatial/nspatialcameracollector.h"
#include "nspatial/nspatialserver.h"
#include "gfx2/ncamera2.h"

#ifndef NGAME
#include "nspatial/nhorizonclipper.h"
#endif // !NGAME

class ncSpatialOccluder;
class nSpatialIndoorCell;
class ncSpatialQuadtreeCell;
class nFrustumClipper;

//------------------------------------------------------------------------------
class ncSpatialCamera : public ncSpatial
{

    NCOMPONENT_DECLARE(ncSpatialCamera, ncSpatial);

public:

    /// constructor
    ncSpatialCamera();
    /// destructor
    virtual ~ncSpatialCamera();

    struct SelectedOccluder
    {
        /// area-angle test result for this occluder
        float m;
        
        /// occluder
        ncSpatialOccluder *oc;
    };

    /// set the max outdoor entities test distance
    void SetMaxTestOutdoorDist(float);
    /// get the max outdoor entities test distance
    float GetMaxTestOutdoorDist() const;
    /// set the radius of selection of occluders
    void SetOcclusionRadius(float);
    /// get the radius of selection of occluders
    float GetOcclusionRadius() const;
    /// set the maximum number of occluders to take into account for occlusion
    void SetMaxNumOccluders(int);
    /// get the maximum number of occluders to take into account for occlusion
    int GetMaxNumOccluders() const;
    /// set the minimum value of the area-angle test to take into account an occluder
    void SetAreaAngleMin(float);
    /// get the minimum value of the area-angle test to take into account an occluder
    float GetAreaAngleMin() const;
    /// set use of camera collector
    void SetUseCameraCollector(bool);
    /// get use of camera collector
    bool GetUseCameraCollector() const;
#ifndef NGAME
    /// Set the attach global entities flag
    void SetAttachGlobalEntities(bool);
    /// Set the attach global entities flag
    bool GetAttachGlobalEntities() const;
#endif // !NGAME

#ifndef NGAME
#endif // !NGAME

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// set the camera
    void SetCamera(const nCamera2 &camera);
    /// get the camera
    const nCamera2 &GetCamera() const;
    /// get camera's view matrix
    const matrix44 &GetViewMatrix() const;
    /// set camera's view matrix
    void SetViewMatrix(const matrix44 &matrix);
    /// get camera's width
    int GetWidth() const;
    /// get camera's height
    int GetHeight() const;
    /// get the scaling parameter
    float GetXScale();

    /// get the max outdoor entities squared test distance
    float GetMaxTestOutdoorSqDist() const;
    /// set the near plane's distance
    void SetNearPlaneDist(const float dist);
    /// get the near plane's distance
    float GetNearPlaneDist();

    /// reset the camera
    void Reset();
    /// append a visible entity to this camera
    void AppendVisibleEntity(nEntityObject *entity);
    /// reset the array of visible entities
    void ResetVisibleEntitiesArray();
    /// get the array of visible entities from this camera
    const nArray<nRef<nEntityObject> >& GetVisibleEntities();
    /// get number of visible entities
    int GetNumVisibleObjects();
    /// get number of visible terrain cells
    int GetNumVisibleTerrainCells();

    /// append a visible light to this camera
    void AppendVisibleLight(nEntityObject *light);
    /// reset the array of visible lights
    void ResetVisibleLightsArray();
    /// get the array of visible lights from this camera
    const nArray<ncSpatialLight*>& GetVisibleLights() const;
    /// add a visible portal
    void AddVisiblePortal(ncSpatialPortal *portal);
    /// get the array of visible portals from this camera
    const nArray<ncSpatialPortal*> &GetVisiblePortals() const;
    /// reset the array of visible portals
    void ResetPortalsArray();
    /// add a visible indoor cell
    void AddVisibleIndoorCell(nSpatialIndoorCell *indoorCell);
    /// get the array of visible indoor cells
    const nArray<nSpatialIndoorCell*> &GetVisibleIndoorCells() const;
    /// reset the array of visible indoor cells
    void ResetIndoorCellsArray();
    /// add a visible quadtree cell
    void AddVisibleQuadtreeCell(ncSpatialQuadtreeCell *quadtreeCell);
    /// get the array of visible quadtree cells
    const nArray<ncSpatialQuadtreeCell*> &GetVisibleQuadtreeCells() const;
    /// reset the array of visible quadtree cells
    void ResetQuadtreeCellsArray();
    /// get the array of selected occluders for this camera
    const nArray<ncSpatialCamera::SelectedOccluder*> &GetSelectedOccluders() const;
    /// reset the array of selected occluders
    void ResetOccludersArray();
    /// build occlusion frustums using the selected occluders
    void BuildOcclusionFrustums();
    /// get the array of occlusion frustums for this camera
    const nArray<nFrustumClipper*> &GetOcclusionFrustums() const;
    /// reset the array of occlusion frustums
    void ResetOcclusionFrustums();

#ifndef NGAME
    /// get the array of discarded occluders for this camera
    const nArray<ncSpatialCamera::SelectedOccluder*> &GetDiscardedOccluders() const;
    /// reset the array of discarded occluders
    void ResetDiscardedOccludersArray();
    /// says if the camera is drawing the horizon
    bool IsDebuggingHorizon() const;
    /// put the debugging horizon flag
    void SetDebuggingHorizon(bool flag);
#endif // !NGAME

    /// make the area-angle test to an occluder to know if it's a good occluder,
    /// and if it is, store it
    bool ConsiderOccluder(ncSpatialOccluder *occluder);
    
#ifndef NGAME
    /// set the current horizon
    void SetHorizon(const nHorizonClipper &horizon);
    /// get the current horizon
    const nHorizonClipper &GetHorizon() const;
#endif // !NGAME

    /// get the camera's collector
    nSpatialCameraCollector *GetCollector();
    /// turn on the camera
    void TurnOn();
    /// turn off the camera
    void TurnOff();
    /// says whether the camera is on or not
    bool IsOn();
    
    /// update camera's view matrix and position
    void Update(const matrix44 &worldMatrix, int frameId);
    /// update camera's projection
    void Update(const nCamera2 &camera);

    /// get the view-projection matrix
    const matrix44& GetViewProjection();

    /// accept a visibility visitor
    virtual void Accept(nVisibleFrustumVisitor &visitor);
    /// accept a spatial visitor
    virtual void Accept(nSpatialVisitor &visitor);

protected:

    /// camera's information (i.e. projection)
    nCamera2 m_camera;
    /// view matrix in space coordinates
    matrix44 m_viewMatrix;
    /// view projection matrix
    matrix44 m_viewProjection;
    /// true if the view projection matrix is not updated
    bool m_vpDirty;
    /// camera's width
    int m_width;
    /// camera's height
    int m_height;
    /// near plane distance
    float m_nearPlaneDist;
    /// distance beyond which terrain cells' entities are not tested (squared for fast queries)
    float testEntitiesMaxSqDist;
    /// spatial camera's collector
    nSpatialCameraCollector *m_collector;
    /// camera state (on/off)
    bool m_isOn;
    /// all the occluders inside this radius around the camera will be taken into
    /// account to do occlusion tests
    float m_occlusionRadius;
    /// area-angle minimum (all the current considered occluders's 'm' have to be greater of equal than this)
    float m_areaAngleMin;
    /// maximum number of occluders to take into account for a occlusion query
    int m_maxNumOccluders;
    /// use camera's collector?
    bool useCameraCollector;

#ifndef NGAME
    bool attachGlobalEntities;
    bool debuggingHorizon;
#endif // !NGAME

    /// visible portals from this camera
    nArray<ncSpatialPortal*> m_visiblePortals;
    /// visible indoor cells from this camera
    nArray<nSpatialIndoorCell*> m_visibleIndoorCells;
    /// visible quadtree cells from this camera
    nArray<ncSpatialQuadtreeCell*> m_visibleQuadtreeCells;
    /// current selected occluders to determine occlusion for this camera
    nArray<SelectedOccluder*> m_selectedOccluders;
    /// current selected occluders' frustums
    nArray<nFrustumClipper*> m_occlusionFrustums;

    /// Builds a new frustum using the occluder's vertices. 
    nFrustumClipper *BuildNewFrustum(ncSpatialOccluder *occluder);

#ifndef NGAME
    /// array of discarded occluders
    nArray<SelectedOccluder*> m_discardedOccluders;
    /// current horizon
    nHorizonClipper horizon;
#endif // !NGAME

    /// update the view projection matrix
    void UpdateViewProjection();

};

//------------------------------------------------------------------------------
/**
    destructor
*/
inline
ncSpatialCamera::~ncSpatialCamera()
{
    n_delete(this->m_collector);
    this->ResetOccludersArray();
    this->ResetOcclusionFrustums();
#ifndef NGAME
    this->ResetDiscardedOccludersArray();
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    set the camera
*/
inline
void 
ncSpatialCamera::SetCamera(const nCamera2 &camera)
{
    this->m_camera = camera;
}

//------------------------------------------------------------------------------
/**
    get the camera
*/
inline
const nCamera2 &
ncSpatialCamera::GetCamera() const
{
    return this->m_camera;
}

//------------------------------------------------------------------------------
/**
    get the camera's collector
*/
inline
nSpatialCameraCollector *
ncSpatialCamera::GetCollector()
{
    return this->m_collector;
}

//------------------------------------------------------------------------------
/**
    get camera's view matrix
*/
inline
const matrix44 &
ncSpatialCamera::GetViewMatrix() const
{
    return this->m_viewMatrix;
}

//------------------------------------------------------------------------------
/**
    set camera's view matrix
*/
inline
void 
ncSpatialCamera::SetViewMatrix(const matrix44 &matrix)
{
    this->m_viewMatrix = matrix;
}

//------------------------------------------------------------------------------
/**
    set the near plane's distance
*/
inline
void 
ncSpatialCamera::SetNearPlaneDist(const float dist)
{
    this->m_nearPlaneDist = dist;
}

//------------------------------------------------------------------------------
/**
    get the near plane's distance
*/
inline
float 
ncSpatialCamera::GetNearPlaneDist()
{
    return this->m_nearPlaneDist;
}

//------------------------------------------------------------------------------
/**
    reset the camera
*/
inline
void 
ncSpatialCamera::Reset()
{
    // reset the visible elements array
    this->m_collector->Reset();

    // reset the portals debug information arrays
    this->ResetPortalsArray();
    this->ResetIndoorCellsArray();
    this->ResetQuadtreeCellsArray();
}

//------------------------------------------------------------------------------
/**
    append a visible entity to this camera
*/
inline
void
ncSpatialCamera::AppendVisibleEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");
    this->m_collector->Accept(entity);
}

//------------------------------------------------------------------------------
/**
    reset the array of visible entities
*/
inline
void 
ncSpatialCamera::ResetVisibleEntitiesArray()
{
    this->m_collector->ResetContainer();
}

//------------------------------------------------------------------------------
/**
    get the array of visible entities from this camera
*/
inline
const nArray<nRef<nEntityObject> >&
ncSpatialCamera::GetVisibleEntities()
{
    return this->m_collector->GetVisibleEntities();
}

//------------------------------------------------------------------------------
/**
    get the number of visible entities for this camera
*/
inline
int
ncSpatialCamera::GetNumVisibleObjects()
{
    return this->m_collector->GetNumVisibleObjects();
}

//------------------------------------------------------------------------------
/**
    get the number of visible terrain cells for this camera
*/
inline
int
ncSpatialCamera::GetNumVisibleTerrainCells()
{
    return this->m_collector->GetNumVisibleTerrainCells();
}

//------------------------------------------------------------------------------
/**
    reset the array of visible lights
*/
inline
void 
ncSpatialCamera::ResetVisibleLightsArray()
{
    this->m_collector->ResetLightsContainer();
}

//------------------------------------------------------------------------------
/**
    get the array of visible lights from this camera
*/
inline
const nArray<ncSpatialLight*>&
ncSpatialCamera::GetVisibleLights() const
{
    return this->m_collector->GetVisibleLights();
}

//------------------------------------------------------------------------------
/**
    add a visible portal
*/
inline
void 
ncSpatialCamera::AddVisiblePortal(ncSpatialPortal *portal)
{
    this->m_visiblePortals.Append(portal);
}

//------------------------------------------------------------------------------
/**
    get the array of visible portals from this camera
*/
inline
const nArray<ncSpatialPortal*> &
ncSpatialCamera::GetVisiblePortals() const
{
    return this->m_visiblePortals;
}

//------------------------------------------------------------------------------
/**
    reset the array of visible portals
*/
inline
void 
ncSpatialCamera::ResetPortalsArray()
{
    this->m_visiblePortals.Reset();
}

//------------------------------------------------------------------------------
/**
    add a visible indoor cell
*/
inline
void 
ncSpatialCamera::AddVisibleIndoorCell(nSpatialIndoorCell *indoorCell)
{
    this->m_visibleIndoorCells.Append(indoorCell);
}

//------------------------------------------------------------------------------
/**
    get the array of visible indoor cells
*/
inline
const nArray<nSpatialIndoorCell*> &
ncSpatialCamera::GetVisibleIndoorCells() const
{
    return this->m_visibleIndoorCells;
}

//------------------------------------------------------------------------------
/**
    reset the array of visible indoor cells
*/
inline
void 
ncSpatialCamera::ResetIndoorCellsArray()
{
    this->m_visibleIndoorCells.Reset();
}

//------------------------------------------------------------------------------
/**
    add a visible quadtree cell
*/
inline
void 
ncSpatialCamera::AddVisibleQuadtreeCell(ncSpatialQuadtreeCell *quadtreeCell)
{
    this->m_visibleQuadtreeCells.Append(quadtreeCell);
}

//------------------------------------------------------------------------------
/**
    get the array of visible quadtree cells
*/
inline
const nArray<ncSpatialQuadtreeCell*> &
ncSpatialCamera::GetVisibleQuadtreeCells() const
{
    return this->m_visibleQuadtreeCells;
}

//------------------------------------------------------------------------------
/**
    reset the array of visible quadtree cells
*/
inline
void 
ncSpatialCamera::ResetQuadtreeCellsArray()
{
    this->m_visibleQuadtreeCells.Reset();
}

//------------------------------------------------------------------------------
/**
    get the array of selected occluders for this camera
*/
inline
const nArray<ncSpatialCamera::SelectedOccluder*> &
ncSpatialCamera::GetSelectedOccluders() const
{
    return this->m_selectedOccluders;
}

//------------------------------------------------------------------------------
/**
    reset the array of selected occluders
*/
inline
void 
ncSpatialCamera::ResetOccludersArray()
{
    for (int i = 0; i < this->m_selectedOccluders.Size(); i++)
    {
        n_delete(this->m_selectedOccluders.At(i));
    }

    this->m_selectedOccluders.Reset();
}

//------------------------------------------------------------------------------
/**
    get the array of occlusion frustums for this camera
*/
inline
const nArray<nFrustumClipper*> &
ncSpatialCamera::GetOcclusionFrustums() const
{
    return this->m_occlusionFrustums;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    get the array of discarded occluders for this camera
*/
inline
const nArray<ncSpatialCamera::SelectedOccluder*> &
ncSpatialCamera::GetDiscardedOccluders() const
{
    return this->m_discardedOccluders;
}

//------------------------------------------------------------------------------
/**
    reset the array of discarded occluders
*/
inline
void 
ncSpatialCamera::ResetDiscardedOccludersArray()
{
    for (int i = 0; i < this->m_discardedOccluders.Size(); i++)
    {
        n_delete(this->m_discardedOccluders.At(i));
    }

    this->m_discardedOccluders.Reset();
}

//------------------------------------------------------------------------------
/**
    says if the camera is drawing the horizon
*/
inline
bool 
ncSpatialCamera::IsDebuggingHorizon() const
{
    return this->debuggingHorizon;
}

//------------------------------------------------------------------------------
/**
    put the debugging horizon flag
*/
inline
void 
ncSpatialCamera::SetDebuggingHorizon(bool flag)
{
    this->debuggingHorizon = flag;
}

//------------------------------------------------------------------------------
/**
    set the current horizon
*/
inline
void 
ncSpatialCamera::SetHorizon(const nHorizonClipper &horizon)
{
    this->horizon = horizon;
}

//------------------------------------------------------------------------------
/**
    get the current horizon
*/
inline
const nHorizonClipper &
ncSpatialCamera::GetHorizon() const
{
    return this->horizon;
}

#endif // !NGAME

//------------------------------------------------------------------------------
/**
    turn on the camera
*/
inline
void
ncSpatialCamera::TurnOn()
{
    this->m_isOn = true;
    if (!this->m_cell)
    {
        nSpatialServer::Instance()->InsertEntity(this->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
    turn off the camera
*/
inline
void
ncSpatialCamera::TurnOff()
{
    this->m_isOn = false;
}

//------------------------------------------------------------------------------
/**
    says whether the camera is on or not
*/
inline
bool 
ncSpatialCamera::IsOn()
{
    return this->m_isOn;
}

//------------------------------------------------------------------------------
/**
    set the max outdoor entities test distance
*/
inline
void 
ncSpatialCamera::SetMaxTestOutdoorDist(float dist)
{
    this->testEntitiesMaxSqDist = dist * dist;
}

//------------------------------------------------------------------------------
/**
    get the max outdoor entities test distance
*/
inline
float 
ncSpatialCamera::GetMaxTestOutdoorDist() const
{
    return n_sqrt(this->testEntitiesMaxSqDist);
}

//------------------------------------------------------------------------------
/**
    get the max outdoor entities squared test distance
*/
inline
float 
ncSpatialCamera::GetMaxTestOutdoorSqDist() const
{
    return this->testEntitiesMaxSqDist;
}

//------------------------------------------------------------------------------
/**
    set the radius of selection of occluders
*/
inline
void 
ncSpatialCamera::SetOcclusionRadius(float radius)
{
    this->m_occlusionRadius = radius;
}

//------------------------------------------------------------------------------
/**
    get the radius of selection of occluders
*/
inline
float
ncSpatialCamera::GetOcclusionRadius() const
{
    return this->m_occlusionRadius;
}

//------------------------------------------------------------------------------
/**
    set the maximum number of occluders to take into account for occlusion
*/
inline 
void
ncSpatialCamera::SetMaxNumOccluders(int num)
{
    this->m_maxNumOccluders = num;
}

//------------------------------------------------------------------------------
/**
    get the maximum number of occluders to take into account for occlusion
*/
inline
int
ncSpatialCamera::GetMaxNumOccluders() const
{
    return this->m_maxNumOccluders;
}

//------------------------------------------------------------------------------
/**
    set the minimum value of the area-angle test to take into account an occluder
*/
inline
void 
ncSpatialCamera::SetAreaAngleMin(float min)
{
    this->m_areaAngleMin = min;
}

//------------------------------------------------------------------------------
/**
    get the minimum value of the area-angle test to take into account an occluder
*/
inline
float 
ncSpatialCamera::GetAreaAngleMin() const
{
    return this->m_areaAngleMin;
}

//------------------------------------------------------------------------------
/**
    get the view-projection matrix
*/
inline
const matrix44& 
ncSpatialCamera::GetViewProjection()
{
    if ( this->m_vpDirty )
    {
        this->UpdateViewProjection();
        this->m_vpDirty = false;
    }

    return this->m_viewProjection;
}

//------------------------------------------------------------------------------
/**
    get camera's width
*/
inline
int 
ncSpatialCamera::GetWidth() const
{
    return this->m_width;
}

//------------------------------------------------------------------------------
/**
    get camera's height
*/
inline
int 
ncSpatialCamera::GetHeight() const
{
    return this->m_height;
}

//------------------------------------------------------------------------------
/**
    get camera scaling parameter
*/
inline
float
ncSpatialCamera::GetXScale()
{
    return this->m_camera.GetProjection().M11;
}

//------------------------------------------------------------------------------
/**
    set use of camera collector
*/
inline
void 
ncSpatialCamera::SetUseCameraCollector(bool flag)
{
    this->useCameraCollector = flag;
}

//------------------------------------------------------------------------------
/**
    get use of camera collector
*/
inline
bool 
ncSpatialCamera::GetUseCameraCollector() const
{
    return this->useCameraCollector;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Set the attach global entities flag
*/
inline
void 
ncSpatialCamera::SetAttachGlobalEntities(bool flag)
{
    this->attachGlobalEntities = flag;
}

//------------------------------------------------------------------------------
/**
    Set the attach global entities flag
*/
inline
bool 
ncSpatialCamera::GetAttachGlobalEntities() const
{
    return this->attachGlobalEntities;
}
#endif // !NGAME

//------------------------------------------------------------------------------
#endif  // NC_SPATIALCAMERA_H

