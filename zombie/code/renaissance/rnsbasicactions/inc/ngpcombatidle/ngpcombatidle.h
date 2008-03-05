#ifndef N_GPCOMBATIDLE_H
#define N_GPCOMBATIDLE_H

//------------------------------------------------------------------------------
/**
    @class nGPCombatIdle
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the combat idle action of an agent
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncLogicAnimator;
class ncSoundLayer;

class nGPCombatIdle: public nGPBasicAction
{
public:
    /// Constructor
    nGPCombatIdle();

    /// Destructor
    ~nGPCombatIdle();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Stop condition
    bool IsDone() const;

    /// Main loop
    bool Run();

private:

    /// Logic animator
    ncLogicAnimator* animator;

    /// Sound layer
    ncSoundLayer* soundLayer;

    /// Combat idle animation index
    int animIndex;
};

#endif
