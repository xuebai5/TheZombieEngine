#ifndef N_GPACTIONMANAGER_H
#define N_GPACTIONMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nGPActionManager
    @ingroup NebulaGameplayBasicActions

    @brief class that manages the creation of the basic AI actions for entities
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "mathlib/vector.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
/**
    @def NNETWORKTYPE_ONLYCLIENT()
    @brief Sets the basic action to execute only in the client without send to the server
*/
#define NNETWORKTYPE_ONLYCLIENT()   \
    nGPActionManager::AddBasicActionType( cl->GetName(), nGPActionManager::ISONLY_CLIENT );

//------------------------------------------------------------------------------
/**
    @def NNETWORKTYPE_ONLYSERVER()
    @brief Sets the basic action to execute only in the server
*/
#define NNETWORKTYPE_ONLYSERVER()   \
    nGPActionManager::AddBasicActionType( cl->GetName(), nGPActionManager::ISONLY_SERVER );

//------------------------------------------------------------------------------
/**
    @def NNETWORKTYPE_INCLIENTS()
    @brief Sets the basic action to execute only in the clients from the server
*/
#define NNETWORKTYPE_INCLIENTS()   \
    nGPActionManager::AddBasicActionType( cl->GetName(), nGPActionManager::IS_IN_CLIENTS );

//------------------------------------------------------------------------------
/**
    @def NNETWORKTYPE_AHEAD()
    @brief Sets the basic action to execute ahead in the client
*/
#define NNETWORKTYPE_AHEAD()   \
    nGPActionManager::AddBasicActionType( cl->GetName(), nGPActionManager::IS_AHEAD );


//------------------------------------------------------------------------------
/**
    @def NNETWORKTYPE_UNRELIABLE()
    @brief Sets the basic action to be sended unreliable 
*/
#define NNETWORKTYPE_UNRELIABLE()   \
    nGPActionManager::AddBasicActionType( cl->GetName(), nGPActionManager::IS_UNRELIABLE );

//------------------------------------------------------------------------------
class nstream;
class nEntityObject;

//------------------------------------------------------------------------------
class nGPActionManager : public nObject
{
public:
    enum BasicActionType
    {
        // ISONLY_CLIENT + ISONLY_SERVER combo depends where is first executed the basic action
        ISONLY_CLIENT = (1<<0),
        ISONLY_SERVER = (1<<1),
        IS_IN_CLIENTS = (1<<2),
        IS_AHEAD      = (1<<3),
        IS_UNRELIABLE = (1<<4),
    };

    /// Constructor
    nGPActionManager();

    /// Destructor
    ~nGPActionManager();

    /// Sets an AI action for a entity 
    //void SetAction (const char* action, ...);
    bool SetAction (const char* action, int numarg, nArg* arglist, bool foreground, bool clearQueue = true);
	void QueueAction (const char* action, int numarg, nArg* arglist, bool foreground);

    /// Returns the instance
    static nGPActionManager* Instance();

    /// say if the basic action only execute on server
    bool CheckExecuteFlags( const char * action , const int flags )const;
    /// set the basic action type
    static void AddBasicActionType( const char * action, int type );

private:

    bool SetActionArgs (const char* action, int numargs, nArg* arglist, bool foreground, bool clearQueue);

    static nGPActionManager* Singleton;

    static nMapTableTypes<int>::NString actionTypes;
};

//------------------------------------------------------------------------------
/**
    Instance
*/
inline
nGPActionManager*
nGPActionManager::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

#endif