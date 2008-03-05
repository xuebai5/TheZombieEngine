#ifndef N_FSMSERVER_H
#define N_FSMSERVER_H

//------------------------------------------------------------------------------
/**
    @class nFSMServer
    @ingroup NebulaFSMSystem

    Finite state machine server.

    FSM server functions:

     - Creation/Deletion of FSMs
     - Persistance of FSMs
     - Load FSMs on demand
     - Editor helper functions
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

class nFSM;
class nState;
class nCondition;
class nEventCondition;
class nScriptCondition;
class nFSMSelector;
class nActionDesc;
class nTransition;

//------------------------------------------------------------------------------
class nFSMServer : public nRoot
{
public:
    /// Default constructor
    nFSMServer();
    /// Destructor
    virtual ~nFSMServer();

    /// Return singleton FSM server
    static nFSMServer* Instance();

    /// Get the directory where all the FSMs, conditions, etc. are stored to/loaded from
    const char* GetStoragePath () const;
    /// Set the directory where all the FSMs, conditions, etc. are stored to/loaded from
    void SetStoragePath(const char*);
    /// Load all the FSMs, conditions, etc.
    bool LoadAll () const;
    /// Return a FSM, loading it if needed
    nFSM* GetFSM(const char*) const;
    /// Return an event condition, loading it if needed
    nEventCondition* GetEventCondition(const char*) const;
    /// Return an script condition, loading it if needed
    nScriptCondition* GetScriptCondition(const char*) const;
    /// Return an FSM selector, loading it if needed
    nFSMSelector* GetFSMSelector(const char*) const;
    /// Save a FSM
    bool SaveFSM(nFSM*) const;
    /// Erase a FSM from disk
    void EraseFSM(nFSM*) const;
    /// Return a state, creating an empty one if needed
    nState* GetState(const char*, const char*, int) const;
    /// Return the FSM which a state belongs to
    nFSM* FindFSMOfState(nState*) const;
    /// Return the state which a transition belongs to
    nState* FindStateOfTransition(nTransition*) const;
    /// Return a transition of a state
    nTransition* FindTransition(nState*, const char*) const;
    /// Return the local condition of a transition
    nCondition* FindLocalCondition(nTransition*, const char*) const;
    /// Return the behavioural action of a state
    nActionDesc* FindBehaviouralAction(nState*, const char*) const;

#ifndef NGAME
    /// Set if events are enqueued or processed instantaneously
    void SetEnqueueEvents( bool enable );
    /// Tell if events are enqueued or processed instantaneously
    bool GetEnqueueEvents() const;
#endif

#ifndef __NEBULA_NO_LOG__
    /// Log used for errors
    static const int ErrorsLog;
    /// Log used for received events
    static const int EventsLog;
    /// Log used for followed transitions
    static const int TransitionsLog;
    /// Log used for visited states
    static const int StatesLog;
#endif

private:
    /// Run a script, doing some success checking
    bool RunScript( const char* filename ) const;
    /// Run a script command, doing some success checking
    bool RunCommand( const char* cmd ) const;
    /// Build a NOH path from subpath and name (path = obj_root/subpath/name)
    void MakeObjPath( const char* subpath, const char* name, nString& resultingPath ) const;
    /// Build a file path from subpath and name (path = file_root/subpath/name.extension)
    void MakeFilePath( const char* subpath, const char* name, nString& resultingPath, const char* extension = NULL ) const;
    /// Return an object, loading it if needed
    nRoot* GetObject( const char* subpath, const char* name, bool hasScript = false ) const;
    /// Return an object, creating it if needed
    nRoot* GetObject2( const char* subpath, const char* name ) const;
    /// Load all objects from the given relative path to root
    bool LoadObjects( const char* subpath, bool haveScript = false ) const;
    /// Save an object into the given relative path to root
    bool SaveObject( nRoot* obj, const char* subpath ) const;
    /// Save an object into the given relative path to root
    void EraseFile( const char* subpath, const char* name, const char* extension = NULL ) const;

    /// Singleton instance
    static nFSMServer* instance;
    /// Root path in disk where all objects are stored
    nString rootFilePath;
    /// Root path in NOH where all objects are placed
    static const char* rootObjPath;
    /// Relative path to root where all FSMs are placed
    static const char* fsmsSubpath;
    /// Relative path to root where all event conditions are placed
    static const char* eventCondsSubpath;
    /// Relative path to root where all script conditions are placed
    static const char* scriptCondsSubpath;
    /// Relative path to root where all FSM selectors are placed
    static const char* fsmSelectorsSubpath;
    /// Relative path to root where all action scripts are placed
    static const char* actionScriptsSubpath;
    /// Relative path to a state where all its transitions are placed
    static const char* transitionsSubpath;
    /// Relative path to a transition where its local condition is placed
    static const char* localCondsSubpath;
    /// Relative path to a state where its behavioural action is placed
    static const char* behactionsSubpath;

#ifndef NGAME
    /// Events are enqueued or processed instantaneously?
    bool enqueueEvents;
#endif
};

//------------------------------------------------------------------------------
inline
nFSMServer* nFSMServer::Instance()
{
    n_assert( nFSMServer::instance );
    return nFSMServer::instance;
}

#endif
