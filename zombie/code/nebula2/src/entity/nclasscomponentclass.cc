//------------------------------------------------------------------------------
/**
    @file nclasscomponentclass.cc
    @author Mateu Batle

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nclasscomponentclass.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nClassComponentClass::nClassComponentClass()
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
nClassComponentClass::nClassComponentClass(
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
nClassComponentClass::~nClassComponentClass()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Create a new component object
*/
void *
nClassComponentClass::NewObject()
{
    nComponentClass * obj = (nComponentClass *) this->n_new_ptr();
    n_assert(obj);
    //obj->SetClass(this);
    return obj;
}

//------------------------------------------------------------------------------
