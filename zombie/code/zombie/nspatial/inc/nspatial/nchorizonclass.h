#ifndef NC_HORIZONCLASS_H
#define NC_HORIZONCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncHorizonClass
    @ingroup NebulaSpatialSystem
    @author Mateu Batle i Sastre

    @brief Class component for horizon info

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class nHorizonSegment;

//------------------------------------------------------------------------------
class ncHorizonClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncHorizonClass, nComponentClass);

public:

    /// constructor
    ncHorizonClass();

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// load resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// are resources loaded & valid
    bool IsValid();
    /// return file where horizon segments are stored
    nString GetHorizonFilename() const;

    /// Set side size scale for the block (must be set previous to load resources)
    void SetSideSizeScale(float sidesize);

    /// Get horizon segment
    nHorizonSegment * GetHorizonSegment(int bx, int bz, bool alongx) const;

    /// begin horizon segments
    void BeginHorizonSegments(int, int);
    /// Command used for persistence only
    void AddHorizonSegment(int, int, bool, float, float);
    /// end horizon segments
    void EndHorizonSegments();

    /// get number of segments along x
    int GetNumSegmentsX() const;
    /// get number of segments along z
    int GetNumSegmentsZ() const;

protected:

    /// destructor
    virtual ~ncHorizonClass();

    /// load the horizon segment data for the whole terrain
    bool LoadChunk();

    /// block side size
    float sidesize;
    /// 3d array indexed as [bz][bx]
    nHorizonSegment * horizonX;
    /// 3d array indexed as [bz][bx]
    nHorizonSegment * horizonZ;
    /// number of segments in each direction
    int numSegmentsX;
    int numSegmentsZ;

};

//------------------------------------------------------------------------------
#endif // NC_HORIZONCLASS_H
