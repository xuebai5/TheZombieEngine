//------------------------------------------------------------------------------
//  ncomponentobjectserver.cc
//  (c) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/ncomponentobjectserver.h"
#include "entity/nclasscomponentobject.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nComponentObjectServer *nComponentObjectServer::instance = 0;

//------------------------------------------------------------------------------
/**
    constructor
*/
nComponentObjectServer::nComponentObjectServer()
{
    if (!instance)
    {
        instance = this;
    }
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nComponentObjectServer::~nComponentObjectServer()
{
    // delete class list
    bool isEmpty;
    long numZeroRefs = 1;
    while (( false == (isEmpty = this->classList.IsEmpty()) ) && (numZeroRefs > 0))
    {
        numZeroRefs = 0;
        nClassComponentObject *actClass = (nClassComponentObject *) this->classList.GetHead();
        nClassComponentObject *nextClass;
        do
        {
            nextClass = (nClassComponentObject *) actClass->GetSucc();
            if (actClass->GetRefCount() == 0)
            {
                NLOG(kernel, (0, "delete component object nClass %s", actClass->GetProperName()));
                numZeroRefs++;
                if (actClass->GetSuperClass())
                {
                    actClass->GetSuperClass()->RemSubClass(actClass);
                }
                this->RemClass(actClass);
                n_delete(actClass);
            }
            actClass = nextClass;
        } while (actClass);
    }

    // reset instance pointer if needed
    if (instance == this)
    {
        instance = 0;
    }
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nComponentObjectServer *
nComponentObjectServer::Instance()
{
    n_assert(instance);
    return instance;
}

//------------------------------------------------------------------------------
/**
    @brief Add a component object nClass to the kernel
    @param superId      component id of the super class
    @param cl           component object nClass
*/
void 
nComponentObjectServer::AddClass(
    const nComponentId & superId, 
    nClassComponentObject * cl)
{
    this->mutex.Lock();
    n_assert(cl);

    // Special case: root classes have "nComponentObject" parent
    if( nComponentObject::GetComponentIdStatic() != superId ) 
    {
        nClassComponentObject * superClass = this->OpenClass(superId);
        if (superClass)
        {
            superClass->AddSubClass(cl);
            this->classList.AddTail(cl);
        }
        else
        {
            n_error("nComponentObjectServer::AddClass(): Could not open super class '%s'\n", superId.AsChar());
        }
    }
    else
    {
        // It's a root class, just add it
        this->classList.AddTail(cl);
    }
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @brief close and remove a component object nClass from the kernel
    @param cl           component object nClass to remove
*/
void 
nComponentObjectServer::RemClass(nClassComponentObject * cl)
{
    this->mutex.Lock();
    n_assert(cl);
    cl->Remove();
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @brief Create a component object nClass object
    @param id           component object id
*/
//nClassComponentObject* 
//nComponentObjectServer::CreateClass(const nComponentId & id)
//{
//    n_assert2_always("not implemented");
//    return 0;
//}

//------------------------------------------------------------------------------
/**
    @brief Release a component object nClass object
    @param cl           component object nClass
*/
void 
nComponentObjectServer::ReleaseClass(nClassComponentObject* cl)
{
    n_assert(cl);
    this->mutex.Lock();
    cl->Release();
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @brief Find a loaded component object nClass object by name, 
    and load it if not loaded
    @param id           component object id of the nClass
    @return pointer to the nClass
*/
nClassComponentObject* 
nComponentObjectServer::FindClass(const nComponentId & id)
{
    this->mutex.Lock();
    nClassComponentObject *cl = (nClassComponentObject *) this->classList.Find(id.AsChar());
    this->mutex.Unlock();
    return cl;
}

//------------------------------------------------------------------------------
/**
    @return pointer to the list of all component object nClasses
*/
const nHashList * 
nComponentObjectServer::GetClassList() const
{
    return &this->classList;
}

//------------------------------------------------------------------------------
/**
    @brief Adds a module to the class component server
*/
void 
nComponentObjectServer::AddModule(
    const char * name, 
    nClassComponentObject * (*_init_func)(const char *, nComponentObjectServer *) )
{
    this->mutex.Lock();
    nClassComponentObject *cl = (nClassComponentObject *) this->classList.Find(name);
    if (!cl)
    {
        cl = _init_func(name, this);
    }
    this->mutex.Unlock();
}
//------------------------------------------------------------------------------
/**
    @brief Creates a new component object
    @return pointer to the component object created
*/
nComponentObject *
nComponentObjectServer::New(const nComponentId & id)
{
    this->mutex.Lock();
    nClassComponentObject *cl = this->OpenClass(id);
    nComponentObject *obj = 0;
    if (cl)
    {
        obj = (nComponentObject *) cl->NewObject();
    }
    this->mutex.Unlock();
    return obj;
}

//------------------------------------------------------------------------------
/**
    Locates a component object nClass and returns pointer to it.

    @param    id        id of the nClass to be opened
    @return             pointer to nClass object
*/
nClassComponentObject*
nComponentObjectServer::OpenClass(const nComponentId & id)
{
    this->mutex.Lock();
    nClassComponentObject *cl = (nClassComponentObject*) this->classList.Find(id.AsChar());
    this->mutex.Unlock();
    return cl;
}
