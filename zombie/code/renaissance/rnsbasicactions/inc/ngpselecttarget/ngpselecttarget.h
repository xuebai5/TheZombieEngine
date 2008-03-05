#ifndef N_GPSELECTTARGET_H
#define N_GPSELECTTARGET_H

//------------------------------------------------------------------------------
/**
    @class nGPSelectTarget
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the selection target
*/

#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"
#include "util/narray.h"

class nEntityObject;

class nGPSelectTarget : public nGPBasicAction
{
public:
    /// Constructor
    nGPSelectTarget();

    /// Destructor
    ~nGPSelectTarget();

        /// Initial condition
        bool Init(nEntityObject*, nEntityObject*);

private:    
};

#endif