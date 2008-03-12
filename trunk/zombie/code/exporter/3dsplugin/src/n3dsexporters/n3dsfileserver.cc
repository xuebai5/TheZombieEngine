#include "precompiled/pchn3dsmaxexport.h"
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dslog.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"
#include "tools/napplauncher.h"


const nString n3dsFileServer::realwc("realwc:");
const nString n3dsFileServer::tmpwc("tmpwc:");


//------------------------------------------------------------------------------
/** 
    Get recusrsive files from a directory, replace "wc:" by "realwc:" or "tmpwc:"
    @param realwc set get from real working copy ore temporal working copy
    @param path the full path of directory
    @param fileList out argument , with all names of file
*/
bool 
n3dsFileServer::GetFilesFrom( const nString& path , nArray<nString>& fileList, bool useRealWc) const
{
    nString newPath;
    nString pattern;
    nString match;
    fileList.Clear();
    
    if (useRealWc)
    {
        newPath = path.ReplaceBegin("wc:", realwc);
        pattern = nFileServer2::Instance()->ManglePath(realwc.Get());
        match = realwc;
    } else
    {
        newPath = path.ReplaceBegin("wc:", tmpwc);
        pattern = nFileServer2::Instance()->ManglePath(tmpwc.Get());
        match = tmpwc;
    }
    pattern += "/";

    if (! GetFilesFrom2( newPath, fileList) )
    {
        return false;
    }

    // replace absolute path by asign path
    for ( int i=0; i< fileList.Size() ; i++)
    {
        fileList[i] = fileList[i].ReplaceBegin( pattern, match);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsFileServer::GetFilesFrom2( const nString& path , nArray<nString>& fileList) const
{
    if ( path.ExtractFileName() == ".svn"  ) //trick for not recursive subversion directories
    {
        return true;
    }

    nFileServer2* server = nFileServer2::Instance();
    // create and open directory
    nDirectory * ndir = server->NewDirectoryObject();
    n_assert(ndir);
    if (ndir->Open(path.Get()))
    {

        // now scan all directories
        bool moreFiles = ndir->SetToFirstEntry();
        while ( moreFiles )
        {
            if ( ndir->GetEntryType() == nDirectory::DIRECTORY )
            {
                GetFilesFrom2( ndir->GetEntryName(), fileList );
            }
            moreFiles = ndir->SetToNextEntry();
        }

        // first scan all files
        moreFiles = ndir->SetToFirstEntry();
        while ( moreFiles )
        {
            if ( ndir->GetEntryType() == nDirectory::FILE )
            {
                fileList.Append( ndir->GetEntryName() );
            }
            moreFiles = ndir->SetToNextEntry();
        }
        ndir->Close();
    }

    n_delete(ndir);
    return true;
}


//------------------------------------------------------------------------------
/**
*/
bool 
n3dsFileServer::QueueDeleteDirectory(const nString& path)
{
    bool result;
    nArray<nString> fileList;
    result  = GetFilesFrom(path , fileList );
    result |= QueueDeleteFiles( fileList );
    return result;
}
//------------------------------------------------------------------------------
/**
*/
bool 
n3dsFileServer::QueueDeleteFiles( const nArray<nString>& fileList)
{
    bool result = true;
    for ( int i = 0;  i < fileList.Size() ; ++i)
    {
        result |= QueueDeleteFile( fileList[i] );
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Enque the files for delete, replace the "wc:" for "realwc:"
*/
bool 
n3dsFileServer::QueueDeleteFile( const nString& fileName)
{
    nString file = fileName.ReplaceBegin( "wc:", realwc);
    fileData* node = filesTable[file];
    if ( 0 != node )
    {
        node->op = fileData::Delete;
        node->src = "";
    }
    else
    {
        fileData newNode;
        newNode.op = fileData::Delete;
        newNode.src = "";
        filesTable.Add( file, &newNode);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsFileServer::QueueCopyFile( const nString& from, const nString& to, bool dstRealwc )
{
    nString file;
    if ( dstRealwc )
    {
        file = to.ReplaceBegin( "wc:", realwc);
    } else
    {
        file = to.ReplaceBegin( "wc:", tmpwc);
    }

    fileData* node = filesTable[file];
    if ( 0 != node )
    {
        node->op = fileData::Copy;
        node->src = from.ReplaceBegin( "wc:", tmpwc);
    }
    else
    {
        fileData newNode;
        newNode.op = fileData::Copy;
        newNode.src = from.ReplaceBegin( "wc:", tmpwc);
        filesTable.Add( file, &newNode);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsFileServer::QueueCopyWC()
{
    nArray<nString> fileList;
    GetFilesFrom("wc:", fileList, false);
    for ( int i = 0; i < fileList.Size() ; ++i )
    {
        QueueCopyFile( fileList[i] , fileList[i].ReplaceBegin( tmpwc , realwc) );
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    @path the directy name, replace "wc" for tmp
    @return true if directory not exist or delete all files and directories return true
*/
bool 
n3dsFileServer::DeleteDirectory(const nString& path) const
{
    nString newPath = path.ReplaceBegin( "wc:", tmpwc );
    return DeleteDirectory2(newPath);
}


//------------------------------------------------------------------------------
/**

*/
bool 
n3dsFileServer::DeleteDirectory2(const nString& path) const
{
    bool result = true;
    nFileServer2* server = nFileServer2::Instance();
    // create and open directory
    nDirectory * ndir = server->NewDirectoryObject();
    n_assert(ndir);
    if (ndir->Open(path.Get()))
    {

        // scan all directories
        bool moreFiles = ndir->SetToFirstEntry();
        while ( moreFiles )
        {
            if ( ndir->GetEntryType() == nDirectory::DIRECTORY )
            {
                result |= DeleteDirectory2( ndir->GetEntryName() );
            }
            moreFiles = ndir->SetToNextEntry();
        }

        // scan all files
        moreFiles = ndir->SetToFirstEntry();
        while ( moreFiles )
        {
            if ( ndir->GetEntryType() == nDirectory::FILE )
            {
                bool val =  server->DeleteFile( ndir->GetEntryName() );
                N3DSERRORCOND( fileServer3ds, !val , (0, "ERROR: Don't delete file \"%s\"", ndir->GetEntryName() ) );
                result |= val;
            }
            moreFiles = ndir->SetToNextEntry();
        }
        ndir->Close();
    }
    server->DeleteDirectory( path.Get() );
    n_delete(ndir);

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsFileServer::Flush()
{
    bool result = true;
    int  size = filesTable.GetCount();
    if ( 0 == size )
    {
        return true;
    }

    fileData* node;
    nString fileName;
    float count = 0;
    bool tmp = true;
    nFileServer2* server = nFileServer2::Instance();
    nMapTableTypes<bool>::NString directoryTable(this->filesTable.GetCount() );
    directoryTable.Add( server->ManglePath(realwc , false )  , &tmp); // Add the root directoty of wc
    directoryTable.Add( server->ManglePath(tmpwc , false )  , &tmp); // Add the root directoty of wc
  
    n3dsExportServer::Instance()->ProgressBarStart( "Copy to WC" );
    for ( filesTable.Begin(), filesTable.Next( fileName, node ) ; 0 != node ; filesTable.Next( fileName, node )  )
    {
        n3dsExportServer::Instance()->ProgressUpdate( count / size , fileName );
        bool addSvn = fileName.IndexOf("realwc:",0) == 0;
        count++;

        if ( node->op == fileData::Delete )
        {
            DeleteFile(fileName , addSvn);
            
        } else
        {
            AddDirecttory( fileName.ExtractDirName() , addSvn, directoryTable ); // add directory to wc
            if ( fileName == node->src )
            {
                // Note not copy this files. It's a trick for no delete a file from delete directory
            } else
            {
                bool copied = server->CopyFile( node->src , fileName );
                if ( copied )
                {
                    AddFile(fileName, addSvn);
                } else
                {
                    nString mangledFromPath = server->ManglePath(node->src.Get(), true);
                    nString mangledToPath   = server->ManglePath(fileName.Get(), false);
                    N3DSWARN( exporter , (0 , "ERROR: not copy %s to %s ", node->src.Get(), fileName.Get()  ) );
                    N3DSWARN( exporter , (0 , "ERROR: not copy %s to %s ", mangledFromPath.Get(), mangledToPath.Get() ) );
                }
                result = result && copied;
            }
        }
    }

    n3dsExportServer::Instance()->ProgressBarEnd();

    return result;
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsFileServer::DeleteFile (const nString& fileName, bool addsvn)
{
    
    if ( addsvn)
    {
        n_assert(nKernelServer::ks);
        // precondition, The directory is in the svn repository
        n_assert(nKernelServer::ks);
        nAppLauncher appLauncher( nKernelServer::ks ); 
        nString name = nKernelServer::ks->GetFileServer()->ManglePath(fileName , false );
        nString dirName( name.ExtractDirName() );
        nString arguments(" delete " + name + " --force" );
        appLauncher.SetExecutable("svn.exe");
        appLauncher.SetWorkingDirectory( dirName.Get() );
        appLauncher.SetArguments( arguments.Get() );
        appLauncher.SetNoConsoleWindow( true );
        appLauncher.LaunchWait();

        // The subversion delete the file , delete it not necesary
    }
    else
    {
        nFileServer2::Instance()->DeleteFile( fileName.Get() );
    }
    
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsFileServer::AddFile(const nString& fileName, bool addsvn)
{
    if (addsvn)
    {
        // precondition, The directory is in the svn repository
        n_assert(nKernelServer::ks);
        nAppLauncher appLauncher( nKernelServer::ks ); 
        nString name = nKernelServer::ks->GetFileServer()->ManglePath(fileName , false );
        nString dirName( name.ExtractDirName() );
        nString arguments(" add " + name );
        appLauncher.SetExecutable("svn.exe");
        appLauncher.SetWorkingDirectory( dirName.Get() );
        appLauncher.SetArguments( arguments.Get() );
        appLauncher.SetNoConsoleWindow( true );
        appLauncher.LaunchWait();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsFileServer::AddDirecttory(const nString& dirName, bool addsvn, nMapTableTypes<bool>::NString& directoryTable)
{
    n_assert( !dirName.IsEmpty() );
    n_assert(nKernelServer::ks);
    nAppLauncher appLauncher( nKernelServer::ks );
    nString mangledDirName( nFileServer2::Instance()->ManglePath( dirName , false ) );

    if ( ! directoryTable[mangledDirName] ) // if it not exist
    {
        bool tmp = true;
        directoryTable.Add( mangledDirName, &tmp) ;
        nString parentDirName( mangledDirName.ExtractDirName() );
        AddDirecttory( parentDirName , addsvn, directoryTable ); // add first the parent directory

        if ( !nFileServer2::Instance()->DirectoryExists( mangledDirName.Get() ) )
        {
            nFileServer2::Instance()->MakePath( mangledDirName.Get() );
        }

        // Add in subversion 
        if (addsvn)
        {
            nString arguments(" add ");
            arguments += mangledDirName;
            arguments += " -N"; //non recursive , only add the directory

            appLauncher.SetExecutable("svn.exe");
            appLauncher.SetWorkingDirectory( parentDirName.Get() );
            appLauncher.SetArguments( arguments.Get() );
            appLauncher.SetNoConsoleWindow( true );
            appLauncher.LaunchWait();
        }
    }

}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------