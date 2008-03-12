#ifndef N_3DS_FILSERVER_H
#define N_3DS_FILSERVER_H
#include "util/narray.h"
#include "util/nmaptabletypes.h"

//-----------------------------------------------------------------------------
/**
    @class n3dsFileServer
    @ingroup n3dsMaxExporterKernel
    @brief manager of diferents workings copy
    automaticaly change "wc:" by temporal wc or original wc use a "WCREAL:" or "WCTMP"
    Only delete files or copy when you call a flush method();
*/
class nString;
class n3dsFileServer 
{
public:
    /// return all files down this path, from original wc
    bool GetFilesFrom( const nString& path , nArray<nString>& fileList, bool realwc = true) const;
    /// enqueue delete file , delete in original wc 
    bool QueueDeleteFile( const nString& filename);
    /// enqueue delete file , delete in original wc
    bool QueueDeleteFiles( const nArray<nString>& fileList);
    /// enque delete recursive directory, delete in original wc, only delete files
    bool QueueDeleteDirectory(const nString& path);
    /// copy file from temporal  to original
    bool QueueCopyFile( const nString& from, const nString& to, bool dstRealwc = true);
    /// copy all files from temporal directory to original directory
    bool QueueCopyWC();
    /// Delete RecursisceDirectory
    bool DeleteDirectory(const nString& path) const;
    /// Flush the Queue
    bool Flush();

    ///
    void AddFile(const nString& fileName, bool addsvn);

private:
    /// return all files down this path, from original wc
    bool GetFilesFrom2( const nString& path , nArray<nString>& fileList) const;
    /// Delete Recursisve Directory
    bool DeleteDirectory2(const nString& path) const;
    /// delete 
    void DeleteFile (const nString& fileName, bool addsvn);
    /////
    //void AddFile(const nString& fileName, bool addsvn);
    /// 
    void AddDirecttory(const nString& dirName, bool addsvn, nMapTableTypes<bool>::NString& directoryTable);

    struct fileData
    {
        enum Operation
        {
            /// Delete File
            Delete = 0,
            /// Copy File
            Copy,
        };
        /// The operation
        Operation op;
        /// src file for the copy operation
        nString src;
    };
    /// replace the "wc:" by from

    /// the key is the fileName in original working copy
    nMapTableTypes<fileData>::NString filesTable;

    static const nString realwc;
    static const nString tmpwc;
};

#endif
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------