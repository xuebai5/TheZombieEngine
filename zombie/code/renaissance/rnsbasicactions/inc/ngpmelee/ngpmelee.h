#ifndef N_GPMELEE_H
#define N_GPMELEE_H

//------------------------------------------------------------------------------
/**
    @class nGPMelee
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action of a melee attack over a target
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncLogicAnimator;
class ncGPWeaponMelee;

class nGPMelee : public nGPBasicAction
{

public:
    /// Constructor
    nGPMelee();

    /// Destructor
    ~nGPMelee();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Main loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:

    /// Defines a melee hit over the target
    bool HitTarget() const;

    /// Says if is at range for a melee attack
    bool IsAtRange() const;

    /// The target of the action
    nEntityObject* target;
    /// Logic animator
    ncLogicAnimator* animator;
    /// Attack weapon
    ncGPWeaponMelee* weapon;
    /// Is a player?
    bool isPlayer;
    /// Animation index for check if is ended
    int animIndex;
    /// time for begin the hit colision
    float initOffset;
    /// time the colission dtection ends
    float endOffset;
    /// time that the basic action starts
    float initTime;
    /// check if the target if hitted
    bool hitTarget;    
};

#endif