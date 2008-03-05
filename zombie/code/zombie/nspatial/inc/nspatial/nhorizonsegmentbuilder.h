#ifndef N_HORIZONSEGMENTBUILDER_H
#define N_HORIZONSEGMENTBUILDER_H
//------------------------------------------------------------------------------
/**
    @file nhorizonsegmentbuilder.h
    @class nHorizonSegmentBuilder
    @ingroup NebulaSpatialSystem

    @author Mateu Batle Sastre

    @brief This class helps building horizon segments. It provides the parameters
    to setup the horizon segments, it does not return nHorizonSegments directly.

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/nfloatmap.h"
#include "mathlib/nleastsquaresline.h"

//------------------------------------------------------------------------------
class nHorizonSegmentBuilder
{
public:
    /// constructor
    nHorizonSegmentBuilder();
    /// destructor
    ~nHorizonSegmentBuilder();

    /// Setup the builder before start building
    void Setup(nFloatMap * heightMap, int blockSize, float sidesize);

    /// Build horizon segment information for a whole terrain
    void Build(nEntityClass * ec);

    /// Build horizon segment along x for block (bx, bz)
    void BuildHorizonSegmentX(int bx, int bz, float & y0, float & y1);
    /// Build horizon segment along z for block (bx, bz)
    void BuildHorizonSegmentZ(int bx, int bz, float & y0, float & y1);

    /// Save data
    bool SaveChunk(nEntityClass * ec);

private:

    int BlockToHeightMapCoord(int a);

    typedef nLeastSquaresLine<float> nLSQ;

    nRef<nFloatMap> refHM;
    int blockSize;
    float sidesize;
};

//------------------------------------------------------------------------------
#endif // N_HORIZONSEGMENTBUILDER_H
