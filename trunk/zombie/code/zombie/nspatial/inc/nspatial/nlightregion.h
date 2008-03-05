#ifndef N_LIGHTREGION_H
#define N_LIGHTREGION_H
//------------------------------------------------------------------------------
/**
    @file nlightregion.h
    @class nLightRegion
    @ingroup NebulaSpatialSystem

    @author Miquel Angel Rujula <miquelangel.rujula@tragnrion.com>

    @brief This class represents a light's influence over a cell

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#include "nspatial/ncspatialcell.h"

class nFrustumClipper;

//------------------------------------------------------------------------------
class nLightRegion
{

public:

    /// Constructor
    nLightRegion();
    /// Destructor
    ~nLightRegion();

    /// Set the light that generated this region
    void SetParentLight(ncSpatialLight *light);
    /// Get the light that generated this region
    ncSpatialLight *GetParentLight() const;

    /// Set the previous light region
    void SetPrevLightRegion(nLightRegion *region);
    /// Get the previous light region
    nLightRegion *GetPrevLightRegion() const;

    /// Add a cell where the light region is in
    void AddCell(ncSpatialCell *cell);
    /// Get the cells the light region is in
    const nArray<ncSpatialCell*> &GetCells() const;

    /// Autoremove from a cell
    void RemoveFromCell(ncSpatialCell *cell);
    /// Autoremove from all the cells
    void RemoveFromCells();

    /// Create the light's frustum clipper
    void CreateFrustumClipper(nCamera2 &camera, const matrix44 &viewMatrix);

    /// Set an already built frustum clipper
    void SetClipper(const nFrustumClipper &newClipper);
    /// Get the frustum clipper
    nFrustumClipper *GetClipper() const;

#ifndef NGAME
    /// Draw the light region, for debug
    void Draw();
#endif // !NGAME

private:

    /// light that generated this region
    ncSpatialLight *parentLight;

    /// link to the previous light region in the tree of the light
    nLightRegion *prevLightRegion;

    /// cell the region is in
    nArray<ncSpatialCell*> cells;

    /// region's frustum clipper
    nFrustumClipper *frustumClipper;
};

//------------------------------------------------------------------------------
/**
    Constructor
*/
inline
nLightRegion::nLightRegion():
    parentLight(0),
    prevLightRegion(0),
    frustumClipper(0),
    cells(2, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Autoremove from all the cells
*/
inline
void
nLightRegion::RemoveFromCells()
{
    while ( !this->cells.Empty() )
    {
        this->RemoveFromCell(this->cells.Back());
    }
}

//------------------------------------------------------------------------------
/**
    Set the light that generated this region
*/
inline
void 
nLightRegion::SetParentLight(ncSpatialLight *light)
{
    this->parentLight = light;
}

//------------------------------------------------------------------------------
/**
    Get the light that generated this region
*/
inline
ncSpatialLight *
nLightRegion::GetParentLight() const
{
    return this->parentLight;
}

//------------------------------------------------------------------------------
/**
    Set the previous light region
*/
inline
void 
nLightRegion::SetPrevLightRegion(nLightRegion *region)
{
    this->prevLightRegion = region;
}

//------------------------------------------------------------------------------
/**
    Get the previous light region
*/
inline
nLightRegion *
nLightRegion::GetPrevLightRegion() const
{
    return this->prevLightRegion;
}

//------------------------------------------------------------------------------
/**
    Add a cell the light is in
*/
inline
void 
nLightRegion::AddCell(ncSpatialCell *cell)
{
    n_assert(this->cells.FindIndex(cell) == -1);
    this->cells.Append(cell);
}

//------------------------------------------------------------------------------
/**
    Get the cells the light is in
*/
inline
const nArray<ncSpatialCell*> &
nLightRegion::GetCells() const
{
    return this->cells;
}

//------------------------------------------------------------------------------
/**
    Get the frustum clipper
*/
inline
nFrustumClipper *
nLightRegion::GetClipper() const
{
    return this->frustumClipper;
}

//------------------------------------------------------------------------------
#endif // N_LIGHTREGION_H
