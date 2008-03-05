#include "precompiled/pchndirect3d9.h"
//------------------------------------------------------------------------------
//  nd3d9texture_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nsurface.h"
#include "gfx2/nd3d9surface.h"
#include "gfx2/nd3d9texture.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/nlogclass.h"
#include "kernel/ntimeserver.h"
#include "mathlib/rectanglei.h"

nNebulaClass(nD3D9Texture, "ntexture2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Texture::nD3D9Texture() :
    refGfxServer("/sys/servers/gfx"),
    baseTexture(0),
    texture2D(0),
    textureCube(0),
    renderTargetSurface(0),
    depthStencilSurface(0),
    destSurface(0)
{   
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Texture::~nD3D9Texture()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    nD3D9Texture support asynchronous resource loading.
*/
bool
nD3D9Texture::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Texture::UnloadResource()
{
    n_assert(this->IsLoaded());

    nD3D9Server* gfxServer = this->refGfxServer.get();
    n_assert(gfxServer->d3d9Device);

    //n_printf("nD3D9Texture::UnloadResource(): %s\n", this->GetName());

    // check if I am one of the current render targets in the gfx server...
    if (this->IsRenderTarget())
    {
        int i;
        for (i = 0; i < nGfxServer2::MaxRenderTargets; i++)
        {
            if (gfxServer->GetRenderTarget(i) == this)
            {
                gfxServer->SetRenderTarget(i, 0);
            }
        }
    }

    // release d3d resources
    if (this->renderTargetSurface)
    {
        this->renderTargetSurface->Release();
        this->renderTargetSurface = 0;
    }
    if (this->depthStencilSurface)
    {
        this->depthStencilSurface->Release();
        this->depthStencilSurface = 0;
    }
    if (this->baseTexture)
    {
        this->baseTexture->Release();
        this->baseTexture = 0;
    }
    if (this->texture2D)
    {
        this->texture2D->Release();
        this->texture2D = 0;
    }
    if (this->textureCube)
    {
        this->textureCube->Release();
        this->textureCube = 0;
    }
    if( this->destSurface )
    {
        this->destSurface->Release();
        this->destSurface = 0;
    }

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Texture::LoadResource()
{
    n_assert(!this->IsLoaded());

    //n_printf("nD3D9Texture::LoadResource(): %s\n", this->GetName());

    bool success = false;
    nString filename = this->GetFilename().Get();

    if (this->IsRenderTarget())
    {
        // create a render target
        success = this->CreateRenderTarget();
    }
    else if (this->GetUsage() & CreateFromRawCompoundFile)
    {
        success = this->LoadFromRawCompoundFile();
    }
    else if (this->GetUsage() & CreateFromDDSCompoundFile)
    {
        success = this->LoadFromDDSCompoundFile();
    }
    else if (this->GetUsage() & CreateEmpty)
    {
        // create an empty texture
        success = this->CreateEmptyTexture();
        if (success)
        {
            this->SetState(Empty);
        }
        return true;
    }
    else if (filename.CheckExtension("dds"))
    {
        // load file through D3DX, assume file has mip maps 
        //success = this->LoadD3DXFile(false);
        success = this->LoadD3DXFile(true);
    }
    else
    {
        // load file through D3DX and generate mip maps
        if (!filename.IsEmpty())
        {
            success = this->LoadD3DXFile(true);
        }
    }
    if (success)
    {
        this->SetState(Valid);
    }
    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nD3D9Texture::SaveResource()
{
    n_assert(this->IsLoaded());

    bool success = false;
    nString filename = this->GetFilename().Get();

    if (this->IsRenderTarget())
    {
        return false;
    }
    else if (this->GetUsage() & CreateFromRawCompoundFile)
    {
        return false;
    }
    else if (this->GetUsage() & CreateFromDDSCompoundFile)
    {
        return false;
    }
    else 
    {
        success = this->SaveResourceAs(filename.Get(), this->GetFormat());
    }

    return success;
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device is lost. We only need to
    react if our texture is not in D3D's managed pool.
    In this case, we need to unload ourselves...
*/
void
nD3D9Texture::OnLost()
{
    if (    this->IsRenderTarget() || 
            ( (this->usage & Dynamic) && (this->IsValid() || this->IsEmpty()) ) )
    {
        this->UnloadResource();
        this->SetState(Lost);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device has been restored. If our
    texture is in the D3D's default pool, we need to restore ourselves
    as well.
*/
void
nD3D9Texture::OnRestored()
{
    if (this->IsRenderTarget() || (this->usage & Dynamic))
    {
        this->SetState(Unloaded);
        this->LoadResource();
        if (this->usage & CreateEmpty)
        {
            this->SetState(Empty);
        }
        else
        {
            this->SetState(Valid);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Check a pixel or depth stencil format for compatibility on the current
    device.

    @param  d3d9            pointer to Direct3D9 inteface
    @param  d3d9Device      pointer to Direct3D9 device
    @param  usage           D3DUSAGE_DEPTHSTENCIL or D3DUSAGE_RENDERTARGET 
    @param  pixelFormat     D3DFORMAT member
*/
bool
nD3D9Texture::CheckRenderTargetFormat(IDirect3D9* d3d9, 
                                      IDirect3DDevice9* d3d9Device,
                                      DWORD usage, 
                                      D3DFORMAT pixelFormat)
{
    n_assert(d3d9);
    n_assert(d3d9Device);
    HRESULT hr;

    // get current display mode
    D3DDISPLAYMODE dispMode;
    hr = d3d9Device->GetDisplayMode(0, &dispMode);
    n_dxtrace(hr, "GetDisplayMode() failed");

    // check format
    hr = d3d9->CheckDeviceFormat(N_D3D9_ADAPTER,
                                 D3DDEVTYPE_HAL,
                                 dispMode.Format,
                                 usage,
                                 D3DRTYPE_SURFACE,
                                 pixelFormat);
    return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
    Create d3d render target texture, and optionally a depthStencil surface

    FIXME: choosing the depthStencil pixel format is not as flexible as
    the code in nD3D9Server
*/
bool
nD3D9Texture::CreateRenderTarget()
{
    n_assert(this->width > 0);
    n_assert(this->height > 0);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->depthStencilSurface);
    n_assert(this->IsRenderTarget());
    HRESULT hr;

    IDirect3D9* d3d9 = this->refGfxServer->d3d9;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9);
    n_assert(d3d9Dev);

    // create render target color surface
    if (this->usage & RenderTargetColor)
    {
        // get d3d compatible pixel format
        D3DFORMAT colorFormat;
        switch (this->format)
        {
            case X8R8G8B8:      colorFormat = D3DFMT_X8R8G8B8;      break;
            case A8R8G8B8:      colorFormat = D3DFMT_A8R8G8B8;      break;
            case G16R16:        colorFormat = D3DFMT_G16R16;        break;
            case R16F:          colorFormat = D3DFMT_R16F;          break;
            case G16R16F:       colorFormat = D3DFMT_G16R16F;       break;
            case A16B16G16R16F: colorFormat = D3DFMT_A16B16G16R16F; break;
            case R32F:          colorFormat = D3DFMT_R32F;          break;
            case G32R32F:       colorFormat = D3DFMT_G32R32F;       break;
            case A32B32G32R32F: colorFormat = D3DFMT_A32B32G32R32F; break;
            case R5G6B5:        colorFormat = D3DFMT_R5G6B5;        break;
            case A1R5G5B5:      colorFormat = D3DFMT_A1R5G5B5;      break;
            case A4R4G4B4:      colorFormat = D3DFMT_A4R4G4B4;      break;
            default:
                n_error("nD3D9Texture: invalid render target pixel format!\n");
                return false;
        }

        // make sure the format is valid as render target
        if (!this->CheckRenderTargetFormat(d3d9, d3d9Dev, D3DUSAGE_RENDERTARGET, colorFormat))
        {
            n_error("nD3D9Texture: Could not create render target surface!\n");
            return false;
        }

        // create render target surface
        hr = d3d9Dev->CreateTexture(
            this->width,                // Width
            this->height,               // Height
            1,                          // Levels
            D3DUSAGE_RENDERTARGET,      // Usage
            colorFormat,                // Format
            D3DPOOL_DEFAULT,            // Pool (must be default)
            &(this->texture2D),
            NULL);
        n_dxtrace(hr, "CreateTexture() failed in nD3D9Texture::CreateRenderTarget()");
        n_assert(this->texture2D);

        // get base texture interface pointer
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_dxtrace(hr, "QueryInterface(IID_IDirect3DBaseTexture9) failed");

        // get pointer to highest mipmap surface
        hr = this->texture2D->GetSurfaceLevel(0, &(this->renderTargetSurface));
        n_dxtrace(hr, "GetSurfaceLevel() failed");
    }

    // create optional render target depthStencil surface
    if (this->usage & RenderTargetDepth)
    {
        // always create a 24 bit depth buffer
        D3DFORMAT depthFormat = D3DFMT_D24X8;

        // make sure the format is compatible on this device
        if (!this->CheckRenderTargetFormat(d3d9, d3d9Dev, D3DUSAGE_DEPTHSTENCIL, depthFormat))
        {
            n_error("nD3D9Texture: Could not create render target surface!\n");
            return false;
        }

        // create render target surface
        hr = d3d9Dev->CreateDepthStencilSurface(
            this->width,                // Width
            this->height,               // Height
            depthFormat,                // Format
            D3DMULTISAMPLE_NONE,        // MultiSampleType
            0,                          // MultiSampleQuality
            TRUE,                       // Discard
            &(this->depthStencilSurface),
            NULL);
        n_dxtrace(hr, "CreateDepthStencilSurface() failed");
        n_assert(this->depthStencilSurface);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Query the texture attributes from the D3D texture object and
    update my own attributes. This method is called by LoadD3DXFile() and
    LoadILFile().
*/
void
nD3D9Texture::QueryD3DTextureAttributes()
{
    n_assert(this->GetType() != TEXTURE_NOTYPE);

    if (this->GetType() == TEXTURE_2D)
    {
        n_assert(this->texture2D);
        HRESULT hr;

        // set texture attributes
        D3DSURFACE_DESC desc;
        hr = this->texture2D->GetLevelDesc(0, &desc);
        n_dxtrace(hr, "QueryD3DTextureAttributes() failed");

        switch(desc.Format)
        {
            case D3DFMT_R8G8B8:
            case D3DFMT_X8R8G8B8:       this->SetFormat(X8R8G8B8); break;
            case D3DFMT_A8R8G8B8:       this->SetFormat(A8R8G8B8); break;
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:       this->SetFormat(R5G6B5); break;
            case D3DFMT_A1R5G5B5:       this->SetFormat(A1R5G5B5); break;
            case D3DFMT_A4R4G4B4:       this->SetFormat(A4R4G4B4); break;
            case D3DFMT_P8:             this->SetFormat(P8); break;
            case D3DFMT_G16R16:         this->SetFormat(G16R16); break;
            case D3DFMT_DXT1:           this->SetFormat(DXT1); break;
            case D3DFMT_DXT2:           this->SetFormat(DXT2); break;
            case D3DFMT_DXT3:           this->SetFormat(DXT3); break;
            case D3DFMT_DXT4:           this->SetFormat(DXT4); break;
            case D3DFMT_DXT5:           this->SetFormat(DXT5); break;
            case D3DFMT_R16F:           this->SetFormat(R16F); break;
            case D3DFMT_G16R16F:        this->SetFormat(G16R16F); break;
            case D3DFMT_A16B16G16R16F:  this->SetFormat(A16B16G16R16F); break;
            case D3DFMT_R32F:           this->SetFormat(R32F); break;
            case D3DFMT_G32R32F:        this->SetFormat(G32R32F); break;
            case D3DFMT_A32B32G32R32F:  this->SetFormat(A32B32G32R32F); break;
            case D3DFMT_A8:             this->SetFormat(A8); break;
		    case D3DFMT_L8:             this->SetFormat(L8); break;
        }
        this->SetWidth( static_cast<ushort>( desc.Width ) );
        this->SetHeight( static_cast<ushort>( desc.Height ) );
        this->SetDepth(1);
        this->SetNumMipLevels( static_cast<ushort>( this->texture2D->GetLevelCount() ) );
    }
    else if (this->GetType() == TEXTURE_3D)
    {
        n_error("3D textures not implemented yet.\n");
    }
    else if (this->GetType() == TEXTURE_CUBE)
    {
        n_assert(this->textureCube);
        HRESULT hr;

        // set texture attributes
        D3DSURFACE_DESC desc;
        hr = this->textureCube->GetLevelDesc(0, &desc);
        n_dxtrace(hr, "GetLevelDesc() failed");

        switch(desc.Format)
        {
            case D3DFMT_R8G8B8:
            case D3DFMT_X8R8G8B8:       this->SetFormat(X8R8G8B8); break;
            case D3DFMT_A8R8G8B8:       this->SetFormat(A8R8G8B8); break;
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:       this->SetFormat(R5G6B5); break;
            case D3DFMT_A1R5G5B5:       this->SetFormat(A1R5G5B5); break;
            case D3DFMT_A4R4G4B4:       this->SetFormat(A4R4G4B4); break;
            case D3DFMT_G16R16:         this->SetFormat(G16R16); break;
            case D3DFMT_DXT1:           this->SetFormat(DXT1); break;
            case D3DFMT_DXT2:           this->SetFormat(DXT2); break;
            case D3DFMT_DXT3:           this->SetFormat(DXT3); break;
            case D3DFMT_DXT4:           this->SetFormat(DXT4); break;
            case D3DFMT_DXT5:           this->SetFormat(DXT5); break;
            case D3DFMT_R16F:           this->SetFormat(R16F); break;
            case D3DFMT_G16R16F:        this->SetFormat(G16R16F); break;
            case D3DFMT_A16B16G16R16F:  this->SetFormat(A16B16G16R16F); break;
            case D3DFMT_R32F:           this->SetFormat(R32F); break;
            case D3DFMT_G32R32F:        this->SetFormat(G32R32F); break;
            case D3DFMT_A32B32G32R32F:  this->SetFormat(A32B32G32R32F); break;
        }
        this->SetWidth( static_cast<ushort>( desc.Width ) );
        this->SetHeight( static_cast<ushort>( desc.Height ) );
        this->SetDepth(1);
        this->SetNumMipLevels( static_cast<ushort>( this->textureCube->GetLevelCount() ) );
    }
    else
    {
        n_assert2_always("nD3D9Texture: unknown texture type in QueryD3DTextureAttributes()");
    }
}

//------------------------------------------------------------------------------
/**
    Create texture from file via D3DX.

    31-May-04   floh    use nFile for file IO
*/
bool
nD3D9Texture::LoadD3DXFile(bool genMipMaps)
{
    n_assert(0 == this->baseTexture);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);
 
    //nTimeServer::nProfilerCounter profCounter = nTimeServer::Instance()->GetProfilerCounter();

    // read file into temp mem buffer
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (!file->Open(this->GetFilename().Get(), "rb"))
    {
        NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to open texture file '%s'!", this->GetFilename().Get()));
        file->Release();
        return false;
    }

    int fileSize = file->GetSize();
    n_assert(fileSize > 0);
    void* fileBuffer = n_malloc(fileSize);
    n_verify( file->Read(fileBuffer, fileSize) == fileSize );
    file->Close();
    file->Release();

    //profCounter = nTimeServer::Instance()->GetProfilerCounter() - profCounter;
    //NLOG(defaultLog, (0, "texture io %s - %f ms", this->GetFilename().Get(), nTimeServer::Instance()->GetTimeFromProfilerCounter(profCounter) * 1000 ));

    //profCounter = nTimeServer::Instance()->GetProfilerCounter();

    // check whether this is a 2d texture or a cube texture
    D3DXIMAGE_INFO imgInfo = { 0 };
    hr = D3DXGetImageInfoFromFileInMemory(fileBuffer, fileSize, &imgInfo);
    if (FAILED(hr))
    {
        NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to obtain image info for file '%s'!", this->GetFilename().Get()));
        n_free(fileBuffer);
        fileBuffer = 0;
        return false;
    }

    //profCounter = nTimeServer::Instance()->GetProfilerCounter() - profCounter;
    //NLOG(defaultLog, (0, "texture info %s - %f ms", this->GetFilename().Get(), nTimeServer::Instance()->GetTimeFromProfilerCounter(profCounter) * 1000 ));

    // Generate mipmaps?
    DWORD mipmapFilter = D3DX_FILTER_NONE;
    if (genMipMaps)
    {
        mipmapFilter = D3DX_DEFAULT;
    }

    // D3D usage flags
    DWORD d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    if (this->usage & Dynamic)
    {
        d3dUsage |= D3DUSAGE_DYNAMIC;
        d3dPool  = D3DPOOL_DEFAULT;
    }

    if (D3DRTYPE_TEXTURE == imgInfo.ResourceType)
    {

    #if 0

        profCounter = nTimeServer::Instance()->GetProfilerCounter();

        hr = D3DXCreateTexture(d3d9Dev,             // pDevice
                                imgInfo.Width,    // Width
                                imgInfo.Height,   // Height
                                imgInfo.MipLevels,                   // MipLevels
                                d3dUsage,            // Usage
                                imgInfo.Format,           // Format
                                d3dPool,             // Pool
                                &(this->texture2D));

        profCounter = nTimeServer::Instance()->GetProfilerCounter() - profCounter;
        NLOG(defaultLog, (0, "texture create %s - %f ms", this->GetFilename().Get(), nTimeServer::Instance()->GetTimeFromProfilerCounter(profCounter) * 1000 ));

        if (FAILED(hr))
        {
            NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to load 2D texture '%s'!", this->GetFilename().Get()));
            n_free(fileBuffer);
            fileBuffer = 0;
            return false;
        }

        profCounter = nTimeServer::Instance()->GetProfilerCounter();

        D3DXIMAGE_INFO imgInfo2 = imgInfo;
        for ( UINT idx = 0; idx <  imgInfo.MipLevels ; ++idx)
        {
            IDirect3DSurface9* surface;
            hr = this->texture2D->GetSurfaceLevel(idx, &surface);
            if (FAILED(hr))
            {
                NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to load 2D texture '%s', mipmap %d!", this->GetFilename().Get(), idx ));
                n_free(fileBuffer);
                fileBuffer = 0;
                return false;
            }

            hr = D3DXLoadSurfaceFromFileInMemory(  surface,//pDestSurface,
                                                    0,//pDestPalette,
                                                    0,//pDestRect,
                                                    fileBuffer,//pSrcData,
                                                    fileSize,//SrcData,
                                                    0,//pSrcRect,
                                                    D3DX_SKIP_DDS_MIP_LEVELS(idx,D3DX_DEFAULT),//D3DX_DEFAULT,//D3DX_FILTER_NONE,//Filter,
                                                    0,//ColorKey,
                                                    &imgInfo2//pSrcInfo
                                                    );
            if (FAILED(hr))
            {
                NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to load 2D texture '%s', mipmap %d!", this->GetFilename().Get(), idx ));
                n_free(fileBuffer);
                fileBuffer = 0;
                return false;
            }

        }

        profCounter = nTimeServer::Instance()->GetProfilerCounter() - profCounter;
        NLOG(defaultLog, (0, "texture load %s - %f ms", this->GetFilename().Get(), nTimeServer::Instance()->GetTimeFromProfilerCounter(profCounter) * 1000 ));

        this->SetType(TEXTURE_2D);
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_dxtrace(hr, "QueryInterface(IID_IDirect3DBaseTexture9) failed");

    #else
        // load 2D texture
        hr = D3DXCreateTextureFromFileInMemoryEx(
                d3d9Dev,                    // pDevice
                fileBuffer,                 // pSrcData
                fileSize,                   // SrcDataSize
                D3DX_DEFAULT,               // Width
                D3DX_DEFAULT,               // Height
                D3DX_DEFAULT,               // MipLevels
                d3dUsage,                   // Usage
                D3DFMT_UNKNOWN,             // Format
                d3dPool,                    // Pool
                D3DX_FILTER_NONE,           // Filter
                mipmapFilter,               // MipFilter
                0,                          // ColorKey
                0,                          // pSrcInfo
                0,                          // pPalette
                &(this->texture2D));
        if (FAILED(hr))
        {
            NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to load 2D texture '%s'!", this->GetFilename().Get()));
            n_free(fileBuffer);
            fileBuffer = 0;
            return false;
        }
        this->SetType(TEXTURE_2D);
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_dxtrace(hr, "QueryInterface(IID_IDirect3DBaseTexture9) failed");
    #endif
    }
    else if (D3DRTYPE_CUBETEXTURE == imgInfo.ResourceType)
    {
        // load cube texture
        hr = D3DXCreateCubeTextureFromFileInMemoryEx(
                d3d9Dev,                    // pDevice
                fileBuffer,                 // pSrcData
                fileSize,                   // SrcDataSize
                D3DX_DEFAULT,               // Size
                D3DX_DEFAULT,               // MipLevels
                d3dUsage,                   // Usage
                D3DFMT_UNKNOWN,             // Format
                d3dPool,                    // Pool
                D3DX_FILTER_NONE,           // Filter
                D3DX_FILTER_NONE,           // MipFilter
                0,                          // ColorKey
                0,                          // pSrcInfo
                0,                          // pPalette
                &(this->textureCube));
        if (FAILED(hr))
        {
            NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadD3DXFile(): Failed to load cube texture '%s'!", this->GetFilename().Get()));
            n_free(fileBuffer);
            fileBuffer = 0;
            return false;
        }
        this->SetType(TEXTURE_CUBE);
        hr = this->textureCube->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_dxtrace(hr, "QueryInterface(IID_IDirect3DBaseTexture9) failed");
    }
    else
    {
        // unsupported texture type
        n_error("nD3D9Texture::LoadD3DXFile(): Unsupported texture type (cube texture?) in file '%s'!", this->GetFilename().Get());
        n_free(fileBuffer);
        fileBuffer = 0;
        return false;
    }
    if ( 0 == ( this->GetUsage() & nTexture2::Dynamic )  )
    {
        this->baseTexture->PreLoad();
    }

    // free file buffer
    n_free(fileBuffer);
    fileBuffer = 0;

    // query texture attributes 
    this->QueryD3DTextureAttributes();
    return true;
}

//------------------------------------------------------------------------------
/**
    Create an empty 2D or cube texture (without mipmaps!).
*/
bool
nD3D9Texture::CreateEmptyTexture()
{
    n_assert(this->GetWidth() > 0);
    n_assert(this->GetHeight() > 0);
    n_assert(this->GetType() != TEXTURE_NOTYPE);
    n_assert(this->GetFormat() != NOFORMAT);
    n_assert(0 == this->texture2D);
    n_assert(0 == this->textureCube);

    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(this->refGfxServer->d3d9);
    n_assert(d3d9Dev);

    DWORD d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    if (this->usage & Dynamic)
    {
        d3dUsage = D3DUSAGE_DYNAMIC;
        d3dPool  = D3DPOOL_DEFAULT;
    }

    D3DFORMAT d3dFormat = nD3D9Texture::FormatToD3DFormat(this->format);

    if (this->GetType() == TEXTURE_2D)
    {
        hr = D3DXCreateTexture(d3d9Dev,             // pDevice
                               this->GetWidth(),    // Width
                               this->GetHeight(),   // Height
                               1,                   // MipLevels
                               d3dUsage,            // Usage
                               d3dFormat,           // Format
                               d3dPool,             // Pool
                               &(this->texture2D));
        if (FAILED(hr))
        {
            n_dxtrace(hr,"nD3D9Texture::CreateEmptyTexture(): Failed to create 2D texture!\n");
            //n_error("nD3D9Texture::CreateEmptyTexture(): Failed to create 2D texture!\n");
            return false;
        }
        hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_dxtrace(hr, "QueryInterface(IID_IDirect3DBaseTexture9)");
    }
    else if (this->GetType() == TEXTURE_CUBE)
    {
        n_assert(this->GetWidth() == this->GetHeight());
        hr = D3DXCreateCubeTexture(d3d9Dev,             // pDevice
                                   this->GetWidth(),    // Size
                                   1,                   // MipLevels
                                   d3dUsage,            // Usage
                                   d3dFormat,           // Format
                                   d3dPool,             // Pool
                                   &(this->textureCube));
        if (FAILED(hr))
        {
            n_error("nD3D9Texture::CreateEmptyTexture(): Failed to create cube texture!\n");
            return false;
        }
        hr = this->textureCube->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
        n_dxtrace(hr, "QueryInterface(IID_IDirect3DBaseTexture9) failed");
    }
    else
    {
        // unsupported texture type
        n_error("nD3D9Texture::CreateEmptyTexture(): Unsupported texture type (cube texture?!\n");
        return false;
    }

    // query texture attributes 
    this->QueryD3DTextureAttributes();
    return true;
}

//------------------------------------------------------------------------------
/**
    Create texture and load contents from as "raw" pixel chunk from
    inside a compound file.
*/
bool
nD3D9Texture::LoadFromRawCompoundFile()
{
    // create texture...
    if (this->CreateEmptyTexture())
    {
        // read data into texture
        nTexture2::LockInfo lockInfo;
        if (this->Lock(nTexture2::WriteOnly, 0, lockInfo))
        {
            const int bytesToRead = this->GetWidth() * this->GetHeight() * this->GetBytesPerPixel();
            n_assert(lockInfo.surfPitch == (this->GetWidth() * this->GetBytesPerPixel()));

            // get seek position from Toc
            n_assert(this->compoundFile);
            this->compoundFile->Seek(this->compoundFilePos, nFile::START);
            n_verify( this->compoundFile->Read(lockInfo.surfPointer, bytesToRead) == bytesToRead);

            this->Unlock(0);
            return true;
        }        
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Load texture as dds file from inside a compound file.
    FIXME: Add cube texture support?
*/
bool
nD3D9Texture::LoadFromDDSCompoundFile()
{
    n_assert(this->compoundFile);
    HRESULT hr;

    IDirect3DDevice9* d3d9Dev = this->refGfxServer->d3d9Device;
    n_assert(d3d9Dev);

    // allocate temp buffer of requested size
    void* buffer = n_malloc(this->compoundFileDataSize);
    n_assert(buffer);

    // seek to start of data
    this->compoundFile->Seek(this->compoundFilePos, nFile::START);
    n_verify( this->compoundFile->Read(buffer, this->compoundFileDataSize) == this->compoundFileDataSize);

    // D3D usage flags
    DWORD d3dUsage = 0;
    D3DPOOL d3dPool = D3DPOOL_MANAGED;
    if (this->usage & Dynamic)
    {
        d3dUsage = D3DUSAGE_DYNAMIC;
        d3dPool  = D3DPOOL_DEFAULT;
    }

    // create texture from memory buffer
    hr = D3DXCreateTextureFromFileInMemoryEx(d3d9Dev,                       // pDevice
                                             buffer,                        // pSrcData
                                             this->compoundFileDataSize,    // SrcDataSize
                                             D3DX_DEFAULT,                  // Width
                                             D3DX_DEFAULT,                  // Height
                                             D3DX_DEFAULT,                  // MipLevels
                                             d3dUsage,                      // Usage
                                             D3DFMT_UNKNOWN,                // Format
                                             d3dPool,                       // Pool
                                             D3DX_FILTER_NONE,              // Filter
                                             D3DX_FILTER_NONE,              // MipFilter
                                             0,                             // ColorKey
                                             0,                             // pSrcInfo
                                             NULL,                          // pPalette
                                             &(this->texture2D));
    if (FAILED(hr))
    {
        NLOG(resource, (NLOGUSER | 0, "nD3D9Texture::LoadFromDDSCompoundFile(): Failed to load 2D texture!\n"));
        return false;
    }
    this->SetType(TEXTURE_2D);
    hr = this->texture2D->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(this->baseTexture));
    n_dxtrace(hr, "QueryInteface(IID_IDirect3DBaseTexture9) failed");

    // free temp buffer
    n_free(buffer);
    buffer = 0;

    // query texture attributes 
    this->QueryD3DTextureAttributes();

    return true;
}

//------------------------------------------------------------------------------
/**
    Locks the 2D texture surface and returns a pointer to the
    image data and the pitch of the surface (refer to the DX9 docs
    for details). Call Unlock() to unlock the texture after accessing it.

    @param  lockType    defines the intended access to the surface (Read, Write)
    @param  level       the mip level
    @param  lockInfo    will be filled with surface pointer and pitch
    @return             true if surface has been locked successfully
*/
bool
nD3D9Texture::Lock(LockType lockType, int level, LockInfo& lockInfo)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_2D);
    n_assert(this->texture2D);

    DWORD d3dLockFlags = 0;
    switch (lockType)
    {
        case ReadOnly:
            d3dLockFlags = D3DLOCK_READONLY;
            break;

        case WriteOnly:
            d3dLockFlags = D3DLOCK_NO_DIRTY_UPDATE;
            break;
    }

    bool retval = false;
    D3DLOCKED_RECT d3dLockedRect = { 0 };
    HRESULT hr = this->texture2D->LockRect(level, &d3dLockedRect, NULL, d3dLockFlags);
    if (SUCCEEDED(hr))
    {
        lockInfo.surfPointer = d3dLockedRect.pBits;
        lockInfo.surfPitch   = d3dLockedRect.Pitch;
        retval = true;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Locks the 2D texture surface and returns a pointer to the
    image data and the pitch of the surface (refer to the DX9 docs
    for details). Call Unlock() to unlock the texture after accessing it.

    @param  lockType    defines the intended access to the surface (Read, Write)
    @param  level       the mip level
    @param  left        left coordinate to lock (inclusive)
    @param  top         top coordinate to lock (inclusive)
    @param  right       right coordinate to lock (not-inclusive)
    @param  bottom      bottom coordinate to lock (not-inclusive) 
    @param  lockInfo    will be filled with surface pointer and pitch
    @return             true if surface has been locked successfully
*/
bool
nD3D9Texture::LockRect(LockType lockType, int level, int left, int top, int right, int bottom, LockInfo& lockInfo)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_2D);
    n_assert(this->texture2D);

    DWORD d3dLockFlags = 0;
    switch (lockType)
    {
        case ReadOnly:
            d3dLockFlags = D3DLOCK_READONLY;
            break;

        case WriteOnly:
            d3dLockFlags = D3DLOCK_NO_DIRTY_UPDATE;
            break;
    }

    bool retval = false;
    D3DLOCKED_RECT d3dLockedRect = { 0 };
    RECT rect;
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
    HRESULT hr = this->texture2D->LockRect(level, &d3dLockedRect, &rect, d3dLockFlags);
    if (SUCCEEDED(hr))
    {
        lockInfo.surfPointer = d3dLockedRect.pBits;
        lockInfo.surfPitch   = d3dLockedRect.Pitch;
        retval = true;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Locks all cube texture surfaces and returns pointers to the image data
    and surface pitches. Call Unlock() to unlock the texture after accessing it.

    @param  lockType    defines the intended access to the surface (Read, Write)
    @param  level       the mip level
    @param  lockInfo    array of size 6 which will be filled with surface pointers and pitches
    @return             true if surface has been locked successfully
*/
bool
nD3D9Texture::LockCubeFace(LockType lockType, CubeFace face, int level, LockInfo& lockInfo)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_CUBE);
    n_assert(this->textureCube);

    DWORD d3dLockFlags = 0;
    switch (lockType)
    {
        case ReadOnly:
            d3dLockFlags = D3DLOCK_READONLY;
            break;

        case WriteOnly:
            d3dLockFlags = 0;
            break;
    }

    bool retval = false;
    D3DLOCKED_RECT d3dLockedRect = { 0 };
    HRESULT hr = this->textureCube->LockRect((D3DCUBEMAP_FACES) face, level, &d3dLockedRect, NULL, d3dLockFlags);
    if (SUCCEEDED(hr))
    {
        lockInfo.surfPointer = d3dLockedRect.pBits;
        lockInfo.surfPitch   = d3dLockedRect.Pitch;
        retval = true;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the 2D texture.
*/
void
nD3D9Texture::Unlock(int level)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_2D);
    n_assert(this->texture2D);
    n_dxverify2( 
        this->texture2D->UnlockRect(level), 
        "UnlockRect() on 2d texture failed");
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Unlock a cube texture face.
*/
void
nD3D9Texture::UnlockCubeFace(CubeFace face, int level)
{
    this->LockMutex();
    n_assert(this->GetType() == TEXTURE_CUBE);
    n_assert(this->textureCube);
    n_dxverify2( 
        this->textureCube->UnlockRect((D3DCUBEMAP_FACES) face, level),
        "UnlockRect() on cube surface failed" );
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Compute the byte size of the texture data.
*/
int
nD3D9Texture::GetByteSize()
{
    if (this->IsLoaded())
    {
        // compute number of pixels
        int numPixels = this->GetWidth() * this->GetHeight();

        // 3d or cube texture?
        switch (this->GetType())
        {
            case TEXTURE_3D:   numPixels *= this->GetDepth(); break;
            case TEXTURE_CUBE: numPixels *= 6; break;
            default: break;
        }

        // mipmaps ?
        if (this->GetNumMipLevels() > 1)
        {
            switch (this->GetType())
            {
                case TEXTURE_2D:
                case TEXTURE_CUBE:
                    numPixels += numPixels / 3;
                    break;

                default:
                    /// 3d texture
                    numPixels += numPixels / 7;
                    break;
            }
        }

        // size per pixel
        int size = 0;
        switch (this->GetFormat())
        {
            case DXT1:
                // 4 bits per pixel
                size = numPixels / 2;
                break;

            case DXT2:
            case DXT3:
            case DXT4:
            case DXT5:
			case L8:
            case P8:
                // 8 bits per pixel
                size = numPixels;
                break;

            case R5G6B5:
            case A1R5G5B5:
            case A4R4G4B4:
            case R16F:
                // 16 bits per pixel
                size = numPixels * 2;
                break;

            case X8R8G8B8:
            case A8R8G8B8:
            case G16R16:
            case R32F:
            case G16R16F:
                // 32 bits per pixel
                size = numPixels * 4;
                break;

            case A16B16G16R16F:
            case G32R32F:
                // 64 bits per pixel
                size = numPixels * 8;
                break;

            case A32B32G32R32F:
                // 128 bits per pixel
                size = numPixels * 16;
                break;
        }
        return size;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**

    @param objName  name of nSurafce which to be created.
    @param level    mipmap level
    @param surface

    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void nD3D9Texture::GetSurfaceLevel(const char* objName, uint level, nSurface** surface)
{
    n_assert(this->texture2D);

    *surface = (nSurface*)this->kernelServer->New ("nd3d9surface", objName);
    n_assert(*surface != NULL);

    n_dxverify( this->texture2D->GetSurfaceLevel(level, &((nD3D9Surface*)*surface)->baseSurface) );
}

//------------------------------------------------------------------------------
/**
    Generante mip maps for surface data.

    - Feb-04 Kim, H.W. added to support ngameswf.
*/
void nD3D9Texture::GenerateMipMaps()
{
    n_assert(this->texture2D);

    n_dxverify( 
         D3DXFilterTexture(this->texture2D,    // pTexture
                           NULL,               // pPalette
                           D3DX_DEFAULT,       // SrcLevel(0)
                           D3DX_FILTER_LINEAR) // MipFilter
    );
}
                
//------------------------------------------------------------------------------
/**
    Convert nTexture2::Fromat to D3DFORMAT.

    -16-Jun-05    kims    Added to remove duplicated code from nD3D9Texture module
                          and nD3D9Surface module.
*/
D3DFORMAT nD3D9Texture::FormatToD3DFormat(nTexture2::Format format)
{
    D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;
    switch (format)
    {
        case X8R8G8B8:          d3dFormat = D3DFMT_X8R8G8B8;      break;
        case A8R8G8B8:          d3dFormat = D3DFMT_A8R8G8B8;      break;
        case R5G6B5:            d3dFormat = D3DFMT_R5G6B5;        break;
        case A1R5G5B5:          d3dFormat = D3DFMT_A1R5G5B5;      break;
        case A4R4G4B4:          d3dFormat = D3DFMT_A4R4G4B4;      break;
        case P8:                d3dFormat = D3DFMT_P8;            break;
        case G16R16:            d3dFormat = D3DFMT_G16R16;        break;
        case DXT1:              d3dFormat = D3DFMT_DXT1;          break;
        case DXT2:              d3dFormat = D3DFMT_DXT2;          break;
        case DXT3:              d3dFormat = D3DFMT_DXT3;          break;
        case DXT4:              d3dFormat = D3DFMT_DXT4;          break;
        case DXT5:              d3dFormat = D3DFMT_DXT5;          break;
        case R16F:              d3dFormat = D3DFMT_R16F;          break;
        case G16R16F:           d3dFormat = D3DFMT_G16R16F;       break;
        case A16B16G16R16F:     d3dFormat = D3DFMT_A16B16G16R16F; break;
        case R32F:              d3dFormat = D3DFMT_R32F;          break;
        case G32R32F:           d3dFormat = D3DFMT_G32R32F;       break;
        case A32B32G32R32F:     d3dFormat = D3DFMT_A32B32G32R32F; break;
        case A8:                d3dFormat = D3DFMT_A8;            break;
	    case L8:                d3dFormat = D3DFMT_L8;            break;
        default:            
            // can't happen
            n_assert_always();
    }
    
    return d3dFormat;
}

//------------------------------------------------------------------------------
/**
    Render a visualization of the texture for debug purposes
*/
void
nD3D9Texture::DisplayOnFramebuffer()
{
    IDirect3DDevice9* d3d9Device = this->refGfxServer->d3d9Device;
    IDirect3DSurface9* d3d9RenderTarget = NULL;
    IDirect3DSurface9* d3d9BackBuffer = this->refGfxServer->backBufferSurface;
    D3DSURFACE_DESC desc;
    
    UINT locationX[6] = { 2,  0,  1,  1,  1,  3};
    UINT locationY[6] = { 1,  1,  0,  2,  1,  1};
    POINT dest;
    RECT sRect;
    HRESULT hr;
    
    // get surface to backbuffer
    hr = d3d9Device->GetBackBuffer( 0,                       // iSwapChain
                                    0,                       // BackBuffer
                                    D3DBACKBUFFER_TYPE_MONO, // Type
                                    &d3d9BackBuffer );       // ppBackBuffer
    n_assert( SUCCEEDED( hr ) );
    n_assert( d3d9BackBuffer );
    
    switch( this->type )
    {
    case TEXTURE_2D:
        //for( int mip = 0; mip < this->numMipMaps ? this->numMipMaps : 1; mip++ )
        {
        int mip = 0;
        //if( this->IsRenderTarget() )
        hr = this->texture2D->GetSurfaceLevel( mip, &d3d9RenderTarget );
        n_assert( SUCCEEDED( hr ) )
        n_assert( d3d9RenderTarget );
        
        // initialize auxiliary surface in system mem
        if( this->destSurface == 0 )
        {
            d3d9RenderTarget->GetDesc( &desc );
            hr = d3d9Device->CreateOffscreenPlainSurface( desc.Width,           // Width
                                                          desc.Height,          // height
                                                          desc.Format,          // format
                                                          D3DPOOL_SYSTEMMEM,    // pool
                                                          &this->destSurface,   // surface
                                                          NULL );
            n_assert( SUCCEEDED( hr ) );
            n_assert( this->destSurface );
        }
        
        // copy data from D3DPOOL_DEFAULT render target to D3DPOOL_SYSTEMMEM surface
        d3d9Device->GetRenderTargetData( d3d9RenderTarget, this->destSurface );

        if( mip == 0 )
        {
            destSurface->GetDesc( &desc );
            dest.x = 0;
            dest.y = 0;
            sRect.left = sRect.top = 0;
            sRect.right = desc.Width;
            sRect.bottom = desc.Height;
        }
        // @TODO rest of mipmaps
        
        n_dxverify( d3d9Device->UpdateSurface( this->destSurface,
            &sRect,
            d3d9BackBuffer,
            &dest) );
        d3d9RenderTarget->Release();
        }
        break;
        
    case TEXTURE_CUBE:
        for( int face = 0; face < 6; face++ )
        {
            //for( int mip = 0; mip < this->numMipMaps ? this->numMipMaps : 1; mip++ )
            //{
            int mip = 0;
            //if( this->IsRenderTarget() )
            hr = this->textureCube->GetCubeMapSurface( (D3DCUBEMAP_FACES)face, mip, &d3d9RenderTarget );
            n_assert( SUCCEEDED( hr ) )
            n_assert( d3d9RenderTarget );
            
            // initialize auxiliary surface in system mem
            if( this->destSurface == 0 )
            {
                d3d9RenderTarget->GetDesc( &desc );
                hr = d3d9Device->CreateOffscreenPlainSurface( desc.Width,           // Width
                                                              desc.Height,          // height
                                                              desc.Format,          // format
                                                              D3DPOOL_SYSTEMMEM,    // pool
                                                              &this->destSurface,   // surface
                                                              NULL );
                n_assert( SUCCEEDED( hr ) );
                n_assert( this->destSurface );
            }
            
            // copy data from D3DPOOL_DEFAULT render target to D3DPOOL_SYSTEMMEM surface
            d3d9Device->GetRenderTargetData( d3d9RenderTarget, this->destSurface );
            
            if( mip == 0 )
            {
                destSurface->GetDesc( &desc );
                dest.x = locationX[face] * desc.Width;
                dest.y = locationY[face] * desc.Height;
                sRect.left = sRect.top = 0;
                sRect.right = desc.Width;
                sRect.bottom = desc.Height;
            }
            //else
            //@TODO rest of mipmaps
            
            n_dxverify( d3d9Device->UpdateSurface( this->destSurface,
                &sRect,
                d3d9BackBuffer,
                &dest) );
            d3d9RenderTarget->Release();
            //}
        }
        break;
    }
}
//------------------------------------------------------------------------------
/**
    Copy to another compatible texture
*/
void
nD3D9Texture::Copy( nTexture2* dest, const rectanglei& srcRectangle , const rectanglei& dstRectangle, Filter filter ) 
{

    n_assert( this->type == TEXTURE_2D );

    HRESULT hr;
    DWORD dxFilter = this->GetDXFilter( filter);

    // it is normally better use mirroring when not using point filtering
    if (filter != FILTER_POINT)
    {
        dxFilter |= D3DX_FILTER_MIRROR;
    }

    // get source texture
    IDirect3DSurface9* srcSurface = NULL;
    hr = this->texture2D->GetSurfaceLevel( 0, &srcSurface );
    n_assert( srcSurface );
    n_assert( SUCCEEDED(hr) );

    // get destination texture
    IDirect3DSurface9* dstSurface = NULL;
    nD3D9Texture *d3d9texdest = (nD3D9Texture *) dest;
    hr = d3d9texdest->texture2D->GetSurfaceLevel( 0, &dstSurface );
    n_assert( dstSurface );
    n_assert( SUCCEEDED(hr) );

    if (! this->IsRenderTarget())
    {
        RECT srcRect;
        srcRect.left = srcRectangle.v0.x;
        srcRect.right = srcRectangle.v1.x + 1;
        srcRect.top = srcRectangle.v0.y;
        srcRect.bottom = srcRectangle.v1.y + 1;

        RECT dstRect;
        dstRect.left = dstRectangle.v0.x;
        dstRect.right = dstRectangle.v1.x + 1;
        dstRect.top = dstRectangle.v0.y;
        dstRect.bottom = dstRectangle.v1.y + 1;

        hr = D3DXLoadSurfaceFromSurface(
            dstSurface, 0, &dstRect,
            srcSurface, 0, &srcRect,
            dxFilter,
            0);
        n_dxtrace( hr , "Can't copy texture" );
    }
    else
    {
        // get source rectangle
        RECT srcRect;
        srcRect.left = srcRectangle.v0.x;
        srcRect.right = srcRectangle.v1.x + 1;
        srcRect.top = srcRectangle.v0.y;
        srcRect.bottom = srcRectangle.v1.y + 1;

        D3DSURFACE_DESC descSrc, descDst, descTemp;
        
        srcSurface->GetDesc( &descSrc );
        dstSurface->GetDesc( &descDst );

        // get device
        IDirect3DDevice9* d3d9Device = this->refGfxServer->d3d9Device;

        // initialize auxiliary surface in system mem
        if( this->destSurface == 0 )
        {
            hr = d3d9Device->CreateOffscreenPlainSurface( descSrc.Width,           // Width
                                                          descSrc.Height,          // height
                                                          descSrc.Format,          // format
                                                          D3DPOOL_SYSTEMMEM,       // pool
                                                          &this->destSurface,      // surface
                                                          NULL );
            n_assert( SUCCEEDED( hr ) );
            n_assert( this->destSurface );

            this->destSurface->GetDesc( &descTemp );
        }
        
        // copy data from D3DPOOL_DEFAULT render target to D3DPOOL_SYSTEMMEM surface
        hr = d3d9Device->GetRenderTargetData( srcSurface, this->destSurface );

        // copy data from temp texture in memory to dest texture
        RECT dstRect;
        dstRect.left = dstRectangle.v0.x;
        dstRect.right = dstRectangle.v1.x + 1;
        dstRect.top = dstRectangle.v0.y;
        dstRect.bottom = dstRectangle.v1.y + 1;

        hr = D3DXLoadSurfaceFromSurface(
            dstSurface, 0, &dstRect,
            srcSurface, 0, &srcRect,
            dxFilter,
            0);
        n_dxtrace( hr , "Can't copy texture" );
    }

    srcSurface->Release();
    dstSurface->Release();
}

//------------------------------------------------------------------------------
/**
    save image under different name with different format
*/
bool
nD3D9Texture::SaveResourceAs(const char* name, nTexture2::Format format)
{
    if (this->GetType() != nTexture2::TEXTURE_2D )
    {
        return false;
    }

    nD3D9Texture * tmpTex = 0;
    if (format != this->GetFormat())
    {
        // need a temporal texture with the format required
        tmpTex = static_cast<nD3D9Texture *> (nGfxServer2::Instance()->NewTexture(0));
        n_assert( tmpTex );
        tmpTex->SetType( nTexture2::TEXTURE_2D );
        tmpTex->SetFormat( format );
        tmpTex->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic );
        tmpTex->SetWidth( this->GetWidth() );
        tmpTex->SetHeight( this->GetHeight() );
        tmpTex->SetFilename( name ); 
        tmpTex->Load();
        rectanglei rect( 0 , 0 , this->GetWidth() - 1 , this->GetWidth() - 1 );
        // copy to the temporal texture
        this->Copy(tmpTex, rect, rect , FILTER_POINT );
    }
    else
    {
        tmpTex = this;
        tmpTex->AddRef();
    }

    // save the texture
    D3DXIMAGE_FILEFORMAT d3dxformat = ( static_cast<nD3D9Server *>( nD3D9Server::Instance() ) )->GetFormatFromExtension( name );
    nString outFileName = nFileServer2::Instance()->ManglePath(name, false);
    HRESULT result = D3DXSaveTextureToFile(outFileName.Get(), d3dxformat, tmpTex->texture2D, 0);

    // release the texture
    tmpTex->Release();

    return result == S_OK;
}
//------------------------------------------------------------------------------
/**
    @return the directx filter
*/
DWORD
nD3D9Texture::GetDXFilter(Filter filter) const
{
    switch ( filter) 
    {
        case FILTER_POINT:
            return D3DX_FILTER_POINT;
        case FILTER_LINEAR:
            return D3DX_FILTER_LINEAR;
        case FILTER_TRIANGLE:
            return D3DX_FILTER_TRIANGLE;
        default:
        	n_assert2_always("Invalid texture filter");
            return D3DX_FILTER_POINT;
    }
         
}