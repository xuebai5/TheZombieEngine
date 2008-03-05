#include "precompiled/pchngeomipmap.h"
//------------------------------------------------------------------------------
//  nbytemap_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngeomipmap/nbytemap.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nfileserver2.h"
#include "il/il.h"
#include "il/ilu.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nByteMap, "nresource");

//------------------------------------------------------------------------------
/**
    @brief Default constructor
*/
nByteMap::nByteMap() :
    usage(0),
    size(0), 
    byteMap(0),
    dirty(false),
    userDirty(false)
#ifndef NGAME
    ,accumulator(0)
#endif
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Destructor
*/
nByteMap::~nByteMap()
{
   if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Allocate memory buffer for the bytemap data. A previous call to SetSize is 
    required.
    @return true if succesful, otherwise false
*/
bool 
nByteMap::Alloc()
{
    n_assert(!this->IsLoaded());
    n_assert(this->size > 0);
    n_assert(this->byteMap == NULL);

    // allocate buffer
    this->byteMap = n_new_array(nuint8, this->size * this->size);

    // make dirty
    this->SetDirty();

    return (0 != this->byteMap);
}

//------------------------------------------------------------------------------
/**
    @brief Free the memory buffer used for bytemap data.
*/
void 
nByteMap::Dealloc()
{
    if (this->byteMap)
	{
        n_delete_array(this->byteMap);
        this->byteMap = 0;
        this->SetState(Unloaded);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Initialize all byte values in the bytemap to the given value
    @param baseLevel byte value to set bytemap to
*/
void 
nByteMap::FillValue(nuint8 value)
{
    n_assert(this->byteMap);
    n_assert(this->size > 0);

    if (this->IsLoaded())
	{
        for(int i = 0;i < (this->size * this->size); i++)
		{
            this->byteMap[i] = value;
        }

#ifndef NGAME
        this->accumulator = this->size * this->size * value;
#endif

        // make dirty
        this->SetDirty();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set the size of the bytemap
    @param size The size
*/
void
nByteMap::SetSize(int size)
{
    // check size is at least 1 x 1
    n_assert(size >= 1 && size < ushort(~0));
    this->size = static_cast<ushort> (size);
}

//------------------------------------------------------------------------------
/**
    @brief Get the bytemap size
    @return the terrain size
*/
int
nByteMap::GetSize() const
{
    return this->size;
}
      
//------------------------------------------------------------------------------
/**
    @brief Return the value at coordinates x, z
*/
nuint8
nByteMap::GetValue(int x, int z) const
{
    n_assert(x >= 0 && x < this->size);
    n_assert(z >= 0 && z < this->size);
    return this->byteMap[z * this->size + x];
}

//------------------------------------------------------------------------------
/**
    @brief Sets the the value at coordinates x, z
*/
void
nByteMap::SetValue(int x, int z, nuint8 v)
{
    n_assert(x >= 0 && x < this->size);
    n_assert(z >= 0 && z < this->size);
#ifndef NGAME
    this->accumulator += (v - this->GetValue(x,z));
#endif
    byteMap[z * this->size + x] = v;
}

//------------------------------------------------------------------------------
/**
    @return the bytemap buffer
*/
nuint8 *
nByteMap::GetByteMap(void) const
{
    return this->byteMap;
}

//------------------------------------------------------------------------------
/**
    @brief set combination of usage flags
*/
void 
nByteMap::SetUsage(ushort useFlags)
{
    this->usage = useFlags;
}

//------------------------------------------------------------------------------
/**
    @return usage flags combination
*/
ushort 
nByteMap::GetUsage() const
{
    return this->usage;
}

//------------------------------------------------------------------------------
/**
    @return space used by the resource
*/
int 
nByteMap::GetByteSize()
{
    if (this->IsLoaded())
    {
        return this->size * this->size * sizeof(nuint8);
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Set dirty flag
*/
void 
nByteMap::SetDirty()
{
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
    @brief clean the dirty flag
*/
void 
nByteMap::CleanDirty()
{
    this->dirty = false;
}

//------------------------------------------------------------------------------
/**
    @return dirty flag
*/
bool 
nByteMap::IsDirty()
{
    return this->dirty;
}

//------------------------------------------------------------------------------
/**
    @brief Set User dirty flag
*/
void 
nByteMap::SetUserDirty()
{
    this->userDirty = true;
}

//------------------------------------------------------------------------------
/**
    @brief clean the dirty flag
*/
void 
nByteMap::CleanUserDirty()
{
    this->userDirty  = false;
}


//------------------------------------------------------------------------------
/**
    @return user dirty flag
*/
bool
nByteMap::IsUserDirty()
{
    return this->userDirty;
}

//------------------------------------------------------------------------------
/**
	@brief Loads byte data from a graphic image (texture) file
    @param fileName Full path of the texture file
    
    This function loads a texture and gets the red channel as the byte data. 
    Compressed dds formats and 16 bit float channels are not supported.

    If the bytemap is not valid, SetSize() will be called so that the texture 
    can fit in the bytemap, and after that, Alloc() will be called. Otherwise 
    the bytempa will be filled only with the portion covered by the texture.
*/
bool
nByteMap::LoadFromTexture()
{
    nString extension = this->GetFilename().GetExtension();
    if ( extension != "bmp" && extension != "tga")
    {
        return false;
    }

    nTexture2* newTexture = nGfxServer2::Instance()->NewTexture(0);
    newTexture->SetFilename(this->GetFilename().Get());

    if ( ! newTexture->Load() )
    {
        return false;
    }

    if (! newTexture->IsLoaded())
    {
        newTexture->Release();
        return false;
    }
    
    int format = newTexture->GetFormat();

    // Get the following format-dependent parameters
    // Mask and shift for the red channel
    int mask, shift;
    // Range of the red channel. 0.0f == 32 bit float value
    float range;
    // Pixel byte size
    int pixsize;
    switch (format)
    {
        case nTexture2::X8R8G8B8:
            //Intentionally left blank
        case nTexture2::A8R8G8B8:
            mask = 0x00FF0000;
            shift = 16;
            range = 256.0f;
            pixsize = 4;
            break;
        case nTexture2::R5G6B5:
            mask = 0x0000F800;
            shift = 11;
            range = 32.0f;
            pixsize = 2;
            break;
        case nTexture2::A1R5G5B5:
            mask = 0x00007C00;
            shift = 10;
            range = 32.0f;
            pixsize = 2;
            break;
        case nTexture2::A4R4G4B4:
            mask = 0x00000F00;
            shift = 8;
            range = 16.0f;
            pixsize = 2;
            break;
        case nTexture2::P8:
            mask = 0x000000FF;
            shift = 0;
            range = 256.0f;
            pixsize = 1;
        case nTexture2::R32F:                       // 32 bit float: red only
            mask = 0;
            shift = 0;
            range = 0.0f;
            pixsize = 4;
            break;
        case nTexture2::G32R32F:                    // 64 bit float: 32 bit red: 32 bit green
            mask = 0;
            shift = 0;
            range = 0.0f;
            pixsize = 8;
            break;
        case nTexture2::A32B32G32R32F:              // 128 bit float: 32 bit rgba each
            mask = 0;
            shift = 0;
            range = 0.0f;
            pixsize = 16;
            break;
        case nTexture2::A8:
            mask = 0x000000FF;
            shift = 0;
            range = 256.0f;
            pixsize = 1;
            break;
        default:
            // Texture format not supported
            return false;
    }

    nTexture2::LockInfo lockInfo;
    if (newTexture->Lock(nTexture2::ReadOnly, 0, lockInfo))
    {

        nuint8* line = (nuint8*) lockInfo.surfPointer;
        int ni = newTexture->GetWidth();
        int nj = newTexture->GetHeight();
        int remainder = lockInfo.surfPitch - ni * pixsize;

        if ( !this->IsLoaded() )
        {
            this->SetSize( max(ni, nj) );
            this->Alloc();
        }

        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {

                nuint8 h;

                if (mask)
                {
                    nint32 pixvalue;
                    if (pixsize == 1)
                    {
                        pixvalue = *line;
                    } else if (pixsize == 2)
                    {
                        pixvalue = *((nint16*) line);
                    } else
                    {
                        pixvalue = *((nint32*) line);
                    }

                    pixvalue = (pixvalue & mask) >> shift;
                    h = nuint8( ( pixvalue / range ) * nuint8( 255 ) );
                } else
                {
                    h = nuint8( *((float*)line) * nuint8( 255 ) );
                }

                this->SetValue(i, j, h);

                line += pixsize;
            }
            line += remainder;
        }

        newTexture->Unlock(0);

    } else
    {
        newTexture->Release();
        return false;
    }

    newTexture->Release();

    this->dirty = false;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nByteMap::LoadResource()
{
    n_assert(!this->IsLoaded());
    bool success = false;

    if (this->GetUsage() == nByteMap::CreateEmpty)
    {
        success = this->Alloc();
        if ( success )
        {
            this->SetState(Valid);
            this->FillValue(0);
        }
        this->SetUsage(0);
    }
    else 
    {
        if (this->GetFilename().CheckExtension("h8"))
        {
            success = this->LoadByteMapH8();
        }
        else if (this->GetFilename().CheckExtension("t8"))
        {
            success = this->LoadByteMapT8();
        }
        else 
        {
            success = this->LoadByteMapOpenIL();
            //success = this->LoadFromTexture();
        }

        if (success)
        {
            this->SetState(Valid);
#ifndef NGAME
            this->CalculateAccumulator();
#endif
        }
        else
        {
            this->SetState(Unloaded);
        }

        this->dirty = false;
    }

   return success;
}

//------------------------------------------------------------------------------
/**
    Unload the resource from memory
    @return true if the load was succesful, otherwise false
*/
void 
nByteMap::UnloadResource()
{
    n_assert(this->IsLoaded());
    this->Dealloc();
}

//------------------------------------------------------------------------------
/**
    @deprecated soon to be removed, save handled through bytemap builder
*/
bool
nByteMap::Save()
{
    bool success = false;

#if 1
    if (!this->GetFilename().IsEmpty())
    {
        if (this->GetFilename().CheckExtension("h8"))
        {
            success = this->SaveByteMapH8();
            this->SetReloadOnChange(false);
        }
        else if (this->GetFilename().CheckExtension("t8"))
        {
            success = this->SaveByteMapT8();
            this->SetReloadOnChange(false);
        }
    }
#endif
    // clean dirty state on save
    this->dirty = false;

    return success;
}

//------------------------------------------------------------------------------
/**
    Load bytemap format 8 bit file (hard-coded)
    @return true if the load was succesful, otherwise false
*/
bool
nByteMap::LoadByteMapH8()
{
    bool status = false;
    n_assert(0 == this->byteMap);

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (hmfile->Open( this->GetFilename().Get(), "r"))
    {
        // total file size
        int filesize = hmfile->GetSize();

        // resolution in x or z direction (assumed squared and 1 byte format)
        int srcsize = static_cast<int> ( (sqrtf(static_cast<float> (filesize))) );

        // Alloc bytemap
        this->SetSize( srcsize );
        this->Alloc();

        // read file contents and close file
        int nbytes = hmfile->Read (this->byteMap, filesize);
        hmfile->Close();

        if (nbytes == filesize)
        {
            status = true;
        }
    }

    hmfile->Release();

    if ( ! status )
    {
        this->Dealloc();
    }

    return status;
}

//------------------------------------------------------------------------------
/**
    Load heightmap format text 8 bit file (hard-coded)
    @return true if the load was succesful, otherwise false
*/
bool
nByteMap::LoadByteMapT8()
{
    bool status = true;
    n_assert(0 == this->byteMap);

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (!hmfile->Open( this->GetFilename().Get(), "r"))
    {
        status = false;
        return status;
    }

    // total file size
    int filesize = hmfile->GetSize();
    char * filebuffer = n_new_array(char, filesize + 1);
    n_assert(filebuffer);

    // read the whole file
    int nbytes = hmfile->Read(filebuffer, filesize);

    // release file resources
    hmfile->Close();
    hmfile->Release();

    if (nbytes != filesize)
    {
        n_delete_array(filebuffer);
        status = false;
        return status;
    }

    // make the buffer null terminated
    filebuffer[nbytes] = 0;

    // pointer to file contents
    char * ptr = filebuffer;

    // parse header: get size of the floatmap
    int size = 0;
    if (!strncmp(ptr, "size ", 5))
    {
        ptr += 5;
        if (sscanf(ptr, "%x", &size) > 0 && size <= 0)
        {
            status = false;
        }
        // next line
        ptr = strchr(ptr, '\n');
        if (ptr)
        {
            ptr++;
        }
    }
    else
    {
        status = false;
    }

    // get contents of the floatmap
    if (status)
    {
        // table for hex conversion
        int hex2val[256];

        // fill -1 for invalid values
        for(int i = 0;i < 256;i++)
        {
            hex2val[i] = -1;
        }

        // fill valid values for numbers
        hex2val['0'] = 0;
        hex2val['1'] = 1;
        hex2val['2'] = 2;
        hex2val['3'] = 3;
        hex2val['4'] = 4;
        hex2val['5'] = 5;
        hex2val['6'] = 6;
        hex2val['7'] = 7;
        hex2val['8'] = 8;
        hex2val['9'] = 9;
        hex2val['a'] = 10;
        hex2val['b'] = 11;
        hex2val['c'] = 12;
        hex2val['d'] = 13;
        hex2val['e'] = 14;
        hex2val['f'] = 15;
        hex2val['A'] = 10;
        hex2val['B'] = 11;
        hex2val['C'] = 12;
        hex2val['D'] = 13;
        hex2val['E'] = 14;
        hex2val['F'] = 15;

        // -2 for end of value
        hex2val['\n'] = -2;
        hex2val['\r'] = -2;

        this->SetSize( size );
        this->Alloc();
        for(int z = 0;status && z < size;z++)           
        {
            for(int x = 0;status && x < size;x++)
            {
                int val = 0;
                while(hex2val[*ptr] >= 0)
                {
                    val = val * 16 + hex2val[*ptr];
                    ptr++;
                }

                this->SetValue(x, z, nuint8( val ));

                if (hex2val[*ptr] == -2)
                {
                    ptr++;
                }
                else
                {
                    status = false;
                }
            }
        }
    }

    n_delete_array(filebuffer);

    return status;
}

//------------------------------------------------------------------------------
/**
    Save heightmap format 8 bit file (hard-coded)
    @return true if the load was succesful, otherwise false
*/
bool
nByteMap::SaveByteMapH8()
{
    bool status = false;

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (!hmfile->Open( this->GetFilename().Get(), "w"))
    {
        return false;
    }

    // resolution in x or z direction (assumed squared and 1 byte format)
    int size = this->GetSize();

    // total file size
    int filesize = size * size;

    // write file contents and close file
    int nbytes = hmfile->Write(this->byteMap, filesize);
    status = (nbytes == filesize);
    hmfile->Close();
    hmfile->Release();

    return status;
}

//------------------------------------------------------------------------------
/**
*/
bool
nByteMap::SaveByteMapT8()
{
    bool status = true;

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (!hmfile->Open( this->GetFilename().Get(), "w"))
    {
        return false;
    }

    int mapSize = this->GetSize();
    int fileSize = ( mapSize * mapSize ) * 3 + 10;
    char * memBuf = n_new_array(char, fileSize );
    n_assert( memBuf );
    int totalBytes = 0;

    // save the string 'size xxxx' where xxxx is the size of the heightmap in hex notation
    memBuf[ totalBytes++ ] = 's';
    memBuf[ totalBytes++ ] = 'i';
    memBuf[ totalBytes++ ] = 'z';
    memBuf[ totalBytes++ ] = 'e';
    memBuf[ totalBytes++ ] = ' ';
    int mask = 0xF000;
    for (int d = 12; d >= 0; d -= 4)
    {
        char v = static_cast <char> ( ( mapSize & mask ) >> d );
        mask = mask >> 4;

        if ( v < 10 )
        {
            memBuf[ totalBytes++ ] = '0' + v;
        }
        else
        {
            memBuf[ totalBytes++ ] = 65 + v - 10;
        }
    }
    memBuf[ totalBytes++ ] = '\n';

    // save each height value as a 4 hex digit terminated by a CR
    for(int z = 0; z < mapSize; z++)
    {
        for(int x = 0; x < mapSize; x++)
        {
            nuint8 value = this->GetValue(x, z);

            // save the 2 characters of the 8 bit number in the file bufer
            mask = 0x00F0;
            for (int d = 4; d >= 0; d -= 4)
            {
                char v = static_cast <char> ( ( value & mask ) >> d );
                mask = mask >> 4;

                if ( v < 10 )
                {
                    memBuf[ totalBytes++ ] = '0' + v;
                }
                else
                {
                    memBuf[ totalBytes++ ] = 65 + v - 10;
                }
            }
            memBuf[ totalBytes++ ] = '\n';
        }                
    }

    N_IFDEF_ASSERTS(int nbytes =) 
    hmfile->Write( memBuf, totalBytes );
    n_assert( nbytes == totalBytes );

    // close file
    hmfile->Close();

    // release the file
    hmfile->Release();

    // release the buffer
    n_delete_array( memBuf );

    return status;
}

//------------------------------------------------------------------------------
bool
nByteMap::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
bool 
nByteMap::LoadByteMapOpenIL()
{
    ilInit();
    ILuint imageName;
    ilGenImages( 1, &imageName );
    ilBindImage( imageName );
    nString fileName = nFileServer2::Instance()->ManglePath( this->GetFilename(), true ).Get();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    ilLoadImage( (ILstring) fileName.Get() );

    int error = ilGetError();
    if ( error != IL_NO_ERROR )
    {
        return false;
    }

    this->SetSize( ilGetInteger(IL_IMAGE_WIDTH) );
    this->Alloc();

    ilCopyPixels(0, 0, 0, this->GetSize(), this->GetSize(), 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, this->GetByteMap());

    if ( ilGetError() != IL_NO_ERROR )
    {
        this->Dealloc();
        return false;
    }

    ilDeleteImages(1, &imageName);

    return true;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
nByteMap::CalculateAccumulator()
{
    unsigned int accumulator = 0;
    nuint8 * buffer = this->GetByteMap();
    for(int i = 0;i < (this->GetSize() * this->GetSize()); i++)
    {
        accumulator += buffer[i];
    }
    this->accumulator = accumulator;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int 
nByteMap::GetAccumulator() const
{
    return this->accumulator;
}

//------------------------------------------------------------------------------
/**
*/
bool nByteMap::GetUniqueValue(nuint8& val)
{
    val =  static_cast<nuint8>( this->GetAccumulator() / size / size ) ;
    bool equal = true;
    for ( int x=val; x< size && equal ; ++x)
    {
        for ( int z=val; z <size && equal ; ++z)
        {
            equal =  (val == this->GetValue(x,z));
        }
    }
    return equal;
}

#endif
