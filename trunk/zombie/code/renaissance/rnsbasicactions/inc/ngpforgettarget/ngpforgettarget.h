#ifndef N_GPFORGETTARGET_H
#define N_GPFORGETTARGET_H

//------------------------------------------------------------------------------
/**
    @class nGPForgetTarget
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the how an entity "forget" a target
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPForgetTarget : public nGPBasicAction
{
public:
    /// Constructor
    nGPForgetTarget();

    /// Destructor
    ~nGPForgetTarget();

        /// Initial condition
        bool Init(nEntityObject*);

private:
};

#endif