#ifndef N_OUTDOORAREA_H
#define N_OUTDOORAREA_H
//------------------------------------------------------------------------------
/**
    @class nOutdoorArea
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A class describing an outdoor area, made of a selection of cells.
    It adds the behavior of loading cells by linear distance comparison.

    (C) 2006 Conjurer Services, S.A.
*/

#include "zombieentity/nloadarea.h"
#include "entity/nrefentityobject.h"

//------------------------------------------------------------------------------
class ncSpatialBatch;
class ncSpatialQuadtreeCell;

//------------------------------------------------------------------------------
class nOutdoorArea : public nLoadArea
{
public:
    /// constructor
    nOutdoorArea();
    /// destructor
    ~nOutdoorArea();
    
    /// object persistence
    virtual bool SaveCmds(nPersistServer *ps);

    /// trigger loading/unloading of neighbors
    virtual void Trigger(const vector3& cameraPos, const int cameraCellId);

    /// load entities in cells of the area, and their resources
    virtual bool Load();

    /// unload entities in cells of the area, and their resources
    virtual void Unload();

    /// get minimum distance from an outdoor position to the area
    virtual float GetMinDistanceSq(const vector3& vector);

    /// set distance to trigger loading of neighbor areas
    void SetLoadDistance(float);
    /// get distance to trigger loading of neighbor areas
    float GetLoadDistance() const;
    /// set distance to trigger unloading of neighbor areas
    void SetUnloadDistance(float);
    /// get distance to trigger unloading of neighbor areas
    float GetUnloadDistance() const;

    #ifndef NGAME
    /// create area statistics (assume area is loaded)
    virtual void CreateDebugStats();
    #endif //NGAME

private:

    /// update spatial cells from id
    void UpdateCells();

    /// schedule for loading all entities in a terrain cell
    void LoadEntitiesInCell(ncSpatialQuadtreeCell* quadtreeCell);
    /// schedule for unloading all entities in a terrain cell
    void UnloadEntitiesInCell(ncSpatialQuadtreeCell* quadtreeCell);

    /// Load entities contained in a spatial batch
    void LoadEntitiesInBatch(ncSpatialBatch* batch);
    /// Unload entities contained in a spatial batch
    void UnloadEntitiesInBatch(ncSpatialBatch* batch);

    #ifndef NGAME
    /// helper method to count entities in an array
    void GetDebugStatsFromEntities(const nArray<nEntityObject*>& entities);
    #endif //NGAME

    /// cells collected from list of ids
    nArray<nRefEntityObject> cellsArray;
    /// portals leading to neighbor indoor areas
    nArray<nRefEntityObject> portalsArray;

    /// minimum distance to any cell center to have the area loaded
    float loadDistanceSq;
    float unloadDistanceSq;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nOutdoorArea::SetLoadDistance(float dist)
{
    this->loadDistanceSq = dist * dist;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nOutdoorArea::GetLoadDistance() const
{
    return n_sqrt(this->loadDistanceSq);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nOutdoorArea::SetUnloadDistance(float dist)
{
    this->unloadDistanceSq = dist * dist;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nOutdoorArea::GetUnloadDistance() const
{
    return n_sqrt(this->unloadDistanceSq);
}

#endif /*N_OUTDOORAREA_H*/
