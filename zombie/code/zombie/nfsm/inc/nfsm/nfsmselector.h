#ifndef N_FSMSELECTOR_H
#define N_FSMSELECTOR_H

//------------------------------------------------------------------------------
/**
    @class nFSMSelector
    @ingroup NebulaFSMSystem

    Selector of a FSM.

    Each node state has a reference to a FSMSelector object, which is used by
    the node state to select the child FSM when the agent enters in that state.

    Typically (and the only existent implementation until now) a FSMSelector
    instance is linked to a script function (the SelectFSM function), although
    native implementations can be provided by inheriting from this class and
    overwrite the Select virtual method.

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

class nFSM;
class nEntityObject;

//------------------------------------------------------------------------------
class nFSMSelector : public nRoot
{
public:
    /// Default constructor
    nFSMSelector();

    /// Select and return a reference to a FSM for the given entity
    virtual nFSM* Select( nEntityObject* entity );

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );
};

#endif
