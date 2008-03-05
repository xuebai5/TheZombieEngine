#ifndef NC_SPATIALCELLSAVER_H
#define NC_SPATIALCELLSAVER_H
//------------------------------------------------------------------------------
/**
    @class nSpatialCellSaver
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Saves all the entities that are in a spatial cell.

    (C) 2005 Conjurer Services, S.A.
*/
class ncSpatialCell;

//------------------------------------------------------------------------------
class nSpatialCellSaver : public nObject
{
    
public:

    /// constructor 1
    nSpatialCellSaver();
    /// constructor 2
    nSpatialCellSaver(ncSpatialCell *cell);
    /// destructor
    ~nSpatialCellSaver();

    /// save the entities contained in the cell
    bool Save(const char *fileName);
    /// load the entities contained in the cell
    bool Load(const char *fileName);
    /// set the cell to save
    void SetCell(ncSpatialCell *cell);
    /// get the cell that's trying to save
    ncSpatialCell *GetCell();

        /// Add an entity id to the cell
        void AddEntityId(nEntityObjectId);
        /// Set wizard key id
        void SetWizard(int);

private:

    /// cell to save
    ncSpatialCell *m_cell;

};

//------------------------------------------------------------------------------
/**
    set the cell to save
*/
inline
void 
nSpatialCellSaver::SetCell(ncSpatialCell *cell)
{
    this->m_cell = cell;
}

//------------------------------------------------------------------------------
/**
    get the cell that's trying to save
*/
inline
ncSpatialCell *
nSpatialCellSaver::GetCell()
{
    return this->m_cell;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSpatialCellSaver::SetWizard( int /*number*/ )
{
    // do nothing
}

#endif // NC_SPATIALCELLSAVER_H 

