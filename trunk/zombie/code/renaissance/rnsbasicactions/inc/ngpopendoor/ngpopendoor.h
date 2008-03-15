#ifndef N_GPOPENDOOR_H
#define N_GPOPENDOOR_H

//------------------------------------------------------------------------------
/**
    @class nGPMovement
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the movement between two points

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"
//------------------------------------------------------------------------------
class ncGamePlayDoor;
//------------------------------------------------------------------------------

class nGPOpenDoor : public nGPBasicAction
{
public:

    /// constructor
    nGPOpenDoor();

    /// destructor
    ~nGPOpenDoor();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:

    /// stores the door component
    ncGamePlayDoor* door;

    /// stores if the actions it's done
    bool done;

};

#endif // N_GPOPENDOOR_H