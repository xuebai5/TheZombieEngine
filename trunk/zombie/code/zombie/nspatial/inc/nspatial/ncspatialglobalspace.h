#ifndef NC_SPATIALGLOBALSPACE_H
#define NC_SPATIALGLOBALSPACE_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialGlobalSpace
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial global space component.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nspatial/ncspatialspace.h"

//------------------------------------------------------------------------------
class ncSpatialGlobalSpace : public ncSpatialSpace
{

    NCOMPONENT_DECLARE(ncSpatialGlobalSpace, ncSpatialSpace);
    
public:

    /// constructor
    ncSpatialGlobalSpace();
    /// destructor
    virtual ~ncSpatialGlobalSpace();

    /// save state of the component
    virtual bool SaveCmds(nPersistServer *);

    /// remove entity from global space
    virtual bool RemoveEntity(nEntityObject *entity);

    /// add an always visible entity
    void AddAlwaysVisibleEntity(nEntityObject *entity);

    /// removes an always visible entity
    bool RemoveAlwaysVisibleEntity(nEntityObject *entity);

    /// get the always visible entities
    const nArray<nEntityObject*> &GetAlwaysVisibleEntities() const;

    /// reset the array of visible global entities
    void ResetVisEntitiesArray();

private:

    /// entities that are always visible
    nArray<nEntityObject*> alwaysVisEntities;

};

//------------------------------------------------------------------------------
/**
    get the always visible entities
*/
inline
const nArray<nEntityObject*> &
ncSpatialGlobalSpace::GetAlwaysVisibleEntities() const
{
    return this->alwaysVisEntities;
}

//------------------------------------------------------------------------------
/**
    reset the array of visible global entities
*/
inline
void 
ncSpatialGlobalSpace::ResetVisEntitiesArray()
{
    n_assert_return(this->m_cellsArray[0],);

    this->m_cellsArray[0]->ResetVisEntitiesArray();
}

#endif // NC_SPATIALGLOBALSPACE_H

