#ifndef N_KEYARRAYITERATOR_H
#define N_KEYARRAYITERATOR_H

//------------------------------------------------------------------------------
/**
    @class KeyArrayIterator
    @ingroup NebulaFSMSystem

    nKeyArray container forward constant iterator.

    (C) 2006 Conjurer Services, S.A.
*/

#include "util/nkeyarray.h"

//------------------------------------------------------------------------------
template< class Type >
class KeyArrayIterator
{
public:
    /// Set the array to iterate and set the iterator to its first element
    KeyArrayIterator( const nKeyArray<Type>& keyarray )
        : container(keyarray), index(0)
    {
        // Empty
    }
    /// Assignment operator
    const KeyArrayIterator& operator = ( const KeyArrayIterator<Type>& it )
    {
        this->container = it.container;
        this->index = it.index;
    }
    /// Tell if the iterator has passed the last element
    bool IsEnd() const
    {
        return this->index >= this->container.Size();
    }
    /// Return the current element
    const Type& Get() const
    {
        return this->container.GetElementAt(index);
    }
    /// Go to the next element
    void Next()
    {
        ++this->index;
    }

private:
    /// KeyArray to iterate
    const nKeyArray<Type>& container;
    /// Current position in the array
    int index;
};

#endif
