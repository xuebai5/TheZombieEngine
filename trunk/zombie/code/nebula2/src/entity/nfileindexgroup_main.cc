//------------------------------------------------------------------------------
/**
    @file nfileindexgroup_main.cc
    @author Cristobal Castillo

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nfileindexgroup.h"
#include "entity/nentityobjectserver.h"
#include "entity/nfileindex.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
nFileIndexGroup::nFileIndexGroup() :
    lastIndex(-1)
{
   
}

//------------------------------------------------------------------------------
nFileIndexGroup::~nFileIndexGroup() 
{
    this->Clear();
}

//------------------------------------------------------------------------------
nFileIndex* 
nFileIndexGroup::LookUpFileIndex( nEntityObjectId id )
{
    id = id & nEntityObjectServer::IDHIGHMASK;
    //Optimization: remember the last index because the acces is sequencial
    if ( this->lastIndex < 0 ||
          ! (this->groupFileIndex[this->lastIndex].id == id) )
    {
        Entry entry; 
        entry.id = id;
        this->lastIndex  = this->groupFileIndex.FindIndex(entry);
    }

    if ( this->lastIndex < 0 ) // create it
    {
        nString filename(this->GetEntityObjectFilenameByGroup(id) );
        Entry entry;
        entry.fileIndex = static_cast<nFileIndex*>(nKernelServer::Instance()->New("nfileindex") );
        n_assert( entry.fileIndex );
        entry.id =  id;

        if ( nFileServer2::Instance()->FileExists( filename.Get() ) )
        {
           n_verify2( entry.fileIndex->Open( filename.Get() ) ,  (nString("Problem read entities at ") + filename  ).Get() )  ;
           
        } else
        {
            // Create empty
            n_verify2(entry.fileIndex->CreateEmpty( filename.Get() )  , (nString("Problem for create empty list") + filename  ).Get() );
        }
        this->groupFileIndex.Append(entry);
        return entry.fileIndex;
    }
         
    return this->groupFileIndex[this->lastIndex].fileIndex;
}
//------------------------------------------------------------------------------
void
nFileIndexGroup::SetPath(const char* path)
{
    this->path = path;
    this->Clear();
}

//------------------------------------------------------------------------------
nString 
nFileIndexGroup::GetEntityObjectFilenameByGroup(nEntityObjectId group) const
{
    nString path;
    path.Format( "%08X.n2" ,  group & nEntityObjectServer::IDHIGHMASK );
    return  this->path + path;
}
//------------------------------------------------------------------------------
void
nFileIndexGroup::Clear()
{
    for ( int idx = 0; idx < this->groupFileIndex.Size() ; ++idx )
    {
        if ( this->groupFileIndex[idx].fileIndex )
        {
            this->groupFileIndex[idx].fileIndex->Release();
        }
    }
    this->groupFileIndex.Clear();
    this->lastIndex = -1;
}

//------------------------------------------------------------------------------
void 
nFileIndexGroup::SetFileIndex( nEntityObjectId id , nFileIndex* fileIndex)
{
    Entry entry; 
    entry.id = id;
    int idx = this->groupFileIndex.FindIndex(entry);
    if ( idx >= 0 )
    {
        if (this->groupFileIndex[idx].fileIndex )
        {
            this->groupFileIndex[idx].fileIndex->Release();
        }
        this->groupFileIndex[idx].fileIndex = fileIndex;
        
    } else
    {
        n_assert_always();
    }

}

//------------------------------------------------------------------------------
int
nFileIndexGroup::Entry::Cmp( const Entry *e0, const Entry *e1)
{
    if ( e0->id < e1->id)
    {
        return -1;
    } else if (e0->id > e1->id )
    {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------
nFileIndexGroup::Entry::~Entry()
{
    // Empty , this is needed by nArray
}

