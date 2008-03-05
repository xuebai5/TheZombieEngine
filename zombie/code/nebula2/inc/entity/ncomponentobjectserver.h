#ifndef N_COMPONENTOBJECTSERVER_H
#define N_COMPONENTOBJECTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nComponentObjectServer
    @ingroup NebulaEntitySystem
    @brief Server managing component objects
    @author Mateu Batle

    All components must be registered in the component server before usage.

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/nkeyarray.h"

//------------------------------------------------------------------------------
class nClassComponentObject;
class nComponentObject;

//------------------------------------------------------------------------------
class nComponentObjectServer
{
public:
    
    /// constructor
    nComponentObjectServer();
    /// destructor
    virtual ~nComponentObjectServer(void);

    /// singleton instance
    static nComponentObjectServer * Instance();

    /// add a component object nClass to the kernel
    void AddClass(const nComponentId & superId, nClassComponentObject * cl);
    /// close and remove a component object nClass from the kernel
    void RemClass(nClassComponentObject * cl);
    /// create a component object nClass object
    //nClassComponentObject* CreateClass(const nComponentId & id);
    /// release a component object nClass object
    void ReleaseClass(nClassComponentObject* cl);
    /// find a loaded component object nClass object by name, and load it if not loaded
    nClassComponentObject* FindClass(const nComponentId & id);
    /// find a loaded component object nClass object by name, return 0 if it is not loaded
    //nClassComponentObject* IsClassLoaded(const nComponentId & id);
    /// return the list of component object nClasses
    const nHashList * GetClassList() const;
    /// add a code module to the component object server
    void AddModule(const char * name, nClassComponentObject * (*_init_func)(const char *, nComponentObjectServer *));

    /// create a component object, fail hard if no object could be created
    nComponentObject * New(const nComponentId & compId);

private:

    /// loads a component object and returns a pointer to it
    nClassComponentObject* OpenClass(const nComponentId & id);

    /// component server singleton instance
    static nComponentObjectServer * instance;

    /// map component id -> component object nClass
    nHashList classList;

    // the lock mutex to make this server multithreading safe
    nMutex mutex;

};

//--------------------------------------------------------------------
#endif
