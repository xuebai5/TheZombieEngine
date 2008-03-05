#ifndef N_RLEFILE_H
#define N_RLEFILE_H

//------------------------------------------------------------------------------
/**
    @class nRleFile
    @ingroup NebulaFileManagement
    @brief A run length encoder/decoder.
    @author Carles Ros

    The run length compression algorithm is based on recording how many times a
    same value is repeated consecutively. Basically, in the encoded file a value
    will be followed by a run length code, which indicates the amount that this
    values repeats. To avoid excessive run length codes, only when there's at
    least two consecutive repeated values a run length code will follow up.
    To know if what follows up to a value is another value or a run length code,
    at least two consecutive values are always writen, writing always a run
    length code after two repeated consecutive values.

    Examples:

      - god -> god
      - good -> goo0d
      - goood -> goo1d
      - gooood -> goo2d
    
    This class inherits from nMemFile to encode/decode files in memory while
    keeping the same file access interface. More exactly, files opened for
    reading are completely read and decoded to memory and successive file access
    operations are done to the memory decoded file instead of the physic encoded
    file. For files opened for writing all file operations are done to the
    memory file, only doing the encode and write to disk process when the file
    is being closed.

    This class can also be used on already opened files by means of the two
    static methods EncodeFile and DecodeFile. These two methods will
    encode/decode a file from an already opened source file towards an already
    opened target file. Those files can be either physic or memory files.

    (C) 2005 Conjurer Services, S.A.
*/

#include "file/nmemfile.h"

//------------------------------------------------------------------------------
class nRleFile : public nMemFile
{
public:
    /// Default constructor
    nRleFile();
    /// Destructor
    ~nRleFile();

    /// Open a file
    virtual bool Open( const nString & fileName, const char* accessMode );
    /// Close the file
    void Close();
    /// Same as Close, but indicating if the file has been successfully writen
    bool Close2();

    /// Write a run length encoded version of a file
    static bool EncodeFile( nFile* sourceFile, nFile* targetFile );
    /// Write a run length decoded version of a file
    static bool DecodeFile( nFile* sourceFile, nFile* targetFile );

private:
    nString fileName;
    bool writeMode;
};

#endif
