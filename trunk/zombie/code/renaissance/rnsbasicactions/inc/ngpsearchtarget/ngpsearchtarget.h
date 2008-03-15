#ifndef N_GPSEARCHTARGET_H
#define N_GPSEARCHTARGET_H

//------------------------------------------------------------------------------
/**
    @class nGPSearchTarget
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the search for a target

	(C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"
#include "util/narray.h"

class nEntityObject;
class ncAIState;
class ncGameplayLiving;

class nGPSearchTarget : public nGPBasicAction
{
public:
    /// Constructor
    nGPSearchTarget();

    /// Destructor
    ~nGPSearchTarget();

        /// Initial condition    
        bool Init(nEntityObject*);

private:

    /// Find the closest entity in a subset of them
    nEntityObject* FindClosest() const;

    // Check if it's valid entity
    bool ConsiderEntity (nEntityObject* target) const;

    // The entity's ncAIState
    ncAIState *aistate;
    // The entity's ncGameplayLiving
    ncGameplayLiving *living;
    // The current target
    nEntityObject *curr_target;
    // The current fight ring index
    int curr_fightring;
};

#endif