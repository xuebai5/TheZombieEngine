//------------------------------------------------------------------------------
/**
    @class nGPWaitSomeTime
    @ingroup NebulaGameplayBasicActions
    
    This basic action stops the move engine for a given number of ticks.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NGPWAITSOMETIME_H
#define NGPWAITSOMETIME_H

#include "ngpbasicaction/ngpbasicaction.h"

class ncAIMovEngine;

class nGPWaitSomeTime : public nGPBasicAction 
{
public:
    /// contructor
    nGPWaitSomeTime();

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
};

#endif//NGPWAITSOMETIME_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
