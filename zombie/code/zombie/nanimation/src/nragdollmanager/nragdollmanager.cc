#include "precompiled/pchnanimation.h"

#include "nragdollmanager/nragdollmanager.h"
#include "entity/nentityobjectserver.h"
#include "ncragdoll/ncragdoll.h"
#include "animcomp/nchumragdoll.h"
#include "nlevel/nlevelmanager.h"
#include "entity/nentityclassserver.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nRagDollManager, "nroot");

//------------------------------------------------------------------------------

nRagDollManager* nRagDollManager::Singleton(0);

//------------------------------------------------------------------------------
/**
    Constructor

    history:
        - 07-Nov-2005   David Reyes    created

*/
nRagDollManager::nRagDollManager() :
    containerRegisters(10, 1),
    counter(0)
    ,init(false)
{
    n_assert2(!Singleton , "Trying to instanciate a second instance of a singleton");

    Singleton = this;

    // bind to level creation and loading
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelUnload, 
                                          this,
                                          &nRagDollManager::Clear,
                                          0);


    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nRagDollManager::InitRegisters,
                                          0);
}

//------------------------------------------------------------------------------
/**
    Destructor

    history:
        - 07-Nov-2005   David Reyes    created

*/
nRagDollManager::~nRagDollManager()
{
    this->Destroy();

    this->Singleton = 0;
}


//------------------------------------------------------------------------------
/**
    Registers a manager for a given type of rag-doll

    history:
        - 07-Nov-2005   David Reyes    created

*/
const int nRagDollManager::Register(const nString& className, const int numInstancesAllowed, const policy appliedpolicy)
{
    nTag tag(className);

    // check it's not already registered
    if (this->CheckRegisteredAlready(className))
    {
        return tag.KeyMap();
    }

    ragdollReg* reg(n_new(ragdollReg));

    n_assert2(reg, "Failed to allocate memory.");

    reg->NumInstances = 0;
    reg->NumInstancesAllowed = numInstancesAllowed;
    reg->policyToBeApplied = appliedpolicy;

    reg->ragdolls = n_new_array(ragdollReg::RagDolls, numInstancesAllowed);

    reg->freeSlots = n_new_array(int, numInstancesAllowed);

    for (int index(0); index < numInstancesAllowed; ++index)
    {
        reg->freeSlots[ index ] = index;

        reg->ragdolls[ index ].counter = 0;

        reg->ragdolls[ index ].className = className;
    }

    n_assert2(reg->ragdolls, "Failed to allocate memory.");

    this->containerRegisters.Add(tag.KeyMap(), reg);

    return tag.KeyMap();
}


//------------------------------------------------------------------------------
/**
    Gets a ragdoll

    @param id register id

    @return ragdoll common interface

    history:
        - 07-Nov-2005   David Reyes    created

*/
ncRagDoll* nRagDollManager::PopRagDoll(const int id)
{
    ragdollReg* reg(0);

    if (!this->containerRegisters.Find(id, reg))
    {
        n_assert2_always("Incorrect register id for ragdolls.");
        return 0;
    }

    if (reg->NumInstances == reg->NumInstancesAllowed)
    {
        // Run out of instances
        //  use policy to chose which one to be descarted
        switch(reg->policyToBeApplied)
        {
        case counting:
            this->ApplyCountingPolicy(reg);
            break;
        }
    }

    // get a free one
    const int numFreeOnes(reg->NumInstancesAllowed - reg->NumInstances);

    n_assert2(numFreeOnes, "Data corruption.");

    const int index(reg->freeSlots[ numFreeOnes - 1 ]);

    ncRagDoll* rdoll(reg->ragdolls[ index ].rdoll);

    ++reg->NumInstances;

    reg->ragdolls[ index ].counter = ++this->counter;

    return rdoll;
}

//------------------------------------------------------------------------------
/**
    Applies counting policy

    @param reg register where the policy will be applied

    history:
        - 07-Nov-2005   David Reyes    created

*/
void nRagDollManager::ApplyCountingPolicy(ragdollReg* reg)
{
    // Method FIFO

    // @todo: make it fast :D

    int indexRagDoll(~0);

    unsigned int counter(unsigned int(~0));

    for (int index(0); index < reg->NumInstancesAllowed; ++index)
    {
        const unsigned int lcounter(reg->ragdolls[ index ].counter);
        if (lcounter)
        {
            if (lcounter < counter)
            {
                counter = lcounter;
                indexRagDoll = index;
            }
        }
    }

    n_assert2(indexRagDoll != ~0, "Data corruption.");

    this->Remove(reg, indexRagDoll);
}

//------------------------------------------------------------------------------
/**
    Removes a ragdoll from the list of a given register

    @param reg register where the ragdoll will be removed
    @param index ragdoll's index

    history:
        - 07-Nov-2005   David Reyes    created

*/
void nRagDollManager::Remove(ragdollReg* reg, const int index)
{
    // get a free ones
    const int numFreeOnes(reg->NumInstancesAllowed - reg->NumInstances);

    reg->freeSlots[ numFreeOnes ] = index;

    reg->ragdolls[ index ].counter = 0;

    --reg->NumInstances;
}

//------------------------------------------------------------------------------
/**
    Not longer needed ragdoll marke it free

    @param reg register where the ragdoll will be removed

    history:
        - 07-Nov-2005   David Reyes    created

*/
void nRagDollManager::PushRagDoll(const int id, ncRagDoll* ragdoll)
{
    n_assert2(ragdoll, "Null pointer.");

    ragdollReg* reg(0);

    if (!this->containerRegisters.Find(id, reg))
    {
        n_assert2_always("Incorrect register id for ragdolls.");
        return;
    }

    this->Remove(reg, ragdoll);
}

//------------------------------------------------------------------------------
/**
    Removes a ragdoll from the list of a given register

    @param reg register where the ragdoll will be removed
    @param rinterface the ragdoll interface

    history:
        - 07-Nov-2005   David Reyes    created

*/
void nRagDollManager::Remove(ragdollReg* reg, ncRagDoll* rinterface)
{
    for (int index(0); index < reg->NumInstancesAllowed; ++index)
    {
        if (reg->ragdolls[ index ].rdoll == rinterface)
        {
            this->Remove(reg, index);            
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Get instance pointer

    @return pointer to the only instance of this object

    history:
        - 07-Nov-2005   David Reyes    created
*/
nRagDollManager* nRagDollManager::Instance()
{
    return Singleton;
}

//-----------------------------------------------------------------------------
/**
    Destroys the data content

    history:
        - 07-Nov-2005   David Reyes    created
*/
void nRagDollManager::Destroy()
{
    this->Clear();

    for (int index(0); index < this->containerRegisters.Size(); ++index)
    {
        ragdollReg* reg(this->containerRegisters.GetElementAt(index));

        n_delete_array(reg->ragdolls);
        n_delete_array(reg->freeSlots);
        n_delete(reg);
    }

    this->containerRegisters.Clear();

    this->init = false;
}

//-----------------------------------------------------------------------------
/**
    Checks if the class it's already registered

    @param className class id

    @return true/false

    history:
        - 08-Nov-2005   David Reyes    created
*/
const bool nRagDollManager::CheckRegisteredAlready(const nString& className)
{
    return this->containerRegisters.HasKey(nTag(className).KeyMap());
}

//-----------------------------------------------------------------------------
/**
    Returns an id for a given class

    @param className class name

    @return class id

    history:
        - 08-Nov-2005   David Reyes    created
*/
const int nRagDollManager::RegisteredClassId(const nString& className) const
{
    return nTag(className).KeyMap();
}

//-----------------------------------------------------------------------------
/**
    Initializes the registered classes

    @param className class name

    @return class id

    history:
        - 08-Nov-2005   David Reyes    created
*/
void nRagDollManager::InitRegisters()
{
    if (this->init)
    {
        return;
    }

    // @note: remember to add new entity with ncRagDoll component.
    nEntityClassServer::Instance()->LoadEntitySubClasses(nEntityClassServer::Instance()->GetEntityClass("necharacter"));
    nEntityClassServer::Instance()->LoadEntitySubClasses(nEntityClassServer::Instance()->GetEntityClass("neagententity"));
    nEntityClassServer::Instance()->LoadEntitySubClasses(nEntityClassServer::Instance()->GetEntityClass("neplayer"));

    for (int index(0); index < this->containerRegisters.Size(); ++index)
    {
        ragdollReg* reg(this->containerRegisters.GetElementAt(index));

        n_assert2(reg, "Data corruption.");

        int inner;

        for ( inner = 0; inner < reg->NumInstancesAllowed; ++inner)
        {
            reg->ragdolls[ inner ].rdoll = 
                nEntityObjectServer::Instance()->NewLocalEntityObject(reg->ragdolls[ inner ].className.Get())
                    ->GetComponent<ncRagDoll>();
        }

        n_assert2(reg->ragdolls[ inner ].rdoll, "Failed to create the rag-doll entity.");
    }

    this->init = true;
}

//-----------------------------------------------------------------------------
/**
    Clears the ragdolls info

    history:
        - 09-Nov-2005   David Reyes    created
*/
void nRagDollManager::Clear()
{
    for (int index(0); index < this->containerRegisters.Size(); ++index)
    {
        ragdollReg* reg(this->containerRegisters.GetElementAt(index));


        for (int inner(0); inner < reg->NumInstancesAllowed; ++inner)
        {
            if (reg->ragdolls[ inner ].rdoll)
            {
                nEntityObjectServer::Instance()->RemoveEntityObject(reg->ragdolls[ inner ].rdoll->GetEntityObject());
            }
            reg->ragdolls[ inner ].rdoll = 0;
            reg->ragdolls[ inner ].counter = 0;
        }

        reg->NumInstances = 0;
    }

    this->init = false;
}

//-----------------------------------------------------------------------------

NSCRIPT_INITCMDS_BEGIN(nRagDollManager)
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
