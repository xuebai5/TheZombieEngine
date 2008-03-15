#ifndef N_NAVMESHFILEWRITER_H
#define N_NAVMESHFILEWRITER_H

//------------------------------------------------------------------------------
/**
    @file nnavmeshfilewriter
    @ingroup NebulaNavmeshSystem

    Navigation mesh file writer classes.

    (C) 2005 Conjurer Services, S.A.
*/

#include "nnavmeshparser/nnavmeshfile.h"

class nFile;

//------------------------------------------------------------------------------
/**
    Navigation mesh file writer interface
*/
struct nNavMeshFileWriter : public nNavMeshFile
{
    // Empty, class used just for type checking
};

//------------------------------------------------------------------------------
/**
    Navigation mesh file writer for ASCII file format
*/
class nNavMeshAscWriter : public nNavMeshFileWriter
{
public:
    /// Constructor
    nNavMeshAscWriter();
    /// Destructor
    virtual ~nNavMeshAscWriter();

    /// Create/Overwrite a file for ASCII writing, returning true if file created with success
    virtual bool OpenFile(const char* filename);
    /// Close the file, returning false if it detects a bad syntax indentation
    virtual bool CloseFile();

    /// Write the start of a block
    virtual bool ParseBlockStart(const NavTag& tag);
    /// Write the end of a block
    virtual bool ParseBlockEnd(const NavTag& tag);
    /// Write the start of a line block
    virtual bool ParseLineBlockStart(const NavTag& tag);
    /// Write the end of a line block
    virtual bool ParseLineBlockEnd(const NavTag& tag);

    /// Write the file format code for this writer
    virtual bool ParseFourCC();
    /// Write a 32 bits integer to file
    virtual bool ParseInt32(const NavTag& tag, int& value);
    /// Write a 16 bits integer to file
    virtual bool ParseInt16(const NavTag& tag, int& value);
    /// Write a 16 bits integer within a line block to file
    virtual bool ParseInt16InLineBlock(int& value);
    /// Write a 8 bits integer to file
    virtual bool ParseInt8(const NavTag& tag, int& value);
    /// Write a vector to file
    virtual bool ParseVector3(const NavTag& tag, vector3& value);

private:
    /// Write in a single line the pair <tag> <value>
    void WriteField(const NavTag& tag, const char* value);

    /// Increment the current indentation level
    void IncIndentation();
    /// Decrement the current indentation level
    void DecIndentation();
    /// Write the spaces needed to match the current indentation level
    void WriteIndentation();

    /// Used to have a more readable file by indenting
    int indent_level;

    /// File where to write
    nFile* file;
};

//------------------------------------------------------------------------------
/**
    Navigation mesh file writer for binary file format
*/
class nNavMeshBinWriter : public nNavMeshFileWriter
{
public:
    /// Constructor
    nNavMeshBinWriter();
    /// Destructor
    virtual ~nNavMeshBinWriter();

    /// Create/Overwrite a file for binary writing, returning true if file created with success
    virtual bool OpenFile(const char* filename);
    /// Close the file
    virtual bool CloseFile();

    /// Write the start of a block
    virtual bool ParseBlockStart(const NavTag& tag);
    /// Write the end of a block
    virtual bool ParseBlockEnd(const NavTag& tag);
    /// Write the start of a line block
    virtual bool ParseLineBlockStart(const NavTag& tag);
    /// Write the end of a line block
    virtual bool ParseLineBlockEnd(const NavTag& tag);

    /// Write the file format code for this writer
    virtual bool ParseFourCC();
    /// Write a 32 bits integer to file
    virtual bool ParseInt32(const NavTag& tag, int& value);
    /// Write a 16 bits integer to file
    virtual bool ParseInt16(const NavTag& tag, int& value);
    /// Write a 16 bits integer within a line block to file
    virtual bool ParseInt16InLineBlock(int& value);
    /// Write a 8 bits integer to file
    virtual bool ParseInt8(const NavTag& tag, int& value);
    /// Write a vector to file
    virtual bool ParseVector3(const NavTag& tag, vector3& value);

protected:
    /// File where to write
    nFile* file;
};

//------------------------------------------------------------------------------
/**
    Navigation mesh file writer for binary run length compressed file format
*/
class nNavMeshRleWriter : public nNavMeshBinWriter
{
public:
    /// Create/Overwrite a file for writing, returning true if file created with success
    virtual bool OpenFile(const char* filename);
};

//------------------------------------------------------------------------------
#endif
