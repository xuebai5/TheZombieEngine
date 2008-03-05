//------------------------------------------------------------------------------
//  nterraincellinfo_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "resource/nresourceserver.h"

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nTerrainCellInfo::nTerrainCellInfo() :
    weightMapSize( 0 ),
    dirty(false),
    flags(EmptyLayer),
    cell(0),
    outdoorClass(0),
    bx(-1),
    bz(-1)
#ifndef NGAME
    ,
    growthMapvalue(-1)
#endif
{
    memset(this->indices, 0xFF, sizeof(this->indices));
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nTerrainCellInfo::~nTerrainCellInfo()
{
#ifndef NGAME
    for ( int iLayer = 0; iLayer < this->GetNumberOfLayers(); iLayer++ )
    {
        WeightMapLayerInfo * wmli = &this->GetLayerInfo( iLayer );
        n_assert( wmli );
        if ( wmli->refLayer.isvalid() )
        {
            wmli->refLayer->Release();
            wmli->refLayer.invalidate();
        }
        if ( wmli->undoLayer.isvalid() )
        {
            wmli->undoLayer->Release();
            wmli->undoLayer.invalidate();
        }
    }
    this->layersInfo.Reset();
#endif
}

//------------------------------------------------------------------------------
/**
    Setup the terrain structural info. This info is not persisted.
*/
void
nTerrainCellInfo::Setup(nEntityClass * pOutdoorClass, nEntityObject * pCell, int pbx, int pbz)
{
    n_assert(pOutdoorClass);
    //n_assert(pCell);

    this->outdoorClass = pOutdoorClass;
    this->cell = pCell;
    this->bx = pbx;
    this->bz = pbz;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Create / load bytemaps for the layers wich don't have
    @param alloc Alloc the weightmaps instead of loading them
*/
void
nTerrainCellInfo::CreateByteMaps(bool alloc)
{
    ncTerrainMaterialClass * tmc = this->outdoorClass->GetComponentSafe<ncTerrainMaterialClass>();

    // loop layer definitions to create bytemaps
    for ( int iLayer = 0; iLayer < this->GetNumberOfLayers(); iLayer++ )
    {
        // if already loaded or created then ignore it
        WeightMapLayerInfo & wmli = this->GetLayerInfo( iLayer );
        if ( wmli.refLayer.isvalid() )
        {
            continue;
        }

        // create bytemap
        nString layerPath = tmc->GetLayerByteMapPath( bx, bz, wmli.layerHandle );
        wmli.refLayer = this->NewByteMap( layerPath.Get() , alloc);
        n_assert(wmli.refLayer.isvalid());
        //this->CalculateLayerUse( wmli.layerHandle );

        // create undo bytemap
        wmli.undoLayer = this->NewByteMap(0, true);
        n_assert(wmli.undoLayer.isvalid());
        wmli.CopyToUndoLayer();
    }
}
#endif

#if 0
//------------------------------------------------------------------------------
/**
    @brief Calculate layer use (sum of all values of the bytemap)
*/
void
nTerrainCellInfo::CalculateLayerUse( nTerrainMaterial::LayerHandle layerHandle )
{
    int layerIndex = this->GetLayerIndexByHandle( layerHandle );
    // If the layer doesn't exist, do nothing
    if ( layerIndex == InvalidLayerIndex )
    {
        return;
    }

    WeightMapLayerInfo & info = this->GetLayerInfo( layerIndex );
    // If the layer is not created, do nothing
    if ( !info.refLayer.isvalid() )
    {
        return;
    }

    //info.refLayer->CalculateAccumulator();
    //info.use = info.refLayer->GetAccumulator();
}
#endif

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Remove a layer
*/
bool
nTerrainCellInfo::RemoveLayer( nTerrainMaterial::LayerHandle handle )
{
    for ( int i = 0; i < this->GetNumberOfLayers(); i++ )
    {
        WeightMapLayerInfo & wmli = this->GetLayerInfo( i );

        if ( wmli.layerHandle == handle )
        {
            if ( wmli.refLayer.isvalid() )
            {
                wmli.refLayer->Release();
                wmli.refLayer.invalidate();
            }

            if ( wmli.undoLayer.isvalid() )
            {
                wmli.undoLayer->Release();
                wmli.undoLayer.invalidate();
            }

            this->layersInfo.Erase( i );
        }
    }
    return true;
}
#endif

//------------------------------------------------------------------------------
/** 
    Sets weightmap size
*/
void
nTerrainCellInfo::SetWeightMapSize( int size )
{
    // Must be power of 2
    n_assert( size > 0 && (!(size & (size-1))) );
    n_assert( size >= 0 && size <= ushort(~0) );

    this->weightMapSize = static_cast<ushort> (size);
}

//------------------------------------------------------------------------------
/** 
    Gets weightmap size
*/
int
nTerrainCellInfo::GetWeightMapSize() const
{
    return this->weightMapSize;
}

//------------------------------------------------------------------------------
/** 
    Gets number of layers in the weightmap
*/
int
nTerrainCellInfo::GetNumberOfLayers() const
{
    return this->layersInfo.Size();
}

//------------------------------------------------------------------------------
/** 
    Gets a layer index
*/
int
nTerrainCellInfo::GetLayerHandle(int index) const
{
    if ( index < 0 || index >= this->GetNumberOfLayers() )
    {
        return 0xFF;
    }

    return this->layersInfo[index].layerHandle;
}

//------------------------------------------------------------------------------
/** 
    Adds a layer to the weightmap by handle
*/
void
nTerrainCellInfo::AddLayerHandle(int layerHandle)
{
    n_assert( layerHandle >= 0 );

    WeightMapLayerInfo & wmli = this->layersInfo.At( this->GetNumberOfLayers() );

    wmli.layerHandle = layerHandle;
#ifndef NGAME
    wmli.refLayer.invalidate();
    wmli.undoLayer.invalidate();
    //wmli.use = ~((nuint64) 0);
#endif
}

//------------------------------------------------------------------------------
/**
    @brief Get layer index
    @param layerIndex Layer index, valid values are 0..4
    @return Layer index
    Returns the computed layer index at index 'i'. This is the 8 bit integer indexing the layer in the global layer array
    Only valid after the weightmap has been generated and the global array has not been edited
*/
int
nTerrainCellInfo::GetLayerIndex( int layerIndex ) const
{
    n_assert( layerIndex >= 0 && layerIndex < MaxWeightMapBlendLayers );
    return int( this->indices[ layerIndex ] );
}

//------------------------------------------------------------------------------
/**
    @brief Set layer index
    @param layerIndex Layer index, valid values are 0..4
    @param index Index of the layer
*/
void
nTerrainCellInfo::SetLayerIndex( int layerIndex, int index )
{
    n_assert( layerIndex >= 0 && layerIndex < MaxWeightMapBlendLayers );
    this->indices[ layerIndex ] = nuint8(index);
}

//------------------------------------------------------------------------------
/**
    @brief Get layer indices
    @return Pointer to indices, MaxWeightMapBlendLayers of them
*/
const nuint8 *
nTerrainCellInfo::GetIndices() const
{
    return this->indices;
}

//------------------------------------------------------------------------------
/**
    @brief Get a layer's index by it's handle
    @return Layer index
*/
int
nTerrainCellInfo::GetLayerIndexByHandle( int layerHandle ) const
{
    for ( int i = 0; i < this->GetNumberOfLayers(); i++ )
    {
        WeightMapLayerInfo & wmli = this->GetLayerInfo( i );
        if ( wmli.layerHandle == layerHandle )
        {
            return i;
        }
    }
    return InvalidLayerIndex;
}

//------------------------------------------------------------------------------
/**
    @brief Get info of a float layer
    @return Layer info reference
*/
nTerrainCellInfo::WeightMapLayerInfo&
nTerrainCellInfo::GetLayerInfo( int index ) const
{
    n_assert( index >= 0 && index < this->GetNumberOfLayers() );
    return this->layersInfo[ index ];
}

//------------------------------------------------------------------------------
/**
    @brief Check if resource is in dirty state
*/
bool
nTerrainCellInfo::IsDirty() const
{
    return this->dirty;
}

//------------------------------------------------------------------------------
/**
    @brief Set if resource is in dirty state flag
*/
void
nTerrainCellInfo::SetDirty( bool dirty )
{
#ifndef NGAME
    this->UpdateFlags();
#endif
    this->dirty = dirty;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Tell if layers are created
*/
bool
nTerrainCellInfo::AreLayersCreated() const
{
    if ( this->GetNumberOfLayers() == 0 )
    {
        return false;
    }

    for ( int i = 0; i < this->GetNumberOfLayers(); i++ )
    {
        WeightMapLayerInfo & wmli = this->GetLayerInfo( i );
        if ( ! wmli.refLayer.isvalid() )
        {
            return false;
        }
    }
    return true;
}
#endif

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief copy the bytemap layer onto the undo layer
*/
void
nTerrainCellInfo::WeightMapLayerInfo::CopyToUndoLayer()
{
    if ( ! this->refLayer.isvalid() || ! this->undoLayer.isvalid() )
    {
        return;
    }

    nByteMapBuilder fmapBuilder;
    fmapBuilder.Copy( this->refLayer, this->undoLayer );
}
#endif

//------------------------------------------------------------------------------
/**
    @brief return true if the layer has no paint infromation
*/
bool
nTerrainCellInfo::IsEmpty() const
{
    return (this->flags & EmptyLayer);
}

//------------------------------------------------------------------------------
/**
    @brief Set block is used
*/
void
nTerrainCellInfo::SetEmpty(bool empty)
{
    if (empty)
    {
        this->flags |= EmptyLayer;
    }
    else
    {
        this->flags &= ~EmptyLayer;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get the block flags (empty, hole, ...)
*/
int 
nTerrainCellInfo::GetFlags() const
{
    return this->flags;
}

//------------------------------------------------------------------------------
/**
    @brief Set the block flags (empty, hole, ...)
*/
void 
nTerrainCellInfo::SetFlags(int f)
{
    this->flags = f;
}

//------------------------------------------------------------------------------
/**
    @brief Set the block flags (empty, hole, ...)
*/
void 
nTerrainCellInfo::SetFlags(int mask, int value)
{
    this->flags = (value & mask) | ( (~mask)& this->flags);
}

//------------------------------------------------------------------------------
/**
    @brief Get the block flags (empty, hole, ...)
*/
int 
nTerrainCellInfo::GetFlags(int mask) const
{
    return this->flags & mask;
}




//------------------------------------------------------------------------------
/**
    @brief Set the block flags (empty, hole, ...)
*/
void 
nTerrainCellInfo::AddFlags(int f)
{
    this->flags |= f;
}

//------------------------------------------------------------------------------
/**
    @brief Set the block flags (empty, hole, ...)
*/
void 
nTerrainCellInfo::ClearFlags(int f)
{
    this->flags &= ~f;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/** 
*/
void 
nTerrainCellInfo::UpdateFlags()
{
    this->ClearFlags( EmptyLayer | SingleLayer );

    int numused = 0;
    for(int layer = 0; layer < this->GetNumberOfLayers();layer++)
    {
        if (this->GetLayerInfo(layer).refLayer->GetAccumulator() > 0)
        {
            numused++;
        }
    }

    if (0 == numused)
    {
        this->SetFlags( EmptyLayer );
    }
    else if (1 == numused)
    {
        this->SetFlags( SingleLayer );
    }
}
#endif

//------------------------------------------------------------------------------
/**
    Returns @true if the weightmap must be persisted.
    If the weightmap has zero or one channel (completely filled), the weightmap 
    does not need to be saved.
*/
bool 
nTerrainCellInfo::NeedToSaveWeightMap() const
{
    return !(this->GetFlags() & (EmptyLayer | SingleLayer));
}

#ifndef NGAME
//------------------------------------------------------------------------------
nByteMap * 
nTerrainCellInfo::NewByteMap(const char * resName, bool alloc)
{
    nByteMap * bytemap =  static_cast<nByteMap *> (
        nResourceServer::Instance()->NewResource("nbytemap", resName, nResource::Other) );
    n_assert(bytemap);

    if (bytemap)
    {
        bytemap->SetSize( this->GetWeightMapSize() );
        bytemap->SetFilename(resName);

        if ( ! alloc )
        {
            // load from file
            if ( ! bytemap->Load() )
            {
                // The layer was still not created
                alloc = true;
            }
        }

        if ( alloc )
        {
            // create empty
            bytemap->SetUsage( nByteMap::CreateEmpty );
            if ( ! bytemap->Load() )
            {
                bytemap->Release();
                return 0;
            }
            bytemap->FillValue( 0 );
        }
    }

    return bytemap;
}
#endif

//------------------------------------------------------------------------------
void 
nTerrainCellInfo::SetTerrainCell(nEntityObject * cell)
{
    this->cell = cell;
}

//------------------------------------------------------------------------------
nEntityObject * 
nTerrainCellInfo::GetTerrainCell() const
{
    return this->cell;
}

#ifndef NGAME
//------------------------------------------------------------------------------
int
nTerrainCellInfo::GetGrowthMapValue()
{
    return this->growthMapvalue;
}
#endif

#ifndef NGAME
//------------------------------------------------------------------------------
void 
nTerrainCellInfo::SetGrowthMapValue(int value)
{
    this->growthMapvalue = value;
}
#endif