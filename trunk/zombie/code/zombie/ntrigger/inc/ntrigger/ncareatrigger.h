#ifndef NC_AREATRIGGER_H
#define NC_AREATRIGGER_H

//------------------------------------------------------------------------------
/**
    @class ncAreaTrigger
    @ingroup NebulaTriggerSystem

    Trigger that fires for area events.

    Trigger that transforms incoming events to area related events and gives
    them to another trigger to handle them. The trigger uses a shape to throw
    its internal events, which are the 'on enter' and 'on exit' events. Since
    the position of incoming events is used to check if they fall inside or
    outside of the shape, only area events must be given to this trigger.

    Has a reference to another trigger used to handle the internally generated
    events ('on enter' and 'on exit').

    An entity with this component must also have the ncTriggerShape component,
    which defines the trigger activation area.

    (C) 2006 Conjurer Services, S.A.
*/

#include "ntrigger/nctrigger.h"

//------------------------------------------------------------------------------
class ncAreaTrigger : public ncTrigger
{

    NCOMPONENT_DECLARE(ncAreaTrigger,ncTrigger);

public:
    /// Default constructor
    ncAreaTrigger();
    /// Destructor
    ~ncAreaTrigger();
    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Answers to an event, first doing additional culling if necessary
    bool HandleEvent( nGameEvent* event );

    /// Set the trigger used to handle internal events (kept for backwards compatibility)
    void SetInternalTriggerById(nEntityObjectId);
    /// Lose track of any entity inside the trigger shape
    void ResetTrigger();

    /// Set same space flag
    void SetCheckSameSpace(bool);
    /// Get same space flag
    bool GetCheckSameSpace();
    /// Set same cell flag
    void SetCheckSameCell(bool);
    /// Get same cell flag
    bool GetCheckSameCell();

    NSIGNAL_DECLARE( 'EENA', void, OnEnterArea, 1, (nGameEvent*), 0, ());
    NSIGNAL_DECLARE( 'EEXA', void, OnExitArea, 1, (nGameEvent*), 0, ());

protected:
    /// Which entities are currently inside the trigger shape
    nArray<nEntityObjectId> entitiesAlreadyInside;

    /// Entities have to be in same space to be inside area
    bool checkSameSpace;

    /// Entities have to be in same space and cell to be inside area
    bool checkSameCell;

};

//-----------------------------------------------------------------------------
#endif // NC_AREATRIGGER_H
