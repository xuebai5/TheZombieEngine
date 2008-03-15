#ifndef NCGAMEPLAYVEHICLECLASS_H
#define NCGAMEPLAYVEHICLECLASS_H

//------------------------------------------------------------------------------
/**
    @class ncGameplayWheelClass

    (C) 2005 Conjurer Services, S.A.
*/

#include "rnsgameplay/ncgameplayclass.h"

class ncGameplayWheelClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGameplayWheelClass,ncGameplayClass);

public:
    /// Constructor
    ncGameplayWheelClass();
    /// Destructor
    ~ncGameplayWheelClass();
};

#endif