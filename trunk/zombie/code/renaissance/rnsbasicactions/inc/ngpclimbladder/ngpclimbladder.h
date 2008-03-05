#ifndef N_GPCLIMBLADDER_H
#define N_GPCLIMBLADDER_H

//------------------------------------------------------------------------------
/**
    @class nGPClimbLadder
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of climb a ladder
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPClimbLadder : public nGPBasicAction
{
public:
    /// Constructor
    nGPClimbLadder();

    /// Destructor
    ~nGPClimbLadder();

        /// Initial condition
        bool Init(nEntityObject*, nEntityObject*);

    /// Stop condition
    bool IsDone() const;

private:
    nEntityObject* ladder;
};

#endif