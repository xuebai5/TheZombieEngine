#ifndef N_REFENTITYOBJECT_H
#define N_REFENTITYOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nRefEntityObject
    @ingroup NebulaSmartPointers

    An nRef with the ability to resolve itself from a entity object id.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "kernel/nkernelserver.h"
#include "entity/nentityobject.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
class nRefEntityObject : public nRef<nEntityObject> 
{
public:
    /// constructor
    nRefEntityObject();
    /// constructor with target object
    nRefEntityObject(nEntityObject * obj);
    /// copy constructor
    nRefEntityObject(const nRefEntityObject & rhs);
    /// constructor with target entity object id
    nRefEntityObject(nEntityObjectId id);

    /// sets the entity object id
    void set(nEntityObjectId id);
    /// get entity object pointer
    nEntityObject* get();
    /// returns the entity object id
    nEntityObjectId getid() const;
    /// returns true if the object is valid
    bool isvalid();
    /// override -> operator
    nEntityObject* operator->();
    /// dereference operator
    nEntityObject& operator*();
    /// cast operator
    operator nEntityObject*();
    /// assign operator
    void operator=(nEntityObjectId id);

protected:
    /// resolve target pointer from id if necessary
    nEntityObject* check();

    nEntityObjectId id;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRefEntityObject::nRefEntityObject() :
    id(nEntityObjectServer::IDINVALID)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nRefEntityObject::nRefEntityObject(nEntityObject * obj) :
    nRef<nEntityObject>(obj)
{
    this->id = obj->GetId();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRefEntityObject::nRefEntityObject(const nRefEntityObject & rhs) :
    nRef<nEntityObject>(rhs)
{
    this->id = rhs.getid();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRefEntityObject::nRefEntityObject(nEntityObjectId oid) :
    id(oid)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRefEntityObject::set(nEntityObjectId oid)
{
    this->invalidate();
    this->id = oid;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nRefEntityObject::check()
{
    if (!this->targetObject) 
    {
        if (this->id == nEntityObjectServer::IDINVALID) 
        {
            return 0;
        }
        this->targetObject = (nEntityObject*) nEntityObjectServer::Instance()->GetEntityObject(this->getid());
        if (this->targetObject) 
        {
            ((nReferenced*)this->targetObject)->AddObjectRef((nRef<nReferenced> *)this);
        }
    }
    return (nEntityObject *) this->targetObject;
}    

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nRefEntityObject::get()
{
    if (!this->check()) 
    {
        n_error("nRefEntityObject: no target object '0x%x'!\n",  this->id);
    }
    return this->targetObject;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObjectId
nRefEntityObject::getid() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRefEntityObject::isvalid()
{
    return this->check() ? true : false;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nRefEntityObject::operator->()
{
    return this->get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject&
nRefEntityObject::operator*()
{
    return *this->get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRefEntityObject::operator nEntityObject*()
{
    return this->get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRefEntityObject::operator=(nEntityObjectId oid)
{
    this->set(oid);
}

//-------------------------------------------------------------------
#endif
