#ifndef NC_GPHEARING_H
#define NC_GPHEARING_H
//------------------------------------------------------------------------------
/**

    @file ncgphearing.h
    @class ncGPHearing
    @ingroup Entities
    @author Miquel Angel Rujula <>
   
    @brief Component Object that represents the hearing perception of an agent.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncTransform;
struct nGameEvent;

//------------------------------------------------------------------------------
class ncGPHearing : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGPHearing,nComponentObject);

public:

    /// Constructor
    ncGPHearing();

    /// Destructor
    ~ncGPHearing();

    /// Initialize instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Update the information comming from the entity class
    void UpdateClassInfo();

    /// Get the hearing radius
    float GetHearingRadius() const;

    /// Do a full perception process on the given event
    bool HearEvent( nGameEvent* event );

private:

    /// entity's hearing radius
    float hearingRadius;
    /// entity's transform component
    ncTransform *trComp;

};

//------------------------------------------------------------------------------
/**
    Get the hearing radius
*/
inline
float
ncGPHearing::GetHearingRadius() const
{
    return this->hearingRadius;
}

//------------------------------------------------------------------------------
#endif //NC_GPHEARING_H
