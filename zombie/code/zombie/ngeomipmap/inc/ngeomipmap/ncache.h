#ifndef N_CACHE_H
#define N_CACHE_H
//------------------------------------------------------------------------------
/**
    @class nCache
    @ingroup NebulaDataTypes
    @author Mateu Batle

    @brief A templatized cache class which provides a generic container of
    cache entries with Least-Recently-Used cache policy. A type from nCacheEntry 
    in order to customize allocation, deallocation, loading and unloading of
    cache entries.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/nlist.h"
#include "util/nkeyarray.h"

//------------------------------------------------------------------------------
typedef int nCacheKey;

//------------------------------------------------------------------------------
const int InvalidCacheKey = (~0);

//------------------------------------------------------------------------------
/**
    Cache interface (never really instantiated)
*/
struct nCacheEntry
{
public:
    // a type nCacheInfo must be defined in derived types to pass extra 
    // information to load method.
    class nCacheInfo;

    /// constructor
    nCacheEntry() :
        key(InvalidCacheKey)
    {
        /// empty
    }
    /// destructor
    ~nCacheEntry()
    {
        this->Dealloc();
    }

    /// allocate the cache entry resources
    bool Alloc()
    {
        return true;
    }
    /// deallocate the cache entry resources
    bool Dealloc()
    {
        return true;
    }
    /// load data and fill the cache entry
    bool Load(nCacheKey key, nCacheInfo * /*info*/)
    {
        this->key = key;
        return true;
    }
    /// unload data and empty the cache entry
    bool Unload()
    {
        this->key = InvalidCacheKey;
        return true;
    }
    /// get the cache key
    nCacheKey GetKey()
    {
        return this->key;
    }

protected:
    /// internal identifier
    nCacheKey key;
};

//------------------------------------------------------------------------------
template<class CacheEntryType>
class nCache
{
public:
    /// constructor 1
    nCache(int capacity);
    /// default constructor
    nCache();
    /// destructor
    ~nCache();

    /// allocate the cache with the entries specified
    bool Alloc(int capacity);
    /// deallocate the cache
    bool Dealloc();
    /// Is the cache allocated
    bool IsAllocated() const;

    /// Discard an entry from the cache if exists
    bool Discard(nCacheKey key);
    /// Discard all entries
    void DiscardAll();
    /// Realloc all entries
    void ReallocAll();
    /// Lookup the cache
    CacheEntryType * Lookup(nCacheKey key, typename CacheEntryType::nCacheInfo * info, bool loadOnMiss = true);

    /// Get the number of cache hits 
    int GetNumHits();
    /// Get the number of cache misses
    int GetNumMisses();
    /// Reset cache statistics
    void ResetStats();

    /// return number of available entries in the cache
    int GetNumEntries() const;
    /// return number of elements
    int GetNumUsedEntries() const;
    /// element at index
    CacheEntryType* GetEntryByIndex(int index) const;

private:

    /// internal type which allows to make a nList of cache entries
    class CacheEntryTypeNode : public CacheEntryType, public nNode
    {
    };

    /// all cache entry nodes allocated in a single array
    CacheEntryTypeNode * cacheEntries;
    /// number of entries in the cache
    int numEntries;

    /// list used to have fast & easy access to least recently used items
    /// (most recently used are on the head, least on the tail)
    nList list;
    /// key array provides fast direct access by key
    nKeyArray<CacheEntryTypeNode *> * items;

    /// number of cache hits
    int numHits;
    /// number of cache misses
    int numMisses;
};

//------------------------------------------------------------------------------
/**
*/
template<class CacheEntryType>
inline
nCache<CacheEntryType>::nCache(int capacity) :
    cacheEntries(0),
    numEntries(0),
    items(0),
    numHits(0),
    numMisses(0)
{
    this->Alloc(capacity);
}

//------------------------------------------------------------------------------
/**
    NOTE: you must call Alloc() when using the default constructor!
*/
template<class CacheEntryType>
inline
nCache<CacheEntryType>::nCache() :
    cacheEntries(0),
    numEntries(0),
    items(0),
    numHits(0),
    numMisses(0)
{
    // empty
}

//---------------------------------------------------------------
/**
*/
template<class CacheEntryType>
nCache<CacheEntryType>::~nCache()
{
    this->Dealloc();
}

//---------------------------------------------------------------
/**
*/
template<class CacheEntryType>
inline
bool
nCache<CacheEntryType>::Dealloc()
{
    if (this->items)
    {
        n_delete(this->items);
        this->items = 0;
    }

    if (this->cacheEntries)
    {
        // remove all elements from the list
        for(int i = 0;i < this->numEntries;i++)
        {
            // remove from the linked list
            this->cacheEntries[i].Remove();
            // unload the cache entry from memory
            this->cacheEntries[i].Unload();
            // dealloc the resources
            this->cacheEntries[i].Dealloc();
        }

        n_delete_array(this->cacheEntries);
        this->cacheEntries = 0;
    }

    return true;
}

//------------------------------------------------------------------------------
template<class CacheEntryType>
inline
bool
nCache<CacheEntryType>::IsAllocated() const
{
    return (this->items != 0);
}

//------------------------------------------------------------------------------
/**
    Alloc with n elements, may only be called when
    default constructor has been used.
*/
template<class CacheEntryType>
inline
bool
nCache<CacheEntryType>::Alloc(int capacity)
{
    n_assert(!this->items);
    n_assert(!this->cacheEntries);

    // allocate all the entries 
    this->cacheEntries = n_new_array(CacheEntryTypeNode, capacity);
    n_assert(this->cacheEntries);
    this->numEntries = capacity;

    // create fixed array of pointers to entries & initialize
    this->items = n_new(nKeyArray<CacheEntryTypeNode *>)(capacity, 0);
    n_assert(this->items);
    for(int i = 0;i < capacity;i++)
    {
        // allocate the resoures for the cache entry
        this->cacheEntries[i].Alloc();
        // add cache entry to the linked list
        this->list.AddHead( &this->cacheEntries[i] );
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Discard an entry from the cache if already exists, otherwise nothing is done
*/
template<class CacheEntryType>
inline
bool 
nCache<CacheEntryType>::Discard(nCacheKey key)
{
    CacheEntryTypeNode * elm;
    if (this->items->Find(key, elm))
    {
        n_assert(elm);

        // if the cache entry was found remove the key
        this->items->Rem(key);

        // unload the cache entry
        elm->Unload();

        // add the entry to least recently used in order to be reused
        elm->Remove();
        this->list.AddTail( elm );

        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
/**
    Discard all entries
*/
template<class CacheEntryType>
inline
void
nCache<CacheEntryType>::DiscardAll()
{
    while( this->GetNumUsedEntries() != 0 )
    {
        this->Discard( this->GetEntryByIndex( 0 )->GetKey() );
    }
}

//------------------------------------------------------------------------------
/**
    Realloc all entries
*/
template<class CacheEntryType>
inline
void
nCache<CacheEntryType>::ReallocAll()
{
    this->DiscardAll();
    for( int i = 0; i < this->GetNumEntries(); i++)
    {
        this->cacheEntries[i].Dealloc();
        this->cacheEntries[i].Alloc();
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class CacheEntryType>
inline
CacheEntryType *
nCache<CacheEntryType>::Lookup(nCacheKey key, typename CacheEntryType::nCacheInfo * info, bool loadOnMiss)
{
    n_assert( key != InvalidCacheKey);
    CacheEntryTypeNode * elm;
    if (this->items->Find(key, elm))
    {
        // if the cache entry was found refresh usage
        elm->Remove();
        this->list.AddHead( elm );

        // update cache hit stats
        this->numHits++;

        return elm;
    }
    else if (loadOnMiss)
    {
        // if not available, get one least recently used and load it
        CacheEntryTypeNode * freenode = static_cast<CacheEntryTypeNode *> (this->list.RemTail());

        // update cache miss stats
        this->numMisses++;

        // remove the key 
        if (freenode->GetKey() != InvalidCacheKey)
        {
            this->items->Rem(freenode->GetKey());
        }

        // unload previous entry
        freenode->Unload();

        // load cache entry
        bool res = freenode->Load(key, info);
        if (!res)
        {
            this->list.AddTail( freenode );
            return 0;
        }

        // add the new key
        this->items->Add(key, freenode);

        // place it in used side
        this->list.AddHead( freenode );

        return freenode;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
template<class CacheEntryType>
inline
int 
nCache<CacheEntryType>::GetNumHits()
{
    return this->numHits;
}

//------------------------------------------------------------------------------
/**
*/
template<class CacheEntryType>
inline
int 
nCache<CacheEntryType>::GetNumMisses()
{
    return this->numMisses;
}

//------------------------------------------------------------------------------
/**
*/
template<class CacheEntryType>
inline
void
nCache<CacheEntryType>::ResetStats()
{
    this->numHits = 0;
    this->numMisses = 0;
}

//------------------------------------------------------------------------------
template<class CacheEntryType>
inline
int 
nCache<CacheEntryType>::GetNumUsedEntries() const
{
    if ( this->items )
    {
        return this->items->Size();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
template<class CacheEntryType>
inline
int 
nCache<CacheEntryType>::GetNumEntries() const
{
    return this->numEntries;
}

//------------------------------------------------------------------------------
template<class CacheEntryType>
inline
CacheEntryType* 
nCache<CacheEntryType>::GetEntryByIndex(int index) const
{
    return this->items->GetElementAt(index);
}

//------------------------------------------------------------------------------
#endif
