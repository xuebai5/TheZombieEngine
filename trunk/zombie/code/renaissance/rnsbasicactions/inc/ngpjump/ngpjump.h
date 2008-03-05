#ifndef N_GPJUMP_H
#define N_GPJUMP_H

//------------------------------------------------------------------------------
/**
    @class nGPJump
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of a jump to a position 
*/
                                                                                       
#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"

class nGPJump : public nGPBasicAction
{
public:
    /// Constructor
    nGPJump();

    /// Destructor
    ~nGPJump();

        /// Initial condition
        bool Init(nEntityObject*, const vector3&);

    /// Stop condition
    bool IsDone() const;

private:
    vector3 goal;
};

#endif