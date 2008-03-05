#ifndef N_CLASSCOMPONENTOBJECT_H
#define N_CLASSCOMPONENTOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nClassComponentObject
    @ingroup NebulaEntitySystem

    Nebula component object nClass. Its responsabilities:
    - Info about the scripting commands 
    - Info about signals
    - Creation of component class instances

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nclass.h"

//------------------------------------------------------------------------------
class nComponentObject;

//------------------------------------------------------------------------------
class nClassComponentObject : public nClass
{
public:

    /// constructor
    nClassComponentObject();
    /// constructor with parameters
    nClassComponentObject(const char * name, void* (*newFunc)(void), bool native);
    /// destructor
    virtual ~nClassComponentObject();

    /// create a component object
    virtual void * NewObject();

private:
};

//------------------------------------------------------------------------------
#endif // N_CLASSCOMPONENTOBJECT_H
