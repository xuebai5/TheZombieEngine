#ifndef N_SORTEDARRAY_H
#define N_SORTEDARRAY_H
//------------------------------------------------------------------------------
/**
    @class nSortedArray
    @ingroup NebulaDataTypes

    @brief A dynamic array template class automatically sorted.
*/
//------------------------------------------------------------------------------
#include "util/narray.h"

//------------------------------------------------------------------------------
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
class nSortedArray : public nArray<TYPE>
{
public:
    /// Append a new element sorted
    void Append(const TYPE& elm);
    /// Append the contents of another array
    void operator += (const  nSortedArray<TYPE,compare> & rhs);
    /// find identical element in array, return index
    int FindIndex(const TYPE& elm) const;
    /// find identical element in array, return iterator
    typename nArray<TYPE>::iterator Find(const TYPE& elm) const;
    /// assignment operator
    nSortedArray<TYPE,compare>& operator=(const nArray<TYPE>& rhs);

private:
    /// Find the insert position of an element 
    int FindPosition(const TYPE& elm, int& idx) const;

    /// deactivate access to the following methods

    /// push element to back of array
    TYPE& PushBack(const TYPE& elm);
    /// reserve 'num' elements at end of array and return pointer to first element
    iterator Reserve(int num);
    /// set element at index, grow array if necessary
    TYPE& Set(int index, const TYPE& elm);
    /// insert element at index
    void Insert(int index, const TYPE& elm);
    /// find array range with element
    void Fill(int first, int num, const TYPE& elm);
    /// Wrap function of compare funcion
    static int __cdecl WrapCompareFunction(const void*, const void*);
};

//------------------------------------------------------------------------------
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
void
nSortedArray<TYPE,compare>::Append(const TYPE& elm)
{
    int num = this->Size();

    int idx;
    int cmp = this->FindPosition(elm,idx);
    if (0 == cmp)
    {
        this->nArray<TYPE>::Set(idx, elm);
    } 
    else
    {
        if (cmp>0)
        {
            idx++;
        } 
        if (idx == num )
        {
            this->nArray<TYPE>::Append(elm);
        } 
        else 
        {
            this->nArray<TYPE>::Insert(idx,elm);
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Search the position of element elm
    @param elm the element
    @param idx output the idx
    @return < 0   elm less    than nSortedArray[idx]
            ==0   elm equal   to   nSortedArray[idx]
            > 0   elm greater than nSortedArray[idx]

*/
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
int  
nSortedArray<TYPE,compare>::FindPosition(const TYPE& elm,int &idx) const
{
    bool valid = false;
    int max = this->Size() - 1;
    int min = 0;
    int cmp = -1;
    idx = 0;

    while (!valid && (min <= max)) 
    {
        idx = (min + max) / 2;
        cmp = compare(&elm, &((*this)[idx]));
        valid = (cmp == 0);
        if (cmp<0)
        {   
            max = idx - 1;
        } 
        else if (cmp>0)
        {
            min = idx + 1;
        }
    }
    
    return cmp;
}

//------------------------------------------------------------------------------
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
void 
nSortedArray<TYPE,compare>::operator +=(const  nSortedArray<TYPE,compare>& rhs)
{
    int idx;
    for (idx = 0 ; idx < rhs.Size() ; idx ++)
    {
        this->Append(rhs[idx]);
    }
}

//------------------------------------------------------------------------------
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
int 
nSortedArray<TYPE,compare>::FindIndex(const TYPE& elm) const
{
    int idx;
    int val = this->FindPosition(elm, idx);
    if (val)
    {
        return -1;
    }
    return idx;
}

//------------------------------------------------------------------------------
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
typename nArray<TYPE>::iterator
nSortedArray<TYPE,compare>::Find(const TYPE& elm) const
{
    int idx;
    if (this->FindPosition(elm, idx))
    {
        return 0;
    }
    return &((*this)[idx]);
}
//------------------------------------------------------------------------------
/**
*/
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
nSortedArray<TYPE,compare>&
nSortedArray<TYPE,compare>::operator=(const nArray<TYPE>& rhs)
{
    if (this != &rhs)
    {
        nArray<TYPE>::operator =(rhs);
        this->QSort(WrapCompareFunction);
    }
   
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE, int ( __cdecl *compare ) ( const TYPE *, const TYPE *)>
int 
nSortedArray<TYPE,compare>::WrapCompareFunction(const void* elm0, const void* elm1)
{
    return compare( static_cast<const TYPE*>(elm0) , static_cast<const TYPE*>(elm1) );
}

#endif
