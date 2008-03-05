#ifndef NCGAMEPLAYVEHICLECLASS_H
#define NCGAMEPLAYVEHICLECLASS_H

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