#ifndef N_GPTAKEPOSITIONVEHICLE_H
#define N_GPTAKEPOSITIONVEHICLE_H

//------------------------------------------------------------------------------
/**
    @class nGPTakePositionVehicle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define how an entity takes position within a vehicle

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPTakePositionVehicle : public nGPBasicAction
{
public:
    /// Constructor
    nGPTakePositionVehicle();

    /// Destructor
    ~nGPTakePositionVehicle();

        /// Initial condition
        bool Init(nEntityObject*, nEntityObject*);

    /// Stop condition
    bool IsDone() const;

private:
    nEntityObject* vehicle;
};

#endif