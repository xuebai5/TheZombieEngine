#ifndef N_FRUSTUMCLIPPER_H
#define N_FRUSTUMCLIPPER_H

//------------------------------------------------------------------------------
/**
    @class nFrustumClipper
    @ingroup NebulaSpatialSystem
    @brief Encapsulates the clipping planes of a frustum, with handling
    of 'active clipping plane' flags.

    This is just a simple frustum class, allowing you to construct a
    frustum and test object against it.  Also included is the concept
    of 'active planes' for recursive object testing (i.e., octrees and
    quadtrees).  When a given tree node is determine to be fully on the
    inside of a given plane, you know all the child objects will be fully
    inside that plane as well so there is no need to test any child objects
    against that plane.  When there are no active planes, you know the
    object and all its children are fully inside the frustum and so can
    trivially add them all without doing any more clipping checks.
*/
//------------------------------------------------------------------------------

#include "nspatial/nplaneclipper.h"
#include "gfx2/ncamera2.h"
#ifndef NGAME
#include "gfx2/ngfxserver2.h"
#include "gfx2/nlineserver.h"
#endif // !NGAME

class nFrustumClipper : public nPlaneClipper 
{
public:
    nFrustumClipper();
    nFrustumClipper(const nFrustumClipper &copyme);
    nFrustumClipper(nCamera2 &camera, const matrix44 &viewtransform);
    nFrustumClipper(const plane *clipplanes, int numplanes);
    nFrustumClipper(const plane clipplanes[6]);

    enum
    {
        NEAR_PLANE = 0,
        FAR_PLANE,
        LEFT_PLANE,
        RIGHT_PLANE,
        TOP_PLANE,
        BOTTOM_PLANE
    };

    /// copy another frustum clipper
    void Copy(const nFrustumClipper &copyme);

    /// initialize the frustum clipper
    void Set(nCamera2 &camera, const matrix44 &viewtransform);

    /// get the frustum's near plane
    const plane &GetNearPlane() const;
    /// set the frustum's near plane
    void SetNearPlane(const plane &p);
    /// get the frustum's far plane
    const plane &GetFarPlane() const;
    /// set the frustum's far plane
    void SetFarPlane(const plane &p);
    /// get the frustum's left plane
    const plane &GetLeftPlane() const;
    /// set the frustum's left plane
    void SetLeftPlane(const plane &p);
    /// get the frustum's right plane
    const plane &GetRightPlane() const;
    /// set the frustum's right plane
    void SetRightPlane(const plane &p);
    /// get the frustum's top plane
    const plane &GetTopPlane() const;
    /// set the frustum's top plane
    void SetTopPlane(const plane &p);
    /// get the frustum's bottom plane
    const plane &GetBottomPlane() const;
    /// set the frustum's bottom plane
    void SetBottomPlane(const plane &p);

    /// get the near plane's index
    int GetNearPlaneIdx() const;
    /// get the far plane's index
    int GetFarPlaneIdx() const;
    /// set the near plane's index
    void SetNearPlaneIdx(int idx);
    /// set the far plane's index
    void SetFarPlaneIdx(int idx);

#ifndef NGAME
    /// visualize the frustum
    void VisualizeFrustum(nGfxServer2 *gfx2, const vector4 &color) const;
    /// visualize the planes set
    void VisualizePlanesSet(const vector4 &color);
#endif // !NGAME

protected:

    /// near plane index
    int nearPlaneIdx;
    /// far plane index
    int farPlaneIdx;

#ifndef NGAME
    /// for debug visualizing
    vector3 frustumcorners[8];
    /// generate the frustum corners. Useful for debugging.
    void GenerateFrustumCorners(nCamera2 &camera, const matrix44 &viewtransform);
    /// generate the near plane corners
    int GenerateNearCorners(vector3 *corners);
    /// generate the far plane corners
    int GenerateFarCorners(vector3 *corners);
#endif // !NGAME

};

//------------------------------------------------------------------------------
/**
*/
inline
nFrustumClipper::nFrustumClipper() : 
nPlaneClipper(),
nearPlaneIdx(NEAR_PLANE),
farPlaneIdx(FAR_PLANE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nFrustumClipper::nFrustumClipper(const nFrustumClipper &copyme)
{
    this->Copy(copyme);
}

//------------------------------------------------------------------------------
/**
*/
inline
nFrustumClipper::nFrustumClipper(const plane *clipplanes, int numplanes) :
nPlaneClipper(clipplanes, numplanes),
nearPlaneIdx(NEAR_PLANE),
farPlaneIdx(FAR_PLANE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nFrustumClipper::nFrustumClipper(nCamera2 &camera, const matrix44 &viewtransform): 
nPlaneClipper(),
nearPlaneIdx(NEAR_PLANE),
farPlaneIdx(FAR_PLANE)
{
    this->Set(camera, viewtransform);
}

//------------------------------------------------------------------------------
/**
*/
inline
nFrustumClipper::nFrustumClipper(const plane clipplanes[6]) : 
nPlaneClipper(clipplanes, 6),
nearPlaneIdx(NEAR_PLANE),
farPlaneIdx(FAR_PLANE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    copy another plane clipper
*/
inline
void
nFrustumClipper::Copy(const nFrustumClipper &copyme)
{
    nPlaneClipper::Copy(copyme);

    this->nearPlaneIdx = copyme.nearPlaneIdx;
    this->farPlaneIdx  = copyme.farPlaneIdx;

#ifndef NGAME
    for (int i(0); i < 8; ++i)
    {
        this->frustumcorners[i] = copyme.frustumcorners[i];
    }
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    initialize the frustum clipper
*/
inline
void 
nFrustumClipper::Set(nCamera2 &camera, const matrix44 &viewtransform)
{
    m_numplanes = 6;

    // initialize clip planes from the projection and transform info
    // stolen from nOctFrustum code, 
    matrix44 proj(camera.GetProjection());
    matrix44 m(viewtransform);
    m.invert();
    m *= proj;

    // near
    m_planes[NEAR_PLANE].a = m.M13;
    m_planes[NEAR_PLANE].b = m.M23;
    m_planes[NEAR_PLANE].c = m.M33;
    m_planes[NEAR_PLANE].d = m.M43;

    // far
    m_planes[FAR_PLANE].a = (m.M14 - m.M13);
    m_planes[FAR_PLANE].b = (m.M24 - m.M23);
    m_planes[FAR_PLANE].c = (m.M34 - m.M33);
    m_planes[FAR_PLANE].d = (m.M44 - m.M43);

    // left
    m_planes[LEFT_PLANE].a = (m.M14 + m.M11);
    m_planes[LEFT_PLANE].b = (m.M24 + m.M21);
    m_planes[LEFT_PLANE].c = (m.M34 + m.M31);
    m_planes[LEFT_PLANE].d = (m.M44 + m.M41);

    // right
    m_planes[RIGHT_PLANE].a = (m.M14 - m.M11);
    m_planes[RIGHT_PLANE].b = (m.M24 - m.M21);
    m_planes[RIGHT_PLANE].c = (m.M34 - m.M31);
    m_planes[RIGHT_PLANE].d = (m.M44 - m.M41);

    // top
    m_planes[TOP_PLANE].a = (m.M14 - m.M12);
    m_planes[TOP_PLANE].b = (m.M24 - m.M22);
    m_planes[TOP_PLANE].c = (m.M34 - m.M32);
    m_planes[TOP_PLANE].d = (m.M44 - m.M42);

    // bottom
    m_planes[BOTTOM_PLANE].a = (m.M14 + m.M12);
    m_planes[BOTTOM_PLANE].b = (m.M24 + m.M22);
    m_planes[BOTTOM_PLANE].c = (m.M34 + m.M32);
    m_planes[BOTTOM_PLANE].d = (m.M44 + m.M42);

    //normalize planes.
    float denom;
    vector3 tmp;
    
    for (int i=0; i<6; i++)
    {
        tmp.set (m_planes[i].a, m_planes[i].b, m_planes[i].c);
        denom = 1.0f / tmp.len();
        m_planes[i].a *= denom;
        m_planes[i].b *= denom;
        m_planes[i].c *= denom;
        m_planes[i].d *= denom;
    }
#ifndef NGAME
    // generate frustum corners, for debugging
    this->GenerateFrustumCorners(camera, viewtransform);
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    Get the frustum's near plane
*/
inline
const plane &
nFrustumClipper::GetNearPlane() const
{
    return this->m_planes[this->nearPlaneIdx];
}

//------------------------------------------------------------------------------
/**
    Set the frustum's near plane.
    Warning! This method doesn't update the frustum corners.
*/
inline
void 
nFrustumClipper::SetNearPlane(const plane &p)
{
    this->m_planes[this->nearPlaneIdx] = p;
}

//------------------------------------------------------------------------------
/**
    Get the frustum's far plane.
*/
inline
const plane &
nFrustumClipper::GetFarPlane() const
{
    n_assert2(this->farPlaneIdx != -1, "Trying to get to an non existing far plane of a frustum!");
    return this->m_planes[this->farPlaneIdx];
}

//------------------------------------------------------------------------------
/**
    Set the frustum's far plane.
    Warning! This method doesn't update the frustum corners.
*/
inline
void 
nFrustumClipper::SetFarPlane(const plane &p)
{
    this->m_planes[this->farPlaneIdx] = p;
}

//------------------------------------------------------------------------------
/**
    Get the frustum's left plane.
*/
inline
const plane &
nFrustumClipper::GetLeftPlane() const
{
    return this->m_planes[LEFT_PLANE];
}

//------------------------------------------------------------------------------
/**
    Set the frustum's left plane.
    Warning! This method doesn't update the frustum corners.
*/
inline
void 
nFrustumClipper::SetLeftPlane(const plane &p)
{
    this->m_planes[LEFT_PLANE] = p;
}

//------------------------------------------------------------------------------
/**
    Get the frustum's right plane.
*/
inline
const plane &
nFrustumClipper::GetRightPlane() const
{
    return this->m_planes[RIGHT_PLANE];
}

//------------------------------------------------------------------------------
/**
    Set the frustum's right plane.
    Warning! This method doesn't update the frustum corners.
*/
inline
void 
nFrustumClipper::SetRightPlane(const plane &p)
{
    this->m_planes[RIGHT_PLANE] = p;
}

//------------------------------------------------------------------------------
/**
    Get the frustum's top plane.
*/
inline
const plane &
nFrustumClipper::GetTopPlane() const
{
    return this->m_planes[TOP_PLANE];
}

//------------------------------------------------------------------------------
/**
    Set the frustum's top plane.
    Warning! This method doesn't update the frustum corners.
*/
inline
void 
nFrustumClipper::SetTopPlane(const plane &p)
{
    this->m_planes[TOP_PLANE] = p;
}

//------------------------------------------------------------------------------
/**
    Get the frustum's bottom plane.
*/
inline
const plane &
nFrustumClipper::GetBottomPlane() const
{
    return this->m_planes[BOTTOM_PLANE];
}

//------------------------------------------------------------------------------
/**
    Set the frustum's bottom plane.
    Warning! This method doesn't update the frustum corners.
*/
inline
void 
nFrustumClipper::SetBottomPlane(const plane &p)
{
    this->m_planes[BOTTOM_PLANE] = p;
}

//------------------------------------------------------------------------------
/**
    get the near plane's index
*/
inline
int 
nFrustumClipper::GetNearPlaneIdx() const
{
    return this->nearPlaneIdx;
}

//------------------------------------------------------------------------------
/**
    get the far plane's index
*/
inline
int 
nFrustumClipper::GetFarPlaneIdx() const
{
    return this->farPlaneIdx;
}

//------------------------------------------------------------------------------
/**
    set the near plane's index
*/
inline
void 
nFrustumClipper::SetNearPlaneIdx(int idx)
{
    this->nearPlaneIdx = idx;
}

//------------------------------------------------------------------------------
/**
    set the far plane's index
*/
inline
void 
nFrustumClipper::SetFarPlaneIdx(int idx)
{
    this->farPlaneIdx = idx;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    generate the frustum corners. Useful for debugging.
*/
inline
void
nFrustumClipper::GenerateFrustumCorners(nCamera2 &camera, const matrix44 &viewtransform)
{
    float minx, maxx, miny, maxy, minz, maxz;
    camera.GetViewVolume(minx, maxx, miny, maxy, minz, maxz);
    float nfscale = maxz / minz;
    this->frustumcorners[0] = viewtransform * (vector3(minx,miny,-minz) * 1.0f);
    this->frustumcorners[1] = viewtransform * (vector3(minx,miny,-minz) * nfscale);
    this->frustumcorners[2] = viewtransform * (vector3(minx,maxy,-minz) * 1.0f);
    this->frustumcorners[3] = viewtransform * (vector3(minx,maxy,-minz) * nfscale);
    this->frustumcorners[4] = viewtransform * (vector3(maxx,miny,-minz) * 1.0f);
    this->frustumcorners[5] = viewtransform * (vector3(maxx,miny,-minz) * nfscale);
    this->frustumcorners[6] = viewtransform * (vector3(maxx,maxy,-minz) * 1.0f);
    this->frustumcorners[7] = viewtransform * (vector3(maxx,maxy,-minz) * nfscale);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nFrustumClipper::VisualizeFrustum(nGfxServer2 *gfx2, const vector4 &color) const
{
    // build two line strips of 8 points each; each line strip draws two faces of the frustum
    vector3 renderpoints[16] = 
    {
        this->frustumcorners[0], this->frustumcorners[1], this->frustumcorners[5], this->frustumcorners[4],
        this->frustumcorners[0], this->frustumcorners[2], this->frustumcorners[6], this->frustumcorners[4],
        this->frustumcorners[3], this->frustumcorners[1], this->frustumcorners[5], this->frustumcorners[7],
        this->frustumcorners[3], this->frustumcorners[2], this->frustumcorners[6], this->frustumcorners[7],
    };

    gfx2->BeginLines();
    gfx2->DrawLines3d(renderpoints, 8, color);
    gfx2->DrawLines3d(renderpoints+8, 8, color);
    gfx2->EndLines();
}

//------------------------------------------------------------------------------
/**
    generate the near plane corners. Useful for debugging.
*/
inline
int
nFrustumClipper::GenerateNearCorners(vector3 *corners)
{
    // calculate the intersection lines between the near plane and the others
    const plane &nearPlane = this->GetNearPlane();

    int numIndices(this->m_numplanes);
    if ( this->GetFarPlaneIdx() != -1 )
    {
        numIndices -= 1;
    }

    int *planesIndices = n_new_array(int, numIndices);
    int numPlanes(0);
    for ( int i(0); i < this->m_numplanes; ++i )
    {
        if ( i == this->nearPlaneIdx || i == this->farPlaneIdx )
        {
            continue;
        }

        planesIndices[numPlanes++] = i;
    }

    planesIndices[numPlanes] = planesIndices[0];

    for ( int i(0); i < numPlanes; ++i )
    {
        if ( !nearPlane.intersect(this->m_planes[planesIndices[i]], 
                                  this->m_planes[planesIndices[i+1]], 
                                  corners[i]) )
        {
            n_delete_array(planesIndices);
            return -1;
        }
    }

    n_delete_array(planesIndices);
    return numPlanes;
}

//------------------------------------------------------------------------------
/**
    generate the far plane corners. Useful for debugging.
*/
inline
int
nFrustumClipper::GenerateFarCorners(vector3 *corners)
{
    // calculate the intersection lines between the far plane and the others
    plane farPlane;
    int numIndices(this->m_numplanes);
    if ( this->GetFarPlaneIdx() != -1 )
    {
        farPlane = this->GetFarPlane();
        numIndices -= 1;
    }
    else
    {
        // there's no far plane, create one paralel to the near plane and very far
        farPlane = this->GetNearPlane();
        vector3 normal(farPlane.a, farPlane.b, farPlane.c);
        vector3 farPoint(vector3(0.f, 0.f, 0.f) + (normal * 1000.f));
        farPlane.a *= -1;
        farPlane.b *= -1;
        farPlane.c *= -1;
        farPlane.calculate_d(farPoint);
    }

    int *planesIndices = n_new_array(int, numIndices);
    int numPlanes(0);
    for ( int i(0); i < this->m_numplanes; ++i )
    {
        if ( i == this->nearPlaneIdx || i == this->farPlaneIdx )
        {
            continue;
        }

        planesIndices[numPlanes++] = i;
    }

    planesIndices[numPlanes] = planesIndices[0];

    for ( int i(0); i < numPlanes; ++i )
    {
        if ( !farPlane.intersect(this->m_planes[planesIndices[i]], 
                                this->m_planes[planesIndices[i+1]], 
                                corners[i]) )
        {
            n_delete_array(planesIndices);
            return -1;
        }
    }

    n_delete_array(planesIndices);
    return numPlanes;
}

//------------------------------------------------------------------------------
/**
    visualize the planes set
*/
inline
void 
nFrustumClipper::VisualizePlanesSet(const vector4 &color)
{
    // generate the near corners
    vector3 *nearCorners = n_new_array(vector3, this->m_numplanes);
    int numNearCorners(this->GenerateNearCorners(nearCorners));
    if ( numNearCorners == -1 )
    {
        n_delete_array(nearCorners);
        return;
    }

    nearCorners[numNearCorners] = nearCorners[0];

    // generate the far corners
    vector3 *farCorners = n_new_array(vector3, this->m_numplanes);
    int numFarCorners(this->GenerateFarCorners(farCorners));
    if ( numFarCorners == -1 )
    {
        n_delete_array(nearCorners);
        n_delete_array(farCorners);
        return;
    }

    farCorners[numFarCorners] = farCorners[0];

    nLineHandler lineHandler(nGfxServer2::LineStrip, nMesh2::Coord);

    matrix44 idMatrix;
    lineHandler.BeginLines(idMatrix);
    vector4 nearColor(color);
    nearColor *= 0.1f;
    nearColor.norm();
    lineHandler.DrawLines3d(nearCorners, 0, numNearCorners + 1, nearColor);
    lineHandler.EndLines();

    lineHandler.BeginLines(idMatrix);
    vector4 farColor(color);
    farColor *= 0.3f;
    farColor.norm();
    lineHandler.DrawLines3d(farCorners, 0, numFarCorners + 1, farColor);
    lineHandler.EndLines();

    vector3 l[2];
    l[0] = nearCorners[0];
    l[1] = farCorners[0];
    lineHandler.BeginLines(idMatrix);
    lineHandler.DrawLines3d(l, 0, 2, color);
    lineHandler.EndLines();

    l[0] = nearCorners[1];
    l[1] = farCorners[1];
    lineHandler.BeginLines(idMatrix);
    lineHandler.DrawLines3d(l, 0, 2, color);
    lineHandler.EndLines();

    l[0] = nearCorners[2];
    l[1] = farCorners[2];
    lineHandler.BeginLines(idMatrix);
    lineHandler.DrawLines3d(l, 0, 2, color);
    lineHandler.EndLines();

    l[0] = nearCorners[3];
    l[1] = farCorners[3];
    lineHandler.BeginLines(idMatrix);
    lineHandler.DrawLines3d(l, 0, 2, color);
    lineHandler.EndLines();

    n_delete_array(nearCorners);
    n_delete_array(farCorners);
}
#endif //!NGAME

#endif

