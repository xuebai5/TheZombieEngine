#ifndef N_COMPONENTCLASSSERVER_H
#define N_COMPONENTCLASSSERVER_H
//------------------------------------------------------------------------------
/**
    @class nComponentClassServer
    @ingroup NebulaEntitySystem
    @brief Server managing component classes
    @author Mateu Batle

    All components classes must be registered in the component class server 
    before usage.

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentitytypes.h"
#include "kernel/nroot.h"
#include "kernel/nmutex.h"
#include "util/nkeyarray.h"
#include "util/nhashlist.h"

//------------------------------------------------------------------------------
class nClassComponentClass;
class nComponentClass;

//------------------------------------------------------------------------------
class nComponentClassServer
{
public:
    
    /// constructor
    nComponentClassServer();
    /// destructor
    virtual ~nComponentClassServer(void);

    /// singleton instance
    static nComponentClassServer * Instance();

    /// add a component class nClass to the kernel
    void AddClass(const nComponentId & superId, nClassComponentClass * cl);
    /// close and remove a component class nClass from the kernel
    void RemClass(nClassComponentClass * cl);
    /// create a component class nClass object
    //nClassComponentClass * CreateClass(const nComponentId & id);
    /// release a component class nClass object
    void ReleaseClass(nClassComponentClass * cl);
    /// find a loaded component class nClass object by name, and load it if not loaded
    nClassComponentClass * FindClass(const nComponentId & id);
    /// find a loaded component class nClass object by name, return 0 if it is not loaded
    //nClassComponentClass* IsClassLoaded(const nComponentId & id);
    /// return the list of component class nClasses
    const nHashList * GetClassList() const;
    /// add a code module to the component class server
    void AddModule(const char * name, nClassComponentClass * (*_init_func)(const char *, nComponentClassServer *));

    /// create a component object, fail hard if no object could be created
    nComponentClass * New(const nComponentId & compId);

private:

    /// loads a component class and returns a pointer to it
    nClassComponentClass* OpenClass(const nComponentId & id);

    /// component server singleton instance
    static nComponentClassServer * instance;

    /// map component id -> component classes nClass
    nHashList classList;

    // the lock mutex to make this server multithreading safe
    nMutex mutex;

};

//--------------------------------------------------------------------
#endif
