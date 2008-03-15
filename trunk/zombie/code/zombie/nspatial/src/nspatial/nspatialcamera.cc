#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nspatialcamera.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/nspatialcamera.h"

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialCamera::ReadXmlElement(const TiXmlElement *spatialCamElem)
{
    n_assert2(spatialCamElem, "miquelangel.rujula");

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialCamera::FillXmlElement(TiXmlElement *spatialCamElem)
{
    n_assert2(spatialCamElem, "miquelangel.rujula");

    return false;
}

//------------------------------------------------------------------------------
/**
    Visibility visitor processing for a basic camera.
*/
void 
nSpatialCamera::Accept(nVisibilityVisitor &/*visitor*/, int /*recursiondepth*/)
{
    //visitor.Visit(this, recursiondepth);
}

//------------------------------------------------------------------------------
/**
    actually, cameras, are ignored for spatial queries
*/
void 
nSpatialCamera::Accept(nSpatialVisitor &/*visitor*/, int /*recursiondepth*/)
{
}

