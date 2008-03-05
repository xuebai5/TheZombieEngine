#ifndef N_NFLOATMAP_BUILDER_H
#define N_NFLOATMAP_BUILDER_H
/*-----------------------------------------------------------------------------
    @file nfloatmapbuilder.h
    @class nFloatMapBuilder
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nFloatMapBuilder class to edit and persist heightmaps

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/nfloatmapbuilder.h"

//------------------------------------------------------------------------------
/**
    Class nFloatMapBuilder
*/
class nFloatMapBuilder
{
public:
    /// constructor
    nFloatMapBuilder();

    /// destructor
    ~nFloatMapBuilder();
    
    /// Clamp the floatmap values to [0, 1]
    void Clamp(nFloatMap *map);
    /// Clamp the floatmap to [0, 1], only in the region covered by the parameter floatmap placed at (x,z)
    void Clamp(nFloatMap *src, int x, int z, nFloatMap *dest);
    /// Adds a constant value to the heightmap
    void Add(nFloatMap *map, float factor);
    /// Multiplies the heightmap by a constant value
    void Mult(nFloatMap *map, float factor);
    /// Copies the heightmap values into 'dest' at (x,z)
    void Copy(nFloatMap *src, int x, int z, nFloatMap* dest);
    /// Adds the heightmap into 'dest' at (x,z)
    void Add(nFloatMap *src, int x, int z, nFloatMap* dest);
    /// Subtracts the heightmap from 'dest' at (x,z)
    void Sub(nFloatMap *src, int x, int z, nFloatMap* dest);
    /// Multiplies two heightmaps and a scalar factor
    void Mult(nFloatMap *src, int x, int z, float factor, nFloatMap* dest);
    /// Multiplies the heightmap by factor and adds into 'dest' at (x,z)
    void MultAdd(nFloatMap *src, int x, int z, float factor, nFloatMap* dest);
    /// Multiplies the heightmap by factor and subs from 'dest' at (x,z)
    void MultSub(nFloatMap *src, int x, int z, float factor, nFloatMap* dest);
    /// Adds the heightmap to 'dest' at (x,z) performing a weighting factor
    void WeightedMean(nFloatMap *src, int x, int z, float sourceWeight, nFloatMap* dest);
    /// Adds the heightmap to 'dest' at (x,z) performing a weighting factor, using a third heightmap as lerp
    void WeightedMean(nFloatMap *src, int x, int z, nFloatMap* sourceWeight, nFloatMap* dest, int margin = 0);
    /// Adds the heightmap to 'dest' at (x,z) performing a weighting factor, using a third heightmap and a constant as lerp
    void WeightedMean(nFloatMap *src, int x, int z, float constSourceWeight, nFloatMap* sourceWeight, nFloatMap* dest, int margin = 0);
    /// Applies the heightmap to 'dest' at (x,z) performing the max function
    void Max(nFloatMap *src, int x, int z, nFloatMap* dest);
    /// Applies the heightmap to 'dest' at (x,z) performing the min function
    void Min(nFloatMap *src, int x, int z, nFloatMap* dest);
    /// Applies the heightmap to 'dest' at (x,z) performing a simple smoothing
    void Smooth(nFloatMap *src, nFloatMap* dest);
    /// Applies the heightmap to 'dest' at (x,z) performing a smoothing with intensity
    void Smooth(nFloatMap *src, int x, int z, float factor, nFloatMap* temp, nFloatMap* dest);

    /// Generate a texture from the heightmap
    nTexture2* GenerateTexture( nFloatMap *src, int size, int bitDepth = 8 );

protected:

    /// Clips a rectangle in HC to the heightmap dimensions
    bool ClipRectangle(nFloatMap *map, int &x0, int &z0, int &x1, int &z1, int margin = 0);
    /// Setup variables for loops
    bool SetupLoop(nFloatMap *src, int x, int z, nFloatMap* dest, int &x0, int &z0, int &x1, int &z1, float** porig, float** pdest, int &incOrig, int &incDest, int margin = 0);
  
};

//------------------------------------------------------------------------------
#endif N_NFLOATMAP_BUILDER_H
