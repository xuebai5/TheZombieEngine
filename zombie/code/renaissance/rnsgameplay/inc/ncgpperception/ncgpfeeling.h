#ifndef NC_GPFEELING_H
#define NC_GPFEELING_H
//------------------------------------------------------------------------------
/**

    @file ncgpfeeling.h
    @class ncGPFeeling
    @ingroup Entities
    @author Miquel Angel Rujula <>
   
    @brief Component Object that represents the feeling perception of an agent.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"

struct nGameEvent;

class ncGPFeeling : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGPFeeling,nComponentObject);

public:

    /// Constructor
    ncGPFeeling();

    /// Destructor
    ~ncGPFeeling();

    /// Initialize instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Update the information comming from the entity class
    void UpdateClassInfo();

    /// Get the feeling radius
    float GetFeelingRadius() const;

    /// Get the transform applied to the feeling field to set it in the world
    void GetFeelingTransform( matrix44& m ) const;

    /// Do a full perception process on the given event
    bool FeelEvent( nGameEvent* event );

private:

    /// entity's feeling radius
    float feelingRadius;

};

//------------------------------------------------------------------------------
/**
    Get the feeling radius
*/
inline
float
ncGPFeeling::GetFeelingRadius() const
{
    return this->feelingRadius;
}

#endif
