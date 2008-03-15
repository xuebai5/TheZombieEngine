#ifndef N_NMAPTABLE_H
#define N_NMAPTABLE_H
//------------------------------------------------------------------------------
/**
   @file nmaptable.h
   @author Luis Jose Cabellos Gomez
   @brief Declaration of nMapTable class.

   (C) Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
/**
    @class nMapTableIntHash
    @ingroup RenaissanceModule
    @brief generic hash function for int and nMapTalbe
*/

struct nMapTableIntHash
{
    unsigned int operator()(int op) const
    {
        return op;
    }    
};

//------------------------------------------------------------------------------
/**
    @class nMapTableCmp
    @ingroup RenaissanceModule
    @brief Template for generic compare
*/
template<class Type>
class nMapTableCmpDefault
{
public:
    bool Less(const Type& elm0, const Type& elm1) const { return elm0 < elm1; } 
    bool Equal( const Type& elm0, const Type& elm1) const { return elm0 == elm1; } 
};

//------------------------------------------------------------------------------
/**
    @class nMapTableHash
    @ingroup RenaissanceModule
    @brief generic hash function for int and nMapTalbe
*/
template<class Type>
class nMapTableHash
{
public:
    unsigned int operator()(const Type& op) const
    {
        return op.Hash();
    }    
};

//------------------------------------------------------------------------------
/**
    @class nMapTable
    @ingroup RenaissanceModule

    @brief Template of map table with integer index and pointer to class value
*/

template<class Data, class Key = int, class KeyHash = nMapTableIntHash , class KeyCmp = nMapTableCmpDefault<Key> >
class nMapTable
{
public:
    /// Constructor
    nMapTable( unsigned int tam = 23 );
    /// Desctructor
    ~nMapTable( );
    /// Add a element to the table in the index
    const Key * Add( const Key & index, Data * data );
    /// Remove the element of the index
    void Remove( const Key & index );
    /// Get the element of the index
    Data * operator[](const Key & index)const;

    /// Put internal iterator in the begin of the table elements
    void Begin();
    /// Get the next element of the table
    Data * Next();
    /// Get the next element of table, return index and data
    void Next(Key& key, Data*& data);
    /// Get the number of objects
    int GetCount();

    #ifndef NDEBUG
    float CalcDistribution() const;
    #endif
private: 
    int Hash(const Key & index)const;
    
    /// Node of the list in the Hash Table
    struct Node
    {
        Key index;
        Data data;
        Node * next;
    };

    /// Entry of the hash Table
    struct Entry
    {
        Node * list;
    };

    Entry *map;
    unsigned int size;

    // internal iterator
    unsigned int itIndex;
    Node * itNode;
    Data * itData;

    #ifndef NDEBUG
    int* count;
    #endif
    int num;

    KeyHash keyHash;
    KeyCmp  keyCmp;
};

//------------------------------------------------------------------------------
/**
    @param tam size of the hash table 
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
nMapTable<Data, Key, KeyHash, KeyCmp>::nMapTable( unsigned int tam )
{
    // create hash table
    this->map = n_new_array( Entry, tam );
    this->size = tam;

    // initialize hash table
    unsigned int i;
    for( i=0 ; i<size ; ++i )
    {
        this->map[i].list = 0;
    }

    // initialize iterator
    this->itIndex = 0;
    this->itNode = 0;
    this->itData = 0;
    this->num = 0;

    #ifndef NDEBUG
    count = n_new_array( int , tam );
    for( i=0 ; i<size ; ++i )
    {
        count[i] = 0;
    }
    #endif
}

//------------------------------------------------------------------------------
/**
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
nMapTable<Data, Key, KeyHash, KeyCmp>::~nMapTable( )
{
    // delete nodes in the hash table
    unsigned int i;
    for( i=0 ; i<size ; ++i )
    {
        Node *p = 0;
        while( this->map[i].list )
        {
            p = this->map[i].list;
            this->map[i].list = this->map[i].list->next;
            n_delete( p );
        }
    }

    // delete hash table
    n_delete_array( this->map );
    this->map = 0;

    #ifndef NDEBUG
    n_delete_array( this->count );
    #endif
}

//------------------------------------------------------------------------------
/**
    @param index index of the element
    @returns index of the hash table that element is
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
inline
int
nMapTable<Data, Key, KeyHash, KeyCmp>::Hash( const Key & index )const
{
    unsigned int value = keyHash(index);
    return ( value % this->size );
}

//------------------------------------------------------------------------------
/**
    @param index index of the element to insert
    @param data element to insert
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
const Key *
nMapTable<Data,Key,KeyHash,KeyCmp>::Add( const Key & index, Data * data )
{
    // get index of hash table
    int i = this->Hash( index );
    this->num++;

    #ifndef NDEBUG
    count[i]++;
    #endif

    // create node
    Node *p = n_new( Node );
    p->data = *data;
    p->index = index;

    // insert first
    if( ( this->map[i].list == 0 ) || 
          keyCmp.Less(p->index, this->map[i].list->index) )
    {
        p->next = this->map[i].list;
        this->map[i].list = p;
        return (&p->index);
    }

    // search where insert the node
    Node *pos = this->map[i].list->next;
    Node *prev = this->map[i].list;
    while( pos && keyCmp.Less( pos->index , p->index) )
    {
        prev = pos;
        pos = pos->next;
    }

    // insert middle or last
    p->next = pos;
    prev->next = p;

    return (&p->index);
}

//------------------------------------------------------------------------------
/**
    @param index index of the element to remove
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
void
nMapTable<Data, Key, KeyHash, KeyCmp>::Remove( const Key & index )
{
    // get index of hash table
    int i = this->Hash( index );

    // if empty
    if( this->map[i].list == 0)
    {
        return;
    }

    // delete first
    Node *p = 0;
    if( keyCmp.Equal( this->map[i].list->index , index ) )
    {
        p = this->map[i].list;
        this->map[i].list = this->map[i].list->next;
        // if actual iterator is deleted, get next iterator
        if( p == this->itNode )
        {
            this->Next();
        }
        n_delete( p );
        this->num--;
        
        #ifndef NDEBUG
        count[i]--;
        #endif

        return;
    }

    // search node in the middle
    p = this->map[i].list->next;
    Node *prev = this->map[i].list;
    while( p && keyCmp.Less( p->index , index) )
    {
        prev = p;
        p = p->next;
    }
    
    // delete last node or middle node
    if( p && keyCmp.Equal(p->index, index) )
    {
        prev->next = p->next;
        // if actual iterator is deleted, get next iterator
        if( p == this->itNode )
        {
            this->Next();
        }
        n_delete( p );
        this->num--;
        #ifndef NDEBUG
        count[i]--;
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    @param index index of the element to retrieve
    @returns element at the index
    @retval 0 if there are no element at the index
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
Data *
nMapTable<Data, Key, KeyHash, KeyCmp>::operator [](const Key & index)const
{
    // get index of hash table
    int i = this->Hash( index );

    // search node in the index
    Node *p = this->map[i].list;
    while( p && ! keyCmp.Equal( p->index , index) )
    {
        p = p->next;
    }

    // if exist the element
    if( p )
    {
        return &p->data;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
void
nMapTable<Data, Key, KeyHash, KeyCmp>::Begin()
{
    // search first index with list
    this->itIndex = 0;
    while( ( this->itIndex < this->size ) && 
        ( this->map[this->itIndex].list == 0 ))
    {
        ++this->itIndex;
    }

    // if there arent nodes
    if( this->itIndex == this->size )
    {
        this->itNode = 0;
        this->itData = 0;
    }
    else
    {
        this->itNode = this->map[this->itIndex].list;
        this->itData = &this->itNode->data;
    }
}

//------------------------------------------------------------------------------
/**
    @remarks retrieve the actual element of the internal iterator and advance
    iterator. If there arent more elements the next call will return 0
    @retval 0 if there are no element in the iterator
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
Data *
nMapTable<Data, Key, KeyHash, KeyCmp>::Next()
{
    // get actual iterator data
    Data *ret = this->itData;

    // search for next iterator data
    if( this->itData )
    {
        bool searching = true;

        // get next node in actual list
        this->itNode = this->itNode->next;

        // while there are elements in the hash table
        while( searching && this->itIndex < this->size )
        {
            // if no node or end of list in actual index
            if( this->itNode == 0)
            {
                // get next index
                ++this->itIndex;
                // get next node
                this->itNode = this->map[this->itIndex].list;
            }
            else
            {
                // get next data
                this->itData = &this->itNode->data;
                // end search
                searching = false;
            }
        }

        // if search no end
        if( searching )
        {
            this->itData = 0;
        }
    }

    // returns actual iterator data
    return ret;
}

//------------------------------------------------------------------------------
/**
    @remarks retrieve the actual element of the internal iterator and advance
    iterator. If there arent more elements the next call will return 0
    @param key, out the current key
    @param data, out the current data, 0 if there are no element in the iterator
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
void
nMapTable<Data, Key, KeyHash, KeyCmp>::Next(Key& key, Data*& data)
{
    Node * temp = this->itNode;
    data = Next();
    if( data )
    {
        n_assert( temp );
        if( temp )
        {
            key = temp->index;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class Data, class Key, class KeyHash , class KeyCmp >
int
nMapTable<Data, Key, KeyHash, KeyCmp>::GetCount()
{
    return this->num;
}

//------------------------------------------------------------------------------
/**
    @remarks function for debug distribution of table
    @return variance / average
*/
#ifndef NDEBUG
template<class Data, class Key, class KeyHash , class KeyCmp >
float 
nMapTable<Data, Key, KeyHash, KeyCmp>::CalcDistribution() const
{
    float var = 0.0f;
    float avg = 0.0f;
    for( unsigned int i=0 ; i<size ; ++i )
    {
        avg += float(count[i]);
        var += float(count[i])*float(count[i]);
    }
    avg /= float(size);
    var /= float(size);
    var -= avg*avg;
    return var /( avg * avg );
}
#endif

#endif//N_NMAPTABLE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
