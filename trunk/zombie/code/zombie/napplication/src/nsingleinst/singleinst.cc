#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nsingleinst.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nsingleinst/singleinst.h"

//------------------------------------------------------------------------------
/**
*/
nSingleInstance::nSingleInstance(LPCSTR ApplicationName) :
    hMemMappedFile(0),
    bIsFirstInstance(true)
{
    char UniqueName[512];

    GetMutexName(UniqueName, ApplicationName);

    // create mutex to protect shared memory access
    HANDLE hMutex = CreateMutex(0, FALSE, "UniqueAppTextMutex");
    WaitForSingleObject(hMutex,INFINITE);

    // create shared memory object
    hMemMappedFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                       0,
                                       PAGE_READWRITE,
                                       0,
                                       sizeof(DWORD),
                                       UniqueName);
    if (hMemMappedFile != 0)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            bIsFirstInstance = false;
            ActivatePreviousInstance();
        }
        else
        {
            SetFirstInstance();
        }
    }

    // exit critical section
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

//------------------------------------------------------------------------------
/**
*/
nSingleInstance::~nSingleInstance()
{
    if (this->hMemMappedFile)
    {
        CloseHandle(hMemMappedFile);
    }
}

//------------------------------------------------------------------------------
/**
    The first application writes it's PID into the
    shared memory area.
*/
void
nSingleInstance::SetFirstInstance()
{
    PDWORD pSharedMem = (PDWORD) MapViewOfFile(this->hMemMappedFile,
                                               FILE_MAP_WRITE,
                                               0,
                                               0,
                                               sizeof(DWORD));
    if (pSharedMem)
    {
        *pSharedMem = GetCurrentProcessId();
        UnmapViewOfFile(pSharedMem);
    }
}

//------------------------------------------------------------------------------
/**
    The second instance gets the PID of the first instance.
    Then it finds the top app window based on the PID.
    The previous application is then activated.
*/
void
nSingleInstance::ActivatePreviousInstance()
{
    PDWORD pSharedMem = (PDWORD) MapViewOfFile(this->hMemMappedFile,
                                               FILE_MAP_READ,
                                               0,
                                               0,
                                               sizeof(DWORD));
    if (pSharedMem)
    {
        DWORD dwPID = *pSharedMem;
        HWND hWnd = FindProcessWindow(dwPID);
        if (!hWnd)
        {
            Sleep(1000);
            hWnd = FindProcessWindow(dwPID);
        }
        if (hWnd)
        {
            SetForegroundWindow(hWnd);
        }
        UnmapViewOfFile(pSharedMem);
    }
}

//------------------------------------------------------------------------------
/**
    @param The process ID
    @return HWND of top app window with the argument PID
*/
HWND
nSingleInstance::FindProcessWindow(DWORD Pid)
{
    FindWindowStruct str(Pid);
    EnumWindows(EnumWindowsProc, (LPARAM) &str);
    return str.hwndRet;
}

//------------------------------------------------------------------------------
/**
    callback function (called by EnumWindows)
    used to find the mapping between PID and application top app window.
*/
BOOL CALLBACK
nSingleInstance::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    FindWindowStruct *pParam = (FindWindowStruct *) lParam;
    DWORD dwPID;

    GetWindowThreadProcessId(hwnd, &dwPID);
    if (dwPID == pParam->dwPID)
    {
        pParam->hwndRet = hwnd;
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
/**
*/
void
nSingleInstance::GetMutexName(LPSTR buf, LPCSTR ApplicationName)
{
    strcpy(buf,"Application-Unique-Check-Is-Present");
    if (strcmp(ApplicationName,"") != 0)
    {
        strcat(buf, ApplicationName);
    }
    else
    {
        char path[UCHAR_MAX];
        strncpy(path, __argv[0], sizeof(buf) - 1);
        strcat(buf, path+3);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nSingleInstance::IsFirstInstance()
{
    return this->bIsFirstInstance;
}
