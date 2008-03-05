//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/ndirectory.h"
#include "kernel/ncrc.h"
#include "util/nstring.h"
#ifdef __WIN32__
#include <direct.h>
#include <shellapi.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#if defined(__MACOSX__)
#include <Carbon/carbon.h>
#endif

nNebulaScriptClass(nFileServer2, "nroot");

nFileServer2* nFileServer2::Singleton = 0;

//------------------------------------------------------------------------------
#ifndef NGAME
const char * FILE_SEPARATOR_CHARACTER = "/";
#endif

//------------------------------------------------------------------------------
/**

    history:
     - 30-Jan-2002   peter    created
*/
nFileServer2::nFileServer2() :
    bytesRead(0),
    bytesWritten(0),
    numSeeks(0)
{
    n_assert(0 == Singleton);
    Singleton = this;

    // Initialize assign repository if not already exists. The latter may happen
    // if nKernelServer::ReplaceFileServer() was used to set another
    // standard file server.
    nRoot* assignRoot = kernelServer->Lookup("/sys/share/assigns");
    if (0 == assignRoot)
    {
        this->InitAssigns();
    }
    else
    {
        this->assignDir = assignRoot;
    }
}

//------------------------------------------------------------------------------
/**

    history:
     - 30-Jan-2002   peter    created
*/
nFileServer2::~nFileServer2()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    creates new or modifies existing assign under /sys/share/assigns

    @param assignName      the name of the assign
    @param pathName        the path to which the assign links

    history:
     - 30-Jan-2002   peter    created
*/
bool
nFileServer2::SetAssign(const char * assignName, const nString& pathName)
{
    // make sure trailing slash exists
    nString pathString = pathName;
    pathString.StripTrailingSlash();
    pathString.Append("/");

    // ex. das Assign schon?
    kernelServer->PushCwd(this->assignDir.get());
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (env && env->GetType() != nArg::String)
    {
        env->Release();
        env = 0;
    } 

    // create the nenv object
    if (!env) 
    {
        env = (nEnv *) kernelServer->New("nenv", assignName);
        n_assert(env);
    }

    env->SetS(pathString.Get());
    kernelServer->PopCwd();
    return true;
}


//------------------------------------------------------------------------------
/**
    creates new or modifies existing assign under /sys/share/assigns

    @param assignName      the name of the assign
    @param paths           array with the paths of the assign group

*/
bool
nFileServer2::SetAssignGroup(const char * assignName, const nArray<nString> & paths)
{
    n_assert(assignName);

    if (0 == paths.Size())
    {
        this->SetAssign(assignName, 0);
    }
    else if (1 == paths.Size())
    {
        this->SetAssign(assignName, paths[0]);
    }
    else if (paths.Size() > 1)
    {
        // create a nArg list (allocate here and let nArg manage from now on)
        nArg * args = n_new_array(nArg , paths.Size() );
        for(int i = 0;i < paths.Size();i++)
        {
            // make sure trailing slash exists
            nString pathString = paths[i];
            pathString.StripTrailingSlash();
            pathString.Append("/");
            args[i].SetS(pathString.Get());
        }
            
        // get the nenv object and delete it if exists
        kernelServer->PushCwd(this->assignDir.get());
        nEnv *env = (nEnv *) this->assignDir->Find(assignName);
        if (env && env->GetType() != nArg::List)
        {
            env->Release();
            env = 0;
        } 

        // create the nenv object
        if (!env)
        {
            env = (nEnv *) kernelServer->New("nenv", assignName);
            n_assert(env);
        }

        // assign the nenv
        env->SetL(args, paths.Size());
        kernelServer->PopCwd();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    queries existing assign under /sys/share/assigns

    @param assignName      the name of the assign
    @return                the path to which the assign links, or NULL if
                           assign is undefined
    history:
    - 30-Jan-2002   peter    created
*/
const char*
nFileServer2::GetAssign(const char * assignName)const
{
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (env)
    {
        return env->GetS();
    }
    else
    {
        n_printf("Assign '%s' not defined!\n", assignName);
        return NULL;
    }
}

//------------------------------------------------------------------------------
/**
    queries existing assign group under /sys/share/assigns

    @param assignName      the name of the assign
    @param assigns         the name of the assign
    @return                true if assign was found, otherwise false

    history:
     - 10-May-2005   mateu  created
*/
bool
nFileServer2::GetAssignGroup(const char* assignName, nArray<nString> & assigns)const
{
    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    assigns.Clear();
    if (env) 
    {
        nArg * args;
        if (env->GetType() == nArg::List)
        {
            int size = env->GetL(args);
            for(int i = 0;i < size;i++)
            {
                assigns.Append(args[i].GetS());
            }
        }
        else if (env->GetType() == nArg::String)
        {
            assigns.Append(env->GetS());
        }
        return true;
    }
    else 
    {
        n_printf("Assign group '%s' not defined!\n", assignName);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Returns a cleaned up path name (replaces backslashes with slashes, 
    Remove all existing assigns and setup base assigns.
*/
void
nFileServer2::ResetAssigns()
{
}

//------------------------------------------------------------------------------
/**
    Returns a cleaned up path name (replaces backslashes with slashes,
    and removes trailing slash if exists.
*/
void
nFileServer2::CleanupPathName(nString& str)const
{
    nString pathString(str.Get());
    pathString.ConvertBackslashes();
    pathString.StripTrailingSlash();
    str = pathString.Get();

/*    n_assert(path);

    char* ptr = path;
    char c;

#ifdef __XBxX__
    // on xbox replace slashes with backslashes
    while (c = *ptr)
    {
        if (c == '/')
        {
            *ptr = '\\';
        }
        else
        {
            *ptr = c;
        }
        ptr++;
    }
    // remove trailing slash
    if ((ptr > path) && (*(--ptr) == '\\'))
    {
        *ptr = 0;
    }
#else
    // on all other systems replace backslashes with slashes
    while ((c = *ptr))
    {
        if (c == '\\')
        {
            *ptr = '/';
        }
        else
        {
            *ptr = c;
        }
        ptr++;
    }
    // remove trailing slash
    if ((ptr > path) && (*(--ptr) == '/'))
    {
        *ptr = 0;
    }
#endif
*/
}

//------------------------------------------------------------------------------
bool 
nFileServer2::IsAssignGroup(const char * assignName)const
{
    n_assert(assignName);

    nEnv *env = (nEnv *) this->assignDir->Find(assignName);
    if (env)
    {
        return (env->GetType() == nArg::List);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Expands assign in path to full absolute path, replaces any backslashes
    by slashes, removes any trailing slash, and makes the path absolute.

    Please note that Nebula does not know the concept of a current working
    directory, thus, all paths MUST be absolute (please note that Nebula
    assigns can be used to create position independent absolute paths).

    @param pathName     the path to expand
    @param read         true when the path will be used for reading,
                        false when the path will be used for writing.
                        This is important when used with multiple assigns
    @return             resulting string

    history:
    - 30-Jan-2002   peter    created
*/
nString
nFileServer2::ManglePath(const nString& pathName, bool read)
{
    nString pathString(pathName);

    // check for assigns
    int colonIndex;
    while ((colonIndex = pathString.FindChar(':', 0)) > 0)
    {
        // special case: ignore one character "assigns" because they are
        // really DOS drive letters
        if (colonIndex > 1)
        {
            nString assignString = pathString.ExtractRange(0, colonIndex);
            nString postAssignString;

            // 2 case "assign:/dir1" and "assign:dir1" ,  the assign has a slashes
            if ( ( pathString.Length() < colonIndex + 1)  || ( pathString[colonIndex + 1] != '/' ) )
            {
                postAssignString = pathString.ExtractRange(colonIndex + 1, pathString.Length() - (colonIndex + 1));
            } else
            {
                postAssignString = pathString.ExtractRange(colonIndex + 2, pathString.Length() - (colonIndex + 2));
            }


            if (!this->IsAssignGroup(assignString.Get()))
            {
                nString replace = this->GetAssign(assignString.Get());
                if (!replace.IsEmpty())
                {
                    replace.Append(postAssignString);
                }
                pathString = replace;
            }
            else
            {
                nArray<nString> assigns;
                if (this->GetAssignGroup(assignString.Get(), assigns))
                {
                    bool usedefault = true;
                    nString replace;
                    if (read)
                    {
                        // check all paths until find the valid path
                        // problem do not allow more assigns in the path
                        for(int i = 0;usedefault && i < assigns.Size();i++)
                        {
                            replace = assigns[i].Get();
                            if (!replace.IsEmpty())
                            {
                                replace.Append(postAssignString);
                            }
                            replace = this->ManglePath(replace, read);
                            /// @todo: comprove if  directoy exists
                            if (this->FileExists(replace))
                            {
                                usedefault = false;
                            }
                        }
                    }
                    // if file not found get the assign
                    if (usedefault)
                    {
                        replace = assigns[0].Get();
                        if (!replace.IsEmpty())
                        {
                            replace.Append(postAssignString);
                        }
                        replace = this->ManglePath(replace, read);
                    }
                    // set the path
                    pathString = replace;
                }
            }
        }
        else 
        {
            break;
        }
    }
    this->CleanupPathName(pathString);
    return pathString;
}

//------------------------------------------------------------------------------
/**
    Expands assign in path to full absolute path, replaces any backslashes
    by slashes, removes any trailing slash, and makes the path absolute.
    Return all paths.


    @param path            the original path with assign
    @param assigns         the output return name of the assign

    history:
     - 24-Aug-2005   Cristobal  created
*/
void
nFileServer2::GetAllManglePath(const char* path, nArray<nString> &assigns )
{
    assigns.Clear();
    GetAllManglePathRecursively( path, assigns);
}

//------------------------------------------------------------------------------
void
nFileServer2::GetAllManglePathRecursively(const char* pathName, nArray<nString> &paths)
{
    nString pathString = pathName;

    // check for assigns
    int colonIndex;
    if  ( ! ((colonIndex = pathString.FindChar(':', 0)) > 0 ))
    {
        this->CleanupPathName(pathString);
        paths.Append( pathString);
    } 
    else
    {
        // special case: ignore one character "assigns" because they are
        // really DOS drive letters
        if ( ! (colonIndex > 1) )
        {
            this->CleanupPathName(pathString);
            paths.Append( pathString);
        } 
        else
        {
            nString assignString = pathString.ExtractRange(0, colonIndex);
            nString postAssignString;
            // 2 case "assign:/dir1" and "assign:dir1" ,  the assign has a slashes
            if ( ( pathString.Length() < colonIndex + 1)  || ( pathString[colonIndex + 1] != '/' ) )
            {
                postAssignString = pathString.ExtractRange(colonIndex + 1, pathString.Length() - (colonIndex + 1));
            } else
            {
                postAssignString = pathString.ExtractRange(colonIndex + 2, pathString.Length() - (colonIndex + 2));
            }


            if (!this->IsAssignGroup(assignString.Get()))
            {
                nString replace = this->GetAssign(assignString.Get());
                if (!replace.IsEmpty())
                {
                    replace.Append(postAssignString);
                }
                this->GetAllManglePathRecursively( replace.Get() , paths );
            }
            else
            {
                nArray<nString> assigns;
                if (this->GetAssignGroup(assignString.Get(), assigns))
                {
                    nString replace;
                    // Append all paths 
                    for(int i = 0; i < assigns.Size();i++)
                    {
                        replace = assigns[i].Get();
                        if (!replace.IsEmpty())
                        {
                            replace.Append(postAssignString);
                        }
                        this->GetAllManglePathRecursively(replace.Get(), paths);
                    }
                }
            }

        }
    }
}


//------------------------------------------------------------------------------
/**
    creates a new nDirectory object

    @return    the nDirectory object

    history:
    - 30-Jan-2002   peter    created
*/
nDirectory*
nFileServer2::NewDirectoryObject() const
{
    nDirectory* result = n_new(nDirectory);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
    creates a new nFile object

    @return          the nFile object

    history:
    - 30-Jan-2002   peter    created
*/
nFile*
nFileServer2::NewFileObject() const
{
    nFile* result = n_new(nFile);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
nFileServer2::InitAssigns()
{
    n_assert(!this->assignDir.isvalid());
    this->assignDir = kernelServer->New("nroot", "/sys/share/assigns");
    this->InitHomeAssign();
    this->InitBinAssign();
    this->InitUserAssign();
    this->InitTempAssign();
}

//------------------------------------------------------------------------------
/**
    Initialize Nebula's home directory assign ("home:").

    - 14-May-04  floh   Win32: if parent directory not "win32", use the executable's
                        directory as home:
*/
void
nFileServer2::InitHomeAssign()
{
#ifdef __XBxX__
    this->SetAssign("home", "d:/");
#else
    #if __WIN32__
        // Win32: Check for the NEBULADIR environment variable first,
        // then try to find the nkernel.dll module handle's filename
        // and cut off the last 2 directories
        //
        // *** NOTE BY FLOH ***
        // Checking for a NEBULADIR env variable is a bad idea because it may
        // lead to hard to find end-user problems if several shipped Nebula
        // application exist on the machine and the user has defined
        // the NEBULADIR variable (for instance because the user happens
        // to be a Nebula developer). This happened when we shipped Nomads
        // and people had the Nebula SDK installed which required a
        // NOMADS_HOME variable :(
        /*
        char* s = getenv("NEBULADIR");
        if (s)
        {
            n_strncpy2(buf,s,sizeof(buf));
        }
        else
        {
        */

        // use the executable's directory to locate the home directory
        char buf[N_MAXPATH];
        DWORD res = GetModuleFileName(NULL, buf, sizeof(buf));
        if (res == 0)
        {
            n_error("nFileServer2::InitHomeAssign(): GetModuleFileName() failed!\n");
        }

        nString pathToExe(buf);
        pathToExe.ConvertBackslashes();

        // check if executable resides in a win32 directory
        nString pathToDir = pathToExe.ExtractLastDirName();
        // converted to lowercase because sometimes the path is in uppercase
        pathToDir.ToLower();
        if (pathToDir == "win32" || pathToDir == "win32d" || pathToDir == "win32do" || pathToDir == "win32a")
        {
            // normal home:bin/win32 directory structure
            // strip bin/win32
            nString homePath = pathToExe.ExtractDirName();
            homePath.StripTrailingSlash();
            homePath = homePath.ExtractDirName();
            homePath.StripTrailingSlash();
            homePath = homePath.ExtractDirName();
            this->SetAssign("home", homePath);
        }
        else
        {
            // not in normal home:bin/win32 directory structure,
            // use the exe's directory as home path
            nString homePath = pathToExe.ExtractDirName();
            this->SetAssign("home", homePath);
        }
    #elif defined(__LINUX__)
        // under Linux, the NEBULADIR environment variable must be set,
        // otherwise the current working directory will be used
        char buf[N_MAXPATH];
        char *s = getenv("NEBULADIR");
        if (s)
        {
            n_strncpy2(buf,s,sizeof(buf));
        }
        else
        {
            n_error("Env variable NEBULADIR not set! Aborting.");
        }
        // if last char is not a /, append one
        if ((strlen(buf) > 0) && (buf[strlen(buf)-1] != '/'))
        {
            strcat(buf,"/");
        }
        this->SetAssign("home", buf);
    #elif defined(__MACOSX__)
        char buf[N_MAXPATH];
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef bundleURL = CFBundleCopyBundleURL(mainBundle);
        FSRef bundleFSRef;
        CFURLGetFSRef(bundleURL, &bundleFSRef);
        FSRefMakePath(&bundleFSRef, (unsigned char*)buf, N_MAXPATH);
        // if last char is not a /, append one
        if ((strlen(buf) > 0) && (buf[strlen(buf)-1] != '/'))
        {
            strcat(buf,"/");
        }
        this->SetAssign("home", buf);
    #else
    #error nFileServer::initHomeAssign() not implemented!
    #endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nFileServer2::InitBinAssign()
{
#ifdef __XBxX__
    this->SetAssign("bin", "d:/");
#else
    #ifdef __WIN32__
        // use the executable's directory to locate the bin directory
        char buf[N_MAXPATH];
        DWORD res = GetModuleFileName(NULL, buf, sizeof(buf));
        if (res == 0)
        {
            n_error("nFileServer2::InitHomeAssign(): GetModuleFileName() failed!\n");
        }
        nString pathToExe( buf );
        pathToExe.ConvertBackslashes();
        nString binPath = pathToExe.ExtractDirName();
        this->SetAssign("bin", binPath);

    #elif defined(__LINUX__)

        char buf[N_MAXPATH];
        const char *home_dir = this->GetAssign("home");
        n_assert(home_dir);
        n_strncpy2(buf,home_dir,sizeof(buf));
        strcat(buf,"bin/linux/");
        this->SetAssign("bin",buf);

    #elif defined(__MACOSX__)

        char buf[N_MAXPATH];
        const char *home_dir = this->GetAssign("home");
        n_assert(home_dir);
        n_strncpy2(buf,home_dir,sizeof(buf));
        strcat(buf, "bin/macosx/");
        this->SetAssign("bin",buf);

    #else
    #error nFileServer::initBinAssign() not implemented!
    #endif

#endif
}

//------------------------------------------------------------------------------
/**
    Initialize the user assign. This is where the application should
    save any type of data, like save games or config options, since
    applications may not have write access to the home: directory (which is by
    tradition the application directory.

    On the Xbox, the user assign points to the application's
    hard disk partition.

    On Windows, the user assign points to CSIDL_PERSONAL.

    On Unix, the user assign should point to the user's home
    directory.
*/
void
nFileServer2::InitUserAssign()
{
#ifdef __XBxX__
    this->SetAssign("user", "d:/");
#elif defined(__WIN32__)
    char rawPath[MAX_PATH];
    N_IFDEF_ASSERTS(HRESULT hr =)
    nWin32Wrapper::Instance()->SHGetFolderPath(0,       // hwndOwner
                    CSIDL_PERSONAL | CSIDL_FLAG_CREATE, // nFolder
                    NULL,                               // hToken
                    0,                                  // dwFlags
                    rawPath);                           // pszPath
    n_assert(S_OK == hr);

    nString path(rawPath);
    path.ConvertBackslashes();
    path.Append("/");
    this->SetAssign("user", path);
#elif defined(__LINUX__) || defined(__MACOSX__)
    nString path(getenv("HOME"));
    path.Append("/");
    this->SetAssign("user", path.Get());
#else
#error "IMPLEMENT ME!"
#endif
}
//------------------------------------------------------------------------------
/**
    Initialize the standard temp: assign.
*/
void
nFileServer2::InitTempAssign()
{
#ifdef __XBxXX__
    this->SetAssign("temp", "d:/");
#elif __WIN32__
    char rawPath[MAX_PATH];
    n_verify( GetTempPath(sizeof(rawPath), rawPath) > 0 );

    nString path(rawPath);
    path.ConvertBackslashes();
    path.Append("/");
    this->SetAssign("temp", path);
#elif defined(__LINUX__) || defined(__MACOSX__)
    const char * tmpDir = getenv("TMPDIR");
    if (NULL == tmpDir)
    {
        tmpDir = "/tmp";
    }
    nString path(tmpDir);
    path.Append("/");
    this->SetAssign("user", path.Get());
#else
#error "IMPLEMENT ME!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileServer2::FileExists(const nString& pathName)
{
    n_assert_return(!pathName.IsEmpty(), false);
    bool result = false;
    nFile* file = this->NewFileObject();
    if (file->Exists(pathName))
    {
        result = true;
    }
    file->Release();
    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileServer2::DirectoryExists(const nString& pathName) const
{
    n_assert_return(!pathName.IsEmpty(), false);
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(pathName))
    {
        dir->Close();
        n_delete(dir);
        return true;
    }
    n_delete(dir);
    return false;
}

//------------------------------------------------------------------------------
/**
    Make any missing directories in path.
*/
bool
nFileServer2::MakePath(const nString& dirName)
{
    nDirectory* dir = this->NewDirectoryObject();
    n_assert(dir);

    // build stack of non-existing dir components
    nString path = this->ManglePath(dirName, false);
    nArray<nString> pathStack;

    if (path[0] == '<') // "<unknown_assign>"
    {
        n_delete( dir );
        return false;
    }

    while ((!path.IsEmpty()) && (!dir->Open(path)))
    {
        pathStack.Append(path);
        path = path.ExtractDirName();
    }
    if (dir->IsOpen())
    {
        dir->Close();
    }
    n_delete(dir);

    // error?
    if (path.IsEmpty())
    {
        return false;
    }

    // create missing directory components
    int i;
    for (i = pathStack.Size() - 1; i >= 0; --i)
    {
        const nString& curPath = pathStack[i];
        #ifdef __WIN32__
            int err = _mkdir(curPath.Get());
        #else
            int err = mkdir(curPath.Get(), S_IRWXU|S_IRWXG);
        #endif
        if (-1 == err)
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Copy a file.
    FIXME: the Non-Win32 version reads the entire file is into RAM!

    - 09-Mar-04     floh    Win32 version now removes write protection on
                            target file before copying.
*/
bool
nFileServer2::CopyFile(const nString& from, const nString& to)
{
    #ifdef __WIN32__
        // Win32 specific method is more efficient
        nString mangledFromPath = this->ManglePath(from, true);
        nString mangledToPath   = this->ManglePath(to, false);

        // if the target file exists, remove the read/only file attribute
        if (this->FileExists(mangledToPath))
        {
            DWORD fileAttrs = GetFileAttributes(mangledToPath.Get());
            fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(mangledToPath.Get(), fileAttrs);
        }
        return ::CopyFile(mangledFromPath.Get(), mangledToPath.Get(), FALSE) ? true : false;
    #else
        nFile* fromFile = this->NewFileObject();
        if (!fromFile->Open(from.Get(), "rb"))
        {
            n_printf("nFileServer2::Copy(): could not open source file '%s'\n", from.Get());
            fromFile->Release();
            return false;
        }
        nFile* toFile = this->NewFileObject();
        if (!toFile->Open(to.Get(), "wb"))
        {
            n_printf("nFileServer2::Copy(): could not open dest file '%s'\n", to.Get());
            fromFile->Close();
            fromFile->Release();
            toFile->Release();
            return false;
        }

        int size = fromFile->GetSize();
        n_assert(size > 0);
        void* buffer = n_malloc(size);
        n_assert(buffer);
        int numRead = fromFile->Read(buffer, size);
        n_assert(numRead == size);
        int numWritten = toFile->Write(buffer, size);
        n_assert(numWritten == size);
        n_free(buffer);

        fromFile->Close();
        toFile->Close();
        fromFile->Release();
        toFile->Release();
        return true;
    #endif
}

//------------------------------------------------------------------------------
/**
    Copy a file or directory.
*/
bool
nFileServer2::MoveFile(const nString& from, const nString& to)
{
    #ifdef __WIN32__
        // Win32 specific method is more efficient
        nString mangledFromPath = this->ManglePath(from, true);
        nString mangledToPath   = this->ManglePath(to, false);

        // if the target file exists, remove the read/only file attribute
        if (this->FileExists(mangledToPath))
        {
            DWORD fileAttrs = GetFileAttributes(mangledToPath.Get());
            fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(mangledToPath.Get(), fileAttrs);
        }
        return ::MoveFile(mangledFromPath.Get(), mangledToPath.Get()) ? true : false;
    #else
        n_assert_always2("nFileServer2::MoveFile not implemented");
        return false;
    #endif
}

//------------------------------------------------------------------------------
/**
    Delete a file.
*/
bool
nFileServer2::DeleteFile(const nString& filename, bool read)
{
    nString mangledPath = this->ManglePath(filename, read);

    #ifdef __WIN32__
        return ::DeleteFile(mangledPath.Get()) ? true : false;
    #elif defined(__LINUX__) || defined(__MACOSX__)
        return (0 == unlink(mangledPath.Get())) ? true : false;
    #else
    #error "nFileServer2::DeleteFile() not implemented yet!"
    #endif
}

//------------------------------------------------------------------------------
/**
    Delete an empty directory.
*/
bool
nFileServer2::DeleteDirectory(const nString& dirName, bool read)
{
    nString mangledPath = this->ManglePath(dirName, read);

    #ifdef __WIN32__
    return ::RemoveDirectory(mangledPath.Get()) ? true : false;
    #elif defined(__LINUX__) || defined(__MACOSX__)
    return (rmdir(mangledPath.Get()) == 0) ? true : false;
    #else
    #error "nFileServer2::DeleteDirectory() not implemented yet!"
    #endif
}

//------------------------------------------------------------------------------
/**
    Delete a directory, even if it is not empty.
*/
bool
nFileServer2::DeleteDirectoryRecursive(const nString& dirName, bool read)
{
    nString mangledPath = this->ManglePath(dirName, read);

    #ifdef __WIN32__
    char buf[N_MAXPATH];
    memset(buf, 0, sizeof(buf));
    mangledPath.SubstituteCharacter('/', '\\');
    strcpy(buf, mangledPath.Get());

    SHFILEOPSTRUCT fileOp;
    memset(&fileOp, 0, sizeof(fileOp));
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = buf;
    fileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
    return ::SHFileOperation(&fileOp) == 0 ? true : false;
    #else
    #error "nFileServer2::DeleteDirectoryRecursive() not implemented yet!"
    #endif
}

//------------------------------------------------------------------------------
/**
    Create a filenode and return its full path name. A filenode is a
    nFile object wrapped into a nRoot subclass. It offers access to filesystem
    functionality for scripting languages which don't offer access to
    the host filesystem (like MicroTcl).
*/
nFileNode*
nFileServer2::CreateFileNode(const nString& name)
{
    nString path = "/sys/share/files/";
    path += name;
    if (kernelServer->Lookup(path.Get()))
    {
        n_error("nFileServer2: file node '%s' exists!", name.Get());
        return 0;
    }
    nFileNode* fileNode = (nFileNode*) kernelServer->New("nfilenode", path.Get());
    return fileNode;
}

//------------------------------------------------------------------------------
/**
    Compute the CRC checksum for a file.
    FIXME: the current implementation loads the entire file into memory.

    @param  filename    [in]    a Nebula filename
    @param  crc         [out]   the computed CRC checksum
    @return             true if all ok, false if file could not be opened
*/
bool
nFileServer2::Checksum(const nString& filename, uint& crc)
{
    crc = 0;
    bool success = false;
    nFile* file = this->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "rb"))
    {
        // read file into RAM buffer
        int numBytes = file->GetSize();
        uchar* buf = (uchar*) n_malloc(numBytes);
        n_assert(buf);

        n_verify( file->Read(buf, numBytes) == numBytes );

        // compute CRC
        nCRC crcSummer;
        crc = crcSummer.Checksum(buf, numBytes);

        // free and close everything
        n_free(buf);
        file->Close();
        success = true;
    }
    file->Release();
    return success;
}
    
//------------------------------------------------------------------------------
/**
    Find a file recursively starting on the given base path.
    @param basePath base path where to start search
    @param fileName name of the file to look for
    @param path output parameter, that will contain the whole path to the file
    @return true if file was found, false otherwise
*/
bool
nFileServer2::FindFile(const char * basePath, const char * fileName, nString & path)
{
    path.Clear();

    // create and open directory
    nDirectory * ndir = this->NewDirectoryObject();
    n_assert(ndir);
    if (ndir->Open(basePath))
    {
        // first scan all files
        bool moreFiles = ndir->SetToFirstEntry();
        while ( moreFiles )
        {
            // build full path to compare
            nString fullpath = basePath;
            fullpath.StripTrailingSlash();
            fullpath = fullpath + "/";
            fullpath = fullpath + fileName;

            if ( ndir->GetEntryType() == nDirectory::FILE && ! strcmp(ndir->GetEntryName(), fullpath.Get()))
            {
                path = fullpath;

                // release resources
                ndir->Close();
                n_delete(ndir);

                return true;
            }
            moreFiles = ndir->SetToNextEntry();
        }

        // now scan all directories
        moreFiles = ndir->SetToFirstEntry();
        while ( moreFiles )
        {
            if ( ndir->GetEntryType() == nDirectory::DIRECTORY )
            {
                if (this->FindFile(ndir->GetEntryName(), fileName, path))
                {
                    // release resources
                    ndir->Close();
                    n_delete(ndir);

                    return true;
                }
            }
            moreFiles = ndir->SetToNextEntry();
        }

        ndir->Close();
    }

    n_delete(ndir);

    return false;
}

//------------------------------------------------------------------------------
/**
    Set the read-only status of a file.
*/
void
nFileServer2::SetFileReadOnly(const nString& filename, bool readOnly)
{
    nString mangledPath = this->ManglePath(filename);
#ifdef __WIN32__
    DWORD fileAttrs = GetFileAttributes(mangledPath.Get());
    if (readOnly)
    {
        fileAttrs |= FILE_ATTRIBUTE_READONLY;
    }
    else
    {
        fileAttrs &= ~FILE_ATTRIBUTE_READONLY;
    }
    SetFileAttributes(mangledPath.Get(), fileAttrs);
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct stat s;
    if (stat(mangledPath.Get(), &s) != -1)
    {
        mode_t mode = s.st_mode;
        if (readOnly)
        {
            // Remove all write flags...
            mode &= ~S_IWUSR;
            mode &= ~S_IWGRP;
            mode &= ~S_IWOTH;
        }
        else
        {
            mode |= S_IWUSR;
        }
        chmod(mangledPath.Get(), mode);
    }
#else
#error "nFileServer2::SetFileReadOnly() not implemented yet!"
#endif
}

//------------------------------------------------------------------------------
/**
    Get the read-only status of a file. If the file does not exist,
    the routine returns false.
*/
bool
nFileServer2::IsFileReadOnly(const nString& filename)
{
    nString mangledPath = this->ManglePath(filename);
#ifdef __WIN32__
    DWORD fileAttrs = GetFileAttributes(mangledPath.Get());
    return (fileAttrs & FILE_ATTRIBUTE_READONLY);
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct stat s;
    if (stat(mangledPath.Get(), &s) == -1)
    {
        return false;
    }
    if (s.st_uid == geteuid() && (s.st_mode & S_IRWXU))
    {
        return true;
    }
    else if (s.st_gid == getegid() && (s.st_mode & S_IRWXG))
    {
        return true;
    }
    return false;
#else
#error "nFileServer2::IsFileReadOnly() not implemented yet!"
#endif
}

//------------------------------------------------------------------------------
/**
    Get the hidden status of a file. If the file does not exist,
    the routine returns false.
*/
bool
nFileServer2::IsFileHidden(const nString& filename)
{
    n_assert_return(!filename.IsEmpty(), false);

    nString mangledPath = this->ManglePath(filename);
#ifdef __WIN32__
    DWORD fileAttrs = GetFileAttributes(mangledPath.Get());
    return (fileAttrs & FILE_ATTRIBUTE_HIDDEN);
#else
#error "nFileServer2::IsFileReadOnly() not implemented yet!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nFileServer2::GetAllFilesFrom( const char * pathName, nArray<nString> & files )
{
    n_assert( pathName );
    nString mangledPath = this->ManglePath(pathName);

#ifdef __WIN32__

    WIN32_FIND_DATA FileData;
    HANDLE hFind;

    hFind = FindFirstFile( mangledPath.Get(), &FileData); 
    if( hFind == INVALID_HANDLE_VALUE ) 
    { 
        // cant found files
        return;
    }
    
    bool finished = false;

    while( ! finished ) 
    {
        files.Append( nString( FileData.cFileName ) );
     
        if( ! FindNextFile( hFind, &FileData ) ) 
        {
            if( GetLastError() == ERROR_NO_MORE_FILES )
            {
                // No more files Search completed
                finished = true;
            }
            else
            {
                // Couldn't find next file
                return;
            } 
        }
    }

    FindClose( hFind );

#else
#error "nFileServer2::GetAllFilesFrom() not implemented yet!"
#endif
}

//------------------------------------------------------------------------------
/**
    List all files in a directory, ignores subdirectories.
*/
nArray<nString>
nFileServer2::ListFiles(const nString& dirName)
{
    nArray<nString> fileList;
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(dirName))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (dir->GetEntryType() == nDirectory::FILE)
            {
                fileList.Append(dir->GetEntryName());
            }
        }
        while (dir->SetToNextEntry());
        dir->Close();
    }
    n_delete(dir);
    return fileList;
}

//------------------------------------------------------------------------------
/**
    List all subdirectories in a directory, ignores files.
*/
nArray<nString>
nFileServer2::ListDirectories(const nString& dirName)
{
    nArray<nString> dirList;
    nDirectory* dir = this->NewDirectoryObject();
    if (dir->Open(dirName))
    {
        if (dir->SetToFirstEntry()) do
        {
            if (dir->GetEntryType() == nDirectory::DIRECTORY)
            {
                dirList.Append(dir->GetEntryName());
            }
        }
        while (dir->SetToNextEntry());
        dir->Close();
    }
    n_delete(dir);
    return dirList;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**

*/
bool
nFileServer2::UnManglePath(nString& pathName, nString assignName)
{
    bool hasPattern(false);
    nArray<nString> allPaths; 
    pathName.ConvertBackslashes();
    #ifdef __WIN32__
    pathName.ToLower();
    #endif
    assignName += ":";

    nFileServer2::Instance()->GetAllManglePath( assignName.Get()  , allPaths);

    for ( int idx  = 0 ; idx < allPaths.Size() && !hasPattern; ++idx )
    {
        #ifdef __WIN32__
        allPaths[idx].ToLower();
        #endif
        hasPattern = pathName.BeginBy( allPaths[idx] );
        if ( hasPattern)
        {
            pathName = pathName.ReplaceBegin( allPaths[idx] ,  assignName , false);
        }
    }
    return hasPattern;
}

//------------------------------------------------------------------------------
/**
    Copy a directory recursively.
    @param sourceDirPath - path of directory to copy from
    @param destinationDirPath - path of directory to copy to
    @return true if copy was successful, false otherwise
*/
bool
nFileServer2::CopyDirectoryRecursively(const nString& sourceDirPath, const nString& destinationDirPath)
{
    n_assert_return(!sourceDirPath.IsEmpty(), false);
    n_assert_return(!destinationDirPath.IsEmpty(), false);
    
    return this->CopyDirectory(sourceDirPath, destinationDirPath, true);
}

//------------------------------------------------------------------------------
/**
    Copy a directory NON-recursively.
    @param sourceDirPath - path of directory to copy from
    @param destinationDirPath - path of directory to copy to
    @return true if copy was successful, false otherwise
*/
bool
nFileServer2::CopyDirectory(const nString& sourceDirPath, const nString& destinationDirPath)
{
    n_assert_return(!sourceDirPath.IsEmpty(), false);
    n_assert_return(!destinationDirPath.IsEmpty(), false);
    
    return this->CopyDirectory(sourceDirPath, destinationDirPath, false);
}

//------------------------------------------------------------------------------
/**
    Copy a directory.
    @param sourceDirPath - path of directory to copy from
    @param destinationDirPath - path of directory to copy to
    @param recursive - flag to indicate whether copy is recursive
    @return true if copy was successful, false otherwise
*/
bool
nFileServer2::CopyDirectory(const nString& sourceDirPath, const nString& destinationDirPath, bool recursive)
{
    n_assert_return(!sourceDirPath.IsEmpty(), false);
    n_assert_return(!destinationDirPath.IsEmpty(), false);

    bool createdDirOK = this->MakePath(destinationDirPath); 
    n_assert_return(createdDirOK, false);

    nArray<nString> fileNames = this->ListFiles( sourceDirPath.Get() );
        
    for( int i = 0; i < fileNames.Size() ; ++i )
    {
        nString fileNameWithPath = fileNames[i];
        
        // do NOT copy hidden files
        if ( !this->IsFileHidden(fileNameWithPath) )
        {
            nString destinationFilePath = destinationDirPath.Get();
            nString fileNameWithoutPath = fileNameWithPath.ExtractFileName();
            destinationFilePath.Append(FILE_SEPARATOR_CHARACTER);
            destinationFilePath.Append(fileNameWithoutPath);

            bool copiedFileOK = this->CopyFile(fileNameWithPath, destinationFilePath);
            n_assert_return(copiedFileOK, false);
        }
    }

    if (!recursive)
    {
        return true;
    }
    
    /// work through the sub directories recursively
    nArray<nString> subDirectoryNames = this->ListDirectories(sourceDirPath.Get() );

    for( int i = 0; i < subDirectoryNames.Size() ; ++i )
    {
        nString subDirNameWithPath = subDirectoryNames[i];

        // do NOT copy hidden directories
        if ( !this->IsFileHidden(subDirNameWithPath) )
        {
            nString subDirName = subDirNameWithPath.ExtractFileName();
            nString destinationDirPath2 = destinationDirPath.Get();
            destinationDirPath2.Append(FILE_SEPARATOR_CHARACTER);
            destinationDirPath2.Append(subDirName);

            bool copiedDirOK = this->CopyDirectory(subDirNameWithPath, destinationDirPath2, true);
            n_assert_return(copiedDirOK, false);
        }
    }

    return true;
}
   
#endif