#ifndef N_HASHTABLE_H
#define N_HASHTABLE_H
//------------------------------------------------------------------------------
/**
    @class nHashTable
    @ingroup NebulaDataTypes

    @brief Implements a simple string hash table.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstrlist.h"

//------------------------------------------------------------------------------
class nHashTable 
{
public:
    /// constructor
    nHashTable(int size);
    /// destructor
    ~nHashTable();
    /// add an entry to the hashtable
    void Add(nStrNode* n);
    /// search hash table for entry
    nStrNode* Find(const char* str) const;
    /// return size;
    int GetSize();
    /// return nstrlist
    nStrList *GetListIdx(int index);

    void removeAll();

private:
    int htable_size;
    nStrList *htable;
};

//------------------------------------------------------------------------------
/**
*/
inline 
nHashTable::nHashTable(int size)
{
    n_assert( size > 0 );
    this->htable_size = size;
    this->htable = n_new_array(nStrList, size);
}

//------------------------------------------------------------------------------
/**
*/
inline nHashTable::~nHashTable()
{
    n_delete_array(this->htable);
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
int hash(const char *str, int htable_size)
{
    int i = 0;
    int j = 1;
    char c;
    while ( 0 != (c = *str++)) i += ((uchar)c) * j++; 
    return (i % htable_size);
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nHashTable::Add(nStrNode* n)
{
    int h_index = hash(n->GetName(), this->htable_size);
    this->htable[h_index].AddHead(n);
}

//------------------------------------------------------------------------------
/**
*/
inline 
nStrNode*
nHashTable::Find(const char* str) const
{
    int h_index = hash(str, this->htable_size);
    return this->htable[h_index].Find(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nHashTable::removeAll()
{
    int idx;
    for ( idx = 0; idx < htable_size ;  idx ++)
    {
        nStrNode* node = this->htable[idx].RemHead() ;
        while ( node )
        {
            n_delete(node);
            node = this->htable[idx].RemHead();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nHashTable::GetSize()
{
    return this->htable_size;
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrList *
nHashTable::GetListIdx(int index)
{
    n_assert( index>= 0  && index< this->htable_size );
    return &(this->htable[index]);
}
//------------------------------------------------------------------------------
#endif
