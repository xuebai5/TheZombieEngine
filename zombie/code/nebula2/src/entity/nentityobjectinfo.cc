#include "precompiled/pchnentity.h"
//-----------------------------------------------------------------------------
//  nentityobjectinfo.cc
//  (C) 2005 Conjurer Services, S.A.
//----------------------------------------------------------------------------
#include "entity/nentityobjectinfo.h"

//----------------------------------------------------------------------------
/**
    constructor
*/
nEntityObjectInfo::nEntityObjectInfo() :
    dirty(false),
    deleted(false)
{
    /// empty
}

//----------------------------------------------------------------------------
/**
    copy constructor
*/
nEntityObjectInfo::nEntityObjectInfo(const nEntityObjectInfo & obj) :
    dirty(obj.dirty),
    deleted(obj.deleted)
{
    this->SetEntityObject(obj.GetEntityObject());
}

//----------------------------------------------------------------------------
/**
    destructor
*/
nEntityObjectInfo::~nEntityObjectInfo()
{
    if (this->refObject.isvalid())
    {
        n_assert(!this->deleted);
        this->refObject->Release();
        this->refObject.invalidate();
    }
}

//----------------------------------------------------------------------------
/**
    assignment operator 
*/
nEntityObjectInfo & 
nEntityObjectInfo::operator =(const nEntityObjectInfo & rhs) 
{
    this->dirty = rhs.dirty;
    this->SetEntityObject(rhs.GetEntityObject());
    this->deleted = rhs.deleted;
    return (*this);
}

//----------------------------------------------------------------------------
/**
    set the entity object
*/
void 
nEntityObjectInfo::SetEntityObject(nEntityObject * obj)
{
    n_assert(!this->deleted || !obj);
    nEntityObject * tmp = this->refObject.get_unsafe();
    this->refObject = obj;
    if (this->refObject.isvalid())
    {
        n_assert(!this->deleted);
        this->refObject->AddRef();
    }
    if (tmp)
    {
        tmp->Release();
    }
}

//----------------------------------------------------------------------------
/**
    get the entity object
*/
nEntityObject * 
nEntityObjectInfo::GetEntityObject() const
{
    return this->refObject.get_unsafe();
}

//----------------------------------------------------------------------------
/**
    get dirty state
*/
bool 
nEntityObjectInfo::GetDirty() const
{
    return dirty;
}

//----------------------------------------------------------------------------
/**
    set dirty state
*/
void 
nEntityObjectInfo::SetDirty(bool d)
{
    this->dirty = d;
}

//----------------------------------------------------------------------------
/**
    get deleted status of the entity object
*/
bool 
nEntityObjectInfo::IsDeleted() const
{
    return this->deleted;
}

//----------------------------------------------------------------------------
/**
    Delete an object
*/
void 
nEntityObjectInfo::Delete()
{
    n_assert(!this->deleted);
    this->deleted = true;
    this->SetDirty(true);
}

//----------------------------------------------------------------------------
/**
    Undelete an object
*/
void 
nEntityObjectInfo::Undelete()
{
    n_assert(this->deleted);
    this->deleted = false;
    this->SetDirty(true);
}

//----------------------------------------------------------------------------
