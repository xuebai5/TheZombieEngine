#ifndef N_GPRELOADWEAPON_H
#define N_GPRELOADWEAPON_H

//------------------------------------------------------------------------------
/**
    @class nGPReloadWeapon
    @ingroup NebulaGameplayBasicActions

    @brief Action class that define the action reload the current weapon
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncGPWeapon;
class ncLogicAnimator;

class nGPReloadWeapon : public nGPBasicAction
{
public:
    /// Constructor
    nGPReloadWeapon();

    /// Destructor
    ~nGPReloadWeapon();

    /// Initial condition
    bool Init(nEntityObject*, bool);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    enum ReloadState{
        RS_RELOADING, RS_NEEDCOCK, RS_END
    };

    /// calculate the bullets and the clips changed
    bool CalculateAmmo( bool fastReload );

    nString plugName;
    nString oldPlugName;

    int newAmmo;
    int newAmmoExtra;
    int removedBullets;

    ncGPWeapon * weapon;
    ncLogicAnimator * animator;
    ncGameplayLiving * gameplay;

    int animIndex;

    bool middleChange;
};

#endif