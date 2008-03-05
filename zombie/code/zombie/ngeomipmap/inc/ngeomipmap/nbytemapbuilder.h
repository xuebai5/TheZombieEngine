#ifndef N_NBYTEMAP_BUILDER_H
#define N_NBYTEMAP_BUILDER_H
//------------------------------------------------------------------------------
/**
    @file nbytemapbuilder.h
    @class nByteMapBuilder
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nByteMapBuilder class to edit and persist bytemaps

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/nbytemap.h"
#include "ngeomipmap/nbytemapbuilder.h"

//------------------------------------------------------------------------------
/**
    Class nByteMapBuilder
*/
class nByteMapBuilder
{
public:
    /// constructor
    nByteMapBuilder();

    /// destructor
    ~nByteMapBuilder();

    /// save a bytemap through Open IL
    bool SaveByteMapOpenIL(nByteMap * src, const char * fileName);

    /// Copy bytemap
    void Copy(nByteMap *src, nByteMap* dest);
    /// Multiplies a floatmap by factor and adds into bytemap 'dest' at (x,z)
    void MultAdd(nFloatMap *src, int x, int z, float factor, nByteMap* dest, nuint8 minValue = 0);
    /// Multiplies a floatmap by factor and subs from bytemap 'dest' at (x,z)
    void MultSub(nFloatMap *src, int x, int z, float factor, nByteMap* dest);
    /// Substitute the bytemap's values where the source floatmap is higher than 0.5
    void Substitute(nFloatMap *src, int x, int z, nuint8 value, nByteMap *dest);

    /// Generate a texture from the bytemap
    nTexture2* GenerateTexture( nByteMap *src, int size );
    /// Copy the bytemap to texture with same size
    static void FastCopyToTexture(nTexture2* dst,nByteMap *src );

protected:

    /// Clips a rectangle to the heightmap dimensions
    bool ClipRectangle(nByteMap *map, int &x0, int &z0, int &x1, int &z1, int margin = 0);
    /// Setup variables for loops
    bool SetupLoop(nByteMap *src, int x, int z, nByteMap* dest, int &x0, int &z0, int &x1, int &z1, nuint8** porig, nuint8** pdest, int &incOrig, int &incDest, int margin = 0);
    /// Setup variables for loops
    bool SetupLoop(nFloatMap *src, int x, int z, nByteMap* dest, int &x0, int &z0, int &x1, int &z1, float** porig, nuint8** pdest, int &incOrig, int &incDest, int margin = 0);
};

//------------------------------------------------------------------------------
#endif N_NBYTEMAP_BUILDER_H
