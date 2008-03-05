#ifndef N_INDOORAREA_H
#define N_INDOORAREA_H
//------------------------------------------------------------------------------
/**
    @class nIndoorArea
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A class describing an indoor area. It adds the behavior of
    loading cells by number of cells through portals.

    (C) 2006 Conjurer Services, S.A.
*/

#include "zombieentity/nloadarea.h"
#include "util/narray2.h"

class ncSpatialCell;
//------------------------------------------------------------------------------
class nIndoorArea : public nLoadArea
{
public:
    /// constructor
    nIndoorArea();
    /// destructor
    ~nIndoorArea();

    /// object persistence
    virtual bool SaveCmds(nPersistServer *ps);

    /// trigger loading/unloading of neighbors
    virtual void Trigger(const vector3& cameraPos, const int cameraCellId);

    /// get minimum distance from an outdoor position to the area
    virtual float GetMinDistanceSq(const vector3& position);

    /// load entities in cells of the area, and their resources
    virtual bool Load();

    /// unload entities in cells of the area, and their resources
    virtual void Unload();

    /// set distance to trigger loading of neighbor areas
    void SetLoadDistance(int);
    /// get distance to trigger loading of neighbor areas
    int GetLoadDistance() const;
    /// set distance to trigger unloading of neighbor areas
    void SetUnloadDistance(int);
    /// get distance to trigger unloading of neighbor areas
    int GetUnloadDistance() const;

    #ifndef NGAME
    /// create area statistics (assume area is loaded)
    virtual void CreateDebugStats();
    #endif //NGAME

private:
    /// update distance to neightbor spaces from all cells
    void UpdateCells();

    /// get distance from a cell to a neighbor space
    int GetCellDistanceTo(ncSpatialCell* spatialCell, ncSpatialSpace* spatialSpace);

    /// min distance (in number of cells) to load a neighbor area
    int loadDistance;
    /// max distance (in number of cells) to unload a neighbor area
    int unloadDistance;

    /// distance (cellIndex, neighborIndex) to neighbor areas for all cells
    nArray2<int> cellDistances;

    /// collected portals to outdoor space
    nArray<nRefEntityObject> outdoorPortals;

    /// auxiliary key array to mark visited cells
    nKeyArray<bool> visitedCells;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nIndoorArea::SetLoadDistance(int dist)
{
    this->loadDistance = dist;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nIndoorArea::GetLoadDistance() const
{
    return this->loadDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIndoorArea::SetUnloadDistance(int dist)
{
    this->unloadDistance = dist;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nIndoorArea::GetUnloadDistance() const
{
    return this->unloadDistance;
}

#endif /*N_INDOORAREA_H*/
