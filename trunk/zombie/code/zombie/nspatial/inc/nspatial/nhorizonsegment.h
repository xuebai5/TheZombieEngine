#ifndef N_HORIZONSEGMENT_H
#define N_HORIZONSEGMENT_H
//------------------------------------------------------------------------------
/**
    @file nhorizonsegment.h
    @class nHorizonSegment
    @ingroup NebulaSpatialSystem

    @author Mateu Batle Sastre

    @brief This class represents a (Talbot) horizon segment, used by occlusion.

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nspatial/nspatialserver.h"

//------------------------------------------------------------------------------
class nHorizonSegment
{
public:
    /// constructor
    nHorizonSegment();
    /// destructor
    ~nHorizonSegment();

    /// Initialize as segment along x
    void InitSegmentX(int bx, int bz, float sidesize, float y0, float y1);
    /// Initialize as segment along z
    void InitSegmentZ(int bx, int bz, float sidesize, float y0, float y1);

    /// Get starting point of the occluder segment
    const vector3 & GetStart() const;
    /// Get end point of the occluder segment
    const vector3 & GetEnd() const;

    /// Set used by the given camera in the current frame
    void SetUsedBy(nEntityObjectId camId);

    /// Says if this segments has already been used in this frame by the given camera
    bool IsUsedBy(nEntityObjectId camId);

private:

    /// starting point of the occluder segment
    vector3 v0;
    /// end point of the occluder segment
    vector3 v1;

    /// id of the frame that the element was visible for the last camera
    int frameId;
    /// id of the last camera that the element was visible for
    nEntityObjectId lastCam;

};

//------------------------------------------------------------------------------
/**
    Set used by the given camera in the current frame
*/
inline
void 
nHorizonSegment::SetUsedBy(nEntityObjectId camId)
{
    this->frameId = nSpatialServer::Instance()->GetFrameId();
    this->lastCam = camId;
}

//------------------------------------------------------------------------------
/**
    Says if this segments has already been used in this frame by the given camera
*/
inline
bool 
nHorizonSegment::IsUsedBy(nEntityObjectId camId)
{
    return (this->frameId == nSpatialServer::Instance()->GetFrameId() &&
            this->lastCam == camId);
}

//------------------------------------------------------------------------------
#endif // N_HORIZONSEGMENT_H
