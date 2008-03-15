#ifndef N_EVENTCONDITION_H
#define N_EVENTCONDITION_H

//------------------------------------------------------------------------------
/**
    @class nEventCondition
    @ingroup NebulaFSMSystem

    Transition condition triggered by an event.

    Optionally, the condition can have attached a script condition. This script
    condition is intended to work as a filter for the event, so that not always
    that the condition's event is catched the condition triggers.

    Who manages this condition is supposed to be listening for the condition's
    event. Then, when it catches the event, it calls to Evaluate, where the
    associated script condition is invoked to filter some situations (that is,
    a same event can come from different sources, but just for some of them
    the condition is wanted to trigger).

    If no filter condition is attached, the Evaluate method will just always
    return true.

    Although the filter condition is supposed to be a script condition, it's
    stored and used as a generic condition. So if new condition types are
    created they may be used as filter condition as well.

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/ncmdprotonativecpp.h"
#include "nfsm/ncondition.h"

//------------------------------------------------------------------------------
class nEventCondition : public nCondition
{
public:
    /// Default constructor
    nEventCondition();
    /// Destructor (does NOT release the associated filter condition)
    ~nEventCondition();

    /// Set the event that triggers this condition
    void SetEvent(int);
    /// Set the event that triggers this condition by unmutable id
    void SetEventByPersistentId(const char*);
    /// Get the event that triggers this condition
    int GetEvent () const;
    /// Set the optional filter condition (replaces the old one, but does NOT release it)
    void SetFilterCondition(nCondition*);
    /// Get the optional filter condition, or NULL if there isn't any
    nCondition* GetFilterCondition () const;
    /// Tell if the filter condition evaluates to true (always return true if there's no filter condition)
    bool Evaluate(nEntityObject*) const;

    /// Set an event condition as the filter condition, loading it if needed (used for peristence)
    void SetEventFilterCondition(const char*);
    /// Set a script condition as the filter condition, loading it if needed (used for peristence)
    void SetScriptFilterCondition(const char*);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Event that triggers this condition
    int eventType;
    /// Optional filter condition
    nCondition* filterCondition;
};

//-----------------------------------------------------------------------------
/**
    Set the event that triggers this condition
*/
inline
int nEventCondition::GetEvent() const
{
    return this->eventType;
}

#endif
