#ifndef N_NAVMESHFILE_H
#define N_NAVMESHFILE_H

//------------------------------------------------------------------------------
/**
    @class nNavMeshFile
    @ingroup NebulaNavmeshSystem

    Navigation mesh file reader/writer base interface.

    Readers and writers inherit from this base class and implement read and write
    operations. Each value writen with some write operation must be read with
    its read operation counterpart.

    (C) 2005 Conjurer Services, S.A.
*/

#include "mathlib/vector.h"

class NavTag;

//------------------------------------------------------------------------------
struct nNavMeshFile
{
    /// Open a file with the correct access mode, returning true if file opened with success
    virtual bool OpenFile(const char* filename) = 0;
    // Close the file, returning false only if some final validation fails (syntax checking for instance)
    virtual bool CloseFile() = 0;

    /// Parse the beginning of a block
    virtual bool ParseBlockStart(const NavTag& tag) = 0;
    /// Parse the ending of a block
    virtual bool ParseBlockEnd(const NavTag& tag) = 0;
    /// Parse the beginning of a line block
    virtual bool ParseLineBlockStart(const NavTag& tag) = 0;
    /// Parse the ending of a line block
    virtual bool ParseLineBlockEnd(const NavTag& tag) = 0;

    /// Parse the file format code
    virtual bool ParseFourCC() = 0;
    /// Parse a 32 bits integer
    virtual bool ParseInt32(const NavTag& tag, int& value) = 0;
    /// Parse a 16 bits integer
    virtual bool ParseInt16(const NavTag& tag, int& value) = 0;
    /// Parse a 16 bits integer within a line block
    virtual bool ParseInt16InLineBlock(int& value) = 0;
    /// Parse a 8 bits integer
    virtual bool ParseInt8(const NavTag& tag, int& value) = 0;
    /// Parse a vector
    virtual bool ParseVector3(const NavTag& tag, vector3& vector) = 0;

    /// Get code used to identify an ascii file format
    static const char* GetAsciiFourCC();
    /// Get code used to identify a binary file format
    static const int GetBinFourCC();
};

//------------------------------------------------------------------------------
#endif
