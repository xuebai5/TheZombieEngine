#ifndef N_GPLEAVEVEHICLE_H
#define N_GPLEAVEVEHICLE_H

//------------------------------------------------------------------------------
/**
    @class nGPLeaveVehicle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define how an entity leaves a vehicle

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nEntityObject;

class nGPLeaveVehicle : public nGPBasicAction
{
public:
    /// Constructor
    nGPLeaveVehicle();

    /// Destructor
    ~nGPLeaveVehicle();

        /// Initial condition
        bool Init(nEntityObject*, nEntityObject*,const int);

    /// Stop condition
    bool IsDone() const;

    /// Main loop
    bool Run();

private:
    /// find a position to exit
    const bool ExitPosition( vector3& exitposition );
    
    /// stores the vehicle entity
    nEntityObject* vehicle;
    /// stores the seat entity
    nEntityObject* seat;

    /// stores if the action is done
    bool isdone;
};

#endif