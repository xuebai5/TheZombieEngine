#ifndef NC_SPATIALMC_H
#define NC_SPATIALMC_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialMC
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial component for entities that can be inserted in more than one cell.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/ncspatial.h"

//------------------------------------------------------------------------------
class ncSpatialMC : public ncSpatial
{

    NCOMPONENT_DECLARE(ncSpatialMC, ncSpatial);

public:

    /// constructor
    ncSpatialMC();
    /// destructor
    virtual ~ncSpatialMC();

    /// add a cell id the entity is in
    void AddCellId(int);
    /// add an indoor entity id this entity is in
    void AddSpaceId(nEntityObjectId);

    /// save state of the component
    bool SaveCmds(nPersistServer *ps);

    /// get the cell the entity is in
    ncSpatialCell *GetCell() const;
    /// get cells array
    const nArray<ncSpatialCell*> *GetCells() const;
    /// add a cell to the array of cells the entity is in
    void SetCell(ncSpatialCell *cell);
    /// remove a cell from the cells array
    bool RemoveCell(ncSpatialCell *cell);
    
    /// get space the entity is in
    ncSpatialSpace *GetSpace();
    /// get spaces the entity is in
    nArray<ncSpatialSpace*> GetSpaces() const;
    /// remove the entity from the spaces
    bool RemoveFromSpaces();

    /// set the cell id the entity is in (does nothing in this subclass)
    virtual void SetCellId(int cellId);
    /// get the cell id the entity is in (does nothing in this subclass)
    virtual int GetCellId() const;
    /// get the space id the entity is in
    virtual nEntityObjectId GetSpaceId() const;
    /// set the indoor entity id this entity is in (does nothing in this subclass)
    virtual void SetSpaceId(nEntityObjectId spaceId);

    /// accept a visibility visitor
    void Accept(nVisibleFrustumVisitor &visitor);

protected:

    /// cells the entity is in
    nArray<ncSpatialCell*> *cells;
    /// cell ids this entity is in (used only when loading the entity)
    nArray<int> *cellIds;
    /// space entity ids this entity is in (used only when loading the entity)
    nArray<nEntityObjectId> *spaceIds;

};

//------------------------------------------------------------------------------
/**
    get the cell the entity is in (the first one in the 'cells' array)
*/
inline
ncSpatialCell *
ncSpatialMC::GetCell() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    get cells array
*/
inline
const nArray<ncSpatialCell*> *
ncSpatialMC::GetCells() const
{
    return this->cells;
}

//------------------------------------------------------------------------------
/**
    add a cell to the array of cells the entity is in
*/
inline
void
ncSpatialMC::SetCell(ncSpatialCell *cell)
{
    this->cells->Append(cell);
}

//------------------------------------------------------------------------------
/**
    remove a cell from the cells array
*/
inline
bool 
ncSpatialMC::RemoveCell(ncSpatialCell *cell)
{
    int idx = this->cells->FindIndex(cell);
    if (idx != -1)
    {
        // cell found
        this->cells->EraseQuick(idx);
        return true;
    }

    // cell not found
    return false;
}

//------------------------------------------------------------------------------
/**
    set the cell id the entity is in (does nothing in this subclass)
*/
inline
void 
ncSpatialMC::SetCellId(int /*cellId*/)
{
    // do nothing
}

//------------------------------------------------------------------------------
/**
    get the cell id the entity is in (does nothing in this subclass)
*/
inline
int
ncSpatialMC::GetCellId() const
{
    return -1;
}

//------------------------------------------------------------------------------
/**
    set the indoor entity id this entity is in (does nothing in this subclass)
*/
inline
void 
ncSpatialMC::SetSpaceId(nEntityObjectId /*spaceId*/)
{
    // do nothing
}

//------------------------------------------------------------------------------
/**
    get the space id the entity is in
*/
inline
nEntityObjectId 
ncSpatialMC::GetSpaceId() const
{
    return nEntityObjectServer::IDINVALID;
}

//------------------------------------------------------------------------------
/**
    add a cell id the entity is in
*/
inline
void 
ncSpatialMC::AddCellId(int cellId)
{
    this->cellIds->Append(cellId);
}

//------------------------------------------------------------------------------
/**
    add an indoor entity id this entity is in
*/
inline
void 
ncSpatialMC::AddSpaceId(nEntityObjectId spaceId)
{
    this->spaceIds->Append(spaceId);
}

#endif // NC_SPATIALMC_H

