#ifndef N_NAVMESHFILEREADER_H
#define N_NAVMESHFILEREADER_H

//------------------------------------------------------------------------------
/**
    @file nnavmeshfilewriter
    @ingroup NebulaNavmeshSystem

    Navigation mesh file reader classes.

    (C) 2005 Conjurer Services, S.A.
*/

#include "nnavmeshparser/nnavmeshfile.h"

class nString;
class nFile;

//------------------------------------------------------------------------------
/**
    Navigation mesh file reader interface
*/
struct nNavMeshFileReader : public nNavMeshFile
{
    // Empty, class used just for type checking
};

//------------------------------------------------------------------------------
/**
    Navigation mesh file reader for ASCII file format
*/
class nNavMeshAscReader : public nNavMeshFileReader
{
public:
    /// Constructor
    nNavMeshAscReader();
    /// Destructor
    virtual ~nNavMeshAscReader();

    /// Open a file for ASCII reading, returning true if file opened with success
    virtual bool OpenFile(const char* filename);
    /// Close the file and return true
    virtual bool CloseFile();

    /// Read the start of a block (really skip it)
    virtual bool ParseBlockStart(const NavTag& tag);
    /// Read the end of a block (really skip it)
    virtual bool ParseBlockEnd(const NavTag& tag);
    /// Read the start of a line block (really skip it)
    virtual bool ParseLineBlockStart(const NavTag& tag);
    /// Read the end of a line block (really skip it)
    virtual bool ParseLineBlockEnd(const NavTag& tag);

    /// Read the file format code, returning true if it's the valid for this reader
    virtual bool ParseFourCC();
    /// Read a 32 bits integer from file
    virtual bool ParseInt32(const NavTag& tag, int& value);
    /// Read a 16 bits integer from file
    virtual bool ParseInt16(const NavTag& tag, int& value);
    /// Read a 16 bits integer within a line block from file
    virtual bool ParseInt16InLineBlock(int& value);
    /// Read a 8 bits integer from file
    virtual bool ParseInt8(const NavTag& tag, int& value);
    /// Read a vector from file
    virtual bool ParseVector3(const NavTag& tag, vector3& value);

private:
    /// Read the next word delimited by ' ' or '\n'
    void ReadWord(nString& str);
    /// Read the word pair (<tag>,<value>) and return the <value> word
    bool ReadField(const NavTag& tag, nString& str);

    /// File from where to read
    nFile* file;
};

//------------------------------------------------------------------------------
/**
    Navigation mesh file reader for binary file format
*/
class nNavMeshBinReader : public nNavMeshFileReader
{
public:
    /// Constructor
    nNavMeshBinReader();
    /// Destructor
    virtual ~nNavMeshBinReader();

    /// Open a file for binary reading, returning true if file opened with success
    virtual bool OpenFile(const char* filename);
    /// Close the file and return true
    virtual bool CloseFile();

    /// Read the start of a block (really skip it)
    virtual bool ParseBlockStart(const NavTag& tag);
    /// Read the end of a block (really skip it)
    virtual bool ParseBlockEnd(const NavTag& tag);
    /// Read the start of a line block (really skip it)
    virtual bool ParseLineBlockStart(const NavTag& tag);
    /// Read the end of a line block (really skip it)
    virtual bool ParseLineBlockEnd(const NavTag& tag);

    /// Read the file format code, returning true if it's the valid for this reader
    virtual bool ParseFourCC();
    /// Read a 32 bits integer from file
    virtual bool ParseInt32(const NavTag& tag, int& value);
    /// Read a 16 bits integer from file
    virtual bool ParseInt16(const NavTag& tag, int& value);
    /// Read a 16 bits integer within a line block from file
    virtual bool ParseInt16InLineBlock(int& value);
    /// Read a 8 bits integer from file
    virtual bool ParseInt8(const NavTag& tag, int& value);
    /// Read a vector from file
    virtual bool ParseVector3(const NavTag& tag, vector3& value);

protected:
    /// File from where to read
    nFile* file;
};

//------------------------------------------------------------------------------
/**
    Navigation mesh file reader for binary run length compressed file format
*/
class nNavMeshRleReader : public nNavMeshBinReader
{
public:
    /// Open a file for reading, returning true if file opened with success
    virtual bool OpenFile(const char* filename);
};

//------------------------------------------------------------------------------
#endif
