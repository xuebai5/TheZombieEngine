//------------------------------------------------------------------------------
/**
@class nGPIdle
@ingroup NebulaGameplayBasicActions

    This basic action idles for a given number of ticks.

(C) 2005 Copyright holder
*/
//------------------------------------------------------------------------------
#ifndef NGPIDLE_H
#define NGPIDLE_H

#include "ngpbasicaction/ngpbasicaction.h"

class ncAIMovEngine;
class ncLogicAnimator;

class nGPIdle : public nGPBasicAction 
{
public:
    /// contructor
    nGPIdle();

    /// Initial condition
    bool Init(nEntityObject*, float);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:

    /// Total time to be stopped
    float finishTime;
    /// Used for time count
    float initTime;

    // Whether SetIdle has been called
    bool idleSet;

    // The entity's animator
    ncLogicAnimator* animator;
};

#endif//NGPIDLE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
