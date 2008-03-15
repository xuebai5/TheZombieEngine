//------------------------------------------------------------------------------
//  nmemfile.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "file/nmemfile.h"

//------------------------------------------------------------------------------
/**
*/
nMemFile::nMemFile() :
    buffer(0),
    fileSize(0),
    allocSize(0),
    growSize(4096),
    filePointer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMemFile::~nMemFile()
{
    this->Destroy();
}

//------------------------------------------------------------------------------
/**
    Opens the memory file.
    history:
     - 27-Jan-2005  ma.garcias  reset size if opening for write.
*/
bool
nMemFile::Open(const nString & /*filename*/, const char* accessMode)
{
    n_assert(!this->IsOpen());
    n_assert(accessMode);
    if (strchr(accessMode,'w') || strchr(accessMode,'W'))
    {
        this->Destroy();
    }

    this->filePointer = 0;
    this->lineNumber = 0;
    this->isOpen = true;

    return true;
}


//------------------------------------------------------------------------------
/**
    closes the file

    history:
     - 30-Jan-2002   peter   created
     - 11-Feb-2002   floh    Linux stuff
*/
void
nMemFile::Close()
{
    n_assert(this->IsOpen());

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    grow buffer to required size, in growSize increments.
*/
void
nMemFile::GrowTo(uint newAllocSize)
{
    if ( this->allocSize < newAllocSize )
    {
        while( this->allocSize < newAllocSize )
        {
            this->allocSize += growSize;
        }
    
        char *ptr = (char *) n_malloc( this->allocSize + 1 );
        if (this->buffer)
        {
            memcpy(ptr, this->buffer, this->fileSize);
            n_free(this->buffer);
        }

        this->buffer = ptr;
        ptr[this->fileSize] = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Append the contents of another file to this file. This and the 'other' file
    must both be open! Returns number of bytes copied.

    @return     number of bytes appended
*/
int
nMemFile::AppendFile(nFile* other,int inputNumBytes)
{
    n_assert(this->IsOpen());
    n_assert(other);
    int numBytes(inputNumBytes >= 0 ?  inputNumBytes : other->GetSize() );

    if (numBytes > 0)
    {
        uint rlen = numBytes;
        uint tlen = this->fileSize + rlen;

        this->GrowTo( tlen );

        char *ptr = (char *) this->buffer;
        ptr += this->fileSize;
       
        n_verify( other->Read(ptr, numBytes) == numBytes);

        ptr[tlen] = 0;
        this->fileSize = tlen;
        return rlen;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    writes a number of bytes to the file
*/
int
nMemFile::Write(const void* buffer, int numBytes)
{
    n_assert(this->IsOpen());
    if (numBytes > 0)
    {
        uint rlen = numBytes;
        uint tlen = this->fileSize + rlen;

        this->GrowTo( tlen );

        char *ptr = (char *) this->buffer;
        memcpy(ptr + this->fileSize, buffer, rlen);
        ptr[tlen] = 0;
        this->fileSize = tlen;
        return rlen;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    reads a number of bytes from the file
*/
int
nMemFile::Read(void* buffer, int numBytes)
{
    n_assert(this->IsOpen());

    uint rlen = numBytes;
    if( this->filePointer + numBytes > this->fileSize )
    {
        rlen = this->fileSize - this->filePointer;
    }
    char *buf = (char *)this->buffer;
    memcpy(buffer, &buf[filePointer], rlen);
    this->filePointer += rlen;
    return rlen;
}

//------------------------------------------------------------------------------
/**
    gets current position of file pointer
*/
int 
nMemFile::Tell() const
{
    n_assert(this->IsOpen());
    return this->filePointer;
}

//------------------------------------------------------------------------------
/**
    Sets the file pointer to given absolute or relative position.
    if position is absolute, 0 <= byteOffset < fileSize
*/
bool
nMemFile::Seek(int byteOffset, nSeekType origin)
{
    n_assert(this->IsOpen());
    switch(origin)
    {
    case CURRENT:
        this->filePointer += byteOffset;
        break;
    case START:
        this->filePointer = byteOffset;
        break;
    case END:
        this->filePointer = this->fileSize - byteOffset;
        break;
    default:
        n_assert_always();
        break;
    }

    // Keep position in bounds.
    if (this->filePointer < 0)
    {
        this->filePointer = 0;
    }
    if (this->filePointer > this->fileSize)
    {
        this->filePointer = this->fileSize;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Check for end-of-file. file pointer points to the last available byte + 1.
*/
bool 
nMemFile::Eof()
{
    n_assert(this->IsOpen());
    return this->filePointer >= this->fileSize;
}

//------------------------------------------------------------------------------
/**
    Returns size of file in bytes.
*/
int
nMemFile::GetSize() const
{
    n_assert(this->IsOpen());
    return this->fileSize;
}

//------------------------------------------------------------------------------
/**
    Returns time of last write access. The file must be opened in "read" mode
    before this function can be called!
*/
nFileTime
nMemFile::GetLastWriteTime() const
{
    n_assert(this->IsOpen());
    return this->fileTime;
}

//------------------------------------------------------------------------------
/**
*/
void
nMemFile::Destroy()
{
    if (this->buffer)
    {
        n_free( this->buffer );
        this->allocSize = 0;
        this->fileSize = 0;
        this->buffer = 0;
    }
}
