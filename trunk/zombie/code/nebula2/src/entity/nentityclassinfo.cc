//-----------------------------------------------------------------------------
//  nentityclassinfo.cc
//  (C) 2005 Conjurer Services, S.A.
//----------------------------------------------------------------------------
#include "precompiled/pchnentity.h"

//----------------------------------------------------------------------------
#include "entity/nentityclassinfo.h"

//----------------------------------------------------------------------------
/**
    constructor
*/
nEntityClassInfo::nEntityClassInfo() :
    dirty(false),
    classKey(0),
    isEntityClass(true),
    deleted(false)
{
    /// empty
}

//----------------------------------------------------------------------------
/**
    constructor
*/
nEntityClassInfo::nEntityClassInfo(const char * name) :
    className(name),
    dirty(false),
    classKey(0),
    isEntityClass(true),
    deleted(false)
{
    /// empty
}

//----------------------------------------------------------------------------
/**
    copy constructor
*/
nEntityClassInfo::nEntityClassInfo(const nEntityClassInfo & cl) :
    className(cl.className),
    superClasses(cl.superClasses),
    dirty(cl.dirty),
    classKey(cl.classKey),
    isEntityClass(true),
    deleted(cl.deleted)
{
    this->SetEntityClass(cl.GetEntityClass());
}

//----------------------------------------------------------------------------
/**
    destructor
*/        
nEntityClassInfo::~nEntityClassInfo() 
{
    if (this->isEntityClass && this->refEntityClass.isvalid())
    {
        this->SetEntityClass(0);
    }
}

//----------------------------------------------------------------------------
/**
    assignment operator 
*/
nEntityClassInfo & 
nEntityClassInfo::operator=(const nEntityClassInfo & rhs) 
{
    this->className = rhs.className;
    this->superClasses = rhs.superClasses;
    this->dirty = rhs.dirty;
    this->deleted = rhs.deleted;
    this->classKey = rhs.classKey;
    this->SetEntityClass(rhs.GetEntityClass());
    this->isEntityClass = rhs.isEntityClass;
    return (*this);
}

//----------------------------------------------------------------------------
/**
    set the entity object
*/
void 
nEntityClassInfo::SetEntityClass(nEntityClass * neclass)
{
    n_assert(!this->deleted || !neclass);
    this->refEntityClass = neclass;
}

//----------------------------------------------------------------------------
/**
    get the entity object
*/

nEntityClass * 
nEntityClassInfo::GetEntityClass() const
{
    return this->refEntityClass.get_unsafe();
}

//----------------------------------------------------------------------------
/**
    mark the entity class to be deleted (on save time)
*/
bool 
nEntityClassInfo::IsDeleted() const
{
    return this->deleted;
}

//----------------------------------------------------------------------------
/**
    mark the entity class to be deleted (on save time)
*/
void
nEntityClassInfo::Delete()
{
    n_assert(!this->deleted);
    this->SetEntityClass(0);
    this->dirty = true;
    this->deleted = true;
}

//----------------------------------------------------------------------------
