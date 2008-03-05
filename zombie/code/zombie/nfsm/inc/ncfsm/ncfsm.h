#ifndef NC_FSM_H
#define NC_FSM_H

//------------------------------------------------------------------------------
/**
    @class ncFSM
    @ingroup NebulaFSMSystem

    Finite state machine object component.

    This components provides to an entity a probabilitic and hierarchical FSM
    functionallity.

    It handles the state transitioning, listens to events that triggers state
    changes and periodically checks the script condition transitions.

    Listening for events is done from a central triggerer. The events are
    handled in 3 methods, according to its event type:

     - Game events: Look into the current states' transitions for that
       transition that answers to the catched event.
     - ActionDone event: Look into the top state's transitions for that
       transition that answers to an ActionDone event. This event is thrown by
       the action controller when the action says it has finished. It's also
       thrown by the ncFSM itself when a FSM finishes, so at parent level the
       FSM can be seen just as an action.
     - FSMDone event: Pop the top state and tell the central triggerer to
       throw an ActionDone event coming from this agent. This event is thrown by
       the ncFSM itself when it sees that the target state while doing a state
       transition is an end state.

    To listen only to those interesting events (those belonging to the current
    state's transitions) a register/unregister process is done each time a state
    transition takes place.

    For transitions triggered by a script, they are checked each some-time
    interval. This time is really mesured in game ticks and its interval is
    defined in the ncFSMClass, so all agents of the same type share the same
    script condition checking frequency.

    About actions, they're descriptions are got from the states to create and
    apply/attach to the agent the ncFSM belong to. The creation/apply/attach
    process is done through the action manager shared among all ncFSM's.

    Related to children FSM, the ncFSM mantains a stack of FSM-state pairs
    that indicate which are the current active FSM-state pairs, being the
    bottom pair the most parent FSM-state pair, and the top one the most child
    FSM-state pair. FSM's are not created when entering a state, but when
    application starts up, so all FSM's are shared among all agents and
    references to them are used instead. The same applies to states and
    transitions (but no to actions).

    @see nFSM to know more about the agent independent FSM part.
    @see ncFSMClass to know more about the agent type dependent FSM part.
*/

#include "entity/nentity.h"
#include "util/narray.h"
#include "ntrigger/ngameevent.h"

class ncFSMClass;
class nFSM;
class nState;
class nTransition;

//------------------------------------------------------------------------------
class ncFSM : public nComponentObject
{

    NCOMPONENT_DECLARE(ncFSM,nComponentObject);

public:
    /// Default constructor
    ncFSM();
    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Set the FSM out of any state
    void Reset();
    /// Set initial state.
    void Init();
    /// Each some-time interval defined in the component class check for scripted transitions.
    void Update();
#ifndef NGAME
    /// Display the states stack
    void Draw( const vector4& color, const vector2& pos ) const;
#endif

    /// Handle all events (but Done) belonging to some of the current states' transitions.
    void OnTransitionEvent(int);
    /// Handle the event relating to an action finishing (ActionDone/ActionFail).
    void OnActionEvent(int);
    /// Handle the event thrown because of entering an end state (FSMDone).
    void OnFSMDoneEvent ();

    /// Same as Init, but with a more intuitive name to be used by designers
    void StartBehavior ();
    /// Replace the current FSM by another one and start executing it.
    void SetBehavior(const char*);
    /// Get the time that has elapsed since the fsm enters in current state
    float GetStateTimeElapsed ();

#ifndef __NEBULA_NO_LOG__
    /// Get the prefix added to all log messages
    const char* GetLogPrefix( int indentOffset = 0 ) const;
#endif

#ifndef NGAME
    NSIGNAL_DECLARE('EOTE', void, OnTransitionEvent, 3, (nEntityObject*, int, bool*), 0, ());
#endif

private:
    /// Make the entity this component belongs to enter in the FSM's initial state
    void EnterFSM( nFSM* fsm );
    /// Make the entity this component belongs to enter in the given state
    void EnterState( nState* state );
    /// Make the entity this component belongs to exit from its most child active state
    void ExitTopState();
    /// Make the FSM follow a transition, if it selects a target.
    bool FollowTransition( nTransition* transition, int stackLevel );
    /// Process the last action event received since the last update process
    bool ProcessLastActionEvent();
    /// Process the non action events received since the last update process
    bool ProcessTransitionEvents();
    /// Evaluate transition scripts and transitionate if anyone triggers
    bool ProcessTransitionScripts();
    /// Make the FSM transitionate if some transition matches the given action event
    bool ApplyActionEvent( nGameEvent::Type eventType );
    /// Make the FSM transitionate if some transition matches the given non action event
    bool ApplyTransitionEvent( nGameEvent::Type eventType );

    /// Describe one of the current fsm-state pair
    struct ActiveState
    {
        /// Constructor
        ActiveState( nState* state = NULL ) : state(state)
        {}

        // nFSM* fsm; Useless since state transitions have references to target states
        /// Current state for this level in the FSM hierarchy
        nState* state;
    };

    /// Current states as a stack, where bottom = most parent state, top = most child state
    nArray<ActiveState> statesStack;
    /// How many ticks are left to check if some of the scripted conditions evaluate to true
    int ticksToCheckConditions;
    /// Time when the state is executed
    float initTime;
    /// Incoming events since the last update process (excluding action events)
    nArray< nGameEvent::Type > incomingEvents;
    /// Result of last action performed by the agent
    nGameEvent::Type lastActionEvent;
};

//------------------------------------------------------------------------------
#endif // NC_FSM_H
