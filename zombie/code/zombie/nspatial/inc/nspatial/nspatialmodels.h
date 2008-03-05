#ifndef N_SPATIALMODELS_H
#define N_SPATIALMODELS_H
//------------------------------------------------------------------------------
#include "mathlib/sphere.h"
#include "gfx2/ncamera2.h"
#include "nspatial/nfrustumclipper.h"

//------------------------------------------------------------------------------
/**
    @class nSpatialModel
    @ingroup NebulaSpatialSystem
    @brief The interface for models.

    A spatial model is a shape used to perform visibility tests.
*/
class nSpatialModel
{
public:

    enum Model_Type
    {
        SPATIAL_MODEL_NONE = 0,
        SPATIAL_MODEL_SPHERE,
        SPATIAL_MODEL_FRUSTUM,
        SPATIAL_MODEL_SHADOW,
    };

    /// constructor
    nSpatialModel();
    /// destructor
    virtual ~nSpatialModel();

    /// get model type
    Model_Type GetType() const;

    /// check if a bounding box intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const sphere &sph);
    /// check if a portal intersects with the model (rewrite it in subclasses)
    virtual bool Intersects(const ncSpatialPortal &portal);
    /// check if the sphere model contains the given point
    virtual bool Contains(const vector3 &point);

protected:

    Model_Type m_type;
};

//------------------------------------------------------------------------------
/**
    get model type
*/
inline
nSpatialModel::Model_Type
nSpatialModel::GetType() const
{
    return this->m_type;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialModel::Intersects(const bbox3 &/*box*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialModel::Intersects(const sphere &/*sph*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialModel::Intersects(const ncSpatialPortal &/*portal*/)
{
    return false;
}

//--------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialModel::Contains(const vector3 &/*point*/)
{
    return false;
}

//--------------------------------------------------------------------
/**
    @class nSpatialSphereModel
    @ingroup NebulaSpatialSystem
    @brief Represents a model which shape is a sphere.

    A sphere model is a sphere used to perform visibility tests.
*/
class nSpatialSphereModel : public nSpatialModel
{
public:
    /// constructor 1
    nSpatialSphereModel ();
    /// constructor 2
    nSpatialSphereModel (const sphere &sph);
    /// constructor 3
    nSpatialSphereModel(const vector3 &center, const float radius);
    /// destructor
    virtual ~nSpatialSphereModel(){}

    /// set the sphere center and radius
    void Set(const vector3& center, float radius);
    /// set the sphere's position (center)
    void Set(const vector3& center);
    /// set the sphere's radius
    void SetRadius(const float radius);
    /// get the sphere's radius
    float GetRadius();
    /// get the sphere
    const sphere& GetSphere();
    
    /// check if a given bounding box intersects with the sphere
    virtual bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model
    virtual bool Intersects(const sphere &sph);
    /// check if a portal intersects with the model
    virtual bool Intersects(const ncSpatialPortal &portal);
    /// check if the sphere model contains the given point
    virtual bool Contains(const vector3 &point);

private:

    sphere m_sphere;

};

//------------------------------------------------------------------------------
/**
    constructor 1
*/
inline
nSpatialSphereModel::nSpatialSphereModel()
{
    this->m_type = SPATIAL_MODEL_SPHERE;
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
inline
nSpatialSphereModel::nSpatialSphereModel(const sphere &sph) : 
m_sphere(sph)
{
    this->m_type = SPATIAL_MODEL_SPHERE;
}

//------------------------------------------------------------------------------
/**
    constructor 3
*/
inline
nSpatialSphereModel::nSpatialSphereModel(const vector3 &center, const float radius)
{
    this->m_sphere = sphere(center, radius);
    this->m_type = SPATIAL_MODEL_SPHERE;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nSpatialSphereModel::Set(const vector3& center, float radius)
{
    this->m_sphere.set(center, radius);
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nSpatialSphereModel::Set(const vector3& center)
{
    this->m_sphere.p = center;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nSpatialSphereModel::SetRadius(const float radius)
{
    this->m_sphere.r = radius;
};

//------------------------------------------------------------------------------
/**
*/
inline
float
nSpatialSphereModel::GetRadius()
{
    return this->m_sphere.r;
};

//------------------------------------------------------------------------------
/**
    get the sphere
*/
inline
const sphere&
nSpatialSphereModel::GetSphere()
{
    return this->m_sphere;
}

//------------------------------------------------------------------------------
/**
    check if a given bounding box intersects with the sphere
*/
inline
bool 
nSpatialSphereModel::Intersects(const bbox3 &box)
{
    return this->m_sphere.intersects(box);
}

//------------------------------------------------------------------------------
/**
    check if a given sphere intersects with the sphere
*/
inline
bool 
nSpatialSphereModel::Intersects(const sphere &sph)
{
    return this->m_sphere.intersects(sph);
}

//--------------------------------------------------------------------
/**
    check if the sphere model contains the given point
*/
inline
bool 
nSpatialSphereModel::Contains(const vector3 &point)
{
    return this->m_sphere.contains(point);
}

//--------------------------------------------------------------------
/**
    @class nSpatialFrustumModel
    @ingroup NebulaSpatialSystem
    @brief Represents a model which shape is a frustum.

    A frustum model is a frustum used to perform visibility tests.
*/
class nSpatialFrustumModel : public nSpatialModel
{
public:
    /// constructor 1
    nSpatialFrustumModel ();
    /// constructor 2
    nSpatialFrustumModel (const nCamera2 &frustum, const matrix44 &transformMatrix);
    /// destructor
    virtual ~nSpatialFrustumModel(){}

    /// set the transformation matrix 
    void SetTransform(const matrix44 &transform);
    /// get the transformation matrix
    const matrix44 &GetTransform() const;
     /// get the viewProjection,  transform.inv() * camera.projection
    const matrix44 &GetViewProjection() const;
    /// set the camera
    void SetCamera(const nCamera2 &cam);
    /// get the camera
    const nCamera2 &GetCamera();
    /// set the frustum clipper
    void SetFrustum(const nFrustumClipper &frus);
    /// get the frustum clipper
    nFrustumClipper &GetFrustum();

    /// check if a given bounding box intersects with the frustum
    virtual bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model
    virtual bool Intersects(const sphere &sph);
    /// check if a portal intersects with the model
    virtual bool Intersects(const ncSpatialPortal &portal);
    /// check if the sphere model contains the given point
    virtual bool Contains(const vector3 &point);

private:

    /// view matrix
    matrix44 transformMatrix;
    /// projection matrix
    nCamera2 camera;

    /// frustum clipper
    nFrustumClipper clipper;

    /// view projection matrix
    matrix44 viewProjection;

    /// clipping information for the tested portals
    struct ClipVertexInfo vertsInfo[4];

    /// update the view projection matrix
    void UpdateViewProjection();
};

//------------------------------------------------------------------------------
/**
    set the transformation matrix 
*/
inline
void 
nSpatialFrustumModel::SetTransform(const matrix44 &transform)
{
    this->transformMatrix = transform;
    this->UpdateViewProjection();
}

//------------------------------------------------------------------------------
/**
    get the transformation matrix
*/
inline
const matrix44 &
nSpatialFrustumModel::GetTransform() const
{
    return this->transformMatrix;
}

//------------------------------------------------------------------------------
/**
    get the transformation matrix
*/
inline
const matrix44 &
nSpatialFrustumModel::GetViewProjection() const
{
    return this->viewProjection;
}

//------------------------------------------------------------------------------
/**
    set the camera
*/
inline
void 
nSpatialFrustumModel::SetCamera(const nCamera2 &cam)
{
    this->camera = cam;
    this->UpdateViewProjection();
}

//------------------------------------------------------------------------------
/**
    get the camera
*/
inline
const nCamera2&
nSpatialFrustumModel::GetCamera()
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
    set the frustum clipper
*/
inline
void 
nSpatialFrustumModel::SetFrustum(const nFrustumClipper &frus)
{
    this->clipper.Copy(frus);
}

//------------------------------------------------------------------------------
/**
    get the frustum clipper
*/
inline
nFrustumClipper &
nSpatialFrustumModel::GetFrustum()
{
    return this->clipper;
}

//------------------------------------------------------------------------------
/**
    constructor 1
*/
inline
nSpatialFrustumModel::nSpatialFrustumModel() :
camera(30.f, 1.f, 0.2f, 10.f)
{
    this->m_type = SPATIAL_MODEL_FRUSTUM;
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
inline
nSpatialFrustumModel::nSpatialFrustumModel(const nCamera2 &cam, const matrix44 &transform) : 
camera(cam),
transformMatrix(transform)
{
    this->m_type = SPATIAL_MODEL_FRUSTUM;
    this->UpdateViewProjection();
}

//--------------------------------------------------------------------
/**
    @class nSpatialShadowModel
    @ingroup NebulaSpatialSystem
    @brief The interface for shadow models.

    A model to represent the shadows projected by an entity.
*/
class nSpatialShadowModel : public nSpatialModel
{
public:

    /// constructor 1
    nSpatialShadowModel();
    /// constructor 2
    nSpatialShadowModel(const cylinder &_cyl);
    /// constructor 3
    nSpatialShadowModel(const vector3 &p1, const vector3 &p2, float radius);
    /// destructor
    virtual ~nSpatialShadowModel();

    /// set cylinder
    void SetCylinder(const cylinder& _cyl);
    /// set cylinder properties
    void SetCylinder(const vector3 &p1, const vector3 &p2, float radius);
    /// set cylinder radius
    void SetCylinderRadius(float radius);
    /// set cylinder p1 and p2
    void SetCylinderExtremes(const vector3 &p1, const vector3 &p2);
    /// get cylinder
    const cylinder& GetCylinder() const;
        
    /// check if a bounding box intersects with the model (rewrite it in subclasses)
    bool Intersects(const bbox3 &box);
    /// check if a sphere intersects with the model (rewrite it in subclasses)
    bool Intersects(const sphere &sph);
    /// check if the shadow model contains the given point
    bool Contains(const vector3 &point);
    /// check if the shadow model touches the given portal
    bool TouchesPortal(ncSpatialPortal *portal) const;

    /// set the entity that casts this shadow
    void SetCaster(ncSpatial *caster);
    /// get the entity that casts this shadow
    ncSpatial *GetCaster() const;

    /// set the light id
    void SetLightId(nEntityObjectId id);
    /// get the light id
    nEntityObjectId GetLightId() const;

    /// Add a cell where the shadow model is in
    void AddCell(ncSpatialCell *cell);
    /// Get the cells the light region is in
    const nArray<ncSpatialCell*> &GetCells() const;

    /// Autoremove from a cell
    void RemoveFromCell(ncSpatialCell *cell);
    /// Autoremove from all the cells
    void RemoveFromCells();

    /// Set shadow id
    void SetShadowId(unsigned short shadowId);
    /// Get shadow id
    unsigned short GetShadowId() const;

private:

    /// cylinder used as model
    cylinder cyl;

    /// entity that casts this shadow
    ncSpatial *caster;

    /// id of the light that generated this shadow model
    nEntityObjectId lightId;

    /// cell the shadow is in
    nArray<ncSpatialCell*> cells;

    /// shadow id mask
    unsigned short shadowId;
};

//------------------------------------------------------------------------------
/**
    constructor 1
*/
inline
nSpatialShadowModel::nSpatialShadowModel():
caster(0),
lightId(nEntityObjectServer::IDINVALID),
shadowId(0),
cells(2, 2)
{
    this->m_type = SPATIAL_MODEL_SHADOW;
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
inline
nSpatialShadowModel::nSpatialShadowModel(const cylinder &_cyl):
caster(0),
lightId(nEntityObjectServer::IDINVALID),
shadowId(0),
cells(2, 2)
{
    this->m_type = SPATIAL_MODEL_SHADOW;
    this->cyl = _cyl; 
}

//------------------------------------------------------------------------------
/**
    constructor 3
*/
inline
nSpatialShadowModel::nSpatialShadowModel(const vector3 &p1, const vector3 &p2, float radius):
caster(0),
lightId(nEntityObjectServer::IDINVALID),
shadowId(0),
cells(2, 2)
{
    this->m_type = SPATIAL_MODEL_SHADOW;
    this->cyl.set(p1, p2, radius);
}

//------------------------------------------------------------------------------
/**
    set cylinder
*/
inline
void 
nSpatialShadowModel::SetCylinder(const cylinder& _cyl)
{
    this->cyl = _cyl;
}

//------------------------------------------------------------------------------
/**
    set cylinder properties
*/
inline
void 
nSpatialShadowModel::SetCylinder(const vector3 &p1, const vector3 &p2, float radius)
{
    this->cyl.set(p1, p2, radius);
}

//------------------------------------------------------------------------------
/**
    set cylinder radius
*/
inline
void 
nSpatialShadowModel::SetCylinderRadius(float radius)
{
    this->cyl.r = radius;
}

//------------------------------------------------------------------------------
/**
    set cylinder p1 and p2
*/
inline
void 
nSpatialShadowModel::SetCylinderExtremes(const vector3 &p1, const vector3 &p2)
{
    this->cyl.p1 = p1;
    this->cyl.p2 = p2;
}

//------------------------------------------------------------------------------
/**
    get cylinder
*/
inline
const cylinder& 
nSpatialShadowModel::GetCylinder() const
{
    return this->cyl;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialShadowModel::Intersects(const bbox3 &/*box*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialShadowModel::Intersects(const sphere &/*sph*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nSpatialShadowModel::Contains(const vector3 &/*point*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    set the entity that casts this shadow
*/
inline
void 
nSpatialShadowModel::SetCaster(ncSpatial *caster)
{
    this->caster = caster;
}

//------------------------------------------------------------------------------
/**
    get the entity that casts this shadow
*/
inline
ncSpatial *
nSpatialShadowModel::GetCaster() const
{
    return this->caster;
}

//------------------------------------------------------------------------------
/**
    set the light id
*/
inline
void 
nSpatialShadowModel::SetLightId(nEntityObjectId id)
{
    this->lightId = id;
}

//------------------------------------------------------------------------------
/**
    get the light id
*/
inline
nEntityObjectId 
nSpatialShadowModel::GetLightId() const
{
    return this->lightId;
}

//------------------------------------------------------------------------------
/**
    Add a cell the light is in
*/
inline
void 
nSpatialShadowModel::AddCell(ncSpatialCell *cell)
{
    n_assert(this->cells.FindIndex(cell) == -1);
    this->cells.Append(cell);
}

//------------------------------------------------------------------------------
/**
    Get the cells the light is in
*/
inline
const nArray<ncSpatialCell*> &
nSpatialShadowModel::GetCells() const
{
    return this->cells;
}

//------------------------------------------------------------------------------
/**
    Autoremove from all the cells
*/
inline
void
nSpatialShadowModel::RemoveFromCells()
{
    while ( !this->cells.Empty() )
    {
        this->RemoveFromCell(this->cells.Back());
    }
}

//------------------------------------------------------------------------------
/**
    Set shadow id
*/
inline
void 
nSpatialShadowModel::SetShadowId(unsigned short shadowId)
{
    this->shadowId = shadowId;
}

//------------------------------------------------------------------------------
/**
    Get shadow id
*/
inline
unsigned short 
nSpatialShadowModel::GetShadowId() const
{
    return this->shadowId;
}

#endif // N_SPATIALMODELS_H
