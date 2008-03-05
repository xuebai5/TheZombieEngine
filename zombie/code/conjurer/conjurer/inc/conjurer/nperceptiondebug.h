#ifndef N_PERCEPTIONDEBUG_H
#define N_PERCEPTIONDEBUG_H
//------------------------------------------------------------------------------
/**
    @class nPerceptionDebug
    @ingroup NebulaDebugSystem

    @brief A class to debug the perception system.
    @author Miquel Angel Rujula  <>

    (C) 2005 Conjurer Services, S.A.
*/
#ifndef NGAME

#include "ndebug/ndebugmodule.h"

//------------------------------------------------------------------------------
class nPerceptionDebug : public nDebugModule
{

public:

    /// constructor
    nPerceptionDebug();
    /// destructor
    virtual ~nPerceptionDebug();
    /// called when the module is created
    virtual void OnDebugModuleCreate();
    /// called when an option is assigned
    virtual void OnDebugOptionUpdated();
    
    enum OptionFlag
    {
        Perception          = 1<<0,
        PerceptionHearing   = 1<<1,
        PerceptionFeeling   = 1<<2,
        PerceptionSight     = 1<<3
    };

    uint optionFlags;
};

#endif // NGAME

//------------------------------------------------------------------------------
#endif // N_PERCEPTIONDEBUG_H
