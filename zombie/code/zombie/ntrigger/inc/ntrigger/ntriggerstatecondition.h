#ifndef N_TRIGGERSTATECONDITION_H
#define N_TRIGGERSTATECONDITION_H

//------------------------------------------------------------------------------
/**
    @class nTriggerStateCondition
    @ingroup NebulaTriggerSystem

    State or filter condition for a generic trigger input.

    This class is used either as an input condition in a generic trigger to
    evaluate a state, or as a filter condition attached to an input event
    condition.

    The Evaluate method must be overwritten with the state evaluation rule
    implementation (otherwise, the condition evaluates always to true).
    As input arguments, the Evaluate method receives the trigger itself plus
    the emitter entity of the event (NULL in the case of state condition,
    since there's no event). Evaluate should return a boolean telling if the
    condition triggers (true) or not (false).
*/

#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nTriggerStateCondition : public nRoot
{
public:

    /// Tell if the condition evaluates to true
    virtual bool Evaluate(nEntityObject*, nEntityObject*);

};

//------------------------------------------------------------------------------
#endif // N_TRIGGERSTATECONDITION_H
