//------------------------------------------------------------------------------
//  nfloatmapbuilder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nFloatMapBuilder::nFloatMapBuilder()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nFloatMapBuilder::~nFloatMapBuilder()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Clip a rectangle to the heightmap dimensions, telling if there was an intersection.
    @return true if clipping was done
*/
bool
nFloatMapBuilder::ClipRectangle(nFloatMap *map, int &x0, int &z0, int &x1, int &z1, int margin)
{
    if (x0 > x1)
	{
        int temp = x0;
        x0 = x1;
        x1 = temp;
    }
    if (z0 > z1)
	{
        int temp = z0;
        z0 = z1;
        z1 = temp;
    }

    if ((x0 >= map->GetSize() - margin) || (x1 < margin) || (z0 >= map->GetSize()- margin) || (z1 < margin))
	{
        return false;
    }

    x0 = n_max( margin , x0 );
    x1 = n_min( map->GetSize() - margin, x1 );
	z0 = n_max( margin, z0 );
    z1 = n_min( map->GetSize() - margin, z1 );

    return true;
}
//------------------------------------------------------------------------------
/**
    Setup variables for loops.
*/
bool
nFloatMapBuilder::SetupLoop(nFloatMap *src, int x, int z, nFloatMap* dest, int &x0, int &z0, int &x1, int &z1, float** porig, float** pdest, int &incOrig, int &incDest, int margin)
{
    n_assert(dest);
    n_assert(porig);
    n_assert(pdest);

    x0 = x;
    z0 = z;
    x1 = x0 + src->GetSize();
    z1 = z0 + src->GetSize();
    if (! ClipRectangle(dest, x0, z0, x1, z1, margin))
	{
        return false;
    }

    *pdest += z0 * dest->GetSize() + x0;
    if (x0 != x)
	{
        x = x0 - x;
    } 
    else 
    {
        x = 0;
    }
    if (z0 != z)
	{
        z = z0 - z;
    } 
    else 
    {
        z = 0;
    }
    *porig += z * src->GetSize() + x;
    
    incDest = dest->GetSize() - ( x1 - x0 );
    incOrig = src->GetSize() - ( x1 - x0 );

    return true;
}
//------------------------------------------------------------------------------
/**
    Clamp the heightmap values to valid range
*/
void
nFloatMapBuilder::Clamp(nFloatMap *map)
{
    n_assert(map && map->IsLoaded());
    float *p = map->GetHeightMap();
    int np = map->GetSize() * map->GetSize();
    for (int i = 0; i < np; i++)
	{
        *p++ = max( map->GetHeightOffset(), min( map->GetMaxHeight(), *p) );
    }

    // make dirty
    map->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Clamp the heightmap to valid range, only in the region covered by the parameter heightmap placed at (x,z)
    @param region The heightmap used as region to clamp
*/
void
nFloatMapBuilder::Clamp(nFloatMap *src, int x, int z, nFloatMap *region)
{
    n_assert(src);
    n_assert(src->IsLoaded());
    n_assert(region);
    n_assert(region->IsLoaded());

    int x0,z0,x1,z1;
    float *porig = src->GetHeightMap();
    float *pdest = region->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(region, x, z, src, x0, z0, x1, z1, &pdest, &porig, incDest, incOrig ) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *porig++ = max( src->GetHeightOffset(), min( src->GetMaxHeight(), *porig) );
        } 
        porig += incOrig;
    }

    // make dirty
    src->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Multiply the heightmap by a constant value   
*/
void
nFloatMapBuilder::Mult(nFloatMap *map, float factor)
{
    n_assert(map);
    n_assert(map->IsLoaded());
    float *p = map->GetHeightMap();
    int np = map->GetSize() * map->GetSize();
    for (int i = 0; i < np; i++)
	{
        *p++ = *p * factor;
    }

    // make dirty
    map->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Copy the heightmap values into 'dest'

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  dest    Destination heightmap
*/
void
nFloatMapBuilder::Copy(nFloatMap *src, int x, int z, nFloatMap *dest)
{
    n_assert(src);
    n_assert(src->IsLoaded());
    n_assert(dest);
    n_assert(dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest++ = *porig++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Add the heightmap into 'dest'

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  dest    Destination heightmap
*/
void
nFloatMapBuilder::Add(nFloatMap *src, int x, int z, nFloatMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
    float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest++ = *pdest + *porig++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Subtract the heightmap from 'dest' at (x,z)

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  dest    Destination heightmap

*/
void
nFloatMapBuilder::Sub(nFloatMap *src, int x, int z, nFloatMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest++ = *pdest - *porig++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Multiply two heightmaps and a factor leaving the result in 'dest', at (x,z)

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  dest    Destination heightmap

*/
void
nFloatMapBuilder::Mult(nFloatMap *src, int x, int z, float factor, nFloatMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest = *porig++ * (*pdest) * factor;
            pdest++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Multiply the heightmap by factor and adds into 'dest' at (x,z)

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  factor    Multiplication factor
    @param  dest    Destination heightmap

*/
void
nFloatMapBuilder::MultAdd(nFloatMap *src, int x, int z, float factor, nFloatMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest++ = *pdest + *porig++ * factor;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Multiplies the heightmap by factor and subs it from 'dest' at (x,z)

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  factor    Multiplication factor
    @param  dest    Destination heightmap

*/
void
nFloatMapBuilder::MultSub(nFloatMap *src, int x, int z, float factor, nFloatMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap(), *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest++ = *pdest - *porig++ * factor;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Adds the heightmap to 'dest' at (x,z) performing a weighting factor

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  sourceWeight Constant used as lerp weight of source heightmap
    @param  dest    Destination heightmap
        
*/
void
nFloatMapBuilder::WeightedMean(nFloatMap *src, int x, int z, float sourceWeight, nFloatMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    float destWeight = 1 - sourceWeight;

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest = *porig * sourceWeight + *pdest * destWeight;
            pdest++;
            porig++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Adds the heightmap to 'dest' at (x,z) performing a weighting factor, using a third heightmap as lerp

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  sourceWeight Heightmap used as lerp weights (of the destination)
    @param  dest    Destination heightmap

    The sourceWeight heightmap must be the same size as src.
        
*/
void
nFloatMapBuilder::WeightedMean(nFloatMap *src, int x, int z, nFloatMap* sourceWeight, nFloatMap *dest, int margin)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());
    n_assert(sourceWeight);
    n_assert(src->GetSize() == sourceWeight->GetSize());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
	float *plerp = sourceWeight->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig, incLerp;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest, margin) )
	{
        return;
    }
    incLerp = incOrig;
    plerp += porig - src->GetHeightMap();

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest = *porig * *plerp + *pdest * ( 1.0f - *plerp );
            pdest++;
            porig++;
            plerp++;
        }
        pdest += incDest;
        porig += incOrig;
        plerp += incLerp;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Adds the heightmap to 'dest' at (x,z) performing a weighting factor, using a third heightmap and a constant as lerp

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  sourceWeight Heightmap used as lerp weights (of the destination)
    @param  dest    Destination heightmap
    @param constSourceWeight Additional constant factor for lerp (source weight)

    The sourceWeight heightmap must be the same size as src.
        
*/
void
nFloatMapBuilder::WeightedMean(nFloatMap *src, int x, int z, float constSourceWeight, nFloatMap* sourceWeight, nFloatMap* dest, int margin)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());
    n_assert(sourceWeight);
    n_assert(src->GetSize() == sourceWeight->GetSize());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
	float *plerp = sourceWeight->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig, incLerp;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest, margin) )
	{
        return;
    }
    incLerp = incOrig;
    plerp += porig - src->GetHeightMap();

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            float plerptot = constSourceWeight * *plerp;
            *pdest = *porig * plerptot + *pdest * ( 1.0f - plerptot );
            pdest++;
            porig++;
            plerp++;
        }
        pdest += incDest;
        porig += incOrig;
        plerp += incLerp;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Applies the heightmap to 'dest' at (x,z) performing the max function
    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  dest    Destination heightmap

*/
void
nFloatMapBuilder::Max(nFloatMap *src, int x, int z, nFloatMap *dest)
{
    n_assert(src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest = max(*pdest, *porig);
            pdest++;
            porig++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Applies the heightmap to 'dest' at (x,z) performing the min function
    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  dest    Destination heightmap

*/
void
nFloatMapBuilder::Min(nFloatMap *src, int x, int z, nFloatMap *dest)
{
    n_assert(src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    float *pdest = dest->GetHeightMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            *pdest = min(*pdest, *porig);
            pdest++;
            porig++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Applies the heightmap to 'dest' at (x,z) performing a smoothing with intensity. A temporary heightmap is needed.
    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  factor   Intensity factor (multiplies the smoothing), in [0, 1]
    @param  temp    Temporary heightmap, must be the same size as src.
    @param  dest    Destination heightmap

    The temporary heightmap must be the same size as src.

*/
void
nFloatMapBuilder::Smooth(nFloatMap *src, int x, int z, float factor, nFloatMap* temp, nFloatMap* dest)
{
    n_assert(src->IsLoaded());
    n_assert(temp);
    n_assert(dest);
    n_assert(temp->GetSize() == src->GetSize());

    int x0,z0,x1,z1;
	float *pdest = dest->GetHeightMap();
    float *ptemp = temp->GetHeightMap();
    int incTemp, incDest;

    if (! SetupLoop(temp, x, z, dest, x0, z0, x1, z1, &ptemp, &pdest, incTemp, incDest) )
	{
        return;
    }
    
    int destSize = dest->GetSize();

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            int numNeighbours = 0;
            *ptemp =  0.0f;
            if (i > x0)
            {
                *ptemp += *( pdest - 1 );
                numNeighbours ++;
            }
            if (i < x1-1)
            {
                *ptemp += *( pdest + 1 );
                numNeighbours ++;
            }
            if (j > z0)
            {
                *ptemp += *( pdest - destSize );
                numNeighbours ++;
            }
            if (j < z1-1)
            {
                *ptemp += *( pdest + destSize );
                numNeighbours ++;
            }

            *ptemp += *pdest;
            numNeighbours ++;
            *ptemp /= numNeighbours;
            
            ptemp++;
            pdest++;
        }
        ptemp += incTemp;
        pdest += incDest;
    }

    // Now do a lerp between the smoothed temp heightmap and the 'dest' one, using 'src' one as weighting factors
    WeightedMean(temp, x,z, factor, src, dest);

    // make dirty
    dest->SetDirty();
    temp->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Smoothes src heightmap and puts the result in another one
    @param  dest Dest heightmap, must be the same size as src.

*/
void
nFloatMapBuilder::Smooth(nFloatMap *src, nFloatMap* dest)
{
    n_assert(src->IsLoaded());
    n_assert(dest && dest->IsLoaded());
    n_assert(dest->GetSize() == src->GetSize());

    int x0,z0,x1,z1;
	float *pdest = dest->GetHeightMap();
    float *ptemp = src->GetHeightMap();
    int incTemp, incDest;

    if (! SetupLoop(src, 0, 0, dest, x0, z0, x1, z1, &ptemp, &pdest, incTemp, incDest) )
	{
        return;
    }
    
    int destSize = src->GetSize();

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            int numNeighbours = 0;
            *pdest =  0.0f;
            if (i > x0)
            {
                *pdest += *( ptemp - 1 );
                numNeighbours ++;
            }
            if (i < x1-1)
            {
                *pdest += *( ptemp + 1 );
                numNeighbours ++;
            }
            if (j > z0)
            {
                *pdest += *( ptemp - destSize );
                numNeighbours ++;
            }
            if (j < z1-1)
            {
                *pdest += *( ptemp + destSize );
                numNeighbours ++;
            }

            *pdest /= numNeighbours;
            
            ptemp++;
            pdest++;
        }
        ptemp += incTemp;
        pdest += incDest;
    }

    // make dirty
    dest->SetDirty();
}

//------------------------------------------------------------------------------
/**
    @brief Generate a texture from the heightmap with specified size, as a grayscale image of the heightmap.
    @param size The size of the generated texture (it's squared, as the heightmap)
    @return The texture, or NULL on failure

    The texture generated will have format nTexture2::A8R8G8B8 and should be released by the caller.
*/
nTexture2*
nFloatMapBuilder::GenerateTexture( nFloatMap *src, int size, int bitDepth )
{
    n_assert(src && src->IsLoaded());
    n_assert( size > 0 );
    nTexture2 *newTexture = nGfxServer2::Instance()->NewTexture(0);
    newTexture->SetType( nTexture2::TEXTURE_2D );
    
    if( bitDepth == 8 )
    {
        newTexture->SetFormat( nTexture2::A8R8G8B8 );
    }
    else
    {
        newTexture->SetFormat( nTexture2::G16R16 );
    }

    newTexture->SetUsage( nTexture2::CreateEmpty );
    newTexture->SetWidth( ushort(size) );
    newTexture->SetHeight( ushort(size) );
    newTexture->Load();
    if (newTexture->IsLoaded())
    {
        nTexture2::LockInfo lockInfo;

        if (newTexture->Lock(nTexture2::WriteOnly, 0, lockInfo))
        {
            nuint8* line = (nuint8*) lockInfo.surfPointer;
            int remainder = lockInfo.surfPitch - newTexture->GetWidth() * 4;

            float scaleFactor = 1.0f / size;

            for (int j = 0; j < newTexture->GetHeight(); j++) {
                for (int i = 0; i < newTexture->GetWidth(); i++) {

                    int hmapX = int( ( scaleFactor / 2.0f + float( i ) * scaleFactor ) * src->GetSize() );
                    int hmapZ = int( ( scaleFactor / 2.0f + float( j ) * scaleFactor ) * src->GetSize() );
                    float h = src->GetHeightHC( hmapX, hmapZ );

                    if( bitDepth == 8 )
                    {
                        nuint8 pixvalue = nuint8( h * 255.0f );

                        *line++ = pixvalue;
                        *line++ = pixvalue;
                        *line++ = pixvalue;
                        *line++ = nuint8( 255 );
                    }
                    else
                    {
                        nuint16 pixvalue = nuint16( h * 65535.0f );

                        *line++ = nuint8( pixvalue & 0x00FF );
                        *line++ = nuint8( ( pixvalue & 0xFF00 ) >> 8 );
                        *line++ = 0;
                        *line++ = 0;
                    }
                }
                line += remainder;
            }

            newTexture->Unlock(0);

            return newTexture;
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------