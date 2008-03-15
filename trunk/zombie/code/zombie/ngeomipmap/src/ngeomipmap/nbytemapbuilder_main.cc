//------------------------------------------------------------------------------
//  nbytemapbuilder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/nbytemapbuilder.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "il/il.h"
#include "il/ilu.h"

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nByteMapBuilder::nByteMapBuilder()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nByteMapBuilder::~nByteMapBuilder()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Clip a rectangle to a floatmap dimensions, telling if there was an intersection.
    @return true if clipping was done
*/
bool
nByteMapBuilder::ClipRectangle(nByteMap *map, int &x0, int &z0, int &x1, int &z1, int margin)
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
nByteMapBuilder::SetupLoop(nFloatMap *src, int x, int z, nByteMap* dest, int &x0, int &z0, int &x1, int &z1, float** porig, nuint8** pdest, int &incOrig, int &incDest, int margin)
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
    Setup variables for loops.
*/
bool
nByteMapBuilder::SetupLoop(nByteMap *src, int x, int z, nByteMap* dest, int &x0, int &z0, int &x1, int &z1, nuint8** porig, nuint8** pdest, int &incOrig, int &incDest, int margin)
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
    Copy the bytemap values into 'dest'. Both btemaps must be the same size
    @param  src     Source bytemap
    @param  dest    Destination bytemap
*/
void
nByteMapBuilder::Copy(nByteMap *src, nByteMap *dest)
{
    n_assert(src);
    n_assert(dest);
    n_assert(src->IsLoaded());
    n_assert(dest->IsLoaded());
    
    if ( src->GetSize() != dest->GetSize() )
    {
        return;
    }

    // Copy values
    memcpy( dest->GetByteMap(), src->GetByteMap(), src->GetByteSize() );

#ifndef NGAME
    // fix accumulator
    dest->accumulator = src->accumulator;
#endif

    // make dirty
    dest->SetDirty();
}

//------------------------------------------------------------------------------
/**
    @brief Multiply the floatmap by factor and adds into bytemap 'dest' at (x,z). Clamp is done also.

    @param  src     Source floatmap
    @param  x       X position to apply in dest bytemap coordinates
    @param  z       Z position to apply in dest bytemap coordinates
    @param  factor  Multiplication factor
    @param  dest    Destination bytemap
    @param minValue Optional minValue for the final result

*/
void
nByteMapBuilder::MultAdd(nFloatMap *src, int x, int z, float factor, nByteMap *dest, nuint8 minValue)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    nuint8 *pdest = dest->GetByteMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    unsigned int fixAccumulator = 0;
    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            float v = *porig++ * factor;
            nuint8 outval = max( minValue, nuint8( max(0.0f, min( float( *pdest ) + v, 255.0f) ) ) );
            fixAccumulator += (outval - *pdest);
            *pdest++ = outval;
        }
        pdest += incDest;
        porig += incOrig;
    }

#ifndef NGAME
    // correct the accumulator value
    dest->accumulator += fixAccumulator;
#endif

    // make dirty
    dest->SetDirty();
}
//------------------------------------------------------------------------------
/**
    Multiplies the floatmap by factor and subs it from 'dest' at (x,z)

    @param  x   X position to apply in dest heightmap coordinates
    @param  z   Z position to apply in dest heightmap coordinates
    @param  factor    Multiplication factor
    @param  dest    Destination heightmap
*/
void
nByteMapBuilder::MultSub(nFloatMap *src, int x, int z, float factor, nByteMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    nuint8 *pdest = dest->GetByteMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    unsigned int fixAccumulator = 0;
    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            float v = *porig++ * factor;
            nuint8 outval = nuint8( max(0.0f, min( float( *pdest ) - v, 255.0f) ) );
            fixAccumulator += (outval - *pdest);
            *pdest++ = outval;
        }
        pdest += incDest;
        porig += incOrig;
    }

#ifndef NGAME
    // correct the accumulator value
    dest->accumulator += fixAccumulator;
#endif

    // make dirty
    dest->SetDirty();
}

//------------------------------------------------------------------------------
/**
    @brief Substitute the bytemaps values where the source floatmap is higher than 0.5

    @param  src     Source floatmap
    @param  x       X position to apply in dest bytemap coordinates
    @param  z       Z position to apply in dest bytemap coordinates
    @param value    Substitute value
    @param  dest    Destination bytemap

*/
void
nByteMapBuilder::Substitute(nFloatMap *src, int x, int z, nuint8 value, nByteMap *dest)
{
    n_assert(src && src->IsLoaded());
    n_assert(dest && dest->IsLoaded());

    int x0,z0,x1,z1;
	float *porig = src->GetHeightMap();
    nuint8 *pdest = dest->GetByteMap();
    int incDest, incOrig;

    if (! SetupLoop(src, x, z, dest, x0, z0, x1, z1, &porig, &pdest, incOrig, incDest) )
	{
        return;
    }

    int fixAccumulator = 0;

    for (int j = z0; j < z1; j++)
	{
        for (int i = x0; i < x1; i++)
		{
            float v = *porig++;
            if ( v > 0.5f )
            {
                fixAccumulator += ((int)value) - (*pdest);
                *pdest = value;
            }
            pdest++;
        }
        pdest += incDest;
        porig += incOrig;
    }

    #ifndef NGAME
    // correct the accumulator value,
     dest->accumulator = fixAccumulator + dest->accumulator; // not use opertator += , erroneous casting
    #endif

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
nByteMapBuilder::GenerateTexture( nByteMap *src, int size )
{
    n_assert(src && src->IsLoaded());
    n_assert( size > 0 );

    nTexture2 *newTexture = nGfxServer2::Instance()->NewTexture(0);
    newTexture->SetType( nTexture2::TEXTURE_2D );
    newTexture->SetFormat( nTexture2::A8R8G8B8 );
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


                    nuint8 pixvalue = src->GetValue( hmapX, hmapZ );

                    *line++ = pixvalue;
                    *line++ = pixvalue;
                    *line++ = pixvalue;
                    *line++ = nuint8( 255 );

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
void
nByteMapBuilder::FastCopyToTexture(nTexture2* dst,nByteMap *src )
{
    n_assert(src && src->IsLoaded());
    n_assert(dst);
    n_assert( src->GetSize() == dst->GetWidth() );
    n_assert( src->GetSize() == dst->GetHeight() );

    int size = src->GetSize();
    nTexture2::LockInfo lockInfo;
    if (dst->Lock(nTexture2::WriteOnly, 0, lockInfo))
    {
        memcpy( lockInfo.surfPointer ,  src->byteMap, sizeof(nuint8)*size*size );
        dst->Unlock(0);
    } else
	{
		n_assert2_always("FastCopyToTexture: Texute is not locked");
	}
}

//------------------------------------------------------------------------------
bool 
nByteMapBuilder::SaveByteMapOpenIL(nByteMap * src, const char * fileName)
{
    int bytemapSize = src->GetSize();

    // Create devil image
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);

    ILuint imageName;
    ilGenImages( 1, &imageName );
    ilBindImage( imageName );

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    ilTexImage( bytemapSize, bytemapSize, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, src->GetByteMap());
    ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

    // TGA with run length encoding
    ilSetInteger(IL_TGA_RLE, 1);

    // file name
    nString outFileName = nFileServer2::Instance()->ManglePath( fileName, false ).Get();

    if (!stricmp(outFileName.GetExtension(), "png") || 
        !stricmp(outFileName.GetExtension(), "dds") ||
        !stricmp(outFileName.GetExtension(), "tga"))
    {
        iluInit();
        iluFlipImage(); 
    }

    // Save and free devil image
    ilSaveImage( (ILstring) outFileName.Get() );
    ilDeleteImages(1, &imageName);

    if ( ilGetError() != IL_NO_ERROR )
    {
        return false;
    }

    return true;
}
