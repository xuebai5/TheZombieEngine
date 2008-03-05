#ifndef N_GPMOVEVEHICLE_H
#define N_GPMOVEVEHICLE_H

//------------------------------------------------------------------------------
/**
    @class nGPMoveVehicle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define how an entity enters on a vehicle
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPMoveVehicle : public nGPBasicAction
{
public:
    /// Constructor
    nGPMoveVehicle();

    /// Destructor
    ~nGPMoveVehicle();

        /// Initial condition
        bool Init(nEntityObject*,const float);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;
};

#endif