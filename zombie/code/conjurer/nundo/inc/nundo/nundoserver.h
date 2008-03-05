#ifndef N_UNDO_SERVER_H
#define N_UNDO_SERVER_H
//------------------------------------------------------------------------------
/**
    @file nundoserver.h
    @class nUndoServer
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class that manages the application undo system

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "signals/nsignalnative.h"
#include "nundo/undocmd.h"
#include "util/nlist.h"

//------------------------------------------------------------------------------

extern const char* undoServerPath;

//------------------------------------------------------------------------------
class nUndoServer: public nRoot
{
public:

    /// Constructor
    nUndoServer();
    /// Destructor
    ~nUndoServer();

    /// Nebula persistency
    virtual bool SaveCmds(nPersistServer * ps);

    /// Get singleton instance
    static nUndoServer* Instance( void );

    /// Dispatch a new command
    bool NewCommand( UndoCmd* cmd );

    /// Check if it's possible to undo
    bool CanUndo( void );

    /// Check if it's possible to redo
    bool CanRedo( void );

    /// Undo last command
    bool Undo( void );
    
    /// Redo last undone command
    bool Redo( void );

    // Get a sequential name for undo objects
    nString GetSequentialName( void );

    /// Get memory used to store the undo objects
    int GetMemoryUsed();

    /// Get memory limit to store the undo objects
    int GetMemoryLimit();

    /// Set memory limit to store the undo objects
    void SetMemoryLimit(int);

    /// Delete all undo commands
    void Clean();

    /// @name Signals interface
    //@{
    ///
    NSIGNAL_DECLARE('JENM', void, EntityModified, 0, (), 0, ());
    //@}

protected:

private:

    /// Stack of commands
    nList commandList;

    /// currentCmd is a pointer to the last done command in the list (or NULL)
    UndoCmd* currentCmd;

    /// Remove a command from the list
    void RemoveCommand( UndoCmd* cmd );

    /// Number of commands in the list
    int cmdCount;

    /// Limit to the total size of undo/redo list
    int sizeLimit;

    /// Total estimated size of the list
    int totalSize;

    /// Integer for generating sequential names for nRoot undo object
    int seqNameInt;

};
//------------------------------------------------------------------------------

#endif

