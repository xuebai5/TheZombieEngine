#ifndef N_GPHIDE_H
#define N_GPHIDE_H

//------------------------------------------------------------------------------
/**
    @class nGPHide
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of hide from point

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"

class nGPHide : public nGPBasicAction
{
public:
    /// Constructor
    nGPHide();

    /// Destructor
    ~nGPHide();

        /// Initial condition
        bool Init(nEntityObject*, const vector3&);

    /// Stop condition
    bool IsDone() const;

private:
    vector3 orign;
};

#endif