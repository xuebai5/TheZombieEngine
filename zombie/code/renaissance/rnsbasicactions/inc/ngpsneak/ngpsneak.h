#ifndef N_GPSNEAK_H
#define N_GPSNEAK_H

//------------------------------------------------------------------------------
/**
    @class nGPSneak
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of sneak

	(C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class nGPSneak : public nGPBasicAction
{
public:
    /// Constructor
    nGPSneak();

    /// Destructor
    ~nGPSneak();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Stop condition
    bool IsDone() const;

private:
};

#endif