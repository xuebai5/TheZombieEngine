//------------------------------------------------------------------------------
/**
    @class nGPWaitSomeTime
    @ingroup NebulaGameplay
    
    This basic action turn on/off the weapon addon flashlight.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NGPSWITCHFLASHLIGHT_H
#define NGPSWITCHFLASHLIGHT_H

#include "ngpbasicaction/ngpbasicaction.h"

class nGPSwitchFlashlight : public nGPBasicAction 
{
public:
    /// contructor
    nGPSwitchFlashlight();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Stop condition
    bool IsDone() const;

};

#endif//NGPSWITCHFLASHLIGHT_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
