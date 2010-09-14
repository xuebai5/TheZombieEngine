#include "shootem/textfile.h"

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#define MAX_LINELEN 512

//------------------------------------------------------------------------------

TextFile::TextFile() :
file(0),
curIndex(0)
{
}

//------------------------------------------------------------------------------

TextFile::TextFile(const char* filename) :
file(0),
curIndex(0)
{
    this->Open(filename);
}

//------------------------------------------------------------------------------

TextFile::~TextFile()
{
    if (this->file)
        this->Close();
}

//------------------------------------------------------------------------------
bool TextFile::Open(const char* filename)
{
    n_assert(!this->file);

    this->file = nFileServer2::Instance()->NewFileObject();
    n_assert(this->file);

    return this->file->Open(filename, "r");
}

//------------------------------------------------------------------------------

void TextFile::Close()
{
    n_assert(this->file);
    this->file->Release();
    this->file = 0;
}

//------------------------------------------------------------------------------

bool TextFile::IsOpen()
{
    return (this->file && this->file->IsOpen());
}

//------------------------------------------------------------------------------

bool TextFile::Eof()
{
    n_assert(this->file);
    return this->file->Eof();
}

//------------------------------------------------------------------------------

void TextFile::ReadLine()
{
    n_assert(this->file);
    char buf[MAX_LINELEN];
    this->file->GetS(buf, MAX_LINELEN);
    this->curLine.Set(buf);
    this->curIndex=0;
}

//------------------------------------------------------------------------------

bool TextFile::Contains(const char* str)
{
    return (this->curLine.IndexOf(str, 0) != -1);
}

//------------------------------------------------------------------------------

bool TextFile::Extract(const char* start, const char* end, nString& out)
{
    int startIndex = this->curLine.IndexOf(start, this->curIndex);
    int endIndex = this->curLine.IndexOf(end, startIndex);
    if (startIndex==-1 || endIndex==-1) return false;

    startIndex += int(strlen(start));
    this->curIndex = endIndex;
    out.Set(this->curLine.ExtractRange(startIndex, endIndex - startIndex).Get());
    return true;
}
