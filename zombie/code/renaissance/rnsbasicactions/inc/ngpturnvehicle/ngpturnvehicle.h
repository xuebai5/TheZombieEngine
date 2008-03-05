#ifndef N_GPTURNVEHICLE_H
#define N_GPTURNVEHICLE_H

//------------------------------------------------------------------------------
/**
    @class nGPTurnVehicle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define how an entity enters on a vehicle
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPTurnVehicle : public nGPBasicAction
{
public:
    /// Constructor
    nGPTurnVehicle();

    /// Destructor
    ~nGPTurnVehicle();

        /// Initial condition
        bool Init(nEntityObject*,const float);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;
};

#endif