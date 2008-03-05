#ifndef N_SPATIALCOLLECTOR_H
#define N_SPATIALCOLLECTOR_H

class nEntityObject;

//------------------------------------------------------------------------------
/**
    @class nSpatialCollector
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialCollector filters and collects entities with a certain criteria.

    (C) 2005  Conjurer Services, S.A.
*/

class nSpatialCollector
{

public:

    /// constructor
    nSpatialCollector(){}
    /// destructor
    virtual ~nSpatialCollector(){}

    /// check an entity and store it
    virtual bool Accept(nEntityObject *entity)=0;

    /// reset all
    virtual void Reset() = 0;

};

#endif