#ifndef TEXTFILE_H
#define TEXTFILE_H

#include "util/nstring.h"

//------------------------------------------------------------------------------
class nFile;
class TextFile
{
public:
    TextFile();
    TextFile(const char* filename);
    ~TextFile();

    bool Open(const char* filename);
    void Close();
    bool IsOpen();
    
    bool Eof();
    void ReadLine();
    bool Contains(const char* str);
    bool Extract(const char* start, const char* end, nString& out);

private:
    nFile* file;
    nString curLine;
    int curIndex;
};

//------------------------------------------------------------------------------

#endif //TEXTFILE_H
