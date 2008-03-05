#ifndef N_NCTERRAINMATERIALCLASS_H
#define N_NCTERRAINMATERIALCLASS_H
//------------------------------------------------------------------------------
/**
    @file ncTerrainMaterialClass.h
    @class ncTerrainMaterialClass
    @ingroup NebulaTerrain

    @author Juan Jose Luna Espinosa
    @author Mateu Batle Sastre

    @brief ncTerrainMaterialClass is a class component for outdoors, which
    has information about the terrain materials and weightmap blocks.

    (c) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/ncomponentclass.h"
#include "entity/nentity.h"
#include "util/nstring.h"
#include "kernel/nref.h"
#include "kernel/nprofiler.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nterrainmaterial.h"
#include "ngeomipmap/nterrainweightmapbuilder.h"
#include "ngeomipmap/nterrainglobaltexturebuilder.h"
#include "ngeomipmap/ngmmtexturecacheentry.h"
#include "ngeomipmap/ngmmlightmapcacheentry.h"

//------------------------------------------------------------------------------
#define MAX_TERRAIN_LAYERS 256
#define DEFAULT_TERRAIN_TEXTURE "wc:libs/system/textures/cesped_test_terrain.dds"

//------------------------------------------------------------------------------
// Path of terrain editor objects
extern const char* terrainEditorPath;
class nTerrainCellInfo;
class ncTerrainGMMClass;
class nGameMaterial;

//------------------------------------------------------------------------------
class ncTerrainMaterialClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncTerrainMaterialClass,nComponentClass);

public:

    friend class nTerrainGlobalTextureBuilder;

    // constructor
    ncTerrainMaterialClass();
    // destructor
    virtual ~ncTerrainMaterialClass();

    /// init instance
    void InitInstance(nObject::InitInstanceMsg);
    /// Nebula class persistence
    bool SaveCmds(nPersistServer * ps);

    /// load resources
    bool LoadResources(void);
    /// unload resources
    void UnloadResources(void);
    /// are resources loaded & valid
    bool IsValid();
    /// save terrain material resources
    bool SaveResources(void);

    // Layer materials commands

    /// Get number of layers
    int GetLayerCount() const;
    /// Get a layer handle by its position in the stack
    nTerrainMaterial::LayerHandle GetLayerHandle(int) const;
    /// Get a layer position by its handle
    int GetLayerPos(nTerrainMaterial::LayerHandle) const;
    /// Create a new blank layer (return ptr), used for persistence
    nTerrainMaterial * AddLayer(nTerrainMaterial::LayerHandle);
    /// Create a new blank layer
    nTerrainMaterial::LayerHandle CreateLayer();
    /// Remove layer
    bool RemoveLayer(nTerrainMaterial::LayerHandle);
    /// Set selected layer for painting
    bool SelectLayer(nTerrainMaterial::LayerHandle);
    /// Get selected layer handle
    nTerrainMaterial::LayerHandle GetSelectedLayerHandle () const;
    /// Reset all layers and deallocate weightmap
    void Reset();

    // Global commands

    /// Get weightmap path for a block related resources
    nString GetWeightMapTexturePath(int, int) const;
    /// Get weightmap path for a block related resources
    nString GetLightMapTexturePath(int, int) const;
    /// Get the current lightmap
    nEntityObject* GetLightMap() const;
    /// Set the current lightmap
    void SetLightMap(nEntityObject*);

    /// Get weightmap path for a block related resources
    nString GetLayerByteMapPath(int, int, nTerrainMaterial::LayerHandle) const;

    /// Get global texture path
    nString GetGlobalTexturePath() const;

    /// Get modulation texture path
    nString GetModulationTexturePath() const;

    /// Get materials path
    nString GetMaterialsPath() const;

    /// Get weightmaps table path
    nString GetWeightMapsTablePath() const;

    /// set all weightmaps size, common for all blocks
    void SetAllWeightMapsSize(int);

    /// get all weightmaps size, common for all blocks
    int GetAllWeightMapsSize() const;

    /// Obsolete - set weightmap size
    void SetWeightMapSize(int);

    /// get weightmap size
    int GetWeightMapSize () const;

    /// Sets global texture size
    void SetGlobalTextureSize(int);
    /// Gets global texture size
    int GetGlobalTextureSize () const;
    /// Set global modulation factor
    void SetModulationFactor(float);
    /// Get global modulation factor
    float GetModulationFactor () const;
    /// set weightmap size
    void SetMaterialLODDistance(int);
    /// get weightmap size
    int GetMaterialLODDistance() const;
    /// set num block ahead preload
    void SetNumBlockPreload(int);
    /// get num block ahead preload
    int GetNumBlockPreload() const;

    /// set the root magic number
    void SetMagicNumber(int);

    // Weightmap table commands

    /// Set number of blocks along terrain side
    void SetMaterialNumBlocks(int);

    /// Get number of blocks along terrain side
    int GetMaterialNumBlocks() const;

    /// set weightmap size
    void SetSingleWeightMapSize(int, int, int);

    /// get weightmap size
    int GetSingleWeightMapSize(int, int) const;

    /// Gets number of layers in the weightmap
    int GetWeightMapNumberOfLayers(int, int) const;

    /// Gets a layer handle
    int GetWeightMapLayerHandle(int, int, int) const;

    /// Adds a layer to the weightmap by handle
    void AddWeightMapLayerHandle(int, int, int);

    /// Set a weightmap layer index
    void SetWeightMapLayerIndex(int, int, int, int);

    /// Get a weightmap layer index
    int GetWeightMapLayerIndex(int, int, int) const;

    /// Set current weightmap, size, flags
    void SetCurrentWeightMap(int, int, int, int);

    /// Set flags in cellinfo with a mask
    void SetCellInfoFlags(int, int);

//#ifndef NGAME
    /// Set flags in cellinfo with a mask
    void SetCellInfoGrowthMapValue(int);
//#endif

    /// Adds a layer to the weightmap by handle
    void AddCurrentWeightMapLayerHandle(int);

    /// Set current weightmap indices
    void SetCurrentWeightMapIndices(int, int, int, int, int);

    /// Set current weightmap handles
    void SetCurrentWeightMapHandles(int, int, int, int, int);

    int GetWeightMapLayerIndexByHandle(int x, int z, int handle) const;

    /// Gets the block flags
    int GetSingleWeightMapFlags(int, int) const;

    /// Sets the block flags
    void SetSingleWeightMapFlags(int, int, int);

    /// Get weightmap reference
    nTerrainCellInfo* GetTerrainCellInfo(int bx, int bz) const;

    /// Get heightmap reference
    nFloatMap* GetHeightMap() const;

    /// Get a layer by its handle
    nTerrainMaterial * GetLayerByHandle(nTerrainMaterial::LayerHandle) const;

    /// Get a layer by its position
    nTerrainMaterial* GetLayerByIndex( int pos ) const;

    /// Get selected layer (Only for read)
    nTerrainMaterial* GetSelectedLayer() const;

    /// Import the material list from another class
    bool ImportMaterials(nEntityClass *);

    /// get terrain material at world pos (x,z)
    nTerrainMaterial * GetTerrainMaterialAt(float, float);
    
    /// Fill a lookup table with most significant layer index in a block
    bool FillMaterialLookupTable( int bx, int bz, nTexture2 * texture, nuint8 * lookupTable, int lookupTableSize);

    /// Decode weights in a texel and fill a weights array
    void DecodeWeightTexture( nTexture2 * texture, int x, int z, unsigned char * ptr, int pitch, int * w );

    /// get game material name at world pos (x,z)
    const char * GetGameMaterialNameAt(float, float);
    /// get game material at world pos (x,z)
    nGameMaterial * GetGameMaterialAt(float, float);

    /// Set material resolution divisor (weightmap / material resolution)
    void SetMaterialResolutionDivisor(int);
    /// Get material resolution divisor (weightmap / material resolution)
    int GetMaterialResolutionDivisor() const;

#ifndef NGAME
    /// Set if default lightmap is black or white
    void SetDefaultLighmapIsBlack(bool);

    ///  Get if default lightmap is black or white
    bool GetDefaultLighmapIsBlack();

    /// Refresh a portion of the weightmap
    void RefreshWeightTexturePortion( int x0, int z0, int x1, int z1 );

    /// Refresh a portion the global texture
    void RefreshGlobalTexturePortion( int x0, int z0, int x1, int z1 );

    /// Refresh weight texture in the weightmap from the layer masks
    void RefreshWeightTexture ();
    /// Refresh global texture
    void RefreshGlobalTexture ();
#endif

    /// Get global texture
    nTexture2 * GetGlobalTexture() const;
    /// Get global texture
    nTexture2 * GetGlobalLighMapTexture() const;

    /// Get modulation texture
    nTexture2 * GetModulationTexture() const;

    /// create a new empty texture for weightmaps
    nTexture2 * NewWeightMapTexture(const char * resName) const;
    /// loads the global texture
    bool LoadGlobalTexture();

    /// Get the lightmap texture
    nTexture2 * GetWeightMapTexture(int bx, int bz, bool loadOnMiss);
    /// return true if lightmap is cached
    bool IsWeightMapCached(int bx, int bz);
    /// Get the lightmap texture
    nTexture2 * GetLightMapTexture(int bx, int bz);
    /// Get number of cached lightmap textures
    int GetNumLightmapTextures() const;
    /// Get cached lightmap texture by index
    nTexture2 * GetLightmapTextureAt(int index, int& bx, int& bz) const;

    /// checks if terrain editor state is selected
    bool InTerrainEditorState();

    /// clamp global weightmap coordinates
    void ClampGlobalWeightMapCoordinates(int & x0, int & z0, int & x1, int & z1) const;

    /// Convert from global weightmap coordinate to block coordinates
    void GlobalWeightMapCoordToBlockCoord(int x, int z, int & bx, int & bz) const;

    /// return the size of cache,
    int GetDetailCacheSize() const;

    /// invalidate material for all cells
    void SetAllCellsMaterialDirty() const;

    /// called when the material has been updated in a given rectangle (update caches ...)
    void UpdateMaterial(int x0, int z0, int x1, int z1);
    /// called to upadte the material in the whole terrain
    void UpdateMaterialAll();

protected:

    /// load materials file
    bool LoadMaterials();
    /// save materials file
    bool SaveMaterials();
    /// unload materials file
    bool UnloadMaterials();

    /// allocate weightmaps table
    bool AllocWeightMapsTable();
    /// load weightmaps table file
    bool LoadWeightMapsTable();
    /// save weightmaps table file
    bool SaveWeightMapsTable();
    /// save bytemaps
    bool SaveByteMaps(void);
    /// Load lightmap
    nTexture2* LoadGlobalLightMapTexture();

    /// Array of layers
    nArray<nTerrainMaterial *> layerArray;

    /// Array of information about each cell
    nArray<nTerrainCellInfo> cellInfo;

    /// Size of the weightmap of one block (must be power of 2)
    ushort weightMapSize;

    /// Number of blocks in the side of the terrain, taken from the GMM component
    int numBlocks;

    /// Size of global texture (must be power of 2)
    ushort globalTextureSize;

    // Reference to the terrain heightmap
    nRef<nFloatMap> refHeightMap;

    // Global texture
    nRef<nTexture2> refGlobalTexture;
    // Global lightmap texture
    nRef<nTexture2> refGlobalLightMapTexture;
    // unpainted system texture
    nRef<nTexture2> refUnpaintedTexture;
    // red texture, used for weightmap with first channel full
    nRef<nTexture2> refRedTexture;
    // texture used as error
    nRef<nTexture2> refErrorTexture;

    nRef<nTexture2> refBlackTexture;

    /// Modulation texture
    nRef<nTexture2> refModulationTexture;
    // white texture to avoid modulation effect
    nRef<nTexture2> refWhiteTexture;
    /// Modulation factor to blend modulation texture with detail/global texture
    float modulationFactor;
    /// Material LOD distance
    int matLODDistance;
    /// number of blocks to preload
    int numPreload;

    /// Factor of resolution of material lookup table (weightmap resolution / material lookup resolution)
    int materialResolutionDivisor;

    // texture cache for weightmaps
    nCache<nGMMTextureCacheEntry> textureCache;
    // texture cache for weightmaps
    nCache<nGMMLightMapCacheEntry> lightMapCache;

    /// Selected layer for painting (handle)
    nTerrainMaterial::LayerHandle selectedLayer;
    
    /// Selected layer for painting (position)
    int selectedLayerPos;

    /// Handle assignment counter
    int newHandle;

    /// true when we are in importing material state
    bool importMaterialState;

    /// All resources loaded flag
    bool isValid;

    /// current block (bx, bz)
    int currentbx, currentbz;

    /// component for terrain geometry 
    ncTerrainGMMClass * gmmclass;

    /// The current lightmap
    nRef<nEntityObject> lightMapEntity;

#ifndef NGAME
    /// Weightmapbuilder
    nTerrainWeightMapBuilder weightMapBuilder;

    /// Global texture builder
    nTerrainGlobalTextureBuilder globalTextureBuilder;

    // if default lightmap is black or white
    bool defaultLighmapIsBlack ;

    // texture blackAlpha
    nRef<nTexture2> refBlackAlphaTexture;
#endif //NGAME

    // profiler for get material
    nProfiler profGMMGetMaterial;

};

#endif //N_NCTERRAINMATERIALCLASS_H
