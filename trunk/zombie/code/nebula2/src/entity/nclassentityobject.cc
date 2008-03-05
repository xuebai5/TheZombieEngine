//------------------------------------------------------------------------------
/**
    @file nclassentityobject.cc
    @author Mateu Batle

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nclassentityobject.h"
#include "entity/ncomponentobjectserver.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nClassEntityObject::nClassEntityObject()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Constructor with parameters
    @param name               name of the component object class
    @param ks                 kernel server instance
    @param newFunc            pointer to n_create function in class package
    @param native             Native or runtime created component object nClass
*/
nClassEntityObject::nClassEntityObject(
    const char* name, 
    nKernelServer * ks,
    void* (*newFunc)(void), 
    bool native ) :
    nClass(name, ks, newFunc, native)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nClassEntityObject::~nClassEntityObject()
{
    /// empty
}


//------------------------------------------------------------------------------
/**
    @brief Begin the definition of the commands for the entity object. 
    First of all the cmds from the components get copied, meaning BeginCmds must 
    be called after having set up the component id list of the entity object.
*/
void 
nClassEntityObject::BeginCmds()
{
    nClass::BeginCmds();

    // add all the cmds from the components 
    if (this->IsNative())
    {
        int i(0);
        while(i < this->compList.GetNumComponents())
        {
            const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
            nClass * cl = nComponentObjectServer::Instance()->FindClass(id);
            n_assert(cl);
            // add all the cmds from the component and all their parent components
            while(cl)
            {
                this->AddCmds(cl);
                this->AddScriptCmds(cl);
                cl = cl->GetSuperClass();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Begin the definition of signals for the entity object. 
    First of all the cmds from the components get copied, meaning BeginSignals 
    must be called after having set up the component id list of the entity object.
*/
void
nClassEntityObject::BeginSignals(int numSignals)
{
    int ns(numSignals);

    // get the number of total signals for the components and their parents
    if (this->IsNative())
    {
        int i(0);
        while(i < this->compList.GetNumComponents())
        {
            const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
            nClass * cl = nComponentObjectServer::Instance()->FindClass(id);
            n_assert(cl);
            while(cl)
            {
                ns += cl->GetNumSignals();
                cl = cl->GetSuperClass();
            }
        }
    }

    nClass::BeginSignals(ns);

    // add all the signals from the components 
    if (this->IsNative())
    {
        int i(0);
        while(i < this->compList.GetNumComponents())
        {
            const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
            nClass * cl = nComponentObjectServer::Instance()->FindClass(id);
            n_assert(cl);
            // add all the cmds from the component and their parent components
            while(cl)
            {
                this->AddSignals(cl);
                cl = cl->GetSuperClass();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Create a new component object
*/
void *
nClassEntityObject::NewObject()
{
    nEntityObject * obj = (nEntityObject *) this->n_new_ptr();
    n_assert(obj);
    obj->AddRef();
    obj->SetClass(this);
    obj->SetComponentIdList(this->GetComponentIdList());
    // create all the components and add them to the entity object

    int i(0);
    while(i < this->compList.GetNumComponents())
    {
        const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
        nComponentObject * comp = nComponentObjectServer::Instance()->New(id);
        n_assert(comp);
        obj->AddComponentUnsafe(comp);
    }

    return static_cast<nObject *> (obj);
}

//------------------------------------------------------------------------------
/**
    @brief Return the list of component ids of the entity object represented
    by this nClass.
*/
nComponentIdList * 
nClassEntityObject::GetComponentIdList()
{
    return &this->compList;
}

//------------------------------------------------------------------------------
void 
nClassEntityObject::SetupComponents(const nComponentId * list)
{
    n_assert(list);
    
    this->compList.BeginComponents();
    int i(0);
    while(list[i] != compIdInvalid)
    {
        const nComponentId & compId = list[i++];
        nClassComponentObject * cco = nComponentObjectServer::Instance()->FindClass(compId);
        if (cco)
        {
            this->compList.AddComponent(compId);
            // add all parents
            cco = static_cast<nClassComponentObject *> (cco->GetSuperClass());
            while(cco)
            {
                this->compList.AddParentComponent(nComponentId(cco->GetProperName()));
                cco = static_cast<nClassComponentObject *> (cco->GetSuperClass());
            }
        }
    }
    this->compList.EndComponents();
}

//------------------------------------------------------------------------------
