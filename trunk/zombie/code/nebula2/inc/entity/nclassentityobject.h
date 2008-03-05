#ifndef N_CLASSENTITYOBJECT_H
#define N_CLASSENTITYOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nClassEntityObject
    @ingroup NebulaEntitySystem

    Nebula entity object nClass. Its responsabilities:
    - Info about the scripting commands 
    - Info about signals
    - Creation of entity object instances
    - Information about which components make up the entity object

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nclass.h"
#include "entity/ncomponentidlist.h"

//------------------------------------------------------------------------------
class nKernelServer;

//------------------------------------------------------------------------------
class nClassEntityObject : public nClass
{
public:

    /// constructor
    nClassEntityObject();
    /// constructor with parameters
    nClassEntityObject(const char * name, nKernelServer * ks, void* (*newFunc)(void), bool native);
    /// destructor
    virtual ~nClassEntityObject();

    /// begin defining commands
    virtual void BeginCmds();
    /// begin definition of signals
    virtual void BeginSignals(int numSignals);

    /// create a component class
    virtual void * NewObject();

    /// return the list of components making up the entity object
    nComponentIdList * GetComponentIdList();

    /// one step setup done with begin, add components in list and end
    void SetupComponents(const nComponentId * list);

private:

    nComponentIdList compList;

};

//------------------------------------------------------------------------------
#endif // N_CLASSENTITYOBJECT_H
