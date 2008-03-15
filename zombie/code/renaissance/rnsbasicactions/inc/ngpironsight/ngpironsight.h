#ifndef NGPIRONSIGHT_H
#define NGPIRONSIGHT_H
//------------------------------------------------------------------------------
/**
    @class nGPIronsight
    @ingroup NebulaGameplayBasicActions
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class ncLogicAnimator;
class ncSoundLayer;
class ncGameplayLiving;

//------------------------------------------------------------------------------
class nGPIronsight : public nGPBasicAction 
{
public:
    /// constructor
    nGPIronsight();
    /// destructor
    ~nGPIronsight();

        /// Initial condition
        bool Init(nEntityObject*, bool);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
    ncGameplayLiving* gameplay;
    ncLogicAnimator* animator;
	ncSoundLayer*	 soundLayer;

    int animIndex;

    bool setIronsight;
};

//------------------------------------------------------------------------------
#endif//NGPIRONSIGHT_H
