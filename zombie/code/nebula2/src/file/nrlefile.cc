//------------------------------------------------------------------------------
//  nrlefile.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "file/nrlefile.h"

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nRleFile::nRleFile() :
    writeMode(false)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nRleFile::~nRleFile()
{
    if ( this->IsOpen() )
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open a file
*/
bool nRleFile::Open(const nString & fileName, const char* accessMode )
{
    n_assert_return( !fileName.IsEmpty(), false );
    n_assert_return( accessMode, false );

    this->fileName = fileName;
    this->writeMode = nString(accessMode).FindChar('w', 0) != -1;

    // Init memory file
    nString emptyFilename;
    bool success = nMemFile::Open( emptyFilename, "r" );

    // If opening a file for read, read and decode it entirely to memory
    if ( !this->writeMode && success )
    {
        nFile* sourceFile = nFileServer2::Instance()->NewFileObject();
        n_assert( sourceFile );
        success = sourceFile->Open( fileName, "rb" );
        if ( success )
        {
            success = this->DecodeFile( sourceFile, this );
            sourceFile->Close();
            this->Seek( 0, nFile::START );
        }
        else
        {
            nMemFile::Close();
        }
        sourceFile->Release();
    }

    return success;
}

//------------------------------------------------------------------------------
/**
    Close the file
*/
void nRleFile::Close()
{
    this->Close2();
}

//------------------------------------------------------------------------------
/**
    Same as Close, but indicating if the file has been successfully writen
*/
bool nRleFile::Close2()
{
    if ( !this->IsOpen() )
    {
        return false;
    }

    bool success( true );

    // If writing a file, encode and write it entirely from memory
    if ( this->writeMode )
    {
        nFile* targetFile = nFileServer2::Instance()->NewFileObject();
        n_assert( targetFile );
        success = targetFile->Open( this->fileName, "wb" );
        if ( success )
        {
            this->Seek( 0, nFile::START );
            success = this->EncodeFile( this, targetFile );
            targetFile->Close();
        }
        targetFile->Release();
    }

    // Clear memory file
    nMemFile::Close();

    return success;
}

//------------------------------------------------------------------------------
/**
    Write a run length encoded version of a file
*/
bool nRleFile::EncodeFile( nFile* sourceFile, nFile* targetFile )
{
    // Copy whole file to memory
    int size = sourceFile->GetSize();
    char* buf = n_new(char)[size];
    size = sourceFile->Read( buf, size );

    // Encode the file
    if ( size > 0 )
    {
        targetFile->Write( buf, 1 );
        --size;
        unsigned int runlength(0);
        char* prev = buf;
        char* next = buf + 1;
        for ( ; size > 0; ++prev, ++next, --size )
        {
            if ( *prev == *next )
            {
                if ( runlength == 0 )
                {
                    // Repeat value so decode knows that 2 equal values indicates that
                    // a runlength code follows up
                    targetFile->Write( next, 1 );
                    ++runlength;
                }
                else if ( runlength == 256 )
                {
                    // Write runlength code even if there's still more repeated values,
                    // since the runlength is limitted to 256
                    unsigned char code( 255 );
                    targetFile->Write( &code, 1 );
                    runlength = 0;
                    targetFile->Write( next, 1 );
                }
                else
                {
                    // Keep counting repeated values
                    ++runlength;
                }
            }
            else
            {
                if ( runlength > 0 )
                {
                    // Write the runlength code only when two equal consecutive values are found
                    n_assert( runlength >= 1 && runlength <= 256 );
                    unsigned char code = static_cast<unsigned char>( runlength - 1 );
                    targetFile->Write( &code, 1 );
                    runlength = 0;
                }
                targetFile->Write( next, 1 );
            }
       } 
    }

    n_delete( buf );
    return true;
}

//------------------------------------------------------------------------------
/**
    Write a run length decoded version of a file
*/
bool nRleFile::DecodeFile( nFile* sourceFile, nFile* targetFile )
{
    // Copy whole file to memory
    int size = sourceFile->GetSize();
    unsigned char* buf = n_new(unsigned char)[size];
    size = sourceFile->Read( buf, size );

    // Decode the file
    if ( size > 0 )
    {
        targetFile->Write( buf, 1 );
        --size;
        unsigned char* prev = buf;
        unsigned char* next = buf + 1;
        unsigned char repetition = 1;
        for ( ; size > 0; ++prev, ++next, --size )
        {
            if ( repetition == 2 )
            {
                // 2 repeated values are followed by a run length code,
                // so read it and write the same value the amount indicated by the run length
                for ( unsigned char runlength = *next; runlength > 0; --runlength )
                {
                    targetFile->Write( prev, 1 );
                }
                repetition = 0;
            }
            else if ( repetition == 0 )
            {
                // Previous value was a run length code, so now follows a non run length code
                // which must be writen direct to target file
                targetFile->Write( next, 1 );
                ++repetition;
            }
            else
            {
                n_assert( repetition == 1 );
                if ( *prev == *next )
                {
                    // Two repetead values, mark it so next iteration knows that a run length code follows up
                    repetition = 2;
                }
                else
                {
                    // A diferent value, no run length code will follow up
                    repetition = 1;
                }
                // The two values following a run length code are always writen
                targetFile->Write( next, 1 );
            }
        }
    }

    n_delete( buf );
    return true;
}
