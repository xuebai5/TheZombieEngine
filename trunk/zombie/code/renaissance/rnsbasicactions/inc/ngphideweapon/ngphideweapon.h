#ifndef NGPHIDEWEAPON_H
#define NGPHIDEWEAPON_H
//------------------------------------------------------------------------------
/**
    @class nGPHideWeapon
    @ingroup NebulaGameplayBasicActions

    Hide/Show the weapon playing the adecuate animation

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class ncLogicAnimator;
class ncSoundLayer;

//------------------------------------------------------------------------------
class nGPHideWeapon : public nGPBasicAction
{
public:
    /// constructor
    nGPHideWeapon();
    /// destructor
    ~nGPHideWeapon();

        /// Initial condition
        bool Init(nEntityObject*,bool);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    ncLogicAnimator* animator;
	ncSoundLayer* soundLayer;

    int animIndex;
};

//------------------------------------------------------------------------------
#endif//NGPHIDEWEAPON_H
