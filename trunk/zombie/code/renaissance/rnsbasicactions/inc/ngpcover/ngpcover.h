#ifndef N_GPCOVER_H
#define N_GPCOVER_H

//------------------------------------------------------------------------------
/**
    @class nGPCover
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of take cover

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPCover : public nGPBasicAction
{
public:
    /// Constructor
    nGPCover();

    /// Destructor
    ~nGPCover();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Stop condition
    bool IsDone() const;

private:
};

#endif