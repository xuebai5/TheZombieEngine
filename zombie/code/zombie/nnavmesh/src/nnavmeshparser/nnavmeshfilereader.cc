#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  nnavmeshfilereader.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/nnavmeshfilereader.h"
#include "kernel/nfileserver2.h"
#include "file/nrlefile.h"
#include "util/nstring.h"
#include "nnavmeshparser/navtag.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMeshAscReader::nNavMeshAscReader()
    : file(NULL)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nNavMeshAscReader::~nNavMeshAscReader()
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
nNavMeshAscReader::OpenFile(const char* filename)
{
    n_assert(!this->file);
    n_assert(filename);

    this->file = nFileServer2::Instance()->NewFileObject();
    n_assert(this->file);
    if ( this->file->Open(filename, "r") )
    {
        return true;
    }
    else
    {
        n_printf( "nNavMeshAscReader::OpenFile(): failed to open file '%s' for reading!\n", filename );
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
nNavMeshAscReader::CloseFile()
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
nNavMeshAscReader::ParseBlockStart(const NavTag& tag)
{
    nString str;
    this->ReadWord(str);
    return str == tag.AsString() + ":";
}

//------------------------------------------------------------------------------
/**
    ParseBlockEnd
*/
bool
nNavMeshAscReader::ParseBlockEnd(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockStart
*/
bool
nNavMeshAscReader::ParseLineBlockStart(const NavTag& tag)
{
    nString str;
    this->ReadWord(str);
    return str == tag.AsString();
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockEnd
*/
bool
nNavMeshAscReader::ParseLineBlockEnd(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseFourCC
*/
bool
nNavMeshAscReader::ParseFourCC()
{
    nString str;
    if ( !this->ReadField(NavTag::FileFormatType, str) )
    {
        return false;
    }
    return str == this->GetAsciiFourCC();
}

//------------------------------------------------------------------------------
/**
    ParseInt32
*/
bool
nNavMeshAscReader::ParseInt32(const NavTag& tag, int& value)
{
    nString str;
    if ( !this->ReadField(tag, str) )
    {
        return false;
    }
    value = str.AsInt();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt16
*/
bool
nNavMeshAscReader::ParseInt16(const NavTag& tag, int& value)
{
    return this->ParseInt32(tag, value);
}

//------------------------------------------------------------------------------
/**
    ParseInt32InLineBlock
*/
bool
nNavMeshAscReader::ParseInt16InLineBlock(int& value)
{
    nString str;
    this->ReadWord(str);
    value = str.AsInt();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt8
*/
bool
nNavMeshAscReader::ParseInt8(const NavTag& tag, int& value)
{
    return this->ParseInt32(tag, value);
}

//------------------------------------------------------------------------------
/**
    ParseVector3
*/
bool
nNavMeshAscReader::ParseVector3(const NavTag& tag, vector3& value)
{
    // Tag
    nString str;
    this->ReadWord(str);
    if ( str != tag.AsString() )
    {
        return false;
    }

    // Values
    this->ReadWord(str);
    value.x = str.AsFloat();
    this->ReadWord(str);
    value.y = str.AsFloat();
    this->ReadWord(str);
    value.z = str.AsFloat();
    return true;
}

//------------------------------------------------------------------------------
/**
    ReadWord
*/
void
nNavMeshAscReader::ReadWord(nString& str)
{
    n_assert(this->file);

    char word[1024];
    int i = 0;
    for ( ; i < sizeof(word)-1 && !file->Eof(); )
    {
        char c = file->GetChar();
        if ( c > ' ' )
        {
            // When found a non delimiter character append it to the word...
            word[i] = c;
            ++i;
        }
        else if ( i > 0 )
        {
            // ...otherwise breaks the for statement to end the word...
            break;
        }
        // ...except when it hasn't been found any non delimiter character yet
        // (the word is empty). Then just skip the character.
    }
    word[i] = '\0';
    str = word;
}

//------------------------------------------------------------------------------
/**
    ReadField
*/
bool
nNavMeshAscReader::ReadField(const NavTag& tag, nString& str)
{
    this->ReadWord(str); // Skip tag
    if ( str != tag.AsString() )
    {
        return false;
    }
    this->ReadWord(str); // Get value
    return true;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
nNavMeshBinReader::nNavMeshBinReader()
    : file(NULL)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nNavMeshBinReader::~nNavMeshBinReader()
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
nNavMeshBinReader::OpenFile(const char* filename)
{
    n_assert(!this->file);
    n_assert(filename);

    this->file = nFileServer2::Instance()->NewFileObject();
    n_assert(this->file);
    if ( this->file->Open(filename, "rb") )
    {
        return true;
    }
    else
    {
        n_printf( "nNavMeshBinReader::OpenFile(): failed to open file '%s' for reading!\n", filename );
        file->Release();
        file = NULL;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    CloseFile
*/
bool
nNavMeshBinReader::CloseFile()
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
nNavMeshBinReader::ParseBlockStart(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseBlockEnd
*/
bool
nNavMeshBinReader::ParseBlockEnd(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockStart
*/
bool
nNavMeshBinReader::ParseLineBlockStart(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseLineBlockEnd
*/
bool
nNavMeshBinReader::ParseLineBlockEnd(const NavTag& /*tag*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseFourCC
*/
bool
nNavMeshBinReader::ParseFourCC()
{
    n_assert(this->file);

    return this->file->GetInt() == this->GetBinFourCC();
}

//------------------------------------------------------------------------------
/**
    ParseInt32
*/
bool
nNavMeshBinReader::ParseInt32(const NavTag& /*tag*/, int& value)
{
    n_assert(this->file);

    value = this->file->GetInt();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt16
*/
bool
nNavMeshBinReader::ParseInt16(const NavTag& /*tag*/, int& value)
{
    n_assert(this->file);

    value = this->file->GetUShort();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt16InLineBlock
*/
bool
nNavMeshBinReader::ParseInt16InLineBlock(int& value)
{
    n_assert(this->file);

    value = this->file->GetUShort();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseInt8
*/
bool
nNavMeshBinReader::ParseInt8(const NavTag& /*tag*/, int& value)
{
    n_assert(this->file);

    value = this->file->GetChar();
    return true;
}

//------------------------------------------------------------------------------
/**
    ParseVector3
*/
bool
nNavMeshBinReader::ParseVector3(const NavTag& /*tag*/, vector3& value)
{
    n_assert(this->file);

    value.x = this->file->GetFloat();
    value.y = this->file->GetFloat();
    value.z = this->file->GetFloat();
    return true;
}

//------------------------------------------------------------------------------
/**
    OpenFile
*/
bool
nNavMeshRleReader::OpenFile(const char* filename)
{
    n_assert(!this->file);
    n_assert(filename);

    this->file = n_new( nRleFile );
    n_assert(this->file);
    if ( this->file->Open(filename, "rb") )
    {
        return true;
    }
    else
    {
        n_printf( "nNavMeshRleReader::OpenFile(): failed to open file '%s' for reading!\n", filename );
        file->Release();
        file = NULL;
        return false;
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
