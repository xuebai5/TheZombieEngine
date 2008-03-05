#ifndef N_RAGDOLLMANAGER_H
#define N_RAGDOLLMANAGER_H

#include "util/nkeyarray.h"
#include "kernel/nobject.h"

//-----------------------------------------------------------------------------
/**
    @class nRagDollManager
    @ingroup Scene
    @brief A rag doll manager.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Ragdoll Manager
    
    @cppclass nRagDollManager
    
    @superclass nRoot

    @classinfo A rag doll manager.
*/

//-----------------------------------------------------------------------------

class ncRagDoll;

//-----------------------------------------------------------------------------

class nRagDollManager : public nRoot
{
public:
    
    typedef enum {
        counting = 0, // always eliminates the older rag-doll
    } policy;

    // constructor
    nRagDollManager();

    // destructor
    ~nRagDollManager();

    // registers a manager for a given type of rag-doll
    const int Register(const nString& className, const int numInstancesAllowed, const policy = counting);

    // gets a ragdoll
    ncRagDoll* PopRagDoll(const int id);

    // not longer needed ragdoll marke it free
    void PushRagDoll(const int id, ncRagDoll* ragdoll);

    /// get instance pointer
    static nRagDollManager* Instance();

    /// returns an if for a given class
    const int RegisteredClassId(const nString& className) const;

private:
    /// destroys the data content
    void Destroy();

    /// container of a registered ragdoll
    struct ragdollReg {

        int NumInstances; // num instances in course
        
        int NumInstancesAllowed; // num instances allowed
        
        policy policyToBeApplied; // policy to be apply once we run out of ragdolls

        struct RagDolls {
            int counter;
            ncRagDoll* rdoll;
            nString className;
            RagDolls() : counter(-1),className(""), rdoll(0) {}
        } *ragdolls;

        int* freeSlots;

        ragdollReg() : ragdolls(0), freeSlots(0) {}
    };

    /// container for all the ragdolls
    nKeyArray< ragdollReg* > containerRegisters;

    /// applies counting policy
    void ApplyCountingPolicy(ragdollReg* reg);

    /// removes a ragdoll from the list
    void Remove(ragdollReg* reg, const int index);

    /// removes a ragdoll from the list
    void Remove(ragdollReg* reg, ncRagDoll* rinterface);

    /// stores the counter
    unsigned int counter;

    /// pointer to the unique instance
    static nRagDollManager* Singleton;

    /// checks if the class it's already registered
    const bool CheckRegisteredAlready(const nString& className);

    /// initializes the registered classes
    void InitRegisters();

    /// stores if init
    bool init;

    /// clears the ragdolls info
    void Clear();

};

#endif // RAGDOLLMANAGER_H