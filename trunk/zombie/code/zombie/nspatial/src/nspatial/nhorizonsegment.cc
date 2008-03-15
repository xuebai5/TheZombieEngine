//------------------------------------------------------------------------------
//  nchorizonsegment.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnspatial.h"
#include "nspatial/nhorizonsegment.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
*/
nHorizonSegment::nHorizonSegment() :
frameId(-1),
lastCam(nEntityObjectServer::IDINVALID)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nHorizonSegment::~nHorizonSegment()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void
nHorizonSegment::InitSegmentX(int bx, int bz, float sidesize, float y0, float y1)
{
    this->v0.set(bx * sidesize, y0, bz * sidesize);
    this->v1.set((bx + 1) * sidesize, y1, bz * sidesize);
}

//------------------------------------------------------------------------------
/**
*/
void
nHorizonSegment::InitSegmentZ(int bx, int bz, float sidesize, float y0, float y1)
{
    this->v0.set(bx * sidesize, y0, bz * sidesize);
    this->v1.set(bx * sidesize, y1, (bz + 1) * sidesize);
}

//------------------------------------------------------------------------------
/**
*/
const vector3 & 
nHorizonSegment::GetStart() const
{
    return this->v0;
}

//------------------------------------------------------------------------------
/**
*/
const vector3 & 
nHorizonSegment::GetEnd() const
{
    return this->v1;
}
