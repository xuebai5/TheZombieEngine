#ifndef NC_AGENTTRIGGER_H
#define NC_AGENTTRIGGER_H

//------------------------------------------------------------------------------
/**
    @class ncAgentTrigger
    @ingroup NebulaTriggerSystem

    Trigger component specific for agent entities.

    It delegates the handling of incoming events to the agent's perception
    system, which may pass them to the finite state machine, either untouched
    or transformed as other event types.
*/

#include "ntrigger/nctrigger.h"

//------------------------------------------------------------------------------
class ncAgentTrigger : public ncTrigger
{

    NCOMPONENT_DECLARE(ncAgentTrigger,ncTrigger);

public:
    /// Default contructor
    ncAgentTrigger();
    /// Answers to an event, culling by perception field first
    bool HandleEvent( nGameEvent* event );
    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Update the culling radius to the max radius perception
    void UpdateCullingRadius();

#ifndef NGAME
    /// Enable/disable sight on agent entities
    static void SetSightEnabled( bool enable );
    /// Enable/disable hearing on agent entities
    static void SetHearingEnabled( bool enable );
    /// Enable/disable feeling on agent entities
    static void SetFeelingEnabled( bool enable );    

private:
    /// Agent entities can use sight to perceive events?
    static bool sightEnabled;
    /// Agent entities can use hearing to perceive events?
    static bool hearingEnabled;
    /// Agent entities can use feeling to perceive events?
    static bool feelingEnabled;
#endif

};

#endif
