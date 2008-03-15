#ifndef N_GPREPAIRVEHICLE_H
#define N_GPREPAIRVEHICLE_H

//------------------------------------------------------------------------------
/**
    @class nGPRepairVehicle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define how an entity repairs on a vehicle

	(C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPRepairVehicle : public nGPBasicAction
{
public:
    /// Constructor
    nGPRepairVehicle();

    /// Destructor
    ~nGPRepairVehicle();

        /// Initial condition
        bool Init(nEntityObject*, nEntityObject*);

    /// Stop condition
    bool IsDone() const;

private:
    nEntityObject* vehicle;
};

#endif