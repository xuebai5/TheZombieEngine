#ifndef N_NBLENDTARGETKEYARRAY_H
#define N_NBLENDTARGETKEYARRAY_H
//------------------------------------------------------------------------------
/**
    @class nnBlendTargetKeyArray
    @ingroup NebulaAnimationSystem
    
    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
#include "nscene/nBlendTargetKey.h"

//------------------------------------------------------------------------------
class nBlendTargetKeyArray
{
public:
    /// default constructor
    nBlendTargetKeyArray();
    /// constructor
    nBlendTargetKeyArray( int initialSize, int initialGrow);
    /// destructor
    ~nBlendTargetKeyArray();
    /// add target
    void AddBlendTargetKey( int keyIndex, const nBlendTargetKey& blendTargetKey );
    /// update targets
    void UpdateTargets( int keyIndex, nArray<int>& curves, nArray<int>& targets );
    /// get number of target keys
    int GetNumberTargetKeys();
    /// get blend target key
    nBlendTargetKey& GetBlendTargetKeyAt( int index );

private:
    /// dicotomic search by time (key), return index
    int DicotomicSearch( int key );

    nArray<nBlendTargetKey> keyArray;

    /// last used index
    int previousIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKeyArray::nBlendTargetKeyArray() :
    keyArray(),
    previousIndex(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKeyArray::nBlendTargetKeyArray( int initialSize, int initialGrow) :
    keyArray(initialSize, initialGrow),
    previousIndex(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKeyArray::~nBlendTargetKeyArray()
{
    //empty
}
//------------------------------------------------------------------------------
/**
    dicotomic search
*/
inline
int 
nBlendTargetKeyArray::DicotomicSearch( int data )
{
    if(keyArray.Size() > 0)
    {
        int left = 0;
        if( this->previousIndex != 0 )
        {
            left = previousIndex;
        }

        int right = this->keyArray.Size() -1;

        while( left <= right )
        {
            int center = static_cast<int>(( left + right ) / 2);
            if (data < this->keyArray[center].GetKeyIndex())
            {
                right= center- 1;
            }
            else
            {
                if (data > this->keyArray[center].GetKeyIndex())
                {
                    left= center + 1;
                }
                else
                {
                    return center;
                }
            }
        }

        // left - right are changed
        if( data > this->keyArray[right].GetKeyIndex() )
        {
            return left;
        }
        else
        {
            return right;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendTargetKeyArray::AddBlendTargetKey( int keyIndex, const nBlendTargetKey & blendTargetKey)
{
    int index = this->DicotomicSearch( keyIndex );
    if( this->keyArray.Size() > keyIndex )
    {
        this->keyArray.Insert( index, blendTargetKey );
    }
    else
    {
        this->keyArray.Append( blendTargetKey );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nBlendTargetKeyArray::UpdateTargets( int keyIndex, nArray<int>& curves, nArray<int>& targets )
{
    int currentIndex = this->DicotomicSearch(keyIndex);
    
    // compare changes between previous and current index
    while( this->previousIndex < currentIndex )
    {
        curves.Append(this->keyArray[this->previousIndex].GetCurveIndex());
        targets.Append(this->keyArray[this->previousIndex].GetTargetIndex());
        this->previousIndex++;
    }

    if( this->previousIndex == this->keyArray.Size())
    {
        this->previousIndex = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nBlendTargetKeyArray::GetNumberTargetKeys()
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKey& 
nBlendTargetKeyArray::GetBlendTargetKeyAt( int index )
{
    return this->keyArray.At(index);
}

//------------------------------------------------------------------------------
#endif
