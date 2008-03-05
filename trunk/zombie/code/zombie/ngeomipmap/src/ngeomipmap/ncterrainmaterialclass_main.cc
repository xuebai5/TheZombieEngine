//------------------------------------------------------------------------------
//  ncterrainmaterialclass.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntypes.h"
#include "resource/nresourceserver.h"
#include "zombieentity/ncloaderclass.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nfileserver2.h"
#include "tools/ntexturebuilder.h"
#include "nscene/ncsceneclass.h"
#include "napplication/napplication.h"
#include "gameplay/ngamematerial.h"
#include "nspatial/nspatialserver.h"
#include "zombieentity/ncdictionary.h"
#include "nscene/ncscenelightenvclass.h"

#include "gfx2/nd3d9texture.h"
#include "mathlib/rectanglei.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncTerrainMaterialClass,nComponentClass);

//------------------------------------------------------------------------------
const char * whiteTexturePath = "wc:libs/system/textures/white.dds";
const char * blackTexturePath = "wc:libs/system/textures/black.dds";
const char * unpaintedTexture = "wc:libs/system/textures/magenta.dds";
const char * redTexturePath = "wc:libs/system/textures/red_noa.dds";
#ifdef NGAME
const char * errorTexturePath = blackTexturePath;
#else
const char * errorTexturePath = "wc:libs/system/textures/error.dds";
const char * blackAlphaTexturePath = "wc:libs/system/textures/black_alpha.dds";
#endif

//------------------------------------------------------------------------------
/**
*/
ncTerrainMaterialClass::ncTerrainMaterialClass():
    selectedLayer( -1 ),
    selectedLayerPos( -1 ),
    newHandle(0),
    weightMapSize(0),
    globalTextureSize(256),
    modulationFactor(0.5f),
    matLODDistance(1),
    importMaterialState(false),
    numBlocks( 0 ),
    isValid( false ),
    currentbx( 0 ),
    currentbz( 0 ),
    numPreload( 0 ),
    materialResolutionDivisor(1)
{
#ifndef NGAME
    this->weightMapBuilder.SetTerrainMaterialComponent( this );
    this->globalTextureBuilder.SetTerrainMaterialComponent( this );
    this->defaultLighmapIsBlack = true;
#endif
}

//------------------------------------------------------------------------------
/**
*/
ncTerrainMaterialClass::~ncTerrainMaterialClass()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainMaterialClass::InitInstance(nObject::InitInstanceMsg)
{
    this->gmmclass = this->GetComponent<ncTerrainGMMClass>();
    n_assert(this->gmmclass);
}

//------------------------------------------------------------------------------
/**
    @brief Get weightmap texture path for weightmap related files (layer paths)
    Format of weightmaps names: weights_x_z.tga
*/
nString
ncTerrainMaterialClass::GetWeightMapTexturePath(int bx, int bz) const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/autogen/w_");
    filename.AppendInt( bx );
    filename.Append("_");
    filename.AppendInt( bz );
    //filename.Append(".tga");
    filename.Append(".dds");
    return filename;
}

//------------------------------------------------------------------------------
/**
    @brief Get weightmap texture path for weightmap related files (layer paths)
    Format of weightmaps names: weights_x_z.tga
*/
nString
ncTerrainMaterialClass::GetLightMapTexturePath(int bx, int bz) const
{
    nEntityObject* lightMap = this->GetLightMap();

    if (!lightMap)
    {
        return "";
    }

    const ncSceneLightEnvClass* sceneLight = lightMap->GetClassComponentSafe<ncSceneLightEnvClass>();
    if ( sceneLight)
    {
        nString filename;
        sceneLight->GetLightmapTexturePath( bx, bz , filename);
        return filename;
    } 
 
    return "";
}

//---------------------------------------------------------------------------
/**    
*/
nTexture2*
ncTerrainMaterialClass::LoadGlobalLightMapTexture()
{

    nEntityObject* lightMap = this->GetLightMap();
    if (!lightMap)
    {
        return 0;
    } 

    const ncSceneLightEnvClass* sceneLight = lightMap->GetClassComponentSafe<ncSceneLightEnvClass>();
    if (!sceneLight)
    {
        return 0;
    }
   
    nTexture2* globalLightMap;
    nString lightMapPath;
    sceneLight->GetGlobalLightmapTexturePath( lightMapPath );
    globalLightMap = nGfxServer2::Instance()->NewTexture(lightMapPath.Get());
    n_assert(globalLightMap);
    globalLightMap->SetFilename(lightMapPath);
    globalLightMap->Load();
    if ( globalLightMap->IsValid() )
    {
        return globalLightMap;
    } else
    {
        globalLightMap->Release();
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
@brief Get weightmap texture path for weightmap related files (layer paths)
Format of weightmaps names: weights_x_z.tga
*/
nString
ncTerrainMaterialClass::GetLayerByteMapPath(int bx, int bz, nTerrainMaterial::LayerHandle handle) const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    char buffer[16];
    sprintf(buffer, "%08X", handle);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/source/l_");
    filename.Append(buffer);
    filename.Append("_");
    filename.AppendInt( bx );
    filename.Append("_");
    filename.AppendInt( bz );
    filename.Append(".tga");
    return filename;
}

//------------------------------------------------------------------------------
/**
    @brief Get number of blocks along terrain side
*/
int
ncTerrainMaterialClass::GetMaterialNumBlocks() const
{
    return this->numBlocks;
}

//------------------------------------------------------------------------------
/**
    @brief Get number of blocks along terrain side
*/
void
ncTerrainMaterialClass::SetMaterialNumBlocks( int numBlocks )
{
    if ( numBlocks <= 0 )
    {
        return;
    }

    this->numBlocks = numBlocks;
}

//------------------------------------------------------------------------------
/**
    @brief Get terrain cell information by block x,z coords
    @param bx block x coord
    @param bz block z coord
    @return Weightmap reference
    For read use only - don't destroy it, etc
*/
nTerrainCellInfo*
ncTerrainMaterialClass::GetTerrainCellInfo(int bx, int bz) const
{
    n_assert( this->GetMaterialNumBlocks() > 0 );
    n_assert( 0 <= bx && bx < this->GetMaterialNumBlocks() );
    n_assert( 0 <= bz && bz < this->GetMaterialNumBlocks() );

    int index = bx + this->GetMaterialNumBlocks() * bz;
    return &this->cellInfo[ index ];
}

//------------------------------------------------------------------------------
/**
    @brief Get heightmap reference
    @return Heightmap reference
    For read use only - don't destroy it, etc
*/
nFloatMap*
ncTerrainMaterialClass::GetHeightMap() const
{
    n_assert( this->refHeightMap.isvalid() );
    return this->refHeightMap.get();
}

//------------------------------------------------------------------------------
/**
    @brief Get global texture path
*/
nString
ncTerrainMaterialClass::GetGlobalTexturePath(void) const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/autogen/global.dds");
    return filename;
}

//------------------------------------------------------------------------------
/**
    @brief Get modulation texture path
*/
nString
ncTerrainMaterialClass::GetModulationTexturePath(void) const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/terrain/modulation.dds");
    return filename;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainMaterialClass::LoadResources(void)
{
    // Get number of blocks
    this->SetMaterialNumBlocks( this->gmmclass->GetNumBlocks() );
    n_assert( this->GetMaterialNumBlocks() > 0 );

    // Get heightmap reference
    n_assert(this->gmmclass->GetHeightMap());
    this->refHeightMap = this->gmmclass->GetHeightMap();

    // get high id from the entity server
    if (!this->newHandle)
    {
        this->newHandle = nEntityObjectServer::Instance()->GetHighId();
    }

    if ( this->layerArray.Size() == 0 )
    {
        this->LoadMaterials();
    }

    if ( this->cellInfo.Size() == 0 )
    {
        this->AllocWeightMapsTable();
        this->LoadWeightMapsTable();
    }

    // Create global texture
    if ( !this->refGlobalTexture.isvalid() )
    {
        this->LoadGlobalTexture();
    }

    if ( !this->refGlobalLightMapTexture.isvalid() )
    {
        this->refGlobalLightMapTexture = this->LoadGlobalLightMapTexture();
    }

    // Create global modulation texture
    if ( !this->refModulationTexture.isvalid() )
    {
        nString path = this->GetModulationTexturePath();

        if (nFileServer2::Instance()->FileExists(path.Get()))
        {
            this->refModulationTexture = nGfxServer2::Instance()->NewTexture(path.Get());
            this->refModulationTexture->SetFilename(path.Get());
            this->refModulationTexture->Load();
        }
        else
        {
            this->refModulationTexture.invalidate();
        }
    }

    /// load white texture to neutralize modulation
    if (! this->refWhiteTexture.isvalid() )
    {
        this->refWhiteTexture = nGfxServer2::Instance()->NewTexture(whiteTexturePath);
        n_assert(this->refWhiteTexture.isvalid());
        this->refWhiteTexture->SetFilename(whiteTexturePath);
        this->refWhiteTexture->Load();
    }
    #ifndef NGAME
    if (! this->refBlackAlphaTexture.isvalid() )
    {
        this->refBlackAlphaTexture= nGfxServer2::Instance()->NewTexture( blackAlphaTexturePath);
        n_assert(this->refBlackAlphaTexture.isvalid());
        this->refBlackAlphaTexture->SetFilename( blackAlphaTexturePath );
        this->refBlackAlphaTexture->Load();
    }
    #endif

    // load system texture for unpainted areas
    if ( ! this->refUnpaintedTexture.isvalid() )
    {
        this->refUnpaintedTexture = nGfxServer2::Instance()->NewTexture(unpaintedTexture);
        n_assert(this->refUnpaintedTexture.get());
        this->refUnpaintedTexture->SetFilename(unpaintedTexture);
        this->refUnpaintedTexture->Load();
    }

    // load black texture for empty weightmaps
    if ( ! this->refBlackTexture.isvalid() )
    {
        this->refBlackTexture = nGfxServer2::Instance()->NewTexture(blackTexturePath);
        n_assert(this->refBlackTexture.get());
        this->refBlackTexture->SetFilename(blackTexturePath);
        this->refBlackTexture->Load();
    }

    if ( ! this->refRedTexture.isvalid() )
    {
        this->refRedTexture = nGfxServer2::Instance()->NewTexture(redTexturePath);
        n_assert(this->refRedTexture.get());
        this->refRedTexture->SetFilename(redTexturePath);
        this->refRedTexture->Load();
    }

    if ( ! this->refErrorTexture.isvalid() )
    {
        this->refErrorTexture = nGfxServer2::Instance()->NewTexture(errorTexturePath);
        n_assert(this->refErrorTexture.get());
        this->refErrorTexture->SetFilename(errorTexturePath);
        this->refErrorTexture->Load();
    }

    if ( ! this->textureCache.IsAllocated() )
    {
        nGMMTextureCacheEntry::Setup( this->GetEntityClass() );
        this->textureCache.Alloc( this->GetDetailCacheSize() );
    }

    if ( ! this->lightMapCache.IsAllocated() )
    {
        nGMMLightMapCacheEntry::Setup(this->GetEntityClass());
        this->lightMapCache.Alloc( this->GetDetailCacheSize() );
    }

    this->profGMMGetMaterial.Initialize("profGMMGetMaterial", true);

    this->isValid = true;

    return true;

}

//---------------------------------------------------------------------------
/**    
*/
int
ncTerrainMaterialClass::GetDetailCacheSize() const
{
    int numEntries = 2 * (this->GetMaterialLODDistance() +  this->GetNumBlockPreload()) + 1;
    numEntries = numEntries * numEntries;
    numEntries = n_min(numEntries, 128); // a bit more than 11 * 11
    return numEntries;
}

//---------------------------------------------------------------------------
/**    
*/
void
ncTerrainMaterialClass::SetLightMap(nEntityObject* lightMapEntity)
{
    this->lightMapEntity = lightMapEntity;

    /// Refresh the texture cache for all lightmaps 
    if ( this->lightMapCache.IsAllocated() )
    {
        this->lightMapCache.DiscardAll();
    }

    if ( this->IsValid() ) // the resource is loded 
    {
        if ( this->refGlobalLightMapTexture.isvalid() ) // Update the global texture
        {
            this->refGlobalLightMapTexture->Release();
        }
        this->refGlobalLightMapTexture = this->LoadGlobalLightMapTexture();
    }

}


//---------------------------------------------------------------------------
/**    
*/
nEntityObject* 
ncTerrainMaterialClass::GetLightMap() const
{
   return this->lightMapEntity.get_unsafe();
}


//------------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::UnloadResources(void)
{
    if (this->refGlobalTexture.isvalid())
    {
        this->refGlobalTexture->Release();
        this->refGlobalTexture.invalidate();
    }

    if (this->refGlobalLightMapTexture.isvalid())
    {
        this->refGlobalLightMapTexture->Release();
        this->refGlobalLightMapTexture.invalidate();
    }

    if (this->refModulationTexture.isvalid())
    {
        this->refModulationTexture->Release();
        this->refModulationTexture.invalidate();
    }

    if (this->refWhiteTexture.isvalid())
    {
        this->refWhiteTexture->Release();
        this->refWhiteTexture.invalidate();
    }

#ifndef NGAME
    if (this->refBlackAlphaTexture.isvalid() )
    {
        this->refBlackAlphaTexture->Release();
        this->refBlackAlphaTexture.invalidate();
    }
#endif

    if (this->refUnpaintedTexture.isvalid())
    {
        this->refUnpaintedTexture->Release();
        this->refUnpaintedTexture.invalidate();
    }

    if (this->refBlackTexture.isvalid())
    {
        this->refBlackTexture->Release();
        this->refBlackTexture.invalidate();
    }

    if (this->refRedTexture.isvalid())
    {
        this->refRedTexture->Release();
        this->refRedTexture.invalidate();
    }

    if (this->refErrorTexture.isvalid())
    {
        this->refErrorTexture->Release();
        this->refErrorTexture.invalidate();
    }

    // unload textures from the texture cache entries
    this->textureCache.Dealloc();
    // unload textures from the lightmap texture cache entries
    this->lightMapCache.Dealloc();


    this->UnloadMaterials();

    this->cellInfo.Reset();

    this->isValid = false;
}

//---------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::IsValid()
{
    return this->isValid;
}

//------------------------------------------------------------------------------   
/**
    @brief Save all resources
*/
bool
ncTerrainMaterialClass::SaveResources(void)
{
    bool succeed = true;

    #ifndef NGAME
    succeed &= this->SaveByteMaps();
    succeed &= this->SaveMaterials();
    succeed &= this->SaveWeightMapsTable();
    #endif

    return succeed;
}

#ifndef NGAME
//------------------------------------------------------------------------------   
/**
    Save all the bytemaps of all the layers of all the blocks.
    Bytemaps completely clean (not used) are not saved.
*/
bool
ncTerrainMaterialClass::SaveByteMaps(void)
{
    for(int bz = 0; bz < this->GetMaterialNumBlocks(); bz++)
    {
        for(int bx = 0; bx < this->GetMaterialNumBlocks(); bx++)
        {
            nTerrainCellInfo* cellInfo = this->GetTerrainCellInfo( bx, bz );
            n_assert( cellInfo );

            for (int layer = 0; layer < this->GetWeightMapNumberOfLayers(bx,bz); layer++)
            {
                nTerrainCellInfo::WeightMapLayerInfo & wmli = cellInfo->GetLayerInfo( layer );

                if ( wmli.refLayer.isvalid() )
                {                    
                    if ( wmli.refLayer->GetAccumulator() )
                    {
                        if (wmli.refLayer->IsDirty())
                        {
                            //wmli.refLayer->Save();
                            nByteMapBuilder bmb;
                            bmb.SaveByteMapOpenIL(wmli.refLayer, wmli.refLayer->GetFilename().Get());
                            wmli.refLayer->CleanDirty();
                        }
                    }
                    else
                    {
                        // If layer use is 0, don't save it (and delete file if exists)
                        nFileServer2::Instance()->DeleteFile( wmli.refLayer->GetFilename() );
                    }
                }
            }
        }
    }

    return true;
}
#endif

//------------------------------------------------------------------------------   
/**
    @brief Add a new layer. Used by persistence
*/
nTerrainMaterial *
ncTerrainMaterialClass::AddLayer(nTerrainMaterial::LayerHandle handle)
{

    if ( this->GetLayerCount() == MAX_TERRAIN_LAYERS )
    {
        return 0;
    }

    // if we are importing materials, shortcut and just create a new material
    if (this->importMaterialState)
    {
        nTerrainMaterial::LayerHandle handle;
        handle = this->CreateLayer();
        nTerrainMaterial * mat = this->GetLayerByHandle(handle);
        if (mat)
        {
            mat->SetImportMaterialState(true);

            // inform to persist server
            nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(mat, nObject::LoadedInstance);
        }

        return mat;
    }

    // initialize handle
    this->newHandle = 0;
    
    /*// get high id from the entity server
    if (!this->newHandle)
    {
        this->newHandle = nEntityObjectServer::Instance()->GetHighId();
    }*/

    // create the terrain material
    nTerrainMaterial* layer = static_cast<nTerrainMaterial*>(nKernelServer::Instance()->New("nterrainmaterial", false));
    if (!layer)
    {
        return 0;
    }
    layer->SetLayerHandle( handle );

    // Append material object
    layerArray.Append( layer );

    // Refresh last assigned handle
    if ((handle & nEntityObjectServer::IDHIGHMASK) == (nEntityObjectServer::Instance()->GetHighId() & nEntityObjectServer::IDHIGHMASK))
    {
        this->newHandle = max(this->newHandle, handle);
    }

    // if layer not selected, autoselect the first one added
    if (this->selectedLayerPos == -1)
    {
        this->selectedLayerPos = this->layerArray.Size() - 1;
        this->selectedLayer = - 1;
    }

    // inform to persist server
    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(layer, nObject::LoadedInstance);

    return layer;
}

//------------------------------------------------------------------------------   
/**
    @brief Add a new blank layer
    @param handle On success, returned layer handle
    @return If returnd false, the max numbers of layers have been reached
*/
nTerrainMaterial::LayerHandle
ncTerrainMaterialClass::CreateLayer()
{
    if ( this->GetLayerCount() == MAX_TERRAIN_LAYERS )
    {
        return -1;
    }

    // get high id from the entity server
    if (!this->newHandle)
    {
        this->newHandle = nEntityObjectServer::Instance()->GetHighId();
    }

    nTerrainMaterial::LayerHandle handle = ++this->newHandle;

    // create the terrain material
    nTerrainMaterial* layer = static_cast<nTerrainMaterial*>(nKernelServer::Instance()->New("nterrainmaterial", false));
    if ( !layer )
    {
        return -1;
    }
    layer->SetLayerHandle( handle );
    nString strTemp = DEFAULT_TERRAIN_TEXTURE;
    layer->SetTextureFileName( strTemp.Get() );
    layer->SetProjection( nTerrainMaterial::ProjXZ );
    layer->SetUVScale( vector2(1.0f, 1.0f) );
    layer->SetMaskColor( vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
    layer->SetLabel("Unnamed layer");

    this->layerArray.Append( layer );

    return this->newHandle;
}

//------------------------------------------------------------------------------   
/**
    @brief Remove a layer
    @param Layer handle to remove
    @return False if the layer handle doesn't exist
*/
bool
ncTerrainMaterialClass::RemoveLayer( nTerrainMaterial::LayerHandle hnd )
{
    int pos = GetLayerPos( hnd );
    if (pos < 0)
    {
        return false;
    }

    // remove layer
    nTerrainMaterial* layer = layerArray[ pos ];
    layer->Release();    

    #ifndef NGAME
    // remove layer from weightmaps
    for ( int i = 0; i < this->GetMaterialNumBlocks() * this->GetMaterialNumBlocks(); i++ )
    {
        this->cellInfo[ i ].RemoveLayer( hnd );
    }
    #endif

    // remove the array entry
    layerArray.EraseQuick( pos );

    // Check if selected layer is now invalid
    if ( this->selectedLayer == hnd )
    {
        this->selectedLayerPos = - 1;
        this->selectedLayer = - 1;
    }
    else
    {
        // Refresh selected layer position
        this->selectedLayerPos = GetLayerPos( this->selectedLayer );
    }

    return true;
}

//------------------------------------------------------------------------------   
/**
    @brief Reset layers
    @param Layer handle to remove
*/
void
ncTerrainMaterialClass::Reset()
{
    while ( this->GetLayerCount() > 0 )
    {
        this->RemoveLayer( this->GetLayerHandle( this->GetLayerCount() - 1 ) );
    }

    this->newHandle = 0;
}

//------------------------------------------------------------------------------
nString 
ncTerrainMaterialClass::GetMaterialsPath() const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/terrain/materials.n2");
    return filename;
}

//------------------------------------------------------------------------------
nString 
ncTerrainMaterialClass::GetWeightMapsTablePath() const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/terrain/weightmaptable.n2");
    return filename;
}

//------------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::LoadMaterials()
{
    // get filename where the material are stored
    nString filename = this->GetMaterialsPath();

    // load the file
    nKernelServer::Instance()->PushCwd(this->GetEntityClass());
    nObject * obj = nKernelServer::Instance()->Load(filename.Get(), false);
    nKernelServer::Instance()->PopCwd();

    // load material resources
    for (int i = 0;i < this->GetLayerCount();i++)
    {
        this->layerArray.At(i)->LoadResources();
    }

    return (obj != 0);
}

//------------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::UnloadMaterials()
{
    // remove all layers one by one
    while( this->GetLayerCount() > 0 )
    {
        nTerrainMaterial::LayerHandle hnd = this->GetLayerHandle(0);
        this->RemoveLayer( hnd );
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Import all the materials from another class. The entity class object pointer
    must be provided, and its materials file is accessed directly and loaded
    in the current entity class.
*/
bool 
ncTerrainMaterialClass::ImportMaterials(nEntityClass * outdoor)
{
    ncTerrainMaterialClass * tmc = outdoor->GetComponent<ncTerrainMaterialClass>();
    if (tmc)
    {
        // get filename where the material are stored
        nString filename = tmc->GetMaterialsPath();

        // load the file
        nKernelServer::Instance()->PushCwd(this->GetEntityClass());
        this->importMaterialState = true;
        nObject * obj = nKernelServer::Instance()->Load(filename.Get(), false);
        this->importMaterialState = false;
        nKernelServer::Instance()->PopCwd();

        // clean the import material state from the imported materials
        for (int i = 0;i < this->GetLayerCount();i++)
        {
            this->layerArray.At(i)->SetImportMaterialState(false);
        }

        // reload resources if needed
        if (obj != 0)
        {
            this->LoadResources();
        }

        return (obj != 0);
    }

    return false;
}

//------------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::SaveMaterials()
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // construct filename of the config file
    nString filename = this->GetMaterialsPath();

    // save configuration with cmd BeginNewObjectConfig cmd
    bool retval = false;
    nCmd * cmd = ps->GetCmd(this->GetEntityClass(), 'THIS');
    if (ps->BeginObjectWithCmd(this->GetEntityClass(), cmd, filename.Get())) 
    {

        for(nEntityObjectId hid = 0;hid < (1 << nEntityObjectServer::IDHIGHBITS);hid++)
        {
            ps->Put(this->GetEntityClass(), 'BSMN', (hid << nEntityObjectServer::IDLOWBITS));

            // persist all layers (nobjects)
            for(int i = 0;i < this->GetLayerCount();i++)
            {
                if ((hid << nEntityObjectServer::IDLOWBITS) == (this->layerArray.At(i)->GetLayerHandle() & nEntityObjectServer::IDHIGHMASK))
                {
                    // --- addlayer
                    cmd = ps->GetCmd(this->GetEntityClass(), 'BADL');
                    cmd->In()->SetI(this->layerArray.At(i)->GetLayerHandle());
                    if (ps->BeginObjectWithCmd(this->GetEntityClass(), cmd))
                    {
                        // save cmds of the layer
                        this->layerArray.At(i)->SaveCmds(ps);
                    }
                    ps->EndObject( false );
                }
            }
        }

        ps->EndObject(false);
        retval = true;
    }

    return retval;
}

//------------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::SaveWeightMapsTable()
{
#ifndef NGAME
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // construct filename of the config file
    nString filename = this->GetWeightMapsTablePath();

    // save table with cmd BeginNewObjectConfig cmd
    bool retval = false;
    nCmd * cmd = ps->GetCmd(this->GetEntityClass(), 'THIS');
    if (ps->BeginObjectWithCmd(this->GetEntityClass(), cmd, filename.Get())) 
    {
        for ( int z = 0; z < this->GetMaterialNumBlocks(); z++ )
        {
            for ( int x = 0; x < this->GetMaterialNumBlocks(); x++ )
            {
                nTerrainCellInfo * cellInfo = this->GetTerrainCellInfo( x, z );
                n_assert( cellInfo );
                
                // Save current weight map size and flags
                ps->Put(this->GetEntityClass(), 'BSCW', 
                    x, z, cellInfo->GetWeightMapSize(), cellInfo->GetFlags() & ~nTerrainCellInfo::HasGrass );

                // Save grass flags,
                ps->Put(this->GetEntityClass(), 'CSIF', nTerrainCellInfo::HasGrass , cellInfo->GetFlags(nTerrainCellInfo::HasGrass) );

#ifndef NGAME
                // Save the default growthMapValue
                ps->Put(this->GetEntityClass(), 'CGMV', cellInfo->GetGrowthMapValue() );
#endif
                
                // Save weight map layer handles
                for ( int layerIndex = 0; layerIndex < cellInfo->GetNumberOfLayers(); layerIndex++ )
                {
                    // Only if layer is used
                    ps->Put(this->GetEntityClass(), 'BAWH', cellInfo->GetLayerHandle(layerIndex));
                }
                
                // Save weight map layer indexes
                const nuint8 * indices = cellInfo->GetIndices();
                ps->Put(this->GetEntityClass(), 'BCWI', 
                    indices[0], indices[1], indices[2], indices[3], indices[4] );
            }
        }

        ps->EndObject(false);
        retval = true;
    }
    return retval;
#else
    return true;
#endif
}

//------------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::LoadWeightMapsTable()
{
    n_assert( this->GetMaterialNumBlocks() > 0);

    // get filename where the material are stored
    nString filename = this->GetWeightMapsTablePath();

    // load the file
    nKernelServer::Instance()->PushCwd(this->GetEntityClass());
    nObject * obj = nKernelServer::Instance()->Load(filename.Get(), false);
    nKernelServer::Instance()->PopCwd();

    return (obj != 0);
}

//------------------------------------------------------------------------------
bool 
ncTerrainMaterialClass::AllocWeightMapsTable()
{
    // Alloc weight maps table
    if ( ! this->cellInfo.Size() == 0 )
    {
        return false;
    }

    int size = this->GetAllWeightMapsSize();

    // Allocate weightmap info table
    this->cellInfo.SetFixedSize(this->GetMaterialNumBlocks() * this->GetMaterialNumBlocks());

    for ( int bz = 0; bz < this->GetMaterialNumBlocks(); bz++ )
    {
        for ( int bx = 0; bx < this->GetMaterialNumBlocks(); bx++ )
        {
            nTerrainCellInfo * cellInfo = this->GetTerrainCellInfo(bx, bz);
            cellInfo->Setup(this->GetEntityClass(), 0, bx, bz);
            if ( size > 0 )
            {
                cellInfo->SetWeightMapSize( size );
            }
            cellInfo->SetDirty( true );
        }
    }

    return true;
}

//------------------------------------------------------------------------------
void
ncTerrainMaterialClass::SetMagicNumber(int)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
    @brief Set all weightmaps size, common for all blocks
*/
void
ncTerrainMaterialClass::SetAllWeightMapsSize(int size)
{
    // Must be power of 2
    n_assert( size == 0 || (!(size & (size-1))) );
    n_assert(size >= 0 && size <= ushort(~0));


    this->weightMapSize = static_cast<ushort> (size);

    if ( this->cellInfo.Size() > 0 )
    {
        for ( int z = 0; z < this->numBlocks; z++ )
        {
            for ( int x = 0; x < this->numBlocks; x++ )
            {
                nTerrainCellInfo * wmap = this->GetTerrainCellInfo( x, z );
                n_assert(wmap);
                wmap->SetWeightMapSize( size );
            }
        }
    }
}

//---------------------------------------------------------------------------
/**
    @brief Get common weightmaps size, common for all blocks
*/
int
ncTerrainMaterialClass::GetAllWeightMapsSize() const
{
    return this->weightMapSize;
}

//---------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::SetWeightMapSize(int size)
{
    // Must be power of 2
    n_assert( size > 0 && (!(size & (size-1))) );
    n_assert(size >= 0 && size <= ushort(~0));

    this->SetAllWeightMapsSize( size );
}

//---------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::SetSingleWeightMapSize(int x, int z, int size)
{
    // Must be power of 2
    n_assert( size > 0 || (!(size & (size-1))) );
    n_assert( size >= 0 && size <= ushort(~0));

    if ( ! ( ( size > 0 && (!(size & (size-1))) ) && ( size >= 0 && size <= ushort(~0)) ) )
    {
        return;
    }

    this->GetTerrainCellInfo(x, z)->SetWeightMapSize( size );
}

//---------------------------------------------------------------------------
/**
*/
int 
ncTerrainMaterialClass::GetSingleWeightMapSize(int x, int z) const
{
    n_assert( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks );
    if ( ! ( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks ) )
    {
        return 0;
    }
    return this->GetTerrainCellInfo(x, z)->GetWeightMapSize();
}

//---------------------------------------------------------------------------

int
ncTerrainMaterialClass::GetWeightMapNumberOfLayers(int x, int z) const
{
    n_assert( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks );
    if ( ! ( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks ) )
    {
        return 0;
    }

    return this->GetTerrainCellInfo(x, z)->GetNumberOfLayers();
}

//---------------------------------------------------------------------------

int
ncTerrainMaterialClass::GetWeightMapLayerHandle(int x, int z, int index) const
{
    n_assert( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks );
    if ( !( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks ) )
    {
        return InvalidLayerHandle;
    }
    return this->GetTerrainCellInfo(x, z)->GetLayerHandle( index );
}

//---------------------------------------------------------------------------

void
ncTerrainMaterialClass::AddWeightMapLayerHandle(int x, int z, int layerHandle)
{
    n_assert( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks );
    if ( ! ( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks ) )
    {
        return;
    }

    this->GetTerrainCellInfo(x, z)->AddLayerHandle( layerHandle );
}

//---------------------------------------------------------------------------

void
ncTerrainMaterialClass::SetWeightMapLayerIndex(int x, int z, int pos, int index)
{
    n_assert( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks );
    n_assert( pos <= pos&& pos < 5 );
    if ( !( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks ) ||  ! ( pos <= pos&& pos < 5 ) )
    {
        return;
    }
    this->GetTerrainCellInfo(x, z)->SetLayerIndex( pos, index );
}

//---------------------------------------------------------------------------

int
ncTerrainMaterialClass::GetWeightMapLayerIndex(int x, int z, int pos ) const
{
    n_assert( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks );
    n_assert( pos <= pos&& pos < 5 );
    if ( !( 0 <= x && x < this->numBlocks && 0 <= z && z < this->numBlocks ) ||  ! ( pos <= pos&& pos < 5 ) )
    {
        return 0xFF;
    }
    return this->GetTerrainCellInfo(x, z)->GetLayerIndex( pos );
}

//---------------------------------------------------------------------------
/**
*/
int
ncTerrainMaterialClass::GetWeightMapLayerIndexByHandle(int x, int z, int handle) const
{
    nTerrainCellInfo * cellInfo = this->GetTerrainCellInfo(x, z);
    
    for(int i = 0;i < cellInfo->GetNumberOfLayers();i++)
    {
        if(cellInfo->GetLayerHandle(i) == handle)
        {
            return i;
        }
    }

    return 255;
}

//---------------------------------------------------------------------------
/**
*/
int 
ncTerrainMaterialClass::GetWeightMapSize() const
{
    return this->weightMapSize;
}

//------------------------------------------------------------------------------
/// Sets global texture size
void
ncTerrainMaterialClass::SetGlobalTextureSize ( int size )
{
    // Must be power of 2
    n_assert( size > 0 && (!(size & (size-1))) );
    n_assert(size >= 0 && size <= ushort(~0));

    this->globalTextureSize = static_cast<ushort> (size);
}

//------------------------------------------------------------------------------
/// Gets global texture size
int
ncTerrainMaterialClass::GetGlobalTextureSize () const
{
    return this->globalTextureSize;
}

//------------------------------------------------------------------------------
/// Get global texture

nTexture2*
ncTerrainMaterialClass::GetGlobalTexture() const
{
    if (this->refGlobalTexture.isvalid())
    {
        return this->refGlobalTexture.get();
    }
    else if (this->refUnpaintedTexture.isvalid())
    {
        return this->refUnpaintedTexture.get();
    }

    return 0;
}

//------------------------------------------------------------------------------
void
ncTerrainMaterialClass::SetMaterialLODDistance( int dist )
{
    n_assert( dist >= 0 );
    this->matLODDistance = dist;
    if (this->textureCache.GetNumEntries() > 0)
    {
        this->textureCache.Dealloc();
        this->textureCache.Alloc( this->GetDetailCacheSize() );
        this->SetAllCellsMaterialDirty();
    }
}

//------------------------------------------------------------------------------
int
ncTerrainMaterialClass::GetMaterialLODDistance() const
{
    return this->matLODDistance;
}

//------------------------------------------------------------------------------
void
ncTerrainMaterialClass::SetNumBlockPreload( int preload )
{
    n_assert( preload >= 0 );
    this->numPreload = preload;
    if (this->textureCache.GetNumEntries() > 0)
    {
        this->textureCache.Dealloc();
        this->textureCache.Alloc( this->GetDetailCacheSize() );
        this->SetAllCellsMaterialDirty();
    }
}

//------------------------------------------------------------------------------
int
ncTerrainMaterialClass::GetNumBlockPreload() const
{
    return this->numPreload;
}

//------------------------------------------------------------------------------
void
ncTerrainMaterialClass::SetModulationFactor(float factor)
{
    this->modulationFactor = factor;
}

//------------------------------------------------------------------------------
float
ncTerrainMaterialClass::GetModulationFactor() const
{
    return this->modulationFactor;
}

//------------------------------------------------------------------------------
/**
    Returns the modulation texture if exists, if not exists then it returns
    a texture that neutralizes the modulation.
*/
nTexture2 * 
ncTerrainMaterialClass::GetModulationTexture() const
{
    if (this->refModulationTexture.isvalid())
    {
        return this->refModulationTexture.get();
    }
    else if (this->refWhiteTexture.isvalid())
    {       
        return this->refWhiteTexture.get();
    }

    n_assert_always();
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Get number of layers
    @return The number of layers
*/
int
ncTerrainMaterialClass::GetLayerCount() const
{
    return layerArray.Size();
}

//------------------------------------------------------------------------------
/**
    @brief Get a layer handle by its position in the stack
    @param pos Layer position, must be >= 0 and < GetLayerCount
    @return The handle of the layer
*/
nTerrainMaterial::LayerHandle
ncTerrainMaterialClass::GetLayerHandle( int pos ) const
{
    n_assert( pos >= 0 && pos < this->GetLayerCount() );
    return layerArray[ pos ]->GetLayerHandle();
}

//------------------------------------------------------------------------------
/**
    @brief Get a layer position by its handle
    @param hnd Layer handle
    @return The layer position, or <0 if the handle doesn't exist
*/
int
ncTerrainMaterialClass::GetLayerPos( nTerrainMaterial::LayerHandle hnd ) const
{
    for (int i=0; i < this->GetLayerCount(); i++)
    {
        if ( layerArray[i]->GetLayerHandle() == hnd )
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    @brief Get a layer by its handle
    @param hnd Handle of the layer
    @return Pointer to the layer
    Only for read use - don't destroy it, etc
    
*/
nTerrainMaterial*
ncTerrainMaterialClass::GetLayerByHandle( nTerrainMaterial::LayerHandle hnd ) const
{
    int p = GetLayerPos( hnd );
    if ( p >= 0 )
    {
        return layerArray[ p ];
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get a layer by its position in the array
    @param pos Layer position, must be >= 0 and < GetLayerCount
    @return Pointer to the layer
    Only for read use - don't destroy it, etc
*/
nTerrainMaterial*
ncTerrainMaterialClass::GetLayerByIndex( int pos ) const
{
    n_assert( pos >= 0 && pos < this->GetLayerCount() );
    return layerArray[pos];
}

//------------------------------------------------------------------------------
/**
    @brief Set the selected layer
    @param Handle of the layer to be selected
*/
bool
ncTerrainMaterialClass::SelectLayer( nTerrainMaterial::LayerHandle layerHnd )
{
    nTerrainMaterial* m = this->GetLayerByHandle( layerHnd );
    if ( m )
    {
        this->selectedLayer = layerHnd;
        this->selectedLayerPos = GetLayerPos( this->selectedLayer );

        return true;
    }
    else
    {
        this->selectedLayer = -1;
        this->selectedLayerPos = -1;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get the selected layer handle
*/
nTerrainMaterial::LayerHandle
ncTerrainMaterialClass::GetSelectedLayerHandle() const
{
    return this->selectedLayer;
}

//------------------------------------------------------------------------------
/**
    @brief Get the selected layer (Only for read)
*/
nTerrainMaterial*
ncTerrainMaterialClass::GetSelectedLayer() const
{
    if (this->selectedLayerPos != -1)
    {
        return GetLayerByIndex( this->selectedLayerPos );
    }

    return 0;
}

#ifndef NGAME
//------------------------------------------------------------------------------   
/**
    @brief Refresh all weight textures in the cache.
    @param Layer handle to remove
*/
void
ncTerrainMaterialClass::RefreshWeightTexture()
{
    for(int bz = 0;bz < this->numBlocks;bz++)
    {
        for(int bx = 0;bx < this->numBlocks;bx++)
        {
            nGMMTextureCacheEntry * entry = this->textureCache.Lookup( nGMMTextureCacheEntry::ToKey(bx, bz), 0, false);
            // only try to rebuild the textures that are generated by the terrain editor (which are uncompressed)
            if(entry && entry->GetWeightMapTexture()->GetFormat() == nTexture2::A8R8G8B8)
            {
                this->weightMapBuilder.SetWeightTexture( entry->GetWeightMapTexture() );
                this->weightMapBuilder.RefreshWeightMap(bx, bz);
            }
        }
    }
}

//------------------------------------------------------------------------------   
/**
    @brief Reset layers
    @param Layer handle to remove
*/
void
ncTerrainMaterialClass::RefreshGlobalTexture()
{
    this->globalTextureBuilder.GenerateGlobalTexture( 0, 0, this->numBlocks - 1, this->numBlocks - 1 );
}

//------------------------------------------------------------------------------
/**
    Refresh global texture. Parameter coordinates are in max weightmap resolution texels.
*/
void
ncTerrainMaterialClass::RefreshWeightTexturePortion( int x0, int z0, int x1, int z1 )
{
    this->ClampGlobalWeightMapCoordinates(x0, z0, x1, z1);

    int bx0, bz0;
    this->GlobalWeightMapCoordToBlockCoord(x0, z0, bx0, bz0);

    int bx1, bz1;
    this->GlobalWeightMapCoordToBlockCoord(x1, z1, bx1, bz1);

    this->weightMapBuilder.CheckBlockBoundaries( bx0, bz0, bx1, bz1 );

    for(int bz = bz0; bz <= bz1; bz++)
    {
        for(int bx = bx0;bx <= bx1;bx++)
        {
            nGMMTextureCacheEntry * entry = this->textureCache.Lookup( nGMMTextureCacheEntry::ToKey(bx, bz), 0, false );
            if (entry)
            {
#if 1
                this->textureCache.Discard( nGMMTextureCacheEntry::ToKey(bx, bz) );
#else
                if (entry->GetWeightMapTexture()->GetFormat() != nTexture2::A8R8G8B8)
                {
                    this->textureCache.Discard( nGMMTextureCacheEntry::ToKey(bx, bz) );
                }
                else
                {
                    this->weightMapBuilder.SetWeightTexture(entry->GetWeightMapTexture());
                    //this->weightMapBuilder.Generate(bx, bz, x0, z0, x1, z1);
                    this->weightMapBuilder.Generate(bx, bz);
                }
#endif
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Refresh a portion of the global texture
*/
void
ncTerrainMaterialClass::RefreshGlobalTexturePortion( int x0, int z0, int x1, int z1 )
{
    n_assert( this->isValid );

    this->globalTextureBuilder.GenerateGlobalTexture( x0, z0, x1, z1 );
}
#endif

//------------------------------------------------------------------------------
/**
    Set current weightmap, size, flags
*/
void
ncTerrainMaterialClass::SetCurrentWeightMap(int bx, int bz, int size, int flags)
{
    this->currentbx = bx;
    this->currentbz = bz;
    this->SetSingleWeightMapSize(bx, bz, size);
    this->GetTerrainCellInfo(bx, bz)->SetFlags( flags );
}

//------------------------------------------------------------------------------
/**
    Set flag with a mask in the current cellinfo
*/
void
ncTerrainMaterialClass::SetCellInfoFlags(int mask, int flags)
{
    this->GetTerrainCellInfo( this->currentbx, this->currentbz )->SetFlags( mask, flags );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Set flag with a mask in the current cellinfo
*/
void
ncTerrainMaterialClass::SetCellInfoGrowthMapValue(int value)
{
    this->GetTerrainCellInfo( this->currentbx, this->currentbz )->SetGrowthMapValue(value);
}
#else
//hack for persisted weightmaptables
void
ncTerrainMaterialClass::SetCellInfoGrowthMapValue(int /*value*/)
{
    //empty
}
#endif NGAME

//------------------------------------------------------------------------------
/**
    Adds a layer to the current weightmap by handle
*/
void
ncTerrainMaterialClass::AddCurrentWeightMapLayerHandle(int layerHandle)
{
    this->AddWeightMapLayerHandle( this->currentbx, this->currentbz, layerHandle);
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::SetCurrentWeightMapIndices(int layer0, int layer1, int layer2, int layer3, int layer4)
{
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 0, layer0);
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 1, layer1);
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 2, layer2);
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 3, layer3);
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 4, layer4);
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::SetCurrentWeightMapHandles(int layer0, int layer1, int layer2, int layer3, int layer4)
{
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 0, 
        this->GetWeightMapLayerIndexByHandle(this->currentbx, this->currentbz, layer0 ) );
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 1, 
        this->GetWeightMapLayerIndexByHandle(this->currentbx, this->currentbz, layer1 ) );
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 2, 
        this->GetWeightMapLayerIndexByHandle(this->currentbx, this->currentbz, layer2 ) );
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 3, 
        this->GetWeightMapLayerIndexByHandle(this->currentbx, this->currentbz, layer3 ) );
    this->SetWeightMapLayerIndex(this->currentbx, this->currentbz, 4, 
        this->GetWeightMapLayerIndexByHandle(this->currentbx, this->currentbz, layer4 ) );
}

//------------------------------------------------------------------------------
/**
    Return the flags for block (bx, bz)
*/
int
ncTerrainMaterialClass::GetSingleWeightMapFlags(int bx, int bz) const
{
    return this->GetTerrainCellInfo(bx, bz)->GetFlags();
}

//------------------------------------------------------------------------------
/**
    Set the flags for the block (bx, bz)
*/
void
ncTerrainMaterialClass::SetSingleWeightMapFlags(int bx, int bz, int flags)
{
    this->GetTerrainCellInfo(bx, bz)->SetFlags(flags);
}

//---------------------------------------------------------------------------
/**
    Create an empty & dynamic texture to be used for weightmaps
*/
nTexture2 *
ncTerrainMaterialClass::NewWeightMapTexture(const char * resName) const
{
    nTexture2 * tex = nGfxServer2::Instance()->NewTexture(resName);
    n_assert(tex);

    if (tex)
    {
        tex->SetType( nTexture2::TEXTURE_2D );
        tex->SetFormat( nTexture2::A8R8G8B8 );
#ifndef NGAME
        tex->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic );
#endif
        tex->SetWidth( ushort( this->GetAllWeightMapsSize() ) );
        tex->SetHeight( ushort( this->GetAllWeightMapsSize() ) );
        tex->SetFilename( resName ); 
        tex->Load();
    }

    return tex;
}

//---------------------------------------------------------------------------
/**
    Loads the terrain global texture resource, used for minimum detail

    In the editor creates an uncompressed texture, since it has to be modified
    from the terrain tools. This uncompressed texture is filled with the
    compressed contents loaded from disk.

    In game, the uncompressed texture is loaded directly.
*/
bool
ncTerrainMaterialClass::LoadGlobalTexture()
{
    #ifndef NGAME

    /// release the texture if valid, reload again (only for the editor)
    if( this->refGlobalTexture.isvalid() && this->refGlobalTexture->IsValid() )
    {
        this->refGlobalTexture->Release();
        this->refGlobalTexture.invalidate();
    }

    nString path = this->GetGlobalTexturePath();
    bool fileExists = nFileServer2::Instance()->FileExists( path );

    this->refGlobalTexture = nGfxServer2::Instance()->NewTexture(0);
    n_assert_return(this->refGlobalTexture.isvalid(), false);

    this->refGlobalTexture->SetType( nTexture2::TEXTURE_2D );
    this->refGlobalTexture->SetFormat( nTexture2::A8R8G8B8 );
    this->refGlobalTexture->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic );
    this->refGlobalTexture->SetWidth( ushort(this->GetGlobalTextureSize()) );
    this->refGlobalTexture->SetHeight( ushort(this->GetGlobalTextureSize()) );
    this->refGlobalTexture->SetFilename( path );
    n_verify( this->refGlobalTexture->Load() );

    if( fileExists )
    {
        // try to load the compressed texture from disk 
        nTexture2 * loadTex = nGfxServer2::Instance()->NewTexture( path.Get() );
        loadTex->SetFilename( path );
        fileExists = loadTex->Load();

        // if the loading was ok -> copy to the allocated uncompressed texture 
        if( fileExists )
        {
            int width = this->GetGlobalTextureSize() - 1;
            if( width > 0 )
            {
                rectanglei rect( 0, 0, width - 1, width - 1 );
                loadTex->Copy( this->refGlobalTexture.get(), rect, rect, nTexture2::FILTER_POINT );
            }
        }
        loadTex->Release();
    }

    if( !fileExists )
    {
        nTextureBuilder texBuilder;
        texBuilder.SetTexture( this->refGlobalTexture.get() );
        // fill color is magenta (unpainted color)
        vector4 fillColor( 1.0f, 0.0f, 1.0f, 1.0f );
        texBuilder.Fill(fillColor);
    }

    // @todo check floatmaps dirty and change the global texture properly

    bool globalOK = this->refGlobalTexture->IsLoaded() &&
                    this->refGlobalTexture->GetWidth() == this->globalTextureSize &&
                    this->refGlobalTexture->GetHeight() == this->globalTextureSize;

    if ( ! globalOK ) 
    {
        this->refGlobalTexture.invalidate();
    }

    #else

    if( !this->refGlobalTexture.isvalid() || !this->refGlobalTexture->IsValid() )
    {
        nString path = this->GetGlobalTexturePath();
        this->refGlobalTexture = nGfxServer2::Instance()->NewTexture( path.Get() );
        n_assert_return(this->refGlobalTexture.isvalid(), false);
        this->refGlobalTexture->SetFilename( path );
        this->refGlobalTexture->Load();
    }

    #endif

    return( this->refGlobalTexture.isvalid() && this->refGlobalTexture->IsValid() );
}

//---------------------------------------------------------------------------
/**    
*/
nTexture2 *
ncTerrainMaterialClass::GetWeightMapTexture(int bx, int bz, bool loadOnMiss)
{
    nTerrainCellInfo * info = this->GetTerrainCellInfo( bx, bz );

    if (info->GetFlags() & nTerrainCellInfo::EmptyLayer)
    {
        return this->refWhiteTexture.get();
    }
    else if (info->GetFlags() & nTerrainCellInfo::SingleLayer)
    {
        if (! this->InTerrainEditorState())
        {
            return this->refRedTexture.get();
        }        
    }

    // check if texture is available for this cell
    nGMMTextureCacheEntry * entry = this->textureCache.Lookup( nGMMTextureCacheEntry::ToKey(bx, bz), 0, loadOnMiss);
    if (entry)
    {
        nTexture2 * tex = entry->GetWeightMapTexture();
        if (tex)
        {
            if (tex->IsLoaded())
            {
                return tex;
            }
        }
    }

    return this->refErrorTexture.get();
}

//---------------------------------------------------------------------------
/**    
*/
bool
ncTerrainMaterialClass::IsWeightMapCached(int bx, int bz)
{
    // check if texture is available for this cell
    nGMMTextureCacheEntry * entry = this->textureCache.Lookup( nGMMTextureCacheEntry::ToKey(bx, bz), 0, false);
    if (entry)
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
/**    
*/
nTexture2 *
ncTerrainMaterialClass::GetLightMapTexture(int bx, int bz)
{
    // check if texture is available for this cell
    nGMMLightMapCacheEntry * entry = this->lightMapCache.Lookup( nGMMTextureCacheEntry::ToKey(bx, bz), 0 );
    if (entry)
    {
        nTexture2 * tex = entry->GetLightMapTexture();
        if (tex)
        {
            if (tex->IsLoaded())
            {
                return tex;
            }
        }
    }

#ifndef NGAME
    if ( this->defaultLighmapIsBlack &&  refBlackAlphaTexture.isvalid() )
    {
        return this->refBlackAlphaTexture.get();
    }
#endif

    if ( this->refWhiteTexture.isvalid() )
    {
        return  refWhiteTexture.get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainMaterialClass::GetNumLightmapTextures() const
{
    return this->lightMapCache.GetNumUsedEntries();
}

//------------------------------------------------------------------------------
/**
*/
nTexture2*
ncTerrainMaterialClass::GetLightmapTextureAt(int index, int& bx, int& bz) const
{
    n_assert_return(index < this->lightMapCache.GetNumUsedEntries(), 0);
    nGMMLightMapCacheEntry *entry = this->lightMapCache.GetEntryByIndex(index);
    nTexture2 * tex = entry->GetLightMapTexture();
    if (tex)
    {
        if (tex->IsLoaded())
        {
            nGMMLightMapCacheEntry::FromKey(entry->GetKey(), bx, bz);
            return tex;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Get global texture
*/
nTexture2*
ncTerrainMaterialClass::GetGlobalLighMapTexture() const
{
    if (this->refGlobalLightMapTexture.isvalid())
    {
        return this->refGlobalLightMapTexture.get();
    }
#ifndef NGAME
    else if ( this->defaultLighmapIsBlack &&  refBlackAlphaTexture.isvalid() )
    {
        return this->refBlackAlphaTexture.get();
    }
#endif 
    else if (this->refWhiteTexture.isvalid())
    {
        return this->refWhiteTexture.get();
    }
    return 0;
}




//---------------------------------------------------------------------------
/**    
    @return true if terrain editor is selected, false otherwise
*/
bool 
ncTerrainMaterialClass::InTerrainEditorState()
{
#ifndef NGAME
    if (nApplication::Instance()->GetCurrentState() == "terrain")
    {
        return true;
    }
#endif
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainMaterialClass::GlobalWeightMapCoordToBlockCoord(int x, int z, int & bx, int & bz) const
{
    bx = x / this->GetAllWeightMapsSize();
    bz = z / this->GetAllWeightMapsSize();

    n_assert( bx >= 0 && bx < this->GetMaterialNumBlocks());
    n_assert( bz >= 0 && bz < this->GetMaterialNumBlocks());
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainMaterialClass::ClampGlobalWeightMapCoordinates(int & x0, int & z0, int & x1, int & z1) const
{
    // swap coords if inverted
    if ( x1 < x0 )
    {
        int t = x0;
        x0 = x1;
        x1 = t;
    }
    if ( z1 < z0 )
    {
        int t = z0;
        z0 = z1;
        z1 = t;
    }

    // clamp the rectangle to the global weightmap coordinates
    int totalWeightMapSize = this->numBlocks * this->weightMapSize;

    x0 = min(max(0, x0), totalWeightMapSize - 1);
    z0 = min(max(0, z0), totalWeightMapSize - 1);
    x1 = min(max(0, x1), totalWeightMapSize - 1);
    z1 = min(max(0, z1), totalWeightMapSize - 1);
}

//------------------------------------------------------------------------------
/**
    get terrain material at world pos (x,z)
*/
nTerrainMaterial *
ncTerrainMaterialClass::GetTerrainMaterialAt(float x, float z)
{
    nTerrainMaterial * tm = 0;

    n_assert(x >= 0 && x <= this->gmmclass->GetTotalSideSizeScaled() + 1.f);
    n_assert(z >= 0 && z <= this->gmmclass->GetTotalSideSizeScaled() + 1.f);

    this->profGMMGetMaterial.StartAccum();

    // obtain block coord (bx, bz) for coordinate (x, z)
    int bx = static_cast<int> (x / this->gmmclass->GetBlockSideSizeScaled());
    int bz = static_cast<int> (z / this->gmmclass->GetBlockSideSizeScaled());

    // check provided point is within the terrain
    if (bx < 0 || bx >= this->gmmclass->GetNumBlocks() ||
        bz < 0 || bz >= this->gmmclass->GetNumBlocks())
    {
        this->profGMMGetMaterial.StopAccum();
        return 0;
    }

    // dx, dz are the normalized to the current block (between [0,1])
    float dx = (x - bx * this->gmmclass->GetBlockSideSizeScaled())
        / this->gmmclass->GetBlockSideSizeScaled();
    float dz = (z - bz * this->gmmclass->GetBlockSideSizeScaled())
        / this->gmmclass->GetBlockSideSizeScaled();

    // try to get the proper texture info
    int layerHandle = 255;
    nTerrainCellInfo * info = this->GetTerrainCellInfo( bx, bz );
    if (info->GetFlags() & nTerrainCellInfo::EmptyLayer)
    {
        this->profGMMGetMaterial.StopAccum();
        return 0;
    }
    else if (info->GetFlags() & nTerrainCellInfo::SingleLayer)
    {
        layerHandle = info->GetLayerHandle(0);
    }
    else
    {
        // check if texture is available for this cell
        nGMMTextureCacheEntry * entry = this->textureCache.Lookup( nGMMTextureCacheEntry::ToKey(bx, bz), 0, false );
        layerHandle = info->GetLayerHandle(0);
        if (entry)
        {
            // get the material handle
            int localIndex = entry->GetLayerIndexAt( dx, dz );
            int globalIndex = info->GetLayerIndex( localIndex );
            layerHandle = info->GetLayerHandle( globalIndex );
        }

/*        if (layerHandle != 255)
        {
            // @todo implement lookup in a global map with lower resolution
            // for now get the material most important in the whole block
            layerHandle = info->GetLayerHandle(0);
        }
*/
    }

    if (layerHandle != 255)
    {
        tm = this->GetLayerByHandle(layerHandle);
    }

    this->profGMMGetMaterial.StopAccum();

    return tm;
}

//------------------------------------------------------------------------------
/**
    @brief Fill a lookup table with most significant layer index in a block
    @param texture weightmap texture
    @param lookupTable The table to fill
    @param lookupTableSize Side size of the table
    @return success
*/
bool
ncTerrainMaterialClass::FillMaterialLookupTable( int bx, int bz, nTexture2 * texture, nuint8 * lookupTable, int lookupTableSize)
{
    n_assert( lookupTableSize > 0 );
    n_assert( texture && texture->IsLoaded() );

    static nProfiler profGMMFillMaterial( "profGMMFillMaterial", true );
    profGMMFillMaterial.StartAccum();

    if ( !texture || !texture->IsLoaded() )
    {
        profGMMFillMaterial.StopAccum();
        return false;
    }

    // Lock texture
    int left = 0;
    int right = texture->GetWidth();
    int top = 0;
    int bottom = texture->GetHeight();
    nTexture2::LockInfo lockInfo;
    if ( ! texture->LockRect(nTexture2::ReadOnly, 0, left, top, right, bottom, lockInfo) )
    {
        profGMMFillMaterial.StopAccum();
        return false;
    }
    unsigned char * ptr = static_cast<unsigned char *> (lockInfo.surfPointer);
    int pitch = lockInfo.surfPitch;

    // Loop over lookup table filling the values
    int w[5];
    int increment = texture->GetHeight() / lookupTableSize;
    int numLayers = min( MaxWeightMapBlendLayers, this->GetWeightMapNumberOfLayers( bx, bz ) );
    for ( int z = 0; z < texture->GetHeight(); z += increment )
    {
        for ( int x = 0; x < texture->GetWidth(); x += increment )
        {
            // Decode texture into array of weights
            this->DecodeWeightTexture(texture, x, z, ptr, pitch, w);

            // get the most important material on that point
            // set maxWeight = 5, the 6th element of weight array is 255 (invalid)
            nuint8 maxWeight = 0;
            for( nuint8 i = 1; i < numLayers; i++ )
            {
                if ( w[i] > w[maxWeight] )
                {
                    maxWeight = i;
                }
            }
            lookupTable[ ( x / this->materialResolutionDivisor ) + ( z / this->materialResolutionDivisor ) * lookupTableSize ] = maxWeight;
        }
    }

    texture->Unlock(0);

    profGMMFillMaterial.StopAccum();
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Decode weights in a texel and fill a weights array
    @param texture The texture
    @param x X coord of texel
    @param z Z coord of texel
    @param ptr Pointer to surface already locked
    @param weights Array of weights to fill (5 elements)
*/
void
ncTerrainMaterialClass::DecodeWeightTexture( nTexture2 * texture, int x, int z, unsigned char * ptr, int pitch, int * w )
{
    n_assert( texture );

    unsigned char * pTexel = 0;

    switch(texture->GetFormat())
    {
    case nTexture2::A8R8G8B8:
        pTexel = ptr + ( x * 4 + z * pitch );
        w[0] = pTexel[2];
        w[1] = pTexel[1];
        w[2] = pTexel[0];
        w[3] = pTexel[3];
        break;
        /*
    case nTexture2::DXT3:
        {
            // alpha, get alpha at (0,0) in 4x4 block
            nuint16 alpha = *(reinterpret_cast<nuint16 *> (ptr));
            w[3] = alpha & 0x000F;
            nuint16 col0 = *(reinterpret_cast<nuint16 *> (ptr + 8));
            // RGB 5:6:5, get color0 in 4x4 block
            w[0] = (col0 & 0xF800) >> (11 - 3);
            w[1] = (col0 & 0x07E0) >> (5 - 2);
            w[2] = (col0 & 0x001F) << 3;
        }
        break;
        */
    case nTexture2::DXT5:
        {
            // Get pointer to block of 4*4 texels
            int mx = x & 0xFFFC;
            int mz = z & 0xFFFC;
            pTexel = ptr + 16 * ( ( mx >> 2 ) + ( mz >> 2 ) * ( texture->GetWidth() >> 2 ) );

            // index for alpha use displacement inside block of 4*4 texels to get the index of the proper texel, now it takes the (0,0)
            nuint8 index = 0;
            switch ( (x % 4) + 4 * (z % 4) )
            {
            case 0:
                // 0 0
                index = pTexel[2] & 0x07;
                break;
            case 1:
                // 0 1
                index = ( pTexel[2] & ( 0x7 << 3 ) ) >> 3;
                break;
            case 2:
                // 0 2
                index = ( ( pTexel[2] & ( 0x03 << 6 ) ) >> 6 ) | ( ( pTexel[3] & 0x01 ) << 2 );
                break;
            case 3:
                // 0 3
                index = ( pTexel[3] & ( 0x7 << 1 ) ) >> 1;
                break;
            case 4:
                // 1 0
                index = ( pTexel[3] & ( 0x7 << 4 ) ) >> 4;
                break;
            case 5:
                // 1 1
                index = ( ( pTexel[3] & ( 0x01 << 7 ) ) >> 7 ) | ( ( pTexel[4] & 0x03 ) << 1);
                break;
            case 6:
                // 1 2
                index = ( pTexel[4] & ( 0x7 << 2 ) ) >> 2;
                break;
            case 7:
                // 1 3
                index = ( pTexel[4] & ( 0x7 << 5 ) ) >> 5;
                break;
            case 8:
                // 2 0
                index = pTexel[5] & 0x07;
                break;
            case 9:
                // 2 1
                index = ( pTexel[5] & ( 0x7 << 3 ) ) >> 3;
                break;
            case 10:
                // 2 2
                index = ( ( pTexel[5] & ( 0x03 << 6 ) ) >> 6 ) | ( ( pTexel[6] & 0x01 ) << 2 );
                break;
            case 11:
                // 2 3
                index = ( pTexel[6] & ( 0x7 << 1 ) ) >> 1;
                break;
            case 12:
                // 3 0
                index = ( pTexel[6] & ( 0x7 << 4 ) ) >> 4;
                break;
            case 13:
                // 3 1
                index = ( ( pTexel[6] & ( 0x01 << 7 ) ) >> 7 ) | ( ( pTexel[7] & 0x03 ) << 1);
                break;
            case 14:
                // 3 2
                index = ( pTexel[7] & ( 0x7 << 2 ) ) >> 2;
                break;
            case 15:
                // 3 3
                index = ( pTexel[7] & ( 0x7 << 5 ) ) >> 5;
                break;
            default:
                // can't happen
                n_assert_always();
            }

            nuint8 alpha;
            if (index > 1)
            {
                // case a) alpha0 > alpha1
                if (pTexel[0] > pTexel[1])
                {
                    alpha = ((8 - index) * pTexel[0] + (index - 1) * pTexel[1] + 3) / 7;
                }
                // case b) alpha0 < alpha1, index < 6
                else if (index < 6)
                {
                    alpha = ((6 - index) * pTexel[0] + (index - 1) * pTexel[1] + 2) / 5;
                }
                // case b) alpha0 < alpha1, index 6 and 7
                else 
                {
                    alpha = (index - 6) * 255;
                }
            }
            else
            {
                // indices 0 and 1
                alpha = pTexel[index];
            }

            // alpha
            w[3] = alpha;

            nuint16 color;
            nuint16 * col = reinterpret_cast<nuint16 *> (pTexel + 8);
            // index for color

            switch(pTexel[12 + (z % 4)] & (0x03 << (x % 4)) >> (x % 4))
            {
            case 0:
                color = col[0];
                break;
            case 1:
                color = col[1];
                break;
            case 2:
                if (col[0] > col[1])
                {
                    color = (2 * col[0] + col[1] + 1) / 3;
                }
                else
                {
                    color = (col[0] + col[1]) >> 1;
                }
                break;
            case 3:
                if (col[0] > col[1])
                {
                    color = (col[0] + 2 * col[1] + 1) / 3;
                }
                else
                {
                    color = 0;
                }
                break;
            default:
                n_assert_always();
                color = 0;
                break;
            }

            // RGB 5:6:5, get color0 in 4x4 block
            w[0] = (color & 0xF800) >> (11 - 3);
            w[1] = (color & 0x07E0) >> (5 - 2);
            w[2] = (color & 0x001F) << 3;
        }
        break;
    default:
        n_assert2_always("texture format not recognized for terrain material, please contact programmers.");
        return;
        break;
    }

    // calculate weight 5 (set to zero, not used)
    w[4] = 0;
}

//------------------------------------------------------------------------------
/**
    get game material name at world pos (x,z)
*/
const char *
ncTerrainMaterialClass::GetGameMaterialNameAt(float x, float z)
{
    nTerrainMaterial * terrainMaterial = this->GetTerrainMaterialAt(x, z);
    if (terrainMaterial)
    {
        return terrainMaterial->GetGameMaterialName();
    }

    return 0;
}

//---------------------------------------------------------------------------
/**
    @brief Set material resolution divisor (weightmap / material resolution)
    Default value = 1
*/
void
ncTerrainMaterialClass::SetMaterialResolutionDivisor(int matResDivisor)
{
    n_assert( matResDivisor >= 1);

    this->materialResolutionDivisor = matResDivisor;

#ifndef NGAME
    this->textureCache.ReallocAll();
#endif
}

//---------------------------------------------------------------------------
/**
    @brief Get material resolution divisor (weightmap / material resolution)
    Default value = 1
*/
int 
ncTerrainMaterialClass::GetMaterialResolutionDivisor() const
{
    n_assert( this->materialResolutionDivisor >= 1);
    return this->materialResolutionDivisor;
}

//------------------------------------------------------------------------------
/**
    get game material at world pos (x,z)
*/
nGameMaterial *
ncTerrainMaterialClass::GetGameMaterialAt(float x, float z)
{
    nTerrainMaterial * terrainMaterial = this->GetTerrainMaterialAt(x, z);
    if (terrainMaterial)
    {
        return terrainMaterial->GetGameMaterial();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::SetAllCellsMaterialDirty() const
{
    for ( int z = 0; z < this->GetMaterialNumBlocks(); z++ )
    {
        for ( int x = 0; x < this->GetMaterialNumBlocks(); x++ )
        {
            nTerrainCellInfo * cellInfo = this->GetTerrainCellInfo( x, z );
            n_assert(cellInfo);
            nEntityObject * obj = cellInfo->GetTerrainCell();
            n_assert(obj);
            
            obj->GetComponentSafe<ncTerrainGMMCell>()->SetMaterialDirty();
        }
    }
}

//------------------------------------------------------------------------------
void 
ncTerrainMaterialClass::UpdateMaterial(int x0, int z0, int x1, int z1)
{
    // get terrain material class
    this->ClampGlobalWeightMapCoordinates(x0, z0, x1, z1);

    int bxmin, bzmin;
    this->GlobalWeightMapCoordToBlockCoord(x0, z0, bxmin, bzmin);

    int bxmax, bzmax;
    this->GlobalWeightMapCoordToBlockCoord(x1, z1, bxmax, bzmax);


#ifndef NGAME
    //int numBlocks = (bzmax - bzmin + 1) * (bxmax - bxmin + 1);
    // hack: only do this update when number of blocks affected is low
    // to avoid load all the terrain
    //if (numBlocks < 16)
    {
        this->RefreshWeightTexturePortion(x0, z0, x1, z1);
        NLOG(conjurer, (0, "ncTerrainMaterialClass::RefreshWeightTexturePortion(%d,%d,%d,%d)", x0, z0, x1, z1));
        this->RefreshGlobalTexturePortion(bxmin, bzmin, bxmax, bzmax);
        NLOG(conjurer, (0, "ncTerrainMaterialClass::RefreshGlobalTexturePortion(%d,%d,%d,%d)", bxmin, bzmin, bxmax, bzmax));
    }
#endif

    // scan all outdoor cells to update the parameters in the render context
    // by calling EntityCreated on each cell
    for(int bz = bzmin; bz <= bzmax; bz++)
    {
        for(int bx = bxmin; bx <= bxmax; bx++)
        {
            nTerrainCellInfo * cellInfo = this->GetTerrainCellInfo(bx, bz);
            n_assert_if(cellInfo)
            {
                nEntityObject * obj = cellInfo->GetTerrainCell();
                n_assert_if(obj)
                {
                    obj->GetComponentSafe<ncTerrainGMMCell>()->SetMaterialDirty();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
ncTerrainMaterialClass::UpdateMaterialAll()
{
    int maxsize = this->GetSingleWeightMapSize(0, 0) - 1;
    n_assert(maxsize > 0);
    maxsize *= this->GetMaterialNumBlocks();

    this->UpdateMaterial(0, 0, maxsize, maxsize);
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncTerrainMaterialClass::SetDefaultLighmapIsBlack(bool val)
{
    this->defaultLighmapIsBlack = val;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainMaterialClass::GetDefaultLighmapIsBlack()
{
    return this->defaultLighmapIsBlack;
}
#endif
