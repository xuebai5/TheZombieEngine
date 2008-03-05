#ifndef NC_OUTDOORBUILDER_H
#define NC_OUTDOORBUILDER_H
//------------------------------------------------------------------------------
/**
    @class ncOutdoorBuilder
    @ingroup Builders
    @author Miquel Angel Rujula <>

    @brief Component that builds an outdoor.

    (C) 2006 Conjurer Services, S.A.
*/

#include "entity/nentity.h"

class bbox3;
class ncTerrainGMMClass;

//------------------------------------------------------------------------------
class ncOutdoorBuilder : public nComponentObject
{

    NCOMPONENT_DECLARE(ncOutdoorBuilder, nComponentObject);

public:

    /// constructor
    ncOutdoorBuilder();
    /// destructor
    virtual ~ncOutdoorBuilder();

    /// it's called once all the entity information is ready for the components
    virtual void InitInstance(nObject::InitInstanceMsg initType);

private:

#ifndef __ZOMBIE_EXPORTER__

    /// get the terrain's bounding box
    bbox3 GetBBoxFromTerrain(ncTerrainGMMClass *terrainGMMClass) const;

    /// initialize gmm terrain
    void InitTerrainGMM();

    /// initialize physics terrain
    void InitPhyTerrain(ncTerrainGMMClass *terrainGMMClass);

    /// initialize navigation mesh
    void InitNavMesh();

#endif // __ZOMBIE_EXPORTER__

};

//------------------------------------------------------------------------------
/**
    constructor
*/
inline
ncOutdoorBuilder::ncOutdoorBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
inline
ncOutdoorBuilder::~ncOutdoorBuilder()
{
    // empty
}

#endif // NC_OUTDOORBUILDER_H
