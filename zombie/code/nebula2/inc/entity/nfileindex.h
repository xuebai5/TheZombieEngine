#ifndef N_FILEINDEX_H
#define N_FILEINDEX_H
//------------------------------------------------------------------------------
/**
    @file nfileindex.h
    @class nFileIndex
    @ingroup NebulaEntitySystem
    @author Cristobal Castillo Domingo
    (c) 2004 Conjurer Services, S.A.
*/

#include "entity/nentitytypes.h"
#include "util/nsortedarray.h"

//------------------------------------------------------------------------------
/**
*/
class nFileIndex: public nObject
{
public:
    /// constructor
    nFileIndex();
    /// destructor
    virtual ~nFileIndex();
    /// called on initialization
    void InitInstance(nObject::InitInstanceMsg);
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// Create a new file object , and open it.
    nFile* GetNewFile(nEntityObjectId id);
    /// Open file Indedx
    bool Open( const char* filename );
    ///
    bool Close();
    /// Start blanck fileIndex
    bool BeginBuild();
    /// Append entry or replace with file
    bool AppendOrReplace(nFile* file,nEntityObjectId id);
    /// Append entry or replace with file
    bool Remove(nEntityObjectId id);
    /// Save and generate index
    bool EndBuild();
    /// 
    int GetCountSave();
    /// Create Empty file
    bool CreateEmpty( const char* filename);

    /// Return the filename
    nString GetFileName();

        /// Get seek point of this entry
        void GetStartAndLength(int, int&, int&);
        /// Set seek point of this entry, use only in persistence
        void SetStartAndLength(int,int, int);
private:
    struct Entry
    {
        nEntityObjectId id;
        unsigned int start;
        unsigned int length;
        ~Entry();
    };

    /// Entry comparison function
    static int CmpEntryInfo( const Entry *, const Entry *);
    /// index for entries
    nSortedArray<Entry, CmpEntryInfo> fileIndex;
    /// Build index for entris
    bool BuildIndex(nFile* file);
    /// Return the length of this entry
    int GetLength(int idx);
    /// Copy to id , return the number of copies
    bool CopyTo(nEntityObjectId id);
    /// Copy entries from another FileIndex
    bool CopyEntriesFromFile( int idxStart, int idxEnd);
    /// Append the persistence of file
    bool Append(nFile* file,nEntityObjectId id);
    /// the indexed file.
    nFile* file;
    /// cache of acces
    int lastIndex;
    /// Data for build
    nFile* buildFile;
    /// Data for build, indexSrc
    int indexSrc;
    /// The number of save entities
    int countSave;
};
//------------------------------------------------------------------------------
#endif // N_FILEINDEX_H
