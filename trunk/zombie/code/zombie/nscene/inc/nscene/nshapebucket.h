#ifndef N_SHAPEBUCKET_H
#define N_SHAPEBUCKET_H
//------------------------------------------------------------------------------
/**
    @class nShapeBucket
    @ingroup SceneNodes

    this class is useful for shader-sorted phases
    but there should be others for depth- or priority- sorted.

    (C) 2004 Conjurer Services, S.A.
*/
#include "util/nsafekeyarray.h"

class nShader2;
//------------------------------------------------------------------------------
class nShapeBucket
{
public:
    /// constructor
    nShapeBucket();
    /// destructor
    ~nShapeBucket();
    /// reset arrays
    void Begin(bool indexed);
    /// empty array of shaders and shapes
    void Clear();
    /// current number of shaders
    int Size();

    /// get index for a shader or else add it
    nArray<int>& GetShapeArray(int shaderIndex);

    /// append a shader and return index, even if it exists
    int Append(int shaderIndex);

    /// get shader index for a bucket
    int GetShaderIndexAt(int bucketIndex);
    /// get array of shapes for a given index
    nArray<int>& GetShapeArrayAt(int bucketIndex);
    
private:
    /// use indexed or non-indexed array?
    bool useIndexed;

    /// array of shape indices, indexed by shader index
    nSafeKeyArray<nArray<int> > shapeKeyArray;
    /// indices into shapeKeyArray
    nArray<int> shapeKeyIndices;

    /// array of shape indices
    nArray<int> shaderArray;
    nArray<nArray<int> > shapeArray;
};

//------------------------------------------------------------------------------
/**    
*/
inline
nShapeBucket::nShapeBucket() :
    shapeKeyArray(32,32),
    useIndexed(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**    
*/
inline
nShapeBucket::~nShapeBucket()
{
    // empty
}

//------------------------------------------------------------------------------
/**    
*/
inline
void
nShapeBucket::Begin(bool indexed)
{
    this->useIndexed = indexed;
    if (indexed)
    {
        int numShapes = this->shapeKeyArray.Size();
        for (int i = 0; i < numShapes; i++)
        {
            this->shapeKeyArray[i].Reset();
        }
        this->shapeKeyIndices.Reset();
    }
    else
    {
        this->shaderArray.Reset();
        int numShapes = this->shapeArray.Size();
        for (int i = 0; i < numShapes; i++)
        {
            this->shapeArray[i].Reset();
        }
    }
}

//------------------------------------------------------------------------------
/**    
*/
inline
void
nShapeBucket::Clear()
{
    this->Begin(this->useIndexed);
    this->shapeKeyArray.Clear();
    this->shapeKeyIndices.Clear();
}

//------------------------------------------------------------------------------
/**    
*/
inline
int
nShapeBucket::Size()
{
    if (this->useIndexed)
    {
        return this->shapeKeyIndices.Size();
    }
    else
    {
        return this->shaderArray.Size();
    }
}

//------------------------------------------------------------------------------
/**  
    Append a shader to the shader array, even if it already
    exists. It is used to group geometry besides sorting order.
    Reuses the shape array, so that it doesn't need to be 
    reallocated every frame.
*/
inline
int
nShapeBucket::Append(int shaderIndex)
{
    n_assert(!this->useIndexed);
    this->shaderArray.Append(shaderIndex);
    while (this->shapeArray.Size() < this->shaderArray.Size())
    {
        this->shapeArray.Append(nArray<int>(16,16));
    }
    return this->shaderArray.Size() - 1;
}

//------------------------------------------------------------------------------
/**  
    Find the shader in the local shader and shapes array,
    or create a new one if not cound.
*/
inline
nArray<int>&
nShapeBucket::GetShapeArray(int shaderIndex)
{
    n_assert(this->useIndexed);
    if (!this->shapeKeyArray.HasKey(shaderIndex))
    {
        this->shapeKeyArray.Add(shaderIndex, nArray<int>(1024,1024));
    }
    nArray<int>& shapeArray = this->shapeKeyArray.GetElement(shaderIndex);
    // optimize access to shapeKeyArray using an array of indices
    if (shapeArray.Empty())
    {
        this->shapeKeyIndices.Append(shaderIndex);
    }
    return shapeArray;
}

//------------------------------------------------------------------------------
/**    
*/
inline
int
nShapeBucket::GetShaderIndexAt(int bucketIndex)
{
    if (this->useIndexed)
    {
        n_assert(bucketIndex < this->shapeKeyIndices.Size());
        return this->shapeKeyIndices.At(bucketIndex);
    }
    else
    {
        n_assert(bucketIndex < this->shaderArray.Size());
        return this->shaderArray.At(bucketIndex);
    }
}

//------------------------------------------------------------------------------
/**    
*/
inline
nArray<int>&
nShapeBucket::GetShapeArrayAt(int bucketIndex)
{
    if (this->useIndexed)
    {
        n_assert(bucketIndex < this->shapeKeyIndices.Size());
        return this->shapeKeyArray.GetElement(this->shapeKeyIndices[bucketIndex]);
    }
    else
    {
        n_assert(bucketIndex < this->shaderArray.Size());
        return this->shapeArray.At(bucketIndex);
    }
}

//------------------------------------------------------------------------------
#endif
