#ifndef N_LOADAREA_H
#define N_LOADAREA_H
//------------------------------------------------------------------------------
/**
    @class nLoadArea
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Base class to define areas and behavior for loading entities.

    Subclasses may describe different proximity criteria,
    types of distance tests to neighbor areas, loading policy, etc.
    Areas may be composed of indoor or outdoor cells, and entities to
    load may be referenced by the cells containing them
    (access through the containing cell) or batch entities.

    (C) 2006 Conjurer Services, S.A.
*/
#include "kernel/nobject.h"
#include "entity/nrefentityobject.h"

//------------------------------------------------------------------------------
class nLoadArea : public nRoot
{
public:
    /// area type
    enum AreaType
    {
        OutdoorArea = 0,
        IndoorArea  = 1,
        InvalidType = 0xffffffff,
    };

    /// constructor
    nLoadArea();

    /// destructor
    virtual ~nLoadArea();

    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// trigger loading/unloading of neighbors
    virtual void Trigger(const vector3& cameraPos, const int cameraCellId);

    /// get minimum distance from an outdoor position to the area
    virtual float GetMinDistanceSq(const vector3& cameraPos);

    /// load entities in cells of the area, and their resources
    virtual bool Load();

    /// unload entities in cells of the area, and their resources
    virtual void Unload();


    /// get whether the area has been modified
    bool IsDirty() const;

    /// get whether the area is loaded
    bool IsLoaded() const;

    /// get area type
    int GetType() const;

    /// get space entity
    nEntityObject* GetSpaceEntity();

    /// find if the area contains the cell if
    bool ContainsCell(int cellId);

    /// add a cell to the area by id
    void AddCell(int cellId);

    /// remove a cell from the area by id
    void RemoveCell(int cellId);

    /// set cells from array of ids
    void SetCellArray(nArray<int>& cells);

    /// clear neighbor areas
    void ClearNeighborAreas();

    /// append neighbor area
    void AddNeighborArea(nLoadArea* neighbor);

    /// get number of neighbor areas
    int GetNumNeighborAreas();

    /// get neighbor area by index
    nLoadArea* GetNeighborAreaAt(int index);

    /// check if area is a neighbor
    bool IsNeighborArea(nLoadArea* otherArea);

    /// setwizard (!NGAME)
    //void SetWizard(nEntityObjectId);
    /// set user description
    void SetDescription(const char *);
    /// get user description
    const char * GetDescription() const;
    /// set space id
    void SetSpaceId(nEntityObjectId);
    /// get space id
    nEntityObjectId GetSpaceId() const;
    /// begin cells
    void BeginCells(int);
    /// set cell at
    void SetCellAt(int, int);
    /// get cell at
    int GetCellAt(int);
    /// get number of cells
    int GetNumCells() const;
    /// end cells
    void EndCells();

    #ifndef NGAME
    /// get area index
    int GetAreaIndex();
    /// reset all counters
    void ResetStatsCounters();
    /// set value for a stats counter
    void SetAreaStatsCounter(const char *, int);
    /// drop size and resource stats into a report file
    virtual void CreateDebugStats();
    /// log debug stats to log file
    virtual bool LogDebugStats();
    /// load size and resource stats from debug file
    virtual void LoadDebugStats(const char *filename);
    /// save size and resource stats to debug file
    virtual void SaveDebugStats(const char *filename);
    #endif //NGAME

protected:
    /// wizard id of the area (!NGAME)
    //int wizardId;

    /// area is dirty, need to recompute neightbors
    bool isDirty;
    /// area is loaded
    bool isLoaded;
    /// area type
    int areaType;
    /// description of the area (user-friendly name)
    nString areaDescription;
    /// id of the spatial space this area belongs to
    nRefEntityObject refSpace;
    /// array of ids of cells in the area
    nArray<int> areaCells;
    /// array of neighbor areas
    nArray<nLoadArea*> neighbors;

    #ifndef NGAME
    enum
    {
        /// number of entities and different classes in the area (TODO: unique+shared)
        StatsNumEntities = 0,
        StatsNumClasses,
        /// number and size of different meshes in the area (TODO: unique+shared)
        StatsNumMeshes,
        StatsMeshSize,
        /// number and size of different textures in the area (TODO: unique+shared)
        StatsNumTextures,
        StatsTextureSize,
        
        NumStatsCounters,
    };
    
    /// convert string to counter index
    static const char *statsCounterStrings[NumStatsCounters];
    static int StringToStatsCounter(const char *);

    /// retrieve class stats from the entity class
    void GetDebugStatsFromClass(nEntityClass* entityClass);
    /// retrieve resource stats for the node
    void GetDebugStatsFromNode(nSceneNode* sceneNode);
    /// copy information from class, mesh and texture arrays to counters
    void GetStatsCountersFromArrays();

    int statsCounters[NumStatsCounters];
    bool statsValid;
    int areaId;
    static uint uniqueAreaId;
    #endif //NGAME
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoadArea::IsDirty() const
{
    return this->isDirty;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoadArea::IsLoaded() const
{
    return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nLoadArea::GetType() const
{
    return this->areaType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoadArea::SetDescription(const char *desc)
{
    this->areaDescription = desc;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nLoadArea::GetDescription() const
{
    return this->areaDescription.IsEmpty() ? 0 : this->areaDescription.Get();
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nLoadArea::ClearNeighborAreas()
{
    this->neighbors.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoadArea::AddNeighborArea(nLoadArea* neighbor)
{
    if (!this->neighbors.Find(neighbor))
    {
        this->neighbors.Append(neighbor);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nLoadArea::GetNumNeighborAreas()
{
    return this->neighbors.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nLoadArea*
nLoadArea::GetNeighborAreaAt(int index)
{
    return this->neighbors.At(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoadArea::IsNeighborArea(nLoadArea* neighbor)
{
    return this->neighbors.Find(neighbor) != 0;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
int
nLoadArea::GetAreaIndex()
{
    return this->areaId;
}
#endif

//------------------------------------------------------------------------------
#endif /*N_LOADAREA_H*/
