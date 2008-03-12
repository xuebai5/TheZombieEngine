#include "precompiled/pchn3dsmaxexport.h"
/**
   @file n3dsbitmaps.cc
   @author Luis Jose Cabellos Gomez

   (C) 2005 Conjurer Services, S.A.
*/

#include "n3dsexporters/n3dslog.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"

#include "il/il.h"
#include "il/ilu.h"

//------------------------------------------------------------------------------
namespace n3dsBitmaps{
    
    N3DSCREATELOGLEVEL ( fileBitmaps , "Log for files" , true , 2 );

//------------------------------------------------------------------------------
/**
    Create a thumbnail image from 3ds Studio thumbnail
    @param path path of the thumbnail to save
*/
void
CreateThumbNail( const nString & path )
{
    // Create devil image
    ilInit();
    iluInit();
    ilEnable(IL_FILE_OVERWRITE);

    Interface* ip = GetCOREInterface();
    const char * dir = ip->GetDir( APP_MAXDATA_DIR );

    nString inputFile( dir );
    inputFile.Append( "nebula\\tmpexport\\thumbnail.bmp" );

    bool valid = (IL_TRUE == ilLoadImage( (ILstring)( inputFile.Get() ) ) );
    if( ! valid )
    {
        N3DSERROR( fileBitmaps, (0, "ERROR: Can't load ThumbNail" ) );
        return;
    }

    ILuint width, height, depth;
    width = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);
    depth = ilGetInteger(IL_IMAGE_DEPTH);
    ILuint xoff = 0, yoff = 0;

    if( width > height )
    {
        xoff = ( width - height ) >> 1;
        width = height;
    }
    else if( height > width )
    {
        yoff = ( height - width ) >> 1;
        height = width;
    }

    iluCrop( xoff, yoff, 0, width, height, depth );
    iluImageParameter( ILU_FILTER, ILU_SCALE_MITCHELL );
    iluScale( 128, 128, 24 );

    nString outputFile( nFileServer2::Instance()->ManglePath( path.Get(), false ) );
    valid = (IL_TRUE == ilSaveImage( (ILstring)( outputFile.Get() ) ) );
    if( ! valid )
    {
        N3DSERROR( fileBitmaps, (0, "ERROR: Can't save ThumbNail" ) );
    }
}

//------------------------------------------------------------------------------
/** 
    Create a normal map from bumpMap
    @param fileIn
    @param factor
    @param fileOut
*/
void 
BuildNormText(const nString &fileIn,  const nString &fileOut)
{
    nString directory = fileOut.ExtractDirName();
    nKernelServer::ks->GetFileServer()->MakePath(directory.Get());

    ilInit(); 
    ILuint image = iluGenImage();
    ilEnable(IL_ORIGIN_SET);
    ilBindImage(image);
    ilEnable(IL_FILE_OVERWRITE);

    if (!ilLoadImage( (char *) fileIn.Get() ))
    {
        N3DSERROR( fileBitmaps, (0, "ERROR: Failed convert to bumpmap :  %s \n", fileIn.Get() ) );
        iluDeleteImage(image);
        //return false;
    } else
    {
        //ilConvertImage(IL_BGRA, IL_FLOAT);IL_LUMINANCE

        ilConvertImage(IL_LUMINANCE, IL_FLOAT);

        ILuint Width, Height;
        Width = ilGetInteger(IL_IMAGE_WIDTH);
        Height = ilGetInteger(IL_IMAGE_HEIGHT);
        float * data;
        data = (float*) ilGetData();

        ILuint y,x;
        float *outData = n_new_array(float,Width*Height*4);
        
/* simple Derivate
        for ( y= 0; y <Height ; y++)
        {
            int y1 = (y+1) % Height;
            for (x = 0; x < Width; x++)
            {
                int x1 =  (x + 1) % Width;

                float val   = data[y * Width + x];
                float val_x = data[y * Width + x1];
                float val_y = data[y1* Width + x];

                vector3 normal(val_x - val, val_y - val, 1);
                normal.norm();

                normal= normal*0.5;
                normal +=  vector3(0.5,0.5,0.5);

                outData[(y * Width + x)*3+0] = normal.x;
                outData[(y * Width + x)*3+1] = normal.y;
                outData[(y * Width + x)*3+2] = normal.z;

            }
        }
*/

        //*Derivate with sobel
        for ( y= 0; y <Height ; y++)
        {
            int y1 = (y + 1) % Height;
            int y_1 = ( y + Height - 1 ) % Height;
            for (x = 0; x < Width; x++)
            {
                int x1 =  (x + 1) % Width;
                int x_1 = ( x + Width - 1 ) % Width;

                float s00 = data[ y_1 * Width + x_1];
                float s01 = data[ y_1 * Width + x  ];  
                float s02 = data[ y_1 * Width + x1];

                float s10 = data[ y * Width + x_1];
                float s12 = data[ y * Width + x1 ];

                float s20 = data[ y1 * Width + x_1];
                float s21 = data[ y1 * Width + x  ];
                float s22 = data[ y1 * Width + x1 ];

                float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
                float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

                vector3 normal(-sobelX, -sobelY, 1);
                normal.norm();

                normal= normal*0.5;
                normal +=  vector3(0.5,0.5,0.5);

                outData[(y * Width + x)*4+0] = normal.y;
                outData[(y * Width + x)*4+1] = normal.y;
                outData[(y * Width + x)*4+2] = normal.y;
                outData[(y * Width + x)*4+3] = normal.x;
            }
        }

        iluDeleteImage(image);

        image = iluGenImage();
        ilBindImage(image);
        ilTexImage(Width,Height,1,4,IL_RGBA, IL_FLOAT,outData);

        nString mangledPath;
        nFileServer2* fileServer = nKernelServer::ks->GetFileServer();
        mangledPath = fileServer->ManglePath ( fileOut.Get() );

        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

        ilSaveImage( (ILstring) mangledPath.Get() );

        iluDeleteImage(image);
        n_delete_array(outData);

    }
}

}//namespace n3dsBitmaps

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
