#ifndef NC_GPSIGHT_H
#define NC_GPSIGHT_H

//------------------------------------------------------------------------------
/**
    @file ncgpsight.h
    @class ncGPSight
    @ingroup Entities
    @author Miquel Angel Rujula
   
    @brief Component Object that represents the sight perception of an agent.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "mathlib/line.h"

class ncTransform;
class cone;
struct nGameEvent;

class ncGPSight : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGPSight,nComponentObject);

public:

    /// Constructor
    ncGPSight();

    /// Destructor
    ~ncGPSight();

    /// Initialize instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Update the information comming from the entity class
    void UpdateClassInfo();

    /// Get the sight radius
    float GetSightRadius() const;

    /// Get the sight cone
    cone* GetSightCone() const;

    /// Get the transform applied to the sight field to set it in the world
    void GetSightTransform( matrix44& m ) const;

    /// Do a full perception process on the given event
    bool SeeEvent( nGameEvent* event );

    /// Return entity character neck transform
    ncTransform* GetNeckTransform( nEntityObject* entity );

#ifndef NGAME
    /// Get the ray used to check for LoS
    const line3 & GetSightRay() const;
#endif

private:

    /// entity's sight radius
    float sightRadius;
    /// entity's sight cone
    cone* sightCone;
    /// entity's transform component
    ncTransform* trComp;
    nEntityObjectId trCompId;

#ifndef NGAME
    // ray used to test if there's a clear LoS towards an entity
    line3 sightRay;
#endif

};

//------------------------------------------------------------------------------
/**
Get the sight cone
*/
inline
cone*
ncGPSight::GetSightCone() const
{
    return this->sightCone;
}

//------------------------------------------------------------------------------
/**
    Get the sight radius
*/
inline
float
ncGPSight::GetSightRadius() const
{
    return this->sightRadius;
}

#endif
