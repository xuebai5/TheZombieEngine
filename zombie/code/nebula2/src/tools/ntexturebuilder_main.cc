#include "precompiled/pchntoollib.h"
//------------------------------------------------------------------------------
//	ntexturebuilder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "resource/nresource.h"
#include "gfx2/ngfxserver2.h"
#include "tools/ntexturebuilder.h"
#include "kernel/nfileserver2.h"
#include "il/il.h"
#include "il/ilu.h"

//------------------------------------------------------------------------------
/**
    default constructor
*/
nTextureBuilder::nTextureBuilder()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nTextureBuilder::~nTextureBuilder()
{
    nTexture2 * texNull = 0;
    this->SetTexture(texNull);
}

//------------------------------------------------------------------------------
/**
    set the texture to work with
*/
void 
nTextureBuilder::SetTexture(nTexture2 * texture)
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }

    if (texture)
    {
        this->refTexture = texture;
        this->refTexture->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    fills the whole texture with color provided by writing directly in the 
    buffer.
*/
bool
nTextureBuilder::Fill(vector4 & rgba)
{
    if (this->refTexture->GetFormat() != nTexture2::A8R8G8B8)
    {
        return false;
    }

    // color in rgba
    nuint8 c[4];

    c[0] = nuint8( n_clamp(rgba.x, 0.0, 1.0) * 255 );
    c[1] = nuint8( n_clamp(rgba.y, 0.0, 1.0) * 255 );
    c[2] = nuint8( n_clamp(rgba.z, 0.0, 1.0) * 255 );
    c[3] = nuint8( n_clamp(rgba.w, 0.0, 1.0) * 255 );

    // Lock texture
    nTexture2::LockInfo lockInfo;
    if ( !refTexture->Lock(nTexture2::WriteOnly, 0, lockInfo) )
    {
        return false;
    }

    // Loop writing color values into image
    nuint8 * dstLine = (nuint8*) lockInfo.surfPointer;
    for (int wj = 0; wj < refTexture->GetHeight(); wj++ )
    {
        for (int wi = 0; wi < refTexture->GetWidth(); wi++ )
        {
            // format ARGB (BGRA in memory order)
            *dstLine++ = c[2];
            *dstLine++ = c[1];
            *dstLine++ = c[0];
            *dstLine++ = c[3];
        }
        dstLine += lockInfo.surfPitch - refTexture->GetWidth() * 4;
    }

    refTexture->Unlock(0);

    return true;
}

//------------------------------------------------------------------------------
/**
    set texture by filename
*/
void 
nTextureBuilder::LoadTexture(const char * fileName)
{
    nTexture2 * tex = nGfxServer2::Instance()->NewTexture(fileName);
    tex->SetFilename(fileName);
    tex->Load();
    this->SetTexture(tex);
    tex->Release();
}

//------------------------------------------------------------------------------
/**
    @brief Save a nTexture2 (must be in format RGBA) to an uncompressed file like bmp, tga etc
    @param fileName String containing the file name. The extension specifies the file format
    @param newSize Size of saved image, optional
    @returns Success flag
*/
bool
nTextureBuilder::Save( const char * fileName, int /*newSize*/ )
{
#if 1
    nString outFileName = nFileServer2::Instance()->ManglePath( fileName, false );
    if (!stricmp(outFileName.GetExtension(), "dds"))
    {
        // save again as it is (with or without mipmaps)
        bool res = this->refTexture->SaveResourceAs(outFileName.Get(), nTexture2::DXT5);

        // ugly trick to save mipmaps due to lack of support in the engine
        if (res && this->refTexture->GetNumMipLevels() <= 1)
        {
            // load again generating mipmaps
            nTexture2 * tmpTex = nGfxServer2::Instance()->NewTexture( outFileName.Get() );
            n_assert(tmpTex);
            tmpTex->SetFilename( outFileName );
            res = tmpTex->Load();
            if (res)
            {
                res = tmpTex->SaveResourceAs( outFileName.Get(), nTexture2::DXT5 );
            }
            tmpTex->Release();
        }

        return res;
    }

    return this->refTexture->SaveResourceAs(outFileName.Get(), this->refTexture->GetFormat());
#else
    n_assert(refTexture->GetFormat() == nTexture2::A8R8G8B8);

    if (refTexture->GetFormat() != nTexture2::A8R8G8B8)
    {
        return false;
    }

    // Lock texture
    // Create devil image
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);
 
    ILuint imageName;
    ilGenImages( 1, &imageName );
    ilBindImage( imageName );

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    nTexture2::LockInfo lockInfo;
    if ( !refTexture->Lock(nTexture2::ReadOnly, 0, lockInfo) )
    {
        return false;
    }
    ilTexImage( refTexture->GetWidth(), refTexture->GetHeight(), 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, lockInfo.surfPointer);
    refTexture->Unlock(0);

    if ( ilGetError() != IL_NO_ERROR )
    {
        return false;
    }

    // Resize
    if ( newSize != 0 )
    {
        iluImageParameter( ILU_FILTER, ILU_SCALE_BSPLINE );
        iluScale( newSize, newSize, 1 );
    }


    nString outFileName = nFileServer2::Instance()->ManglePath( fileName, false );

    if (!stricmp(outFileName.GetExtension(), "png") || 
        !stricmp(outFileName.GetExtension(), "dds") ||
        !stricmp(outFileName.GetExtension(), "tga") )
    {
        iluInit();
        iluFlipImage();
    }

    // TGA with run length encoding
    ilSetInteger(IL_TGA_RLE, 1);
    // DDS saved in DXT 3 by default
    ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

    // Save and free devil image
    ilSaveImage( (ILstring) outFileName.Get() );
    ilDeleteImages(1, &imageName);
    
    if ( ilGetError() != IL_NO_ERROR )
    {
        return false;
    }

    return true;
#endif
}

//------------------------------------------------------------------------------
/**
    @brief Load an image file, resize it and save it as a different file
    @param sourceFileName source image file
    @param fileName dest file name. Can have different extension, e.g. source can be dds and dest can be a bmp
    @newSize optional resize parameter
*/
bool
nTextureBuilder::LoadResizeSaveImage( nString sourceFileName, nString fileName, int newSize )
{

    // Create devil image
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);
 
    ILuint imageName;
    ilGenImages( 1, &imageName );
    ilBindImage( imageName );

    nString f(nFileServer2::Instance()->ManglePath( sourceFileName.Get() ));

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    ilLoadImage( (ILstring) f.Get() );
    if ( ilGetError() != IL_NO_ERROR )
    {
        return false;
    }

    // Resize
    if ( newSize != 0 )
    {
        iluImageParameter( ILU_FILTER, ILU_SCALE_BSPLINE );
        iluScale( newSize, newSize, 1 );
    }
    if ( ilGetError() != IL_NO_ERROR )
    {
        return false;
    }

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    // Save and free devil image
    ilSaveImage( (ILstring) nFileServer2::Instance()->ManglePath( fileName.Get(), false ).Get() );
    ilDeleteImages(1, &imageName);
    if ( ilGetError() != IL_NO_ERROR )
    {
        return false;
    }
    return true;
}
