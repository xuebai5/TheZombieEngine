#ifndef N_GPCLOSEDOOR_H
#define N_GPCLOSEDOOR_H

//------------------------------------------------------------------------------
/**
    @class nGPMovement
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the movement between two points
*/

//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"
//------------------------------------------------------------------------------
class ncGamePlayDoor;
//------------------------------------------------------------------------------

class nGPCloseDoor : public nGPBasicAction
{
public:

    /// constructor
    nGPCloseDoor();

    /// destructor
    ~nGPCloseDoor();

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

#endif // N_GPCLOSEDOOR_H