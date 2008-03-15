#ifndef N_GPENTERVEHICLE_H
#define N_GPENTERVEHICLE_H

//------------------------------------------------------------------------------
/**
    @class nGPEnterVehicle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define how an entity enters on a vehicle

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPEnterVehicle : public nGPBasicAction
{
public:
    /// Constructor
    nGPEnterVehicle();

    /// Destructor
    ~nGPEnterVehicle();

        /// Initial condition
        bool Init(nEntityObject*,nEntityObject*);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:

    /// stores the vehicle entity
    nEntityObject* vehicle;

    /// store if the job is done
    bool isdone;
};

#endif