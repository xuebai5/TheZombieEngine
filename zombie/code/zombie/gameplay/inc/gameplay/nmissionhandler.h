#ifndef N_MISSIONHANDLER_H
#define N_MISSIONHANDLER_H

//------------------------------------------------------------------------------
/**
    @class nMissionHandler
    @ingroup Gameplay

    Manager of all the objectives for a player in a single level

    This class contains all the objectives for a single level, even if they
    haven't been assigned to the player yet. An objective can be in one of the
    following states:

     - Hidden: The objective hasn't been assigned to the player yet
     - Assigned: The objective has been assigned to the player, but it still
                 must be completed
     - Completed: The objective has been assigned and completed
     - Removed: The objective has been assigned or completed but it's no longer
                displayed to the player

    Each objective consists of:

     - Name: how the objective is referenced from scripting
     - Description: the text shown to the player to describe the objective
     - State: into which state the objective is currently in

    Whenever the state of an objective changes the ObjectiveStateChanged signal
    is thrown, indicating which objective has changed.

    The objectives list is automatically loaded and saved by this class by
    binding itself to the level manager's level loaded and unloaded signals
    respectively.

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"

class nstream;

//------------------------------------------------------------------------------
class nMissionHandler : public nRoot
{
public:
    // Possible states of each objective
    enum ObjectiveState
    {
        HIDDEN,
        ASSIGNED,
        COMPLETED,
        REMOVED
    };

    /// Default constructor
    nMissionHandler();
    /// Destructor
    ~nMissionHandler();
    /// Do any required initialization
    void Init();

    /// Return singleton mission handler
    static nMissionHandler* Instance();

    /// Delete all the objectives
    void Clear();

    /// Get an objective state by index
    ObjectiveState GetObjectiveStateByIndex( int index ) const;
    /// Set the state of an objective by index
    void SetObjectiveStateByIndex( int index, ObjectiveState state );

    /// Send a state change to all clients
    void SendObjectiveStateChange( int index, ObjectiveState state );
    /// Method used to update an objective state change notified from the server side
    void ReceiveObjectiveStateChange( nstream* data );

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

    /// Append a new objective to the end of the objectives list
    void AppendObjective(const char*, const char*);
    /// Get the total number of objectives stored
    int GetObjectivesNumber () const;
    /// Get an objective name by index
    const char* GetObjectiveNameByIndex(int) const;
    /// Get an objective description by index
    const char* GetObjectiveDescriptionByIndex(int) const;
    /// Set an objective name by index
    void SetObjectiveNameByIndex(int, const char*);
    /// Set an objective description by index
    void SetObjectiveDescriptionByIndex(int, const char*);
    /// Delete an objective by index
    void DeleteObjectiveByIndex(int);

    /// Set the state of an objective by name
    void SetObjectiveState(const char*, const char*);

    /// Get an objective state string by index
    const char *  GetObjectiveStateStringByIndex(int) const;
    /// Set an objective state string by index
    void SetObjectiveStateStringByIndex(int, const char*);

    NSIGNAL_DECLARE( 'EOSC', void, ObjectiveStateChanged, 1, (int), 0, () );

private:
    /// Code returned by GetObjectiveIndex to indicate that an objective hasn't been found
    static const int NOT_FOUND = -1;
    /// Get the index of the objective associated to the given name, or NOT_FOUND if there isn't any
    int FindObjectiveIndex( const char* name ) const;
    /// Load the objectives list for the current level, replacing the old ones
    void OnLevelLoaded();
    /// Save the objectives list into the current level
    void OnLevelSaved();
    /// Get the objective state id assigned to a state label
    ObjectiveState StateLabelToId( const char* stateLabel ) const;
    /// Get the objective state label assigned to a state id
    const char* StateIdToLabel( ObjectiveState state ) const;

    /// Objective data
    struct Objective
    {
        /// Name of the objective
        nString name;
        /// Description of the objective
        nString description;
        /// State of the objective
        ObjectiveState state;

        /// Default constructor
        Objective()
        {}
        /// Constructor with full args
        Objective(const char* name, const char* description, ObjectiveState state)
            : name(name), description(description), state(state)
        {}
    };

    /// Objectives
    nArray<Objective> objectives;

    /// Singleton instance
    static nMissionHandler* instance;
};

//------------------------------------------------------------------------------
/**
    Return singleton mission handler
*/
inline
nMissionHandler*
nMissionHandler::Instance()
{
    n_assert( nMissionHandler::instance );
    return nMissionHandler::instance;
}

//-----------------------------------------------------------------------------
/**
    Get the total number of objectives stored
*/
inline
int
nMissionHandler::GetObjectivesNumber() const
{
    return this->objectives.Size();
}

//-----------------------------------------------------------------------------
/**
    Get an objective name by index
*/
inline
const char*
nMissionHandler::GetObjectiveNameByIndex( int index ) const
{
    n_assert( index >= 0 && index < this->objectives.Size() );
    return this->objectives[index].name.Get();
}

//-----------------------------------------------------------------------------
/**
    Get an objective description by index
*/
inline
const char*
nMissionHandler::GetObjectiveDescriptionByIndex( int index ) const
{
    n_assert( index >= 0 && index < this->objectives.Size() );
    return this->objectives[index].description.Get();
}

//-----------------------------------------------------------------------------
/**
    Get an objective state by index
*/
inline
nMissionHandler::ObjectiveState
nMissionHandler::GetObjectiveStateByIndex( int index ) const
{
    n_assert( index >= 0 && index < this->objectives.Size() );
    return this->objectives[index].state;
}

//------------------------------------------------------------------------------
#endif // N_MISSIONHANDLER_H
