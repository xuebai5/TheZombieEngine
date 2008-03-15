#ifndef N_SCRIPTCONDITION_H
#define N_SCRIPTCONDITION_H

//------------------------------------------------------------------------------
/**
    @class nScriptCondition
    @ingroup NebulaFSMSystem

    Transition condition triggered by a script.

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/ncmdprotonativecpp.h"
#include "nfsm/ncondition.h"

//------------------------------------------------------------------------------
class nScriptCondition : public nCondition
{
public:
    /// Default constructor
    nScriptCondition();

    /// Tell if the condition rule evaluates to true for the given entity
   virtual bool Evaluate(nEntityObject*) const;

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );
};

#endif
