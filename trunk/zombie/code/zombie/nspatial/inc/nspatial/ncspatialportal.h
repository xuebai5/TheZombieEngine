#ifndef NC_SPATIALPORTAL_H
#define NC_SPATIALPORTAL_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialPortal
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial portal component for portal entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/ncspatial.h"
#include "mathlib/plane.h"
#include "nspatial/nspatialserver.h"
#include "mathlib/obbox.h"

//------------------------------------------------------------------------------
class ncSpatialPortal : public ncSpatial
{

    NCOMPONENT_DECLARE(ncSpatialPortal, ncSpatial);

public:

    /// constructor
    ncSpatialPortal();
    /// destructor
    virtual ~ncSpatialPortal();

    /// set the other side cell id
    void SetOtherSideCellId(int);
    /// set a clip rectangle's vertex
    void SetClipVertex(int, const vector3&);
    /// set the portal's original plane
    void SetOriginalPlane(float, float, float, float);
    /// set the original oriented bounding box's size
    void SetOrigOBBoxSize(const vector3&);
    /// set the original oriented bounding box's scale
    void SetOrigOBBoxScale(const vector3&);
    /// set the original oriented bounding box's euler
    void SetOrigOBBoxEuler(const vector3&);
    /// set the original oriented bounding box's translation
    void SetOrigOBBoxTranslation(const vector3&);
    /// activate the portal
    void Activate();
    /// deactivate the portal
    void Deactivate();
    /// set the active flag
    void SetActive(bool);
    /// set the active flag for this portal and its twin one (if exists)
    void SetActiveAndTwin(bool);
    /// is the portal active?
    bool IsActive() const;
    /// set the distance the portal is deactivated
    void SetDeactivationDistance(float);
    /// get the distance the portal is deactivated
    float GetDeactivationDistance() const;
    /// get the squared distance the portal is deactivated
    float GetSqDeactivationDistance() const;

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// set the portal's vertices
    void SetVertices(const vector3 portalVertices[4]);
    /// set the portal's original vertices
    void SetOriginalVertices(const vector3 portalVertices[4]);
    /// get the portal's vertices
    const vector3 *GetVertices() const;
    /// get the clip rectangle's center
    const vector3 &GetClipCenter() const;
    /// get the portal's original vertices
    const vector3 *GetOriginalVertices() const;
    /// set the portal's plane
    void SetPlane(const plane &pl);
    /// set the portal's original plane
    void SetOriginalPlane(const plane &pl);
    /// get the portal's plane
    const plane &GetPlane() const;
    /// get the portal's original plane
    const plane &GetOriginalPlane() const;
    /// calculates the projected bounding rectangle of the portal
    rectangle GetCliprect(const matrix44 &viewProjection, const rectangle &rect) const;
    /// check for the intersection of the clip rectangle with a view volume
    bbox3::ClipStatus ClipStatus(const matrix44 &viewProjection) const;
    /// same as previous 'ClipStatus' but also using a rectangle to make the test
    bbox3::ClipStatus ClipStatus(const matrix44& viewProjection, const rectangle& rect, bool useRect) const;
    /// get the other side cell
    ncSpatialCell *GetOtherSideCell() const;
    /// get the other side cell's id
    int GetOtherSideCellId() const;
    /// set frame id
    void SetVisitFrameId(int id);
    /// get frame id
    int GetVisitFrameId() const;
    /// set the original oriented bounding box
    void SetOriginalOBBox(obbox3 &box);
    /// get the original oriented bounding box
    obbox3 &GetOriginalOBBox();
    /// set the oriented bounding box
    void SetOBBox(obbox3 &box);
    /// get the oriented bounding box
    obbox3 &GetOBBox();
    /// determines if a given portal is this one's twin
    bool IsTwin(ncSpatialPortal *portal);
    /// set twin portal
    void SetTwinPortal(ncSpatialPortal *twinPortal);
    /// get twin portal
    ncSpatialPortal *GetTwinPortal();
    /// says if this portal touches another one
    bool Touches(ncSpatialPortal *portal);
    /// connect two portals
    bool ConnectTo(ncSpatialPortal *portal);
    /// copy the necessary information from another portal
    void operator=(const ncSpatialPortal &portal);
    /// set the portal as visited by the given light and the given camera
    void SetLightVisited(nEntityObjectId currentLight, nEntityObjectId currentCam);
    /// return true if this portal has been visited in this frame by the a light
    bool IsLightVisited(nEntityObjectId currentLight, nEntityObjectId currentCam) const;

    /// associate a brush to the portal
    void AddBrush(nEntityObject *brush);
    /// remove a brush that was associated to this portal
    bool RemoveBrush(nEntityObject *brush);
    /// get the array of brushes associated to this portal
    const nArray<nEntityObject*> *GetBrushes() const;

    /// update element's position 
    virtual void Update(const matrix44 &worldMatrix);
    
    /// apply a transformation in world coordinates to the original bounding box
    /// and update the current component's bounding box
    virtual void UpdateBBox(const matrix44 &worldMatrix);
    /// apply a transformation in world coordinates to the original oriented bounding box
    /// and update the current component's oriented bounding box
    virtual void UpdateOBBox(const matrix44 &worldMatrix);

    /// visibility visitor processing for an entity with a spatial portal component
    virtual void Accept(nVisibleFrustumVisitor &visitor);
    /// accept a spatial visitor
    virtual void Accept(nSpatialVisitor &visitor);

protected:

    /// portal's vertices
    vector3 *m_vertices;
    /// clip rectangle's center
    vector3 m_clipCenter;
    /// portal's original vertices
    vector3 *m_originalVertices;
    /// portal's plane
    plane m_plane;
    /// portal's original plane
    plane m_originalPlane;
    /// original oriented bounding box
    obbox3 m_originalOBox;
    /// oriented bounding box
    obbox3 m_oBox;
    /// cell that this portal points to
    ncSpatialCell *m_otherSide;
    /// the equivalent portal (twin) in the other side cell
    ncSpatialPortal *m_twinPortal;
    /// other side cell's id
    int m_otherSideId;
    /// frame id. If it's equal to the actual frame id, this portal has already been visited
    int m_visitFrameId;
    /// frame id for the light determination algorithm
    int m_lightFrameId;
    /// last light that visited the portal
    nEntityObjectId m_lastLight;
    /// last camera that the last light visited the portal from
    nEntityObjectId m_lastLightCam;
    /// is the portal active?
    bool m_active;
    /// if the portal is farther than this distance from the camera it will be deactivated
    /// NOTE: this is the squared distance (to speed up distance tests)
    float m_sqDeactivationDist;
    /// brushes associated to this portal
    nArray<nEntityObject*> *brushes;

};

//------------------------------------------------------------------------------
/**
    destructor
*/
inline
ncSpatialPortal::~ncSpatialPortal()
{
    n_delete_array(this->m_vertices);
    n_delete_array(this->m_originalVertices);
    n_delete(this->brushes);
}

//------------------------------------------------------------------------------
/**
    set the portal's vertices
*/
inline
void 
ncSpatialPortal::SetVertices(const vector3 portalVertices[4])
{
    for (int i = 0; i < 4; i++)
    {
        this->m_vertices[i] = portalVertices[i];
    }
}

//------------------------------------------------------------------------------
/**
    set the portal's original vertices
*/
inline
void 
ncSpatialPortal::SetOriginalVertices(const vector3 portalVertices[4])
{
    for (int i = 0; i < 4; i++)
    {
        this->m_originalVertices[i] = portalVertices[i];
    }
}

//------------------------------------------------------------------------------
/**
    set the portal's original vertices
*/
inline
void 
ncSpatialPortal::SetClipVertex(int numVertex, const vector3 &vertex)
{
    this->m_originalVertices[numVertex] = vertex;
}

//------------------------------------------------------------------------------
/**
    get the portal's vertices
*/
inline
const vector3 *
ncSpatialPortal::GetVertices() const
{
    return this->m_vertices;
}

//------------------------------------------------------------------------------
/**
    get the portal's original vertices
*/
inline
const vector3 *
ncSpatialPortal::GetOriginalVertices() const
{
    return this->m_originalVertices;
}

//------------------------------------------------------------------------------
/**
    set the portal's plane
*/
inline
void 
ncSpatialPortal::SetPlane(const plane &pl)
{
    this->m_plane = pl;
}

//------------------------------------------------------------------------------
/**
    set the portal's original plane
*/
inline
void 
ncSpatialPortal::SetOriginalPlane(const plane &pl)
{
    this->m_originalPlane = pl;
}

//------------------------------------------------------------------------------
/**
    set the portal's original plane
*/
inline
void 
ncSpatialPortal::SetOriginalPlane(float a, float b, float c, float d)
{
    plane pl(a, b, c, d);
    this->SetOriginalPlane(pl);
}

//------------------------------------------------------------------------------
/**
    get the portal's plane
*/
inline
const plane &
ncSpatialPortal::GetPlane() const
{
    return this->m_plane;
}

//------------------------------------------------------------------------------
/**
    get the portal's original plane
*/
inline
const plane &
ncSpatialPortal::GetOriginalPlane() const
{
    return this->m_originalPlane;
}

//------------------------------------------------------------------------------
/**
    get the other side cell
*/
inline
ncSpatialCell *
ncSpatialPortal::GetOtherSideCell() const
{
    if ( this->m_twinPortal )
    {
        return this->m_twinPortal->GetCell();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    set the other side cell's id
*/
inline
void
ncSpatialPortal::SetOtherSideCellId(int cellId)
{
    this->m_otherSideId = cellId;
}

//------------------------------------------------------------------------------
/**
    get the other side cell's id
*/
inline
int 
ncSpatialPortal::GetOtherSideCellId() const
{
    return this->m_otherSideId;
}

//------------------------------------------------------------------------------
/**
    set frame id that means in which frame the portal was visited
*/
inline
void 
ncSpatialPortal::SetVisitFrameId(int id)
{
    this->m_visitFrameId = id;
}

//------------------------------------------------------------------------------
/**
    get frame id that means in which frame the portal was visited
*/
inline
int 
ncSpatialPortal::GetVisitFrameId() const
{
    return this->m_visitFrameId;
}

//------------------------------------------------------------------------------
/**
    set the original oriented bounding box
*/
inline
void 
ncSpatialPortal::SetOriginalOBBox(obbox3 &box)
{
    this->m_originalOBox = box;
}

//------------------------------------------------------------------------------
/**
    get the original oriented bounding box
*/
inline
obbox3 &
ncSpatialPortal::GetOriginalOBBox()
{
    return this->m_originalOBox;
}

//------------------------------------------------------------------------------
/**
    set the original oriented bounding box's size
*/
inline
void 
ncSpatialPortal::SetOrigOBBoxSize(const vector3& size)
{
    this->m_originalOBox.SetSize(size);
}

//------------------------------------------------------------------------------
/**
    set the original oriented bounding box's scale
*/
inline
void 
ncSpatialPortal::SetOrigOBBoxScale(const vector3& scale)
{
    this->m_originalOBox.SetScale(scale);
}

//------------------------------------------------------------------------------
/**
    set the original oriented bounding box's euler
*/
inline
void 
ncSpatialPortal::SetOrigOBBoxEuler(const vector3& rot)
{
    this->m_originalOBox.SetEuler(rot);
}

//------------------------------------------------------------------------------
/**
    set the original oriented bounding box's translation
*/
inline
void 
ncSpatialPortal::SetOrigOBBoxTranslation(const vector3& trans)
{
    this->m_originalOBox.SetTranslation(trans);
}

//------------------------------------------------------------------------------
/**
    activate the portal
*/
inline
void
ncSpatialPortal::Activate()
{
    this->SetActive(true);
}

//------------------------------------------------------------------------------
/**
    deactivate the portal
*/
inline
void
ncSpatialPortal::Deactivate()
{
    this->SetActive(false);
}

//------------------------------------------------------------------------------
/**
    is the portal active?
*/
inline
bool
ncSpatialPortal::IsActive() const
{
    return this->m_active;
}

//------------------------------------------------------------------------------
/**
    set the active flag for this portal and its twin one (if exists)
*/
inline
void
ncSpatialPortal::SetActiveAndTwin(bool flag)
{
    this->SetActive(flag);
    if (this->m_twinPortal)
    {
        this->m_twinPortal->SetActive(flag);
    }
}

//------------------------------------------------------------------------------
/**
    set the oriented bounding box
*/
inline
void 
ncSpatialPortal::SetOBBox(obbox3 &box)
{
    this->m_oBox = box;
}

//------------------------------------------------------------------------------
/**
    get the oriented bounding box
*/
inline
obbox3 &
ncSpatialPortal::GetOBBox()
{
    return this->m_oBox;
}

//------------------------------------------------------------------------------
/**
    set twin portal
*/
inline
void 
ncSpatialPortal::SetTwinPortal(ncSpatialPortal *twinPortal)
{
    this->m_twinPortal = twinPortal;
    if ( twinPortal )
    {
        this->m_otherSide = twinPortal->GetCell();
    }
    else
    {
        this->m_otherSide = 0;
    }
}

//------------------------------------------------------------------------------
/**
    get twin portal
*/
inline
ncSpatialPortal *
ncSpatialPortal::GetTwinPortal()
{
    return this->m_twinPortal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpatialPortal::operator=(const ncSpatialPortal &portal)
{
    ncSpatial::operator=(portal);
    this->m_frameId = portal.m_frameId;
    this->m_visitFrameId = portal.m_visitFrameId;
    this->m_lightFrameId = portal.m_lightFrameId;
    this->m_otherSide = portal.m_otherSide;
    this->m_otherSideId = portal.m_otherSideId;
    this->m_twinPortal = portal.m_twinPortal;

    for (int i = 0; i < 4; ++i)
    {
        this->m_vertices[i] = portal.m_vertices[i];
    }

    for (int i = 0; i < 4; ++i)
    {
        this->m_originalVertices[i] = portal.m_originalVertices[i];
    }

    this->m_plane = portal.m_plane;
    this->m_oBox = portal.m_oBox;
    this->m_originalOBox = portal.m_originalOBox;
    this->m_active = portal.m_active;
    this->m_sqDeactivationDist = portal.m_sqDeactivationDist;

    if (portal.brushes)
    {
        if (!this->brushes)
        {
            this->brushes = n_new(nArray<nEntityObject*>(2, 2));
        }

        for (int i(0); i < portal.brushes->Size(); ++i)
        {
            this->brushes->Append((*portal.brushes)[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    set the portal as visited by the given light and the given camera
*/
inline
void 
ncSpatialPortal::SetLightVisited(nEntityObjectId currentLight, nEntityObjectId currentCam)
{
    this->m_lightFrameId = nSpatialServer::Instance()->GetFrameId();
    this->m_lastLight = currentLight;
    this->m_lastLightCam = currentCam;
}

//------------------------------------------------------------------------------
/**
    return true if this portal has been visited in this frame by the a light
*/
inline
bool 
ncSpatialPortal::IsLightVisited(nEntityObjectId currentLight, nEntityObjectId currentCam) const
{
    return ((this->m_lightFrameId == nSpatialServer::Instance()->GetFrameId()) &&
            (this->m_lastLight == currentLight) &&
            (this->m_lastLightCam == currentCam));
}

//------------------------------------------------------------------------------
/**
    set the distance the portal is deactivated
*/
inline
void 
ncSpatialPortal::SetDeactivationDistance(float dist)
{
    this->m_sqDeactivationDist = (dist * dist);
}

//------------------------------------------------------------------------------
/**
    get the distance the portal is deactivated
*/
inline
float 
ncSpatialPortal::GetDeactivationDistance() const
{
    return n_sqrt(this->m_sqDeactivationDist);
}

//------------------------------------------------------------------------------
/**
    get the squared distance the portal is deactivated
*/
inline
float 
ncSpatialPortal::GetSqDeactivationDistance() const
{
    return this->m_sqDeactivationDist;
}

//------------------------------------------------------------------------------
/**
    get the clip rectangle's center
*/
inline
const vector3 &
ncSpatialPortal::GetClipCenter() const
{
    return this->m_clipCenter;
}

#endif  // NC_SPATIALPORTAL_H

