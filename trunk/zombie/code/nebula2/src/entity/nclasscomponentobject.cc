//------------------------------------------------------------------------------
//  nclasscomponentobject.cc
//  (c) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nclasscomponentobject.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nClassComponentObject::nClassComponentObject()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Constructor with parameters
    @param name               name of the component object class
    @param newFunc            pointer to n_create function in class package
    @param native             Native or runtime created component object nClass
*/
nClassComponentObject::nClassComponentObject(
    const char* name, 
    void* (*newFunc)(void), 
    bool native ) :
    nClass(name, 0, newFunc, native)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nClassComponentObject::~nClassComponentObject()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Create a new component object
*/
void *
nClassComponentObject::NewObject()
{
    nComponentObject * obj = (nComponentObject *) this->n_new_ptr();
    n_assert(obj);
    //obj->SetClass(this);
    return obj;
}

//------------------------------------------------------------------------------
