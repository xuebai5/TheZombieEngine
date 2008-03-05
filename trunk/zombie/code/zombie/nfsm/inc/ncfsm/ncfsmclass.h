#ifndef NC_FSMCLASS_H
#define NC_FSMCLASS_H

//------------------------------------------------------------------------------
/**
    @class ncFSMClass
    @ingroup NebulaFSMSystem

    Finite state machine class component.

    It just holds a reference to the parent FSM and the frequency at which
    script condition transitions are checked.

    @see ncFSM to know more about the agent dependent FSM part.
    @see ncFSMClass to know more about the agent type dependent FSM part.
*/

#include "entity/nentity.h"
#include "util/nstring.h"
#include "nfsm/nfsm.h"

//------------------------------------------------------------------------------
class ncFSMClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncFSMClass,nComponentClass);

public:
    /// Default constructor
    ncFSMClass();

    /// Set the parent FSM
    void SetParentFSM(nFSM*);
    /// Get the parent FSM
    nFSM* GetParentFSM () const;
    /// Set the frequency at which the script condition transitions are checked
    void SetConditionPoolingFrequency(int);
    /// Get the frequency at which the script condition transitions are checked
    int GetConditionPoolingFrequency () const;

    /// Set the parent FSM by name (used for persistence)
    void SetParentFSMByName(const char*);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Name of the parent FSM
    nString parentFSMName;
    /// Ticks interval at which the script condition transitions are checked
    int ticksInterval;

};

//-----------------------------------------------------------------------------
/**
    Get the frequency at which the script condition transitions are checked
*/
inline
int
ncFSMClass::GetConditionPoolingFrequency() const
{
    return this->ticksInterval;
}

//-----------------------------------------------------------------------------
#endif // NC_FSMCLASS_H
