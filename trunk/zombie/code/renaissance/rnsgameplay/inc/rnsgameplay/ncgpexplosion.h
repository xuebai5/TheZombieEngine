#ifndef NCGPEXPLOSION_H
#define NCGPEXPLOSION_H
//------------------------------------------------------------------------------
/**
    @class ncGPExplosion
    @ingroup Entities

    Component object that represents an explosive behaviour

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

struct nLaunchSpecialFxData;

//------------------------------------------------------------------------------
class ncGPExplosion : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGPExplosion,nComponentObject);

public:
    /// explode the grenade
    void Explode( const vector3 & direction );

private:
    void CreateEffect(  const vector3 & direction, const vector3 & position, nLaunchSpecialFxData& effect )const;

};

//------------------------------------------------------------------------------
#endif//NCGPEXPLOSION_H
