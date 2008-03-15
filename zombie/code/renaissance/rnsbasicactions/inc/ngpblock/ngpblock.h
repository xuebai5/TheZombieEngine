#ifndef N_GPBLOCK_H
#define N_GPBLOCK_H

//------------------------------------------------------------------------------
/**
    @class nGPBlock
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of a block attack over a target

    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncLogicAnimator;
class ncGPWeaponMelee;
class ncGameplayLiving;

class nGPBlock : public nGPBasicAction
{

public:
    /// Constructor
    nGPBlock();

    /// Destructor
    ~nGPBlock();

        /// Initial condition
        bool Init(nEntityObject*, int, int);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

    /// End
    virtual void End();

private:

    /// This variables controls the blocking state
    int startOffset;
    int endOffset;

    /// Time passed since the start of the basic action
    int ellapsedTime;
    /// Logic animator
    ncLogicAnimator* animator;
    /// Attack weapon
    ncGPWeaponMelee* weapon;
    /// Living component
    ncGameplayLiving *living;

    int animIndex;
    
};

#endif