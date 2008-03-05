#ifndef NC_SPATIALGLOBALCELL_H
#define NC_SPATIALGLOBALCELL_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialGlobalCell
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial global cell component.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/ncspatialcell.h"

//------------------------------------------------------------------------------
class ncSpatialGlobalCell : public ncSpatialCell
{

    NCOMPONENT_DECLARE(ncSpatialGlobalCell, ncSpatialCell);

public:

    /// constructor
    ncSpatialGlobalCell();
    /// destructor
    virtual ~ncSpatialGlobalCell();

    /// adds an entity to the cell
    virtual bool AddEntity(nEntityObject *entity);
    /// removes an entity from the cell without destroying it
    virtual bool RemoveEntity(nEntityObject *entity);

};

#endif // NC_SPATIALGLOBALCELL_H

