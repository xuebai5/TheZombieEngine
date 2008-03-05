#ifndef N_TERRAINCELLINFO_H
#define N_TERRAINCELLINFO_H
//------------------------------------------------------------------------------
/**
    @file nterraincellinfo.h
    @class nTerrainCellInfo
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa
    @author Mateu Batle

    @brief nTerrainCellInfo class holds information about a terrain cell / block.
    All nTerrainCellInfo for all cels are loaded for a given terrain. It has
    information about materials, flags, etc.

    (C) 2004 Conjurer Services, S.A. 
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/nterrainmaterial.h"

//------------------------------------------------------------------------------
/// Max. weightmap texture size. If a weightmap's size is greater, it will be partitioned
static const int MaxWeightMapSize = 2048;

/// Number of terrain material layers in one weightmap
static const int MaxWeightMapBlendLayers = 5;

//------------------------------------------------------------------------------
class nByteMap;
class nEntityObject;

//------------------------------------------------------------------------------
class nTerrainCellInfo
{
public:

    /// info of a layer 
    struct WeightMapLayerInfo
    {
        // The layer handle
        nTerrainMaterial::LayerHandle layerHandle;

#ifndef NGAME
        // bytemap containing mix values for the layer
        nRef<nByteMap> refLayer;
        // sumation of all the values of the layer
        //nuint64 use;
        // The undo layer
        nRef<nByteMap> undoLayer;
        // Editor flag, used while generating weightmap texture
        bool selected;
        /// Copy layer to undo layer
        void CopyToUndoLayer();
#endif
    };

    enum 
    {
        // no paint layer info for this block
        EmptyLayer      = (1 << 0),
        // a single full layer for this blcok
        SingleLayer     = (1 << 1),
        // the block has a hole
        HasHole         = (1 << 2),
        // the block has a hole
        HasGrass        = (1 << 3),
    };

    /// constructor
    nTerrainCellInfo();
    /// destructor
    ~nTerrainCellInfo();

    /// Setup terrain cell info
    void Setup(nEntityClass * oc, nEntityObject * cell, int bx, int bz);

    /// Sets terrain cell weightmap size
    void SetWeightMapSize(int);    
    /// Gets terrain cell weightmap size
    int GetWeightMapSize() const;

    /// Gets number of layers in the weightmap
    int GetNumberOfLayers() const;
    /// Gets a layer handle
    int GetLayerHandle(int index) const;
    /// Adds a layer to the weightmap by handle
    void AddLayerHandle(int layerHandle);
    /// Set a weightmap layer index
    void SetLayerIndex(int, int);
    /// Get a weightmap layer index
    int GetLayerIndex(int layerIndex) const;
    /// Get a layer's index by it's handle
    int GetLayerIndexByHandle( int ) const;
    /// Get layer indices that are blended in the weight texture
    const nuint8* GetIndices() const;
    /// Get info of a float layer
    WeightMapLayerInfo& GetLayerInfo( int index ) const;

    /// Set terrain cell
    void SetTerrainCell(nEntityObject *);
    /// Get terrain cell
    nEntityObject * GetTerrainCell() const;

    /// set weightmap to dirty state
    void SetDirty( bool dirty );
    /// check if weightmap is in dirty state
    bool IsDirty() const;

    /// get all flags 
    int GetFlags() const;
    /// set all flags in one operation
    void SetFlags(int);
    /// get flags 
    int GetFlags(int mask) const;
    /// set flags with a mask
    void SetFlags(int mask, int value);
    /// enable specified flags
    void AddFlags(int);
    /// clear specified flags
    void ClearFlags(int);
    /// get block is used
    bool IsEmpty() const;
    /// set/reset the empty flag
    void SetEmpty(bool);
    /// return true when weightmap must be saved
    bool NeedToSaveWeightMap() const;

#ifndef NGAME
    /// recalculates all the flags of this terrain cell
    void UpdateFlags();
    /// Tell if layers are created
    bool AreLayersCreated() const;
    /// Calculate layer use
    //void CalculateLayerUse( nTerrainMaterial::LayerHandle layerHandle );
    /// Remove a layer
    bool RemoveLayer( nTerrainMaterial::LayerHandle handle );
    /// Create bytemaps for the layers wich don't have
    void CreateByteMaps(bool alloc);
    /// Recalculate the computable flags for this terrain cell
    void RecalculateFlags();
    /// Get the growth map value 
    int GetGrowthMapValue();
    /// Set the growth map value;
    void SetGrowthMapValue(int value);
#endif

private:

    /// create a new bytemap to use for this block
    nByteMap * NewByteMap(const char * resName, bool alloc);

    /// load bytemap properties
    bool LoadProperties();
    /// load weightmap textures
    bool LoadWeightMap();

    // terrain outdoor class
    nEntityClass * outdoorClass;

    // coordinates of the terrain block
    int bx, bz;

    /// Size of weightmap (must be power of 2)
    ushort weightMapSize;

    // flags about the block
    int flags;

    // Indices of layers blended in the texture
    nuint8 indices[ MaxWeightMapBlendLayers ];

    // resource has been changed after load / save
    bool dirty;

    // points to the cell if exists
    nEntityObject * cell;

    #ifndef NGAME
    int growthMapvalue;
    #endif

    // Layers info
    nArray<WeightMapLayerInfo> layersInfo;
};

//------------------------------------------------------------------------------
#endif // N_TERRAINCELLINFO_H
