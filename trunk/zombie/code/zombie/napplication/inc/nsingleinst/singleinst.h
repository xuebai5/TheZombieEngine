#ifndef N_SINGLEINST_H
#define N_SINGLEINST_H
//------------------------------------------------------------------------------
/**
    @class nSingleInstance
    @ingroup Application
    @brief makes sure that this application has only one running instance.

    @author MA Garcias <ma.garcias@yahoo.es>

    Ported from original singleinst module by Michael Moser
    http://www.michaelmoser.org/instance.htm

    (C) 2005 Conjurer Services, S.A.
*/
#ifdef __WIN32__

//------------------------------------------------------------------------------
class nSingleInstance
{
public:
    /// constructor
    nSingleInstance(LPCSTR ApplicationName = "");
    /// destructor
    ~nSingleInstance();
    /// return whether this is the first instance
    bool IsFirstInstance();

protected:
    /// activate previous instance
    void ActivatePreviousInstance();
    /// mark current as first application instance
    void SetFirstInstance();
    /// find window by its process id
    HWND FindProcessWindow(DWORD Pid);
    /// get mutex name
    void GetMutexName(LPSTR buf, LPCSTR ApplicationName);

    static BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam);

    class FindWindowStruct
    {
    public:
        /// constructor
        FindWindowStruct(DWORD Pid) : dwPID(Pid), hwndRet(0)
        {
        }
        DWORD dwPID;
        HWND  hwndRet;
    };

    HANDLE hMemMappedFile;
    bool   bIsFirstInstance;
};

#endif __WIN32__

//------------------------------------------------------------------------------
#endif // N_SINGLEINST_H
