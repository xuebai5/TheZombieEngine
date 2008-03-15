#ifndef N_GPAPPLYHEALTH_H
#define N_GPAPPLYHEALTH_H
//------------------------------------------------------------------------------
/**
    @class nGPApplyHealth
    @ingroup NebulaGameplay
    
    This basic action injects to the player an injection for restore health.
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

class ncGameplayLiving;
class ncLogicAnimator;

class nGPApplyHealth : public nGPBasicAction 
{
public:
    /// contructor
    nGPApplyHealth();

        /// Initial condition
        bool Init(nEntityObject*, int);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:

    ncGameplayLiving* livingEntity;
    ncLogicAnimator* animator;
    int finalHealth;
};

#endif//NGPAPPLYHEALTH_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
