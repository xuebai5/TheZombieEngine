#ifndef N_GPGOTOALLY_H
#define N_GPGOTOALLY_H

//------------------------------------------------------------------------------
/**
    @class nGPGoToAlly
    @ingroup NebulaGameplayBasicActions

    @brief Action class that makes an agent goes near a calling ally

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncLogicAnimator;
class ncAIMovEngine;

class nGPGoToAlly: public nGPBasicAction
{
public:
    /// Constructor
    nGPGoToAlly();

    /// Destructor
    ~nGPGoToAlly();

        /// Initial condition
        bool Init(nEntityObject*, int, float);

    /// Stop condition
    bool IsDone() const;

    /// Main loop
    bool Run();

    /// End
    void End();

private:

    /// Calling ally
    nEntityObject* callingAlly;

    /// Logic animator
    ncLogicAnimator* animator;

    /// Move engine
    ncAIMovEngine* movEngine;

    /// Destination point
    vector3 destPoint;

    /// Calculate a point near the calling ally
    void CalculateDestPoint();

    /// Restore old move engine settings?
    bool restoreSettings;

    /// Movement style when this basic action is called (backup)
    int previousMoveStyle;

    /// Movement engine previous speed (before calling this basic action)
    float previousSpeed;

};

#endif
