#ifndef N_NCTERRAINGMMCELL_H
#define N_NCTERRAINGMMCELL_H
/*-----------------------------------------------------------------------------
    @file ncterraingmmcell.h
    @class ncTerrainGMMCell
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief class to handle graphics data for each individual terrain cell

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngeomipmap.h"
#include "ngeomipmap/ngmmmeshcacheentry.h"
#include "nscene/ncscene.h"
#include "kernel/nref.h"
#include "mathlib/vector.h"
#include "nspatial/ncspatialquadtreecell.h"

//------------------------------------------------------------------------------
class nFloatMap;
class nGeoMipMapViewport;
class ncTerrainGMMClass;
class ncTerrainGMM;

//------------------------------------------------------------------------------
/**
    ncTerrainGMMCell represents a terrain block/tile/patch in the geomipmapping
    terrain rendering system.
*/
class ncTerrainGMMCell : public ncScene
{

    NCOMPONENT_DECLARE(ncTerrainGMMCell, ncScene);

public:

    // relation with other cells
    typedef enum {
        North = 0,
        East,
        South,
        West,

        NumDirections
    } Direction;

    /// constructor
    ncTerrainGMMCell();

    /// Initialize component on object creation
    void InitInstance(nObject::InitInstanceMsg);
    /// load the graphic object
    virtual bool Load();
    /// unload the graphic object
    virtual void Unload();

    /// Set cell number X and Z
    void SetCellXZ(int bx, int bz);
    /// Get ccell number X
    int GetCellX() const;
    /// Get ccell number Z
    int GetCellZ() const;
    /// Set neighbor cells
    void SetNeighbors(
        ncTerrainGMMCell * north,
        ncTerrainGMMCell * east,
        ncTerrainGMMCell * south,
        ncTerrainGMMCell * west);
    // Get neighbor cell
    ncTerrainGMMCell * GetNeighbor(Direction dir) const;
    // Set outdoor object
    void SetOutdoor(ncTerrainGMM * outdoor);
    // Set outdoor object
    ncTerrainGMM * GetOutdoor() const;

    /// Initialize data contents for later usage
    void InitLODParams(nFloatMap * hm, int numLODLevels, int startx, int startz, int blockSize);

    /// Mark the node as visible
    void SetVisible(int frameId);
    /// check if a block is visible or not
    bool IsVisible(int currentFrameId) const;

    /// Get current LOD level
    int GetGeometryLODLevel() const;
    /// Set LOD level
    void SetGeometryLODLevel(int lod);
    /// Get current LOD level
    int GetMaterialLODLevel() const;
    /// Set LOD level
    void SetMaterialLODLevel(int lod);
    /// Get material LOD id
    nFourCC GetMaterialLODFourCC() const; 
    /// Check if material has changed
    bool IsMaterialDirty();
    /// Set material dirty
    void SetMaterialDirty();

    /// Get the LOD of the neighbor block
    int GetNeighborLOD(Direction dir);
    /// has crack on direction specified
    bool HasCrack(Direction dir);
    /// get the index key
    int GetIndexKey();

    /// Check if already rendered for the render id provided
    bool NotRendered(void * renderId);

    /// Return terrain cell bounding box
    const bbox3 & GetBBox() const;
    /// Return the center of the terrain cell
    const vector3 & GetCenter() const;

    /// return pointer to an array with delta 2 values used for LOD selection
    const float * GetMinDelta2() const;

    /// return true if this cell has any hole
    bool HasHoles() const;

    /// set pink unpainted cell to disabled state for visibility
    bool SetUnpaintedCellsInvisible(bool enable);

protected:

    /// destructor
    virtual ~ncTerrainGMMCell();

    /// Calculate bilinear interpolation
    float InterpolateBilinear(float * h, float dx, float dz) const;
    /// Linear interpolation
    float InterpolateLinear(float h0, float h1, float lerp) const;

    /// outdoor
    ncTerrainGMM * outdoor;
    ncTerrainGMMClass * outdoorClass;
    /// neighbors
    ncTerrainGMMCell* neighbor[NumDirections];
    /// center of the block (bounding box)
    vector3 center;
    /// delta 2 values used for LOD selection
    float minDelta2[MaxLODLevels];
    /// last frame id when the terrain block was visible
    int frameId;
    /// to handle properly rendering in different viewports
    void * renderId;
    // cell x coord.
    nint16 bx;
    // cell z coord.
    nint16 bz;
    /// current LOD used for geometry and material
    nuint8 geometryLOD;
    nuint8 materialLOD;
    /// bounding box of the terrain cell
    bbox3 bbox;

    // material dirty (it has changed lod)
    bool materialDirty;

};

//------------------------------------------------------------------------------
/**
    Constructor
*/
inline
ncTerrainGMMCell::ncTerrainGMMCell() : 
    center(0,0,0),
    frameId(0),
    renderId(0),
    geometryLOD(0),
    materialLOD(0),
    bx(~0),
    bz(~0),
    //entry(0),
    //texEntry(0),
    outdoor(0),
    outdoorClass(0),
    materialDirty(false)
{
    for(int i = 0;i < MaxLODLevels;i++)
    {
        this->minDelta2[i] = 0;
    }
    for(int i = 0;i < NumDirections;i++)
    {
        this->neighbor[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
inline
ncTerrainGMMCell::~ncTerrainGMMCell()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Obtain the interpolated height given four heights, where
    h[0] = top left
    h[1] = top right
    h[2] = bottom left
    h[3] = bottom right
*/
inline
float 
ncTerrainGMMCell::InterpolateBilinear(float * h, float dx, float dz) const
{
    float top = h[0] * (1.0f - dx) + dx * h[1]; 
    float bottom = h[2] * (1.0f - dx) + dx * h[3];

    return top * (1.0f - dz) + dz * bottom;
}

//------------------------------------------------------------------------------
/**
    Calculate linear interpolation
*/
inline
float 
ncTerrainGMMCell::InterpolateLinear(float h0, float h1, float lerp) const
{
    return (h1 - h0) * lerp + (h1 - h0) * (1.0f - lerp);
}

//------------------------------------------------------------------------------
/**
    Mark the node as visible
*/
inline
void 
ncTerrainGMMCell::SetVisible(int frameId)
{
    //n_assert(frameId < this->frameId);
    this->frameId = frameId;
    this->renderId = 0;

    static int ncSpatialQuadtreeCellIndex = this->GetEntityObject()->IndexOfComponent( ncSpatialQuadtreeCell::GetComponentIdStatic() );
    n_assert( ncSpatialQuadtreeCellIndex != nComponentIdList::InvalidIndex );
    static int ncTerrainGMMCellIndex = this->GetEntityObject()->IndexOfComponent( ncTerrainGMMCell::GetComponentIdStatic() );
    n_assert( ncTerrainGMMCellIndex != nComponentIdList::InvalidIndex );

    // propagate visible state to children cells
    ncSpatialQuadtreeCell * qtcell = static_cast<ncSpatialQuadtreeCell *> (this->GetComponentSafeByIndex( ncSpatialQuadtreeCellIndex ));
    ncSpatialQuadtreeCell ** qtchildren = qtcell->GetSubcells();
    for(int i = 0;i < qtcell->GetNumSubcells();i++)
    {
        ncTerrainGMMCell * child = static_cast<ncTerrainGMMCell *> ( qtchildren[i]->GetComponentByIndex( ncTerrainGMMCellIndex ) );
        if (child)
        {
            child->SetVisible(frameId);
        }
    }
}

//------------------------------------------------------------------------------
/**
    @return true if the block is visible
*/
inline
bool 
ncTerrainGMMCell::IsVisible(int currentFrameId) const
{
    return (currentFrameId == this->frameId);
}

//------------------------------------------------------------------------------
/**
    @return current geometry LOD level for rendering the terrain block
*/
inline
int
ncTerrainGMMCell::GetGeometryLODLevel() const
{
    return this->geometryLOD;
}

//------------------------------------------------------------------------------
/**
    Set the current geometry LOD (used when levelling LODs)
*/
inline
void
ncTerrainGMMCell::SetGeometryLODLevel(int lod)
{
    n_assert(lod >= 0 && lod < MaxLODLevels);
    this->geometryLOD = static_cast<nuint8> (lod);
}

//------------------------------------------------------------------------------
/**
    @return current material LOD level for rendering the terrain block
*/
inline
int
ncTerrainGMMCell::GetMaterialLODLevel() const
{
    return this->materialLOD;
}

//------------------------------------------------------------------------------
/**
    Set the current material LOD
*/
inline
void
ncTerrainGMMCell::SetMaterialLODLevel(int lod)
{
    n_assert(lod >= 0 && lod < 2);
    this->materialDirty |= (this->materialLOD != static_cast<nuint8> (lod));
    this->materialLOD = static_cast<nuint8> (lod);
}

//------------------------------------------------------------------------------
inline
nFourCC 
ncTerrainGMMCell::GetMaterialLODFourCC() const
{
    if (this->GetMaterialLODLevel() == 0)
    {
        return FOURCC('gmm0');
    }
    else
    {
        return FOURCC('gmm1');
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
ncTerrainGMMCell::IsMaterialDirty()
{
    bool ret = this->materialDirty;
    this->materialDirty = false;
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ncTerrainGMMCell::SetMaterialDirty()
{
    this->materialDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncTerrainGMMCell::GetNeighborLOD(Direction dir)
{
    ncTerrainGMMCell * blockDir = this->neighbor[dir];
    return blockDir && blockDir->IsVisible(this->frameId) ? blockDir->GetGeometryLODLevel() : this->GetGeometryLODLevel();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncTerrainGMMCell::HasCrack(Direction dir)
{
    n_assert_always();
    ncTerrainGMMCell * blockDir = this->neighbor[dir];
    return blockDir && blockDir->IsVisible(this->frameId) ? blockDir->GetGeometryLODLevel() > this->GetGeometryLODLevel() : false;
}

//------------------------------------------------------------------------------
inline
bool 
ncTerrainGMMCell::NotRendered(void * renderId)
{
    if (this->renderId == renderId)
    {
        return false;
    }

    this->renderId = renderId;
    return true;
}

//------------------------------------------------------------------------------
/**
    Return the center of the cell.
*/
inline
const vector3 &
ncTerrainGMMCell::GetCenter() const
{
    return this->center;
}

//------------------------------------------------------------------------------
inline
const float * 
ncTerrainGMMCell::GetMinDelta2() const
{
    return this->minDelta2;
}

//------------------------------------------------------------------------------
#endif