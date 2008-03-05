#ifndef N_MEMFILE_H
#define N_MEMFILE_H
//------------------------------------------------------------------------------
/**
    @class nFile
    @ingroup NebulaFileManagement

    Wrapper for accessing a memory buffer as a file.

    (C) 2004 Conjurer Services, S.A.
*/
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nMemFile : public nFile 
{
public:
    /// constructor
    nMemFile();
    /// destructor
    virtual ~nMemFile();
    /// opens a file
    virtual bool Open(const nString & fileName, const char* accessMode);
    /// closes the file
    virtual void Close();

    /// writes some bytes to the file
    virtual int Write(const void* buffer, int numBytes);
    /// reads some bytes from the file
    virtual int Read(void* buffer, int numBytes);

    /// append one file to another file
    virtual int AppendFile(nFile* other, int numBytes = -1);

    /// gets actual position in file
    virtual int Tell() const;
    /// sets new position in file
    virtual bool Seek(int byteOffset, nSeekType origin);
    /// is the file at the end
    virtual bool Eof();
    /// get size of file in bytes
    virtual int GetSize() const;
    /// get the last write time
    virtual nFileTime GetLastWriteTime() const;

protected:
    /// clear buffer
    void Destroy();
    /// grow buffer
    void GrowTo(uint newAllocSize);

    void *buffer;
    uint fileSize;        // size of file in bytes.
    uint growSize;        // grow by this size if buffer exhausted.
    uint allocSize;       // number of allocated bytes.
    uint filePointer;     // pointer to file buffer.
    nFileTime fileTime;
};

//------------------------------------------------------------------------------
#endif