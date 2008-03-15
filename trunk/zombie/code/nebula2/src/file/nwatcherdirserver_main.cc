//-----------------------------------------------------------------------------
//  nwatcherdirserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "file/nwatcherdirserver.h"
#include "kernel/nfileserver2.h"
#include <tchar.h>
#include <winbase.h>
#include "resource/nresourceserver.h"
#include "kernel/nlogclass.h"

#ifdef __WIN32__

nNebulaScriptClass(nWatcherDirServer, "nroot");


//------------------------------------------------------------------------------
/**
*/
nWatcherDirServer::nWatcherDirServer():
    active (true),
    assignName (nString("dirwatches"))
{
    //empty
}
//------------------------------------------------------------------------------
/**
*/
nWatcherDirServer::~nWatcherDirServer()
{
    for (int i=0; i < this->messages.Size(); i++)
    {
        n_delete(this->messages[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nWatcherDirServer::LaunchWatcher()
{
    DWORD bytesReturned;

    for (int i=0; i < this->messages.Size(); i++)
    {
        AsyncMessage* msg = this->messages[i];
        if (!ReadDirectoryChangesW(msg->hDir, msg->buffer, msg->bufferLength, msg->recursive,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_CREATION |
            FILE_NOTIFY_CHANGE_SIZE,
            &bytesReturned,
            &msg->overlapped,
            0 ))
        {
            int error = GetLastError();
            if ( error != 0)
            {
                n_printf("Cannot start a watch in %s. Disabling the watcher. (Error: %d)", msg->directory.Get(), error);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
nWatcherDirServer::AsyncMessage*
nWatcherDirServer::NewMessage(const nString & dir)
{    
    AsyncMessage* msg = n_new(AsyncMessage);
    msg->directory = dir;
    msg->recursive = true;
    msg->event = INVALID_HANDLE_VALUE;
    msg->hDir = INVALID_HANDLE_VALUE;
    msg->bufferLength = sizeof(char [4096]); // buffer length for result

    // Initialize buffers
    memset(msg->buffer, 0, msg->bufferLength);
    memset(&msg->overlapped, 0, sizeof(OVERLAPPED));

    return msg;
}


//------------------------------------------------------------------------------
/**
*/
void 
nWatcherDirServer::Init()
{       
    nArray<nString> dirs;

    nFileServer2::Instance()->GetAssignGroup(this->assignName.Get(), dirs);      

    for (int i=0; i < dirs.Size(); i++)
    {
        this->messages.Append(this->NewMessage(dirs[i]));
        
        // Mangle the path
        nString magledDir = nFileServer2::Instance()->ManglePath(this->messages[i]->directory);

        // Get a handle to the directory to watch
        this->messages[i]->hDir = CreateFile(magledDir.Get(), 
            FILE_LIST_DIRECTORY, 
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL);

        // create event for asynchronous operation
        this->messages[i]->event = CreateEvent(NULL,FALSE,FALSE,NULL);
        n_assert( this->messages[i]->event );
        this->messages[i]->overlapped.hEvent = this->messages[i]->event;
    }  

    // Launch watcher for all messages
    this->LaunchWatcher();    
}


//------------------------------------------------------------------------------
/**
*/
void
nWatcherDirServer::Trigger()
{
    if (this->active)
    {        
        if (this->messages.Size() == 0)
        {
            this->Init();
        }
        for (int i=0; i < this->messages.Size(); i++)
        {

            AsyncMessage* msg = this->messages[i];
            DWORD bytes = 0;
            LPOVERLAPPED lpOverlapped = &msg->overlapped;
            
            if (GetOverlappedResult(msg->hDir, lpOverlapped, &bytes, FALSE))
            {
                if (bytes > 0)
                {
                    nString rscName;                                 
                    int action = 0;            
                    int nextIndex = 1; 
                    FILE_NOTIFY_INFORMATION * nextBuffer = (FILE_NOTIFY_INFORMATION *) msg->buffer;

                    // Search in all structures returned by GetOverlappedResult
                    while (nextIndex > 0)
                    {
                        rscName = this->GetFileName(nextBuffer, &action, msg->directory);
                        rscName.ConvertBackslashes();
                        nextIndex = nextBuffer->NextEntryOffset;
                        nextBuffer = (FILE_NOTIFY_INFORMATION *) (((char *) nextBuffer) + nextBuffer->NextEntryOffset);

                        // Reload the resource for modified or renamed files
                        static const nString dirShadersMaterials = "shaders:/materials/";
                        switch ( action )
                        {            
                        case FILE_ACTION_MODIFIED:
                            // FIXME: this can be fixed once we refactor dirwatcher
                            if (rscName.IndexOf(dirShadersMaterials, 0) == -1)
                            {
                                NLOG(resource, (NLOGUSER | 0, "Directory watcher: Modified file %s", rscName.Get()));
                                nResourceServer::Instance()->ReloadResourceByFileName(rscName.Get());
                            }
                            break;
                        // This case is for programs like photoshop that works in a temp file and when
                        // the work is done renamed to original file
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            // FIXME: this can be fixed once we refactor dirwatcher
                            if (rscName.IndexOf(dirShadersMaterials, 0) == -1)
                            {
                                NLOG(resource, (NLOGUSER | 0, "Directory watcher: Renamed file %s", rscName.Get()));
                                nResourceServer::Instance()->ReloadResourceByFileName(rscName.Get());
                            }
                            break;
                        default:
                            //NLOG(resource, (NLOGUSER | 0, "Directory watcher: Ignored file %s action=%d", rscName.Get(), action));
                            break;
                        }                        
                    }
                    // Restart the watcher
                    this->LaunchWatcher();                                               
                }                
            }
            else
            {
                if (GetLastError() != ERROR_IO_INCOMPLETE)
                {
                    n_printf("Error getting the overlapped result. Option will be disable. (Error: %d)", GetLastError());
                    this->Disable();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
nString
nWatcherDirServer::GetFileName (FILE_NOTIFY_INFORMATION * infoBuffer, int* action, nString directory)
{
    char mbName[160];    
    int nameLength;
    nString stringName = directory;

    // Get action and name length
    *action = infoBuffer->Action;
    nameLength = infoBuffer->FileNameLength / 2;

    // convert from wide character to multibyte character string
    wcstombs(mbName, infoBuffer->FileName, min(sizeof(mbName)-1,nameLength));
    mbName[min(sizeof(mbName)-1,nameLength)] = 0;

    stringName.Append(mbName);
    return stringName;
}

//------------------------------------------------------------------------------
/**
*/
void
nWatcherDirServer::SetAssign(const nString & assignName)
{
    // Get new assign
    this->assignName = assignName;

    // Clear previous directories
    for (int i=0; i < this->messages.Size(); i++)
    {
        n_delete(this->messages[i]);        
    }
    this->messages.Reset();
}
#endif
//-----------------------------------EOF----------------------------------------
