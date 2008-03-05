#ifndef N_NWATCHERDIR_H
#define N_NWATCHERDIR_H
//------------------------------------------------------------------------------
/**
    @file nwatcherdirserver.h
    @class nWatcherDir
    @ingroup NebulaConjurerEditor

    @author Javier Ferrer Gomez

    @brief Watcher for resources directories. Monitorize a list of directories and 
    try to reload resources if changed. The directories list is in an assign. This 
    assign could be specified/changed by scripting.
    Use win32 API, not support for other platforms yet...

    (C) 2005 Conjurer Services, S.A.
*/

#ifdef __WIN32__

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

class nWatcherDirServer : public nRoot
{
public:
    /// constructor
    nWatcherDirServer();    

    /// Initialize the watch for a directory
    void Init();
    /// Trigger that is executed every frame
    void Trigger();

    /// Look if the server is activated
    bool IsEnabled();
    /// Switch on the server
    void Enable();
    /// Switch off the server
    void Disable();
    /// Set the assign name for watch
    void SetAssign(const nString &);
    /// Get the assign name for watch
    nString GetAssign();

protected:    

    struct AsyncMessage 
    {
        // buffer must be DWORD aligned ?
        char buffer[4096];
        nString directory;      // Directory to watch
        OVERLAPPED overlapped;  // Buffer for async operations
        HANDLE event;           // Event for the message
        HANDLE hDir;            // Handle to watch directory
        int bufferLength;       // Length of buffer for send
        bool recursive;         // Set look for directory changes recursively
    };        

    /// destructor
    ~nWatcherDirServer();

    /// Get the filename of the changed file. Not mangled.
    nString GetFileName (FILE_NOTIFY_INFORMATION * infoBuffer, int* action, nString directory);
    /// Launch the watch for all directories
    void LaunchWatcher();
    /// Add a new message to send
    AsyncMessage* NewMessage(const nString & dir);        
        
    bool active;
    nString assignName;
    nArray<AsyncMessage*> messages;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nWatcherDirServer::Disable()
{
    this->active = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nWatcherDirServer::Enable()
{
    this->active = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nWatcherDirServer::IsEnabled()
{
    return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nWatcherDirServer::GetAssign()
{
    return this->assignName;
}

#endif // __WIN32__

#endif // N_NWATCHERDIR_H
//-----------------------------------EOF----------------------------------------
