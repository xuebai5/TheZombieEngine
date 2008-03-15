#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  nnavmeshfilewriter.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nnavmeshparser/nnavmeshfilewriter.h"
#include "kernel/nfileserver2.h"
#include "file/nrlefile.h"
#include "util/nstring.h"
#include "nnavmeshparser/navtag.h"
#undef min
#undef max
#include <limits>

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMeshAscWriter::nNavMeshAscWriter()
    : file(NULL), indent_level(0)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nNavMeshAscWriter::~nNavMeshAscWriter()
{
    if ( this->file )
    {
        this->file->Release();
    }
}

//------------------------------------------------------------------------------
/**
    OpenFile
*/
bool
nNavMeshAscWriter::OpenFile(const char* filename)
{
    n_assert(!this->file);
    n_assert(filename);

    this->file = nFileServer2::Instance()->NewFileObject();
    n_assert(this->file);
    if ( this->file->Open(filename, "w") )
    {
        this->indent_level = 0;
        return true;
    }
    else
    {
        n_printf( "nNavMeshAscWriter::OpenFile(): failed to open file '%s' for writing!\n", filename );
        this->file->Release();
        this->file = NULL;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    CloseFile
*/
bool
nNavMeshAscWriter::CloseFile()
{
    n_assert(this->file);

    this->file->Close();
    this->file->Release();

    // A little syntax check: assert the indenting rules have been followed
    return this->indent_level == 0;
}

//------------------------------------------------------------------------------
/**
    ParseBlockStart
*/
bool
nNavMeshAscWriter::ParseBlockStart(const NavTag& tag)
{
    n_assert(this->file);

    this->WriteIndentation();
    this->file->PutS(tag.AsString().Get());
    this->file->PutS(":\n");
    this->IncIndentation();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseBlockEnd
*/
bool
nNavMeshAscWriter::ParseBlockEnd(const NavTag& /*tag*/)
{
    this->DecIndentation();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockStart
*/
bool
nNavMeshAscWriter::ParseLineBlockStart(const NavTag& tag)
{
    n_assert(this->file);

    this->WriteIndentation();
    this->file->PutS(tag.AsString().Get());
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockEnd
*/
bool
nNavMeshAscWriter::ParseLineBlockEnd(const NavTag& /*tag*/)
{
    n_assert(this->file);

    this->file->PutS("\n");
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseFourCC
*/
bool
nNavMeshAscWriter::ParseFourCC()
{
    this->WriteField(NavTag::FileFormatType, this->GetAsciiFourCC());
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt32
*/
bool
nNavMeshAscWriter::ParseInt32(const NavTag& tag, int& value)
{
    this->WriteField(tag, nString(value).Get());
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt16
*/
bool
nNavMeshAscWriter::ParseInt16(const NavTag& tag, int& value)
{
    return this->ParseInt32(tag, value);
}

//------------------------------------------------------------------------------
/**
    ParseInt16InLineBlock
*/
bool
nNavMeshAscWriter::ParseInt16InLineBlock(int& value)
{
    n_assert(this->file);

    this->file->PutS(" ");
    this->file->PutS(nString(value).Get());
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt8
*/
bool
nNavMeshAscWriter::ParseInt8(const NavTag& tag, int& value)
{
    return this->ParseInt32(tag, value);
}

//------------------------------------------------------------------------------
/**
    ParseVector3
*/
bool
nNavMeshAscWriter::ParseVector3(const NavTag& tag, vector3& value)
{
    n_assert(this->file);

    this->WriteIndentation();
    this->file->PutS(tag.AsString().Get());
    this->file->PutS(" ");
    this->file->PutS(nString(value.x).Get());
    this->file->PutS(" ");
    this->file->PutS(nString(value.y).Get());
    this->file->PutS(" ");
    this->file->PutS(nString(value.z).Get());
    this->file->PutS("\n");
    return true;
}

//------------------------------------------------------------------------------
/**
    WriteField
*/
void
nNavMeshAscWriter::WriteField(const NavTag& tag, const char* value)
{
    n_assert(this->file);

    this->WriteIndentation();
    this->file->PutS(tag.AsString().Get());
    this->file->PutS(" ");
    this->file->PutS(value);
    this->file->PutS("\n");
}

//------------------------------------------------------------------------------
/**
    IncIndentation
*/
void
nNavMeshAscWriter::IncIndentation()
{
    this->indent_level++;
}

//------------------------------------------------------------------------------
/**
    DecIndentation
*/
void
nNavMeshAscWriter::DecIndentation()
{
    this->indent_level--;
}

//------------------------------------------------------------------------------
/**
    WriteIndentation
*/
void
nNavMeshAscWriter::WriteIndentation()
{
    n_assert(this->file);

    for ( int i = 0; i < this->indent_level; ++i )
    {
        this->file->PutChar('\t');
    }
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMeshBinWriter::nNavMeshBinWriter()
    : file(NULL)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nNavMeshBinWriter::~nNavMeshBinWriter()
{
    if ( this->file )
    {
        this->file->Release();
    }
}

//------------------------------------------------------------------------------
/**
    OpenFile
*/
bool
nNavMeshBinWriter::OpenFile(const char* filename)
{
    n_assert(!this->file);
    n_assert(filename);

    this->file = nFileServer2::Instance()->NewFileObject();
    n_assert(this->file);
    if ( this->file->Open(filename, "wb") )
    {
        return true;
    }
    else
    {
        n_printf( "nNavMeshBinWriter::OpenFile(): failed to open file '%s' for writing!\n", filename );
        this->file->Release();
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    CloseFile
*/
bool
nNavMeshBinWriter::CloseFile()
{
    n_assert(this->file);

    this->file->Close();
    this->file->Release();

    return true;
}

//------------------------------------------------------------------------------
/**
    ParseBlockStart
*/
bool
nNavMeshBinWriter::ParseBlockStart(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseBlockEnd
*/
bool
nNavMeshBinWriter::ParseBlockEnd(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockStart
*/
bool
nNavMeshBinWriter::ParseLineBlockStart(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockEnd
*/
bool
nNavMeshBinWriter::ParseLineBlockEnd(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseFourCC
*/
bool
nNavMeshBinWriter::ParseFourCC()
{
    n_assert(this->file);

    return this->file->PutInt( this->GetBinFourCC() ) == sizeof(this->GetBinFourCC());
}

//------------------------------------------------------------------------------
/**
    ParseInt32
*/
bool
nNavMeshBinWriter::ParseInt32(const NavTag& /*tag*/, int& value)
{
    n_assert(this->file);

    return this->file->PutInt(value) == sizeof(int);
}

//------------------------------------------------------------------------------
/**
    ParseInt16
*/
bool
nNavMeshBinWriter::ParseInt16(const NavTag& /*tag*/, int& value)
{
    n_assert(this->file);
    n_assert( value >= std::numeric_limits<short>::min() && value <= std::numeric_limits<short>::max() );

    return this->file->PutShort(short(value)) == sizeof(short);
}

//------------------------------------------------------------------------------
/**
    ParseInt16InLineBlock
*/
bool
nNavMeshBinWriter::ParseInt16InLineBlock(int& value)
{
    n_assert(this->file);
    n_assert( value >= std::numeric_limits<short>::min() && value <= std::numeric_limits<short>::max() );

    return this->file->PutShort(short(value)) == sizeof(short);
}

//------------------------------------------------------------------------------
/**
    ParseInt8
*/
bool
nNavMeshBinWriter::ParseInt8(const NavTag& /*tag*/, int& value)
{
    n_assert(this->file);
    n_assert( value >= std::numeric_limits<char>::min() && value <= std::numeric_limits<char>::max() );

    return this->file->PutChar(char(value)) == sizeof(char);
}

//------------------------------------------------------------------------------
/**
    ParseVector3
*/
bool
nNavMeshBinWriter::ParseVector3(const NavTag& /*tag*/, vector3& value)
{
    n_assert(this->file);

    if ( this->file->PutFloat(value.x) != sizeof(value.x) )
    {
        return false;
    }
    if ( this->file->PutFloat(value.y) != sizeof(value.y) )
    {
        return false;
    }
    if ( this->file->PutFloat(value.z) != sizeof(value.z) )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    OpenFile
*/
bool
nNavMeshRleWriter::OpenFile(const char* filename)
{
    n_assert(!this->file);
    n_assert(filename);

    this->file = n_new( nRleFile );
    n_assert(this->file);
    if ( this->file->Open(filename, "wb") )
    {
        return true;
    }
    else
    {
        n_printf( "nNavMeshRleWriter::OpenFile(): failed to open file '%s' for writing!\n", filename );
        this->file->Release();
        return false;
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
