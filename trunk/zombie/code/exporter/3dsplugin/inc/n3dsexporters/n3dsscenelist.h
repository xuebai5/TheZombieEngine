#ifndef N_3DS_SCENE_LIST_H
#define N_3DS_SCENE_LIST_H

#include "util/narray.h"
#include "util/nstring.h"
#include "n3dsexporters/n3dsobject.h"

//------------------------------------------------------------------------------
/**
    @class n3dsSceneList
    @ingroup n3dsMaxExporterKernel

    Is a nArray of object scene.
    Implement a iterator for friendly use with 3dsMax progressbar.

    (C) 2005 Tragnarion
*/
class n3dsSceneList 
{
public:
    class iterator
    {
    public:
        /// default constructor;
        iterator();
        /// default destructor;
        ~iterator();
        /// copy constructor
        iterator(iterator const& it);
        /// operator assignament
        iterator& operator = ( iterator const& it );
        /// equality operator
        bool operator == ( iterator const& it ) const;
        /// different operator
        bool operator != ( iterator const& it ) const;
        /// unreference operator
        n3dsObject& operator * ();
        /// pre-increment operator
        iterator& operator ++ ();
        /// Set Use show bar progress in 3ds Max
        void ShowProgressBar( nString const& title);
    protected:
        iterator(int index, n3dsSceneList const* list);
        iterator(int index, n3dsSceneList const* list, n3dsObject::ObjectType const& type);
        iterator(int index, n3dsSceneList const* list, n3dsObject::ObjectType const& type, n3dsObject::ObjectSubType const& subType );
        n3dsSceneList const* list;
        int  index;
        int count;
        bool showBar;
        nString barTile;
        TSTR    tmpString;
        friend class n3dsSceneList;
        bool validType;
        bool validSubType;
        n3dsObject::ObjectType type;
        n3dsObject::ObjectSubType subType;
    };

    n3dsSceneList();
    n3dsSceneList(int initialSize, int initialGrow);
    /// append element to array (synonym for PushBack())
    void Append(const n3dsObject& elm);
    /// [] operator
    n3dsObject& operator[](int index) const;
    /// get number of elements in array
    int Size() const;
    /// return the number of object for this type
    int Count(n3dsObject::ObjectType type) const;
    /// return the number of object for this type
    int Count(n3dsObject::ObjectSubType type) const;
    /// return iterator to beginning of array
    iterator Begin() const;
    /// return iterator for this type
    iterator Begin(n3dsObject::ObjectType const& type) const;
    /// return iterator for this type
    iterator Begin(n3dsObject::ObjectType const& type, n3dsObject::ObjectSubType const& subType) const;
    /// return iterator to end of array
    iterator End() const;
    ///
    void SortMaxId();
protected:
    nArray<n3dsObject> list;
    int listCount[n3dsObject::NumTypes];
    int subCount[n3dsObject::NumSubTypes];
    friend class iterator;
    static int __cdecl SortId(const void* elm0, const void* elm1);
};


//------------------------------------------------------------------------------
/**
*/
inline
n3dsObject& 
n3dsSceneList::operator[](int index) const
{
    return this->list[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
n3dsSceneList::Size() const
{
    return this->list.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsSceneList::iterator 
n3dsSceneList::Begin() const
{
    return iterator( 0, this);
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsSceneList::iterator 
n3dsSceneList::Begin(n3dsObject::ObjectType const& type) const
{
    iterator it( -1, this, type);
    ++it;
    return it;
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsSceneList::iterator 
n3dsSceneList::Begin(n3dsObject::ObjectType const& type, n3dsObject::ObjectSubType const& subType) const
{
    iterator it( -1, this, type, subType);
    ++it;
    return it;
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsSceneList::iterator 
n3dsSceneList::End() const
{
    return iterator(list.Size(), this);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3dsSceneList::iterator::operator != ( iterator const& it ) const
{
    return !(*this == it );
}


//------------------------------------------------------------------------------
/**
*/
inline
n3dsObject& 
n3dsSceneList::iterator::operator * ()
{
    return (*list)[index];
}

#endif

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------