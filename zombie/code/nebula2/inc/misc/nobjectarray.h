#ifndef N_OBJECTARRAY_H
#define N_OBJECTARRAY_H
//------------------------------------------------------------------------------
/**
    @class nObjectContainer
    @brief Class used for entity instances
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "util/narray.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class nObjectArray : public nObject
{
public:
    /// constructor
    nObjectArray();
    /// destructor
    virtual ~nObjectArray();

    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// initialize array of objects with initial size and grow size
    void Init(int, int);
    /// clean 
    void Clear();
    /// get the number of elements of the array
    int Size() const;
    /// return true if array empty
    bool Empty() const;
    /// add new object
    void Append(nObject *);
    /// add new object with a name
    void AppendWithName(nObject *, const char *);
    /// Set the element at index in the array
    void Set(int, nObject *);
    /// Set the element at index in the array with a name
    void SetWithName(int, nObject *, const char *);
    /// erase element at index
    void Erase(int);
    /// return reference to nth element in array
    nObject * At(int) const;
    /// return nth object name in array
    const char * GetObjectNameAtIndex(int) const;
    /// find element, return index or -1 if not found
    int FindIndex(nObject *);

    /// INTERNAL, DONT USE DIRECTLY, used for persistence (create object)
    virtual nObject * BeginNewObject(const char * objClass, const char * objName);

protected:

    void Dealloc();

    struct ObjectInfo
    {
        nRef<nObject> obj;
        nString objName;
    };

    /// container of objects
    nArray<ObjectInfo> * objs;
};

//------------------------------------------------------------------------------
inline
nObjectArray::nObjectArray() :
    objs(0)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nObjectArray::~nObjectArray()
{
    this->Dealloc();
}

//------------------------------------------------------------------------------
inline
void
nObjectArray::Dealloc(void)
{
    if (this->objs)
    {
        n_delete(this->objs);
    }
}

//------------------------------------------------------------------------------
#endif N_OBJECTARRAY_H
