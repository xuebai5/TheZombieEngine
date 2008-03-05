#include "precompiled/pchnspatial.h"
//------------------------------------------------------------
// nspatialmodels.cc
// (C) 2005 Conjurer Services, S.A.
// @author Miquel Angel Rujula <>
//------------------------------------------------------------

#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/nspatialvisitor.h"

//------------------------------------------------------------------------------
/**
*/
nSpatialModel::nSpatialModel() :
    m_type(SPATIAL_MODEL_NONE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSpatialModel::~nSpatialModel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    check if a given portal intersects with the sphere
*/
bool 
nSpatialSphereModel::Intersects(const ncSpatialPortal &portal)
{
    const bbox3 &box = portal.GetBBox();
    return this->m_sphere.intersects(box);
}

//------------------------------------------------------------------------------
/**
    check if a given bounding box intersects with the frustum
*/
bool 
nSpatialFrustumModel::Intersects(const bbox3 &box) 
{
    bbox3::ClipStatus clipStatus = box.clipstatus(this->viewProjection);
    
    return ((clipStatus == bbox3::Clipped) || (clipStatus == bbox3::Inside));
}

//------------------------------------------------------------------------------
/**
    check if a given sphere intersects with the frustum.
    We'll make a conservative aproximation. We'll use the sphere's bounding box.
*/
bool 
nSpatialFrustumModel::Intersects(const sphere &sph) 
{
    bbox3 sphereBox(sph.p, vector3(sph.r, sph.r, sph.r));
    return this->Intersects(sphereBox);
}

//------------------------------------------------------------------------------
/**
    check if a given portal intersects with the frustum.
*/
bool 
nSpatialFrustumModel::Intersects(const ncSpatialPortal &portal) 
{
    VisitorFlags vf(false, false);
    vf = this->clipper.TestPortalVertices(portal.GetVertices(), 4, vf, this->vertsInfo);
    return !vf.AntiTestResult();
}

//--------------------------------------------------------------------
/**
    check if the frustum model contains the given point
*/
bool 
nSpatialFrustumModel::Contains(const vector3 &point)
{
    vector4 v0;
    vector4 v1;

    v0.set(point);
    v0.w = 1.0f;

    v1 = this->viewProjection * v0;

    // special case: a point is behind the camera
    if (v1.w <= 0)
    {
        return false;
    }

    if ((v1.x < -v1.w) || (v1.x > v1.w) ||
        (v1.y < -v1.w) || (v1.y > v1.w) ||
        (v1.z < -v1.w) || (v1.z > v1.w))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------
/**
    update the view projection matrix
*/
void 
nSpatialFrustumModel::UpdateViewProjection()
{
    this->viewProjection = this->transformMatrix;
    this->viewProjection.invert();
    this->viewProjection *= this->camera.GetProjection();

    this->clipper.Set(this->camera, this->transformMatrix);
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialShadowModel::~nSpatialShadowModel()
{
    this->RemoveFromCells();

    if ( this->caster )
    {
        this->caster = 0;
    }

    this->lightId = nEntityObjectServer::IDINVALID;
    this->shadowId = 0;
}

//--------------------------------------------------------------------
/**
    check if the shadow model touches the given portal
*/
bool 
nSpatialShadowModel::TouchesPortal(ncSpatialPortal *portal) const
{
    nSpatialVisitor &spatialVisitor = nSpatialServer::Instance()->GetSpatialVisitor();
    vector3 pos(portal->GetClipCenter() + (portal->GetPlane().normal() * 3.f));
    nFrustumClipper frus(spatialVisitor.BuildNewFrustumNoTest(portal, pos, 0));

    VisitorFlags vf(false, false);
    vf = frus.TestCylinderQuick(this->cyl, vf);

    return !vf.AntiTestResult();
}

//------------------------------------------------------------------------------
/**
    Autoremove from a cell
*/
void
nSpatialShadowModel::RemoveFromCell(ncSpatialCell *cell)
{
    if ( !this->cells.Empty() )
    {
        // remove the cell from the cells array
        int cellIndex(this->cells.FindIndex(cell));
        n_assert(cellIndex != -1);
        this->cells.EraseQuick(cellIndex);

        // remove the shadow from the cell
        nArray<nSpatialShadowModel*> *cellShadows = cell->GetShadowModels();
        n_assert(cellShadows);
        int shadowIndex(cellShadows->FindIndex(this));
        n_assert( shadowIndex != -1 );

        cellShadows->EraseQuick(shadowIndex);
    }
}
