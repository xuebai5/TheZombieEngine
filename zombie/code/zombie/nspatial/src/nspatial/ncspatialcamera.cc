#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialcamera.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialoccluder.h"
#include "zombieentity/nctransform.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nfrustumclipper.h"
#include "nspatial/nspatialvisitor.h"
#include "nspatial/ncspatiallight.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialCamera, ncSpatial);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialCamera)
    NSCRIPT_ADDCMD_COMPOBJECT('RSTD', void, SetMaxTestOutdoorDist, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGTD', float, GetMaxTestOutdoorDist, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOR', void, SetOcclusionRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGOR', float, GetOcclusionRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSMO', void, SetMaxNumOccluders, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGMO', int, GetMaxNumOccluders, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSAM', void, SetAreaAngleMin, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGAM', float, GetAreaAngleMin, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSUC', void, SetUseCameraCollector, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGUC', bool, GetUseCameraCollector, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('RSAG', void, SetAttachGlobalEntities, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGAG', bool, GetAttachGlobalEntities, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor 1
*/
ncSpatialCamera::ncSpatialCamera() : 
ncSpatial(),
testEntitiesMaxSqDist(FLT_MAX),
m_isOn(false),
m_occlusionRadius(160.f),
m_areaAngleMin(0.005f),
useCameraCollector(false),
#ifndef NGAME
attachGlobalEntities(true),
debuggingHorizon(false),
#endif // !NGAME
m_maxNumOccluders(4)
{
    this->m_spatialComponentType = ncSpatial::N_SPATIAL_CAMERA;
    this->m_collector = n_new(nSpatialCameraCollector);
    this->m_width  = nGfxServer2::Instance()->GetDisplayMode().GetWidth();
    this->m_height = nGfxServer2::Instance()->GetDisplayMode().GetHeight();
}

//------------------------------------------------------------------------------
/**
    make the area-angle test to an occluder to know if it's a good occluder,
    and if it is, store it.

    Returns true if the occluder passed the area-angle test and was stored.
*/
bool 
ncSpatialCamera::ConsiderOccluder(ncSpatialOccluder *occluder)
{
    ncTransform *trComp = this->GetComponent<ncTransform>();
    quaternion cameraQuat = trComp->GetQuat();
    vector3 cameraDir(cameraQuat.rotate(vector3(0.f, 0.f, 1.f)));
    vector3 camToOccluderVect(trComp->GetPosition() - occluder->GetCenter());

    float minDist(occluder->GetMinSqDist());
    float camToOccluderDist(camToOccluderVect.lensquared());

    if ( (minDist != -1) && (camToOccluderDist < minDist) )
    {
#ifndef NGAME
            ncSpatialCamera::SelectedOccluder *discSO = n_new(ncSpatialCamera::SelectedOccluder);
            discSO->m = -1.f;
            discSO->oc = occluder;
            this->m_discardedOccluders.Append(discSO);
#endif // !NGAME
        return false;
    }

    float maxDist(occluder->GetMaxSqDist());
    if ( (maxDist != -1) && (camToOccluderDist > maxDist) )
    {
#ifndef NGAME
            ncSpatialCamera::SelectedOccluder *discSO = n_new(ncSpatialCamera::SelectedOccluder);
            discSO->m = -1.f;
            discSO->oc = occluder;
            this->m_discardedOccluders.Append(discSO);
#endif // !NGAME
        return false;
    }

    plane occluderPlane = occluder->GetPlane();
    if (occluderPlane.distance(trComp->GetPosition()) >= 0)
    {
        if (occluder->GetDoubleSided())
        {
            occluder->ChangeActiveSide();
        }
        else
        {
#ifndef NGAME
            ncSpatialCamera::SelectedOccluder *discSO = n_new(ncSpatialCamera::SelectedOccluder);
            discSO->m = -1.f;
            discSO->oc = occluder;
            this->m_discardedOccluders.Append(discSO);
#endif // !NGAME
            // the camera is in the shadow of the occluder, discard it
            return false;
        }
    }

    float m = -occluder->GetArea() * (occluderPlane.normal() % cameraDir) / camToOccluderDist;

    if ( m > this->GetAreaAngleMin() )
    {
        // the occluder is good enough. Insert it in the corresponding position.
        // (the selected occluders array is sorted from the best one to the worst one)
        int i;
        for (i = 0; i < this->m_selectedOccluders.Size(); i++)
        {
            if (this->m_selectedOccluders[i]->m < m)
            {
                break;
            }
        }

        if ( i < this->GetMaxNumOccluders() )
        {
            ncSpatialCamera::SelectedOccluder *newSO = n_new(ncSpatialCamera::SelectedOccluder);
            newSO->m = m;
            newSO->oc = occluder;
            this->m_selectedOccluders.Insert(i, newSO);

            if (this->m_selectedOccluders.Size() > this->GetMaxNumOccluders())
            {
                // the selected occluders array is full. Remove the last one, this is, the worst one
                ncSpatialCamera::SelectedOccluder *back = this->m_selectedOccluders.Back();
#ifndef NGAME
                ncSpatialCamera::SelectedOccluder *discSO = n_new(ncSpatialCamera::SelectedOccluder);
                discSO->m = back->m;
                discSO->oc = back->oc;
                this->m_discardedOccluders.Append(discSO);
#endif // !NGAME
                n_delete(back);
                this->m_selectedOccluders.EraseQuick(this->m_selectedOccluders.Size() - 1);
            }

            return true;
        }
        else
        {
#ifndef NGAME
            ncSpatialCamera::SelectedOccluder *discSO = n_new(ncSpatialCamera::SelectedOccluder);
            discSO->m = m;
            discSO->oc = occluder;
            this->m_discardedOccluders.Append(discSO);
#endif // !NGAME
            return false;
        }
    }
    else
    {
        // the occluder is not good enough
#ifndef NGAME
        ncSpatialCamera::SelectedOccluder *discSO = n_new(ncSpatialCamera::SelectedOccluder);
        discSO->m = m;
        discSO->oc = occluder;
        this->m_discardedOccluders.Append(discSO);
#endif // !NGAME
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    reset the array of occlusion frustums
*/
void 
ncSpatialCamera::ResetOcclusionFrustums()
{
    int numFrustums = this->m_occlusionFrustums.Size();
    for (int i = 0; i < numFrustums; i++)
    {
        n_delete(this->m_occlusionFrustums.At(i));
    }

    this->m_occlusionFrustums.Reset();
}

//------------------------------------------------------------------------------
/**
    update camera
*/
void 
ncSpatialCamera::Update(const matrix44 &worldMatrix, int frameId)
{
    n_assert2((this->GetType()== ncSpatial::N_SPATIAL_CAMERA),
              "Error: Spatial component should be a spatial camera component!");

    // if the camera is on, update it
    if (this->IsOn())
    {
        // update frame id
        this->m_frameId = frameId;

        // update position
        this->SetViewMatrix(worldMatrix);

        // update view projection matrix
        this->m_vpDirty = true;

        nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, 
                                                              worldMatrix.pos_component());
    }
}

//------------------------------------------------------------------------------
/**
    update camera's projection
*/
void 
ncSpatialCamera::Update(const nCamera2 &camera)
{
    // if the camera is on, update it
    if (this->IsOn())
    {
        // set the new camera's projection
        this->SetCamera(camera);

        // update view projection matrix
        this->m_vpDirty = true;
    }
}

//------------------------------------------------------------------------------
/**
    build occlusion frustums using the selected occluders
*/
void 
ncSpatialCamera::BuildOcclusionFrustums()
{
    this->ResetOcclusionFrustums();

    int numOccluders = this->m_selectedOccluders.Size();
    ncSpatialOccluder *currOccluder = 0;
    bool occluded = false;
    struct ClipVertexInfo vertsInfo[4];
    for (int i = 0; i < numOccluders; i++)
    {
        // check if the current occluder is occluded by the others that are already created
        currOccluder = this->m_selectedOccluders[i]->oc;
        occluded = false;
        int numFrustums = this->m_occlusionFrustums.Size();
        for (int j = 0; j < numFrustums; j++)
        {
            VisitorFlags of(false, false);
            of = this->m_occlusionFrustums[j]->TestPortalVertices(currOccluder->GetVertices(),
                                                                  4,
                                                                  of,
                                                                  vertsInfo);
            if (of.TestResult())
            {
                // the current occluder is occluded by another one, so, don't use it
                occluded = true;
                break;
            }
        }

        if (!occluded)
        {
            // the occluder is not occluded by others, so create its frustum and append it
            this->m_occlusionFrustums.Append(this->BuildNewFrustum(currOccluder));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Builds a new frustum using the occluder's vertices. 
    This method is called when the occluder (vertices)
    is completely inside the frustum.
*/
nFrustumClipper *
ncSpatialCamera::BuildNewFrustum(ncSpatialOccluder *occluder)
{
    n_assert(occluder);

    const vector3 *vertices = occluder->GetVertices();
    n_assert2(vertices, "miquelangel.rujula: NULL pointer to occluder vertices!");

    ncTransform* trComp = this->GetComponent<ncTransform>();
    const vector3 &camPos = trComp->GetPosition();
    plane newClipPlanes[8];
    int numNewClipPlanes = 0;
    
    // set the near plane
    n_assert(numNewClipPlanes == 0);
    newClipPlanes[numNewClipPlanes] = occluder->GetPlane();
    int nearPlaneIdx = numNewClipPlanes;
    numNewClipPlanes++;

    int i2;
    // initialize vertex index 1 to the first vertex (in the loop)
    for (int i1 = 0; i1 < 4; i1++)
    {
        if (i1 == 2 && !occluder->GetFly())
        {
            // don't create down plane
            continue;
        }

        i2 = (i1+1) % 4;
        n_assert(numNewClipPlanes < 5);
        newClipPlanes[numNewClipPlanes].set(camPos, vertices[i1], vertices[i2]);
        numNewClipPlanes++;
    }

    //n_assert(numNewClipPlanes == 5);
    nFrustumClipper *newFrustum = n_new(nFrustumClipper(newClipPlanes, numNewClipPlanes));
    newFrustum->SetNearPlaneIdx(nearPlaneIdx);

    return newFrustum;
}

//------------------------------------------------------------------------------
/**
    update the view projection matrix
*/
void 
ncSpatialCamera::UpdateViewProjection()
{
    this->m_viewProjection = this->m_viewMatrix;
    this->m_viewProjection.invert();
    this->m_viewProjection *= this->m_camera.GetProjection();
}

//------------------------------------------------------------------------------
/**
    append a visible light to this camera
*/
void 
ncSpatialCamera::AppendVisibleLight(nEntityObject *light)
{
    n_assert2(light, "miquelangel.rujula: NULL pointer to light entity!");
    n_assert2(light->GetComponent<ncSpatialLight>(), 
              "miquelangel.rujula: expected light entity!");

    this->m_collector->Accept(light);
}

//------------------------------------------------------------------------------
/**
    Visibility visitor processing for a basic camera.
*/
void 
ncSpatialCamera::Accept(nVisibleFrustumVisitor &/*visitor*/)
{
    // do nothing
}

//------------------------------------------------------------------------------
/**
    actually, cameras, are ignored for spatial queries
*/
void 
ncSpatialCamera::Accept(nSpatialVisitor &/*visitor*/)
{
    // do nothing
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialCamera::SaveCmds(nPersistServer * ps)
{
    if ( !ncSpatial::SaveCmds(ps) )
    {
        return false;
    }

    if (!ps->Put(this->GetEntityObject(), 'RSOR', this->GetOcclusionRadius()))
    {
        return false;
    }

    if (!ps->Put(this->GetEntityObject(), 'RSMO', this->GetMaxNumOccluders()))
    {
        return false;
    }

    if (!ps->Put(this->GetEntityObject(), 'RSAM', this->GetAreaAngleMin()))
    {
        return false;
    }

    return true;
}
