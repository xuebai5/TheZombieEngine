//------------------------------------------------------------------------------
//  nfileindex_main.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nfileindex.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "file/nmemfile.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nFileIndex, "nobject");

//------------------------------------------------------------------------------
/**
*/
nFileIndex::nFileIndex() :
    file(0),
    lastIndex(-1),
    buildFile(0)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
nFileIndex::~nFileIndex() 
{
    this->Close();
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
void
nFileIndex::InitInstance(nObject::InitInstanceMsg)
{
    // Emtpy
}

//------------------------------------------------------------------------------
/**
*/
int
nFileIndex::CmpEntryInfo( const Entry *elm0, const Entry *elm1)
{
    if ( elm0->id < elm1->id )
    {
        return -1;
    } else if ( elm0->id > elm1->id )
    {
        return 1;
    } else
    {
        return 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::Open( const char* filename )
{
    n_assert(filename);
    n_assert( 0 == this->file);
    bool retval(false);
    this->file = kernelServer->GetFileServer()->NewFileObject();
    if ( this->file->Open( filename, "rb"))
    {
        retval = this->BuildIndex( file );
    } else
    {
        this->file->Release();
        this->file = 0;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::Close()
{
    if ( this->file )
    {
        if ( this->file->IsOpen() )
        {
            this->file->Close();
        }
        this->file->Release();
        this->file = 0;
    }
    this->fileIndex.Clear();
    return true;
}



//------------------------------------------------------------------------------
/**
           '/n'
          |--->|1|
          |
          | '$'    'i'    'd'    ':'    '$'    '\n'  _
      -->|0|--->|2|--->|3|--->|4|--->|5|--->|6|---->|7|
*/
bool
nFileIndex::BuildIndex(nFile* file)
{
    int state = 0;
    int posibleEnd = 0;
    nString posibleId;
    nArray<Entry> entryList;
    int idx = -1;

    //Scan in file the  ID, search it with state machine.
    const int blockSize = 64*1024;
    char* buffer = n_new_array( char, blockSize);
    n_assert(buffer);
    int remainingChars = this->file->GetSize() / sizeof(char);
    int currentPos = 0;
    while ( remainingChars > 0 )
    {
        int bufferSize = n_min( remainingChars,blockSize );
        remainingChars  -= bufferSize;
        int readBytes = file->Read( buffer , bufferSize *sizeof(char) );
        n_assert( readBytes == int(bufferSize *sizeof(char)) );
    
        char* character = buffer;
        for ( int indexBuffer = 0  ; indexBuffer < bufferSize ; ++indexBuffer , ++character , ++currentPos )
        {
            switch( state )
            {
            case 0:
                if ( *character ==  '$' )
                {
                    state = 2;
                } 
                break;
            case 1:
                if ( *character ==  '$' )
                {
                    state = 2;
                } else if ( *character != '\n' )
                {
                    state = 0;
                }
                break;
            case 2:
                if ( *character ==  'i' )
                {
                    state = 3;
                } else
                {
                    state = 0;
                }
                break;
            case 3:
                if ( *character ==  'd' )
                {
                    state = 4;
                } else
                {
                    state = 0;
                }
                break;
            case 4:
                if ( *character ==  ':' )
                {
                    state = 5;
                    posibleId = "";
                } else
                {
                    state = 0;
                }
                break;
            case 5:
                if ( (*character) >=  '0' && (*character) <= '9' )
                {
                    state = 5;
                    posibleId.AppendRange(character,1);
                } else if ( *character == '$')
                {
                    state =6;
                }
                else
                {
                    state = 0;
                }
                break;
            case 6:
                if ( *character ==  '\n' )
                {
                    state = 7;
                } else
                {
                    state = 0;
                }
                break;
            case 7:
                if ( *character ==  '$' )
                {
                    state = 2;
                } else
                {
                    state = 0;
                }
                break;
            }

            if ( state == 0 && *character == '\n' )
            {
                state = 1;
            }

            // Take a action
            switch ( state )
            {
                case 1: // Get posible end of file
                    posibleEnd = currentPos +1; // the curren position is the next line
                    break;
                case 7:
                    if ( idx >= 0 )
                    {
                        entryList[idx].length = posibleEnd - entryList[idx].start;
                    }
                    ++idx;
                    Entry& entry = entryList.At(idx);
                    entry.id = nEntityObjectId( atol( posibleId.Get() ) );
                    entry.start = posibleEnd = currentPos + 1;
                    break;
            }

        }
    }
    n_assert2( idx >= 0 || (file->GetSize() == 0), (nString( "The fileIndex not has a entries ") + file->GetFilename() ).Get() );
    n_delete(buffer);
    
    NLOG(entityObjectServer , ( 2 | NLOG1 , "File Index has %d entries, %s" , idx +1  , file->GetFilename().Get() )  );
    if ( idx >= 0 )
    {
        entryList[idx].length = posibleEnd - entryList[idx].start;
        this->fileIndex = entryList;
        return true;
    } else
    {
        this->fileIndex = entryList;
        return file->GetSize() == 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFile* 
nFileIndex::GetNewFile(nEntityObjectId id)
{
    n_assert( this->file);
    n_assert( this->buildFile == 0);
    int start,length;
    nMemFile* newFile(0);

    if ( this->lastIndex < 0 || this->lastIndex >= this->fileIndex.Size() || 
        ! (this->fileIndex[this->lastIndex].id == id) )
    {
        Entry entry;
        entry.id = id;
        this->lastIndex = this->fileIndex.FindIndex(entry);
    }

    if ( this->lastIndex >= 0 )
    {
        this->GetStartAndLength(this->lastIndex, start, length);
        newFile = new(nMemFile);
        nString emptyFilename;
        newFile->Open(emptyFilename, "r");
        this->file->Seek( start , nFile::START );
        newFile->AppendFile( this->file, length );
    }
    ++this->lastIndex; // Fast cache for sequencial read
    return newFile;
    /// @TODO: Optimized it for sequencial read
}


//------------------------------------------------------------------------------
/**
*/
nString
nFileIndex::GetFileName()
{
    n_assert( this->file );
    n_assert( this->file->IsOpen() );
    return this->file->GetFilename();
}

//------------------------------------------------------------------------------
/**
*/
void
nFileIndex::GetStartAndLength( int idx, int& start, int& length)
{
    start = this->fileIndex[idx].start ;
    length = this->fileIndex[idx].length ;
}

//------------------------------------------------------------------------------
/**
*/
void
nFileIndex::SetStartAndLength( int /*idx*/, int /*start*/, int /*length*/)
{
    /// @TODO: Implement the persistence of nFileIndex
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::CopyEntriesFromFile(  int idxStart, int idxEnd )
{
    n_assert( this->file);
    n_assert( this->buildFile);
    this->countSave += idxEnd - idxStart +1 ;

    if ( idxStart < 0 || idxEnd < 0 || idxEnd < idxStart )
    {
        return false;
    }

    int startPos, endPos;
    int start,length;

    if (  idxStart == 0 )
    {
        startPos = 0;
    } else
    {
        // Need copy a mark --$ID: $ . Use the end of previous index  
        this->GetStartAndLength(idxStart - 1, start, length);
        startPos = start + length; 
    }
    this->GetStartAndLength(idxEnd, start, length);
    endPos = start + length;

    n_assert2( start <= endPos , "Problem in save entities the index are invalids");

    this->file->Seek( startPos, nFile::START );
    this->buildFile->AppendFile( this->file,  endPos - startPos );

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::AppendOrReplace(nFile* file,nEntityObjectId id)
{
    bool retval(false);
    if ( this->CopyTo(id) )
    {
        retval = this->Append( file , id );
    }

    // if exist the id then skip it 
    if ( (this->indexSrc < this->fileIndex.Size())  && (this->fileIndex[this->indexSrc].id == id ) )
    {
        this->indexSrc++;
    }

    return retval;
}
//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::Remove(nEntityObjectId id)
{
    bool retval(false);
    retval = this->CopyTo(id);

    // nothing with current id

    // if exist the id then skip it 
    if ( (this->indexSrc < this->fileIndex.Size())  && (this->fileIndex[this->indexSrc].id == id ) )
    {
        this->indexSrc++;
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::Append(nFile* file,nEntityObjectId id)
{
    n_assert( this->buildFile)
    nString token;
    bool retval(false);
    token.Format("-- $id:%u$\n", id);

    if ( this->buildFile->PutS( token.Get() ) )
    {
        this->buildFile->AppendFile(file);
        retval = true;
    }
    this->countSave++;
    return retval;
}
//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::CopyTo(nEntityObjectId id)
{
    int indexIni = this->indexSrc;
    while( ( this->indexSrc < this->fileIndex.Size() )  && 
           ( this->fileIndex[this->indexSrc].id < id )
           )
    {
        this->indexSrc++;
    }

    if ( indexIni != this->indexSrc )
    {
        return this->CopyEntriesFromFile(indexIni , this->indexSrc -1 );
    } 
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::BeginBuild()
{
    n_assert( this->file );
    n_assert( this->buildFile == 0 );
    nString name( this->file->GetFilename() );
    name +=".tmp";
    this->indexSrc = 0;
    this->countSave = 0;

    this->buildFile = kernelServer->GetFileServer()->NewFileObject();
    if ( this->buildFile->Open( name.Get() , "w"))
    {
        return true;
    } else
    {
        this->buildFile->Release();
        this->buildFile = 0;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::EndBuild()
{
    n_assert( this->file );
    n_assert( this->buildFile );
    nString fileName( this->file->GetFilename() );
    nString tmpFileName( this->buildFile->GetFilename() );

    this->CopyTo(0xFFFFFFFF); //Append the next objects


    this->file->Close();
    this->file->Release();
    this->file = 0;

    this->buildFile->Close();
    this->buildFile->Release();
    this->buildFile = 0;

    if ( nFileServer2::Instance()->FileExists( fileName ) )
    {
        nFileServer2::Instance()->DeleteFile( fileName);
    }

    if ( nFileServer2::Instance()->MoveFile( tmpFileName , fileName ) )
    {
        if ( this->Open(  fileName.Get() ) ) // Temporal solution the indexes are not valid
        {
            if ( this->countSave == this->fileIndex.Size()) 
            {
                return true;
            } else
            {
                n_assert2_always( "Lost entities"  ) ;    
            }
        }
    }
       
    n_message( "Problems for save level entities, exit of conjurer and rename files\n %s by %s" , tmpFileName.Get() , fileName.Get() ) ;
    return false;
    
}

//------------------------------------------------------------------------------
/**
*/
int 
nFileIndex::GetCountSave()
{
    return this->countSave;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFileIndex::CreateEmpty( const char* filename)
{
    n_assert( this->file == 0 );
    nString name(filename);
    this->file = kernelServer->GetFileServer()->NewFileObject();
    if ( this->file->Open( name.Get() , "w"))
    {
        this->file->Close();
        this->file->Release();
        this->file = 0;
        return this->Open(  name.Get() ); 
    } else
    {
        this->file->Release();
        this->file = 0;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFileIndex::Entry::~Entry()
{
    //Emtpy , this is needed by narray
}

//------------------------------------------------------------------------------
