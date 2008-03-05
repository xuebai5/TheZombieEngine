//------------------------------------------------------------------------------
//  nnpkfileserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "kernel/nkernelserver.h"
#include "file/nnpkfileserver.h"
#include "file/nnpkfilewrapper.h"
#include "file/nnpktocentry.h"
#include "file/nnpkfile.h"
#include "file/nnpkdirectory.h"
#include "tools/nnpkbuilder.h"

nNebulaScriptClass(nNpkFileServer, "nfileserver2");

////------------------------------------------------------------------------------
const int nNpkFileServer::MAX_NPK_NODES = ( 1 << 4 ) - 1;
const char * nNpkFileServer::NPK_EXTENSION = ".npk";

//------------------------------------------------------------------------------
namespace{

//------------------------------------------------------------------------------
/**
    function object to search in the heap using absolute path
*/
class SearchAbsPath
{
public:
    SearchAbsPath( const nString & path ):searchPath( path ){}

    bool operator()( nNpkFileWrapper * file )const
    { 
        return ( file->GetAbsPath() == searchPath ); 
    }

private:
    // null assignment operator to eliminate warning
    void operator=( SearchAbsPath & /*search*/){};

    const nString & searchPath;
};

//------------------------------------------------------------------------------
/**
    function object to search a filename in the TOC heap
    returns the TOC entry in a public member
*/
class SearchTocEntry
{
public:
    SearchTocEntry( const char * filename ):searchName( filename ),tocEntry(0){}

    bool operator()( nNpkFileWrapper * file )
    {
        this->tocEntry = file->GetTocObject().FindEntry( searchName );

        return ( 0 != this->tocEntry );
    }

    nNpkTocEntry * tocEntry;
private:
    const char * searchName;
};

//------------------------------------------------------------------------------
/**
    function object to search a TOC using a pattern string
*/
class SearchPattern
{
public:
    SearchPattern( const nString & pattern):searchPattern( pattern ){}

    bool operator()( nNpkFileWrapper * file )const
    {
        return ( file->GetAbsPath().MatchPattern( this->searchPattern ) );
    }
private:
    // null assignment operator to eliminate warning
    void operator=( SearchPattern & /*search*/){};

    const nString & searchPattern;
};

}//namespace

//------------------------------------------------------------------------------
/**
*/
nNpkFileServer::nNpkFileServer():loadedNpks( nNpkFileServer::MAX_NPK_NODES )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNpkFileServer::~nNpkFileServer()
{
    for( int i = 0 ; i < this->loadedNpks.Size() ; ++i )
    {
        n_delete( this->loadedNpks[ i ] );
    }

    this->loadedNpks.Clear();
}

//------------------------------------------------------------------------------
/**
*/
nFile*
nNpkFileServer::NewFileObject() const
{
    nNpkFile* result = n_new(nNpkFile);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
nDirectory*
nNpkFileServer::NewDirectoryObject() const
{
    nNpkDirectory* result = n_new(nNpkDirectory);
    n_assert(result != 0);
    return result;
}

//------------------------------------------------------------------------------
/**
    Check the extension of a pathname.
*/
bool
nNpkFileServer::CheckExtension(const char* path, const char* ext)
{
    n_assert(path);
    const char* dot = strrchr(path, '.');
    if (dot)
    {
        if (strcmp(dot + 1, ext) == 0)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Parses a single npk file and adds it to the list of npk files.
*/
bool
nNpkFileServer::ParseNpkFile(const nString& path)
{
    n_assert(!path.IsEmpty());
    if (this->FileExists(path.Get()))
    {
        nString dirname  = path.ExtractDirName();
        nString absDirPath = this->ManglePath(dirname.Get());
        nString absFilePath = this->ManglePath(path.Get());
        if (this->AddNpkFile(absDirPath, absFilePath))
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Scan through a directory for npk files and create a nNpkFileWrapper
    object for each npk file found, and open the npk files. Returns
    the number of npk files found.
*/
int
nNpkFileServer::ParseDirectory(const nString& dirName, const nString& extension)
{
    n_assert(!dirName.IsEmpty());
    n_assert(!extension.IsEmpty());

    nString absPath = this->ManglePath(dirName.Get());

    // scan directory for npk files...
    int numNpks = 0;
    nDirectory* dir = nFileServer2::NewDirectoryObject();
    n_assert(dir);
    if (dir->Open(absPath.Get()))
    {
        if (!dir->IsEmpty()) do
        {
            const char* entryName = dir->GetEntryName();
            nDirectory::EntryType entryType = dir->GetEntryType();
            if ((entryType == nDirectory::FILE) && this->CheckExtension(entryName, extension.Get()))
            {
                n_printf("*** Reading npk file '%s'\n", entryName);
                if (this->AddNpkFile(absPath, entryName))
                {
                    numNpks++;
                }
            }
        } while (dir->SetToNextEntry());
        dir->Close();
    }
    else
    {
        n_printf("nNpkFileServer::ParseDirectory(): could not open dir '%s'\n", absPath.Get());
    }
    n_delete(dir);
    return numNpks;
}

//------------------------------------------------------------------------------
/**
    Parse a single npk file, creates a nNpkFileWrapper object, adds it the
    the npkFiles list and opens it.
*/
bool
nNpkFileServer::AddNpkFile(const nString& rootPath, const nString& absFilename)
{
    n_assert(!rootPath.IsEmpty());
    n_assert(!absFilename.IsEmpty());

    SearchAbsPath search( absFilename );

    int index = this->loadedNpks.Search( search );

    bool wasAdded = false;

    nNpkFileWrapper * oldFile = 0;

    // if npk file was already loaded
    if( index != NPKHeap::InvalidIndex )
    {
        // refresh npk TOC
        this->loadedNpks[ index ]->Close();

        wasAdded = this->loadedNpks[ index ]->Open( this, rootPath.Get(), absFilename.Get() );
        if( ! wasAdded )
        {
            oldFile = this->loadedNpks.Remove( index );
        }
    }
    else
    {
        // load npk TOC
        nNpkFileWrapper * file = n_new( nNpkFileWrapper );
        n_assert_if( file )
        {
            wasAdded = file->Open( this, rootPath.Get(), absFilename.Get() );
            if( wasAdded )
            {
                oldFile = this->loadedNpks.Insert( file );
            }
            else
            {
                n_delete( file );
            }
        }
    }

    // delete old file
    if( oldFile )
    {
        n_delete( oldFile );
    }

    return wasAdded;
}

//------------------------------------------------------------------------------
/**
    Find a nTocEntry by filename. The filename must be absolute.
    @param absPath complete name of the file
*/
nNpkTocEntry*
nNpkFileServer::FindTocEntry(const char* absPath)
{
    SearchTocEntry search( absPath );

    this->loadedNpks.Search( search );

    return search.tocEntry;
}

//------------------------------------------------------------------------------
/**
    This releases all NPK file wrappers whose filename matches a given
    pattern.
*/
void
nNpkFileServer::ReleaseNpkFiles(const nString& pattern)
{
    SearchPattern search( pattern );

    bool remainNodes = true;

    while( remainNodes )
    {
        remainNodes = false;

        int index = this->loadedNpks.Search( search );

        if( index != NPKHeap::InvalidIndex )
        {
            remainNodes = true;
            
            nNpkFileWrapper * oldFile = this->loadedNpks.Remove( index );
            if( oldFile )
            {
                n_delete( oldFile );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkFileServer::FileExists( const nString & pathName )
{
    if( nFileServer2::FileExists( pathName ) )
    {
        return true;
    }

    nString npkPath = pathName;

    int pathSize;

    npkPath.StripExtension();

    // for every directory in the path
    do
    {
        pathSize = npkPath.Length();

        nString npkFile = npkPath + nNpkFileServer::NPK_EXTENSION;

        // if directory has a npk version
        if( nFileServer2::FileExists( npkFile ) )
        {
            // load npk toc
            nString dirname  = npkFile.ExtractDirName();
            nString absDirPath = this->ManglePath(dirname.Get());
            nString absFilePath = this->ManglePath(npkFile.Get());
            if( this->AddNpkFile(absDirPath, absFilePath) )
            {
                // if file exist in toc
                if( this->FindTocEntry( pathName.Get() ) )
                {
                    return true;
                }
            }
        }

        npkPath = npkPath.ExtractDirName();
        npkPath.StripTrailingSlash();
    }
    while( pathSize != npkPath.Length() );

    return false;
}

//------------------------------------------------------------------------------
