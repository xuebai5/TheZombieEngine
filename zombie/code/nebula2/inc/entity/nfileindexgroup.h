#ifndef N_FILEINDEXGROUP_H
#define N_FILEINDEXGROUP_H
//------------------------------------------------------------------------------
/**
    @file nfileindexgroup.h
    @class nFileIndexGroup
    @ingroup NebulaEntitySystem
    @author Cristobal Castillo Domingo
    (c) 2004 Conjurer Services, S.A.
*/

#include "entity/nentitytypes.h"
#include "util/nsortedarray.h"

class nFileIndex;
//------------------------------------------------------------------------------
/**
*/
class nFileIndexGroup
{
public:
    /// Constructor
    nFileIndexGroup();
    /// destructor
    ~nFileIndexGroup();
    // The entry
    struct Entry
    {
        nEntityObjectId id;
        nFileIndex* fileIndex;
        ///destructor
        ~Entry();
        /// Compare function
        static int Cmp( const Entry*, const Entry *);
    };
    /// lookupFileIndex , create it if is necesary
    nFileIndex* LookUpFileIndex( nEntityObjectId id);
    /// Change the specific fileIndex
    void SetFileIndex( nEntityObjectId id , nFileIndex* fileIndex);
    /// Empty and release  all nFileIndex
    void Clear();
    /// Set the path
    void SetPath(const char*);
private:
    //return name
    nString GetEntityObjectFilenameByGroup(nEntityObjectId group) const;
    nSortedArray<Entry, Entry::Cmp> groupFileIndex;
    /// The last index
    int lastIndex;
    /// The basePath
    nString path;
};

//------------------------------------------------------------------------------
#endif // N_FILEINDEXGROUP_H
