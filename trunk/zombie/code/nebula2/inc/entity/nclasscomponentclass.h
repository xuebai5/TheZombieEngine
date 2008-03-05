#ifndef N_CLASSCOMPONENTCLASS_H
#define N_CLASSCOMPONENTCLASS_H
//------------------------------------------------------------------------------
/**
    @class nClassComponentClass
    @ingroup NebulaEntitySystem

    Nebula component class nClass. Its responsabilities:
    - Info about the scripting commands 
    - Info about signals
    - Creation of component class instances

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nclass.h"

//------------------------------------------------------------------------------
class nComponentClass;

//------------------------------------------------------------------------------
class nClassComponentClass : public nClass
{
public:

    /// constructor
    nClassComponentClass();
    /// constructor with parameters
    nClassComponentClass(const char * name, void* (*newFunc)(void), bool native);
    /// destructor
    virtual ~nClassComponentClass();

    /// create a component class
    virtual void * NewObject();

private:
};

//------------------------------------------------------------------------------
#endif // N_CLASSCOMPONENTCLASS_H
