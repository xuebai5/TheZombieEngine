//------------------------------------------------------------------------------
/**
    @file nclassentityclass.cc
    @author Mateu Batle

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nclassentityclass.h"
#include "entity/ncomponentclassserver.h"
#include "entity/nclasscomponentclass.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nClassEntityClass::nClassEntityClass()
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
nClassEntityClass::nClassEntityClass(
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
nClassEntityClass::~nClassEntityClass()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Begin the definition of the commands for the entity class. 
    First of all the cmds from the components get copied, meaning BeginCmds must 
    be called after having set up the component id list of the entity class.
*/
void 
nClassEntityClass::BeginCmds()
{
    nClass::BeginCmds();

    /// for native classes add all the cmds from the components and parent components
    if (this->IsNative())
    {
        int i(0);
        while(i < this->compList.GetNumComponents())
        {
            const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
            nClass * cl = nComponentClassServer::Instance()->FindClass(id);
            n_assert(cl);
            // add all the cmds from the components and parent components
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
nClassEntityClass::BeginSignals(int numSignals)
{
    int ns(numSignals);

    // get the number of total signals for the components and their parents
    if (this->IsNative())
    {
        int i(0);
        while(i < this->compList.GetNumComponents())
        {
            const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
            nClass * cl = nComponentClassServer::Instance()->FindClass(id);
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
            nClass * cl = nComponentClassServer::Instance()->FindClass(id);
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
nClassEntityClass::NewObject()
{
    nEntityClass * ec = (nEntityClass *) this->n_new_ptr();
    n_assert(ec);
    ec->AddRef();
    ec->SetClass(this);
    ec->SetComponentIdList(this->GetComponentIdList());

    // create all the components and add them to the entity object
    int i(0);
    while(i < this->compList.GetNumComponents())
    {
        const nComponentId & id = this->compList.GetComponentIdByIndex(i++);
        nComponentClass * comp = nComponentClassServer::Instance()->New(id);
        n_assert2(comp, id.AsChar());
        ec->AddComponentUnsafe(comp);
    }

    return static_cast<nObject *> (ec);
}

//------------------------------------------------------------------------------
/**
    @brief Return the list of component ids of the entity class represented
    by this nClass.
*/
nComponentIdList * 
nClassEntityClass::GetComponentIdList()
{
    return &this->compList;
}


//------------------------------------------------------------------------------
void 
nClassEntityClass::SetupComponents(const nComponentId * list)
{
    n_assert(list);
    
    this->compList.BeginComponents();
    int i(0);
    while(list[i] != compIdInvalid)
    {
        const nComponentId & compId = list[i++];
        nClassComponentClass * ccc = nComponentClassServer::Instance()->FindClass(compId);
        if (ccc)
        {
            this->compList.AddComponent(compId);
            // add all parents
            ccc = static_cast<nClassComponentClass *> (ccc->GetSuperClass());
            while(ccc)
            {
                this->compList.AddParentComponent(nComponentId(ccc->GetProperName()));
                ccc = static_cast<nClassComponentClass *> (ccc->GetSuperClass());
            }
        }
    }
    this->compList.EndComponents();
}

//------------------------------------------------------------------------------
