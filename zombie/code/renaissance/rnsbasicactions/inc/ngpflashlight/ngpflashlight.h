//------------------------------------------------------------------------------
/**
@class nGPWaitSomeTime
@ingroup NebulaGameplay

    This basic action stops the move engine for a given number of ticks.

(C) 2005 Copyright holder
*/
//------------------------------------------------------------------------------
#ifndef NGPFLASHLIGHT_H
#define NGPFLASHLIGHT_H

#include "ngpbasicaction/ngpbasicaction.h"

class ncLogicAnimator;
class ncGPSight;

class nGPFlashlight : public nGPBasicAction 
{
public:
    /// contructor
    nGPFlashlight();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Stop condition
    bool IsDone() const;

private:
        
    ncLogicAnimator* animator;

    int animIndex;
};

#endif//NGPLOOKAROUND_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
