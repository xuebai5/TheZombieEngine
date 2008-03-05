#ifndef N_AITESTER_H
#define N_AITESTER_H

//------------------------------------------------------------------------------
/**
    @class nAITester
    @ingroup RnsStates

    Singleton used to manage AI updating outside of the conjurer/renaissance states.

    This helps to enable/disable it globally, as well as avoids having to port
    the whole AI update process each time an old state is removed or this
    functionallity is desired in another state.

    Also, since the AI updating is decoupled from the states, this test class
    can later be replaced for the release one without needing to worry about
    the states.

    @todo Create a mechanism to generically store/restore entity states,
          doing the store process only when the entity changes
          (idea: listen to dirty flag changed signal).
    @todo Update only those entities that are in unlocked layers
    @todo Remove from perception and interaction entities that are in hidden layers

    (C) 2005 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ntrigger/nflags.h"
#include "ntrigger/ngameevent.h"

class nEntityObject;

//------------------------------------------------------------------------------
class nAITester : public nRoot
{
#ifndef NGAME
public:
    /// Deafult constructor
    nAITester();
    /// Destructor
    ~nAITester();

    /// Return singleton AI tester
    static nAITester* Instance();

    /// Tell how many time per frame an AI update cycle must be done
    int GetAIUpdatesPerFrame();

    /// Keep updating all the AI related entities at the specified update frequency
    void Trigger();

    /// Tell if the FSM is in pause mode or may be updated
    bool IsFSMPaused() const;

    /// Enable gameplay entities for updating, doing any needed initialization
    void EnableGameplayUpdating ();
    /// Disable gameplay entities from updating, doing any needed clean up/state restoring
    void DisableGameplayUpdating ();
    /// Tell if the gameplay updating is currently enabled
    bool IsGameplayUpdatingEnabled () const;
    /// Enable sound sources, doing any needed initialization
    void EnableSoundSources ();
    /// Disable sound sources, doing any needed clean up/state restoring
    void DisableSoundSources ();
    /// Tell if the sound sources are currently enabled
    bool AreSoundSourcesEnabled () const;
    /// Get the AI update frequency
    int GetUpdateFrequency () const;
    /// Set the AI update frequency
    void SetUpdateFrequency(int);
    /// Enable gameplay debugging mode
    void EnableGameplayDebugging ();
    /// Disable gameplay debugging mode
    void DisableGameplayDebugging ();
    /// Tell if the gameplay debugging is enabled
    bool IsGameplayDebuggingEnabled () const;
    /// If there's any pending transition event, make the owner entity proccess it now
    void ApplyPendingEvent ();
    /// Discard the pending transition event
    void DiscardPendingEvent ();
    /// Get the current pending transition event and its owner entity
    void GetPendingEvent(nEntityObjectId&, nString&);

    /// @name Signals interface
    //@{
    NSIGNAL_DECLARE('EUPE', void, GameplayUpdatingEnabled, 0, (), 0, ());
    NSIGNAL_DECLARE('EUPD', void, GameplayUpdatingDisabled, 0, (), 0, ());
    NSIGNAL_DECLARE('ESSE', void, SoundSourcesEnabled, 0, (), 0, ());
    NSIGNAL_DECLARE('ESSD', void, SoundSourcesDisabled, 0, (), 0, ());
    NSIGNAL_DECLARE('EPEC', void, PendingEventChanged, 0, (), 0, ());
    //@}

private:
    /// Store a copy of that data that may change due to AI
    void StoreGameState();
    /// Restore the last stored game state
    void RestoreGameState();
    /// Prepare sound sources to be triggered for 'on enter'/'on exit' area events
    void InitSoundSources();
    /// Restore sound sources to the state previous to the init call
    void RestoreSoundSources();
    /// Pause the FSM transitions if in gameplay debug mode
    void OnTransitionEvent(nEntityObject* entity, int event, bool* acceptEvent);

    /// Singleton instance
    static nAITester* instance;

    /// Flags of which entities to update
    enum UpatingGroup
    {
        GAMEPLAY,       // update all entities with gameplay component
        SOUND_SOURCES,  // trigger sound sources on entering/exiting their sound radius

        // Debug options, not really entities to update
        GAMEPLAY_DEBUG,         // gameplay debugging mode
        DO_NEXT_GAMEPLAY_STEP,  // allow next transition event to be applied (used to apply a pending event)

        // Must be always the last
        UPDATING_FLAGS_NUMBER
    };
    nFlags updatingFlags;

    /// Ticks since last AI update
    int ticks;
    /// AI update frequency
    int ticksPerAIUpdate;
    /// Struct to hold a transition event pending of being applied to an entity
    struct PendingEvent
    {
        /// constructor
        PendingEvent()
        {
        }
        /// destructor
        ~PendingEvent()
        {
        }
        /// Transition event catched when pausing the updating in gameplay debug mode
        nGameEvent::Type event;
        /// Entity that should receive the pending transition event
        nEntityObject* entity;
    };
    /// Queue of transition events pending to be applied on entities
    nArray<PendingEvent> pendingEvents;
#else
public:
    // default constructor
    nAITester() {}
#endif // !NGAME
};

//------------------------------------------------------------------------------
#endif // N_AITESTER_H
