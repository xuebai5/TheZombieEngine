#ifndef N_CLASSENTITYCLASS_H
#define N_CLASSENTITYCLASS_H
//------------------------------------------------------------------------------
/**
    @class nClassEntityClass
    @ingroup NebulaEntitySystem

    Nebula entity class nClass. Its responsabilities:
    - Info about the scripting commands 
    - Info about signals
    - Creation of entity class instances
    - Information about which components make up the entity class

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nclass.h"
#include "entity/ncomponentidlist.h"

//------------------------------------------------------------------------------
class nKernelServer;
class nClassComponentClass;

//------------------------------------------------------------------------------
class nClassEntityClass : public nClass
{
public:

    /// constructor
    nClassEntityClass();
    /// constructor with parameters
    nClassEntityClass(const char * name, nKernelServer * ks, void* (*newFunc)(void), bool native);
    /// destructor
    virtual ~nClassEntityClass();

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
#endif // N_CLASSENTITYCLASS_H
