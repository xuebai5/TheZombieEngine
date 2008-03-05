#ifndef N_GPMOVEMENT_H
#define N_GPMOVEMENT_H

//------------------------------------------------------------------------------
/**
    @class nGPMovement
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the movement between two points
*/

#include "ngpbasicaction/ngpbasicaction.h"
#include "mathlib/vector.h"

class nEntityObject;
class ncAIMovEngine;

class nGPMovement : public nGPBasicAction
{
public:
    /// Constructor
    nGPMovement();

    /// Destructor
    ~nGPMovement();

        /// Initial condition
        bool Init(nEntityObject*, const vector3&);

    /// Stop condition
    bool IsDone() const;

    /// End
    void End();

private:

    /// Move engine
    ncAIMovEngine* movEngine;

    /// Restore old move engine settings?
    bool restoreSettings;

    /// Movement engine previous speed (before calling this basic action)
    float previousSpeed;

};

#endif