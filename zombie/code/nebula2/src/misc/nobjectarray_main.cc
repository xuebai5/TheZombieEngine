//------------------------------------------------------------------------------
/**
    @file nobjectarray_main.cc
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "misc/nobjectarray.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nObjectArray, "nobject");

//------------------------------------------------------------------------------
void 
nObjectArray::Init(int initialSize, int initialGrow)
{
    n_assert(!this->objs);
    this->objs = n_new(nArray<ObjectInfo>)(initialSize, initialGrow);
    n_assert(this->objs);
}

//------------------------------------------------------------------------------
void 
nObjectArray::Clear()
{
    n_assert(this->objs);
    this->objs->Clear();
}

//------------------------------------------------------------------------------
int 
nObjectArray::Size() const
{
    if ( this->objs)
    {
        return this->objs->Size();
    } else
    {
        return 0; 
    }
}

//------------------------------------------------------------------------------
bool 
nObjectArray::Empty() const
{
    n_assert(this->objs);
    return this->objs->Empty();
}

//------------------------------------------------------------------------------
void 
nObjectArray::Append(nObject * obj)
{
    n_assert(obj);
    n_assert(this->objs);
    this->AppendWithName(obj, "");
}

//------------------------------------------------------------------------------
void 
nObjectArray::AppendWithName(nObject * obj, const char * name)
{
    n_assert(obj);
    n_assert(name);
    n_assert(this->objs);
    ObjectInfo oi;
    oi.obj = obj;
    oi.objName = name;
    this->objs->Append(oi);
}

//------------------------------------------------------------------------------
void 
nObjectArray::Set(int index, nObject * obj)
{
    n_assert(obj);
    n_assert(this->objs);
    this->SetWithName(index, obj, "");
}

//------------------------------------------------------------------------------
void 
nObjectArray::SetWithName(int index, nObject * obj, const char * name)
{
    n_assert(obj);
    n_assert(name);
    n_assert(this->objs);
    (*this->objs)[index].obj = obj;
    (*this->objs)[index].objName = name;
}

//------------------------------------------------------------------------------
void 
nObjectArray::Erase(int index)
{
    n_assert(this->objs);
    this->objs->Erase(index);
}

//------------------------------------------------------------------------------
nObject * 
nObjectArray::At(int index) const
{
    n_assert(this->objs);
    return this->objs->At(index).obj;
}

//------------------------------------------------------------------------------
const char *
nObjectArray::GetObjectNameAtIndex(int index) const
{
    n_assert(this->objs);
    return this->objs->At(index).objName.Get();
}

//------------------------------------------------------------------------------
int
nObjectArray::FindIndex(nObject * elem)
{
    n_assert(elem);
    n_assert(this->objs);
    int i(0);
    while(i < this->objs->Size())
    {
        if ((*this->objs)[i].obj == elem)
        {
            return i;
        }
        i++;
    }

    return -1;
}

//------------------------------------------------------------------------------
nObject * 
nObjectArray::BeginNewObject(const char * objClass, const char * objName)
{
    // autoinitialize if it was not done before
    if (!this->objs)
    {
        this->Init(64, 64);
    }

    nObject * obj = nObject::BeginNewObject(objClass, objName);
    n_assert(obj);
    this->AppendWithName(obj, objName);
    return obj;
}

//------------------------------------------------------------------------------
