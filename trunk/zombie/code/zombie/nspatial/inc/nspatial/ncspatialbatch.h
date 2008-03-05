#ifndef NC_SPATIALBATCH_H
#define NC_SPATIALBATCH_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialBatch
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial component for batched entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/nspatialtypes.h"
#include "nspatial/ncspatial.h"

//------------------------------------------------------------------------------
class ncSpatialBatch : public ncSpatial
{

    NCOMPONENT_DECLARE(ncSpatialBatch, ncSpatial)

public:

    /// constructor
    ncSpatialBatch();
    /// destructor
    virtual ~ncSpatialBatch();

    /// set the test subentities flag
    void SetTestSubentities(bool);
    /// get the test subentities flag
    bool GetTestSubentities() const;
    /// add an entity to the subentities array
    void AddSubentity(nEntityObject*);
    /// add an entity to the subentities array using its id
    void AddSubentityId(nEntityObjectId);
    /// remove an entity from the subentities array
    void RemoveSubentity(nEntityObject*);
    /// remove an entity from the subentities array using its id
    void RemoveSubentityId(nEntityObjectId);
    /// remove all entities from the subentities array
    void ClearSubentities();
    /// get an entity from the subentities array
    nEntityObject* GetSubentity(int);
    /// update the batch's bounding box from its subentities
    void UpdateBBox();
    /// set the minimum distance the camera has to be to the batch to use it instead of its subentities
    void SetMinDistance(float);
    /// get the minimum distance the camera has to be to the batch to use it instead of its subentities
    float GetMinDistance() const;

    /// set the cell the entity is in
    virtual void SetCell(ncSpatialCell *cell);
    
    /// update element's transformation 
    virtual void Update(const matrix44 &worldMatrix);

    /// begin adding subentities- do not update in space until EndUpdate
    void BeginBatch();

    /// end adding subentities- update in space
    void EndBatch();

    /// get the subentities array
    const nArray<nEntityObject*> &GetSubentities();

    /// load subentities from file using ids
    bool LoadSubentities();

    /// get the minimum distance factor from camera to the batch to use it instead of its subentities
    float GetMaxDistanceFactor() const;

    /// recalculate the light links
    virtual void CalculateLightLinks();

    /// save state of the component
    bool SaveCmds(nPersistServer *ps);

private:

    /// flag to know if the subentities have to be tested for visibility
    bool testSubentities;

    /// subentities
    nArray<nEntityObject*> subentities;
    nArray<nEntityObjectId> subentityIds;

    /// min test distance
    float minDistance;
    /// min magnification factor (saved as xscale / distance ^2)
    float maxDistFactor;

    /// BeginSubentities has been called
    bool inBeginBatch;
    /// subentities have been added by id
    bool areSubentitiesLoaded;

};

//------------------------------------------------------------------------------
/**
    set the test subentities flag   
*/
inline
void 
ncSpatialBatch::SetTestSubentities(bool flag)
{
    this->testSubentities = flag;
}

//------------------------------------------------------------------------------
/**
    get the test subentities flag
*/
inline
bool 
ncSpatialBatch::GetTestSubentities() const
{
    return this->testSubentities;
}

//------------------------------------------------------------------------------
/**
    get an entity from the subentities array
*/
inline
nEntityObject*
ncSpatialBatch::GetSubentity(int index)
{
    return this->subentities.At(index);
}

//------------------------------------------------------------------------------
/**
    get the subentities array
*/
inline
const nArray<nEntityObject*> &
ncSpatialBatch::GetSubentities()
{
    return this->subentities;
}

//------------------------------------------------------------------------------
/**
    set the minimum distance the camera has to be to the batch to use it 
    instead of its subentities
*/
inline
void 
ncSpatialBatch::SetMinDistance(float dist)
{
    this->minDistance = dist;
    this->maxDistFactor = XSCALE_60 / (dist * dist);
}

//------------------------------------------------------------------------------
/**
    get the minimum distance the camera has to be to the batch to use it 
    instead of its subentities
*/
inline
float 
ncSpatialBatch::GetMinDistance() const
{
    return this->minDistance;
}

//------------------------------------------------------------------------------
/**
    get the minimum squared distance the camera has to be to the batch 
    to use it instead of its subentities
*/
inline
float 
ncSpatialBatch::GetMaxDistanceFactor() const
{
    return this->maxDistFactor;
}

//------------------------------------------------------------------------------
#endif // NC_SPATIALBATCH_H
