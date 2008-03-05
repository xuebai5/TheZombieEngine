#ifndef NGPSWITCHFIREMODE_H
#define NGPSWITCHFIREMODE_H
//------------------------------------------------------------------------------
/**
    @class nGPSwitchFireMode
    @ingroup
    
    Basic Action to switch the mode of the weapon
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class ncGameplayLiving;
class ncLogicAnimator;
class ncGPWeapon;

//------------------------------------------------------------------------------
class nGPSwitchFireMode : public nGPBasicAction
{
public:
    /// constructor
    nGPSwitchFireMode();
    /// destructor
    ~nGPSwitchFireMode();

        /// Initial condition
        bool Init(nEntityObject*);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    ncGameplayLiving * gameplay;
    ncLogicAnimator * animator;
    ncGPWeapon * weapon;
    int newMode;
    int animIndex;
};

//------------------------------------------------------------------------------
#endif//NGPSWITCHFIREMODE_H
