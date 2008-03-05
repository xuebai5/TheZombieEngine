//------------------------------------------------------------------------------
/**
    @file ncomponentclassserver.cc
    @author Mateu Batle

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/ncomponentclassserver.h"
#include "entity/nclasscomponentclass.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nComponentClassServer *nComponentClassServer::instance = 0;

//------------------------------------------------------------------------------
/**
    constructor
*/
nComponentClassServer::nComponentClassServer()
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
nComponentClassServer::~nComponentClassServer()
{
    // delete class list
    bool isEmpty;
    long numZeroRefs = 1;
    while (( false == (isEmpty = this->classList.IsEmpty()) ) && (numZeroRefs > 0))
    {
        numZeroRefs = 0;
        nClassComponentClass *actClass = (nClassComponentClass *) this->classList.GetHead();
        nClassComponentClass *nextClass;
        do
        {
            nextClass = (nClassComponentClass *) actClass->GetSucc();
            if (actClass->GetRefCount() == 0)
            {
                NLOG(kernel, (0, "delete component class nClass %s", actClass->GetProperName()));
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
nComponentClassServer *
nComponentClassServer::Instance()
{
    n_assert(instance);
    return instance;
}

//------------------------------------------------------------------------------
/**
    @brief Add a component class nClass to the server
    @param superId      component id of the super class
    @param cl           component class nClass
*/
void 
nComponentClassServer::AddClass(
    const nComponentId & superId, 
    nClassComponentClass * cl)
{
    this->mutex.Lock();
    n_assert(cl);

    // Special case: root classes have "nComponentClass" parent
    //OutputDebugStr(cl->GetName());
    if( nComponentClass::GetComponentIdStatic() != superId ) 
    {
        nClassComponentClass * superClass = this->OpenClass(superId);
        if (superClass)
        {
            superClass->AddSubClass(cl);
            this->classList.AddTail(cl);
        }
        else
        {
            n_error("nComponentClassServer::AddClass(): Could not open super class '%s'\n", superId.AsChar());
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
    @brief close and remove a component class nClass from the kernel
    @param cl           component class nClass to remove
*/
void 
nComponentClassServer::RemClass(nClassComponentClass * cl)
{
    this->mutex.Lock();
    n_assert(cl);
    cl->Remove();
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @brief Create a component class nClass object
    @param id           component class id
*/
//nClassComponentClass* 
//nComponentClassServer::CreateClass(const nComponentId & id)
//{
//    n_assert2_always("not implemented");
//    return 0;
//}

//------------------------------------------------------------------------------
/**
    @brief Release a component class nClass object
    @param cl           component class nClass
*/
void 
nComponentClassServer::ReleaseClass(nClassComponentClass* cl)
{
    n_assert(cl);
    this->mutex.Lock();
    cl->Release();
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @brief Find a loaded component class nClass object by name, 
    and load it if not loaded
    @param id           component class id of the nClass
    @return pointer to the nClass
*/
nClassComponentClass* 
nComponentClassServer::FindClass(const nComponentId & id)
{
    this->mutex.Lock();
    nClassComponentClass *cl = (nClassComponentClass *) this->classList.Find(id.AsChar());
    this->mutex.Unlock();
    return cl;
}

//------------------------------------------------------------------------------
/**
    @return pointer to the list of all component class nClasses
*/
const nHashList * 
nComponentClassServer::GetClassList() const
{
    return &this->classList;
}

//------------------------------------------------------------------------------
/**
    @brief Adds a module to the class component server
*/
void 
nComponentClassServer::AddModule(
    const char * name, 
    nClassComponentClass * (*_init_func)(const char *, nComponentClassServer *) )
{
    this->mutex.Lock();
    //OutputDebugStr(name);
    nClassComponentClass *cl = (nClassComponentClass *) this->classList.Find(name);
    if (!cl)
    {
        cl = _init_func(name, this);
    }
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @brief Creates a new component class 
    @return pointer to the component class created
*/
nComponentClass *
nComponentClassServer::New(const nComponentId & id)
{
    this->mutex.Lock();
    nClassComponentClass *cl = this->OpenClass(id);
    nComponentClass *obj = 0;
    if (cl)
    {
        obj = (nComponentClass *) cl->NewObject();
    }
    this->mutex.Unlock();
    return obj;
}

//------------------------------------------------------------------------------
/**
    Locates a component class nClass and returns pointer to it.

    @param    id        id of the nClass to be opened
    @return             pointer to nClass class
*/
nClassComponentClass *
nComponentClassServer::OpenClass(const nComponentId & id)
{
    this->mutex.Lock();
    nClassComponentClass *cl = (nClassComponentClass *) this->classList.Find(id.AsChar());
    this->mutex.Unlock();
    return cl;
}
