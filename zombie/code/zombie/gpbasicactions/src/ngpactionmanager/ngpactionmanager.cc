#include "precompiled/pchgpbasicactions.h"
//------------------------------------------------------------------------------
//  naiactionmanager.cc
//------------------------------------------------------------------------------

#include "ngpactionmanager/ngpactionmanager.h"
#include "entity/nentity.h"
#ifndef __ZOMBIE_EXPORTER__
#include "rnsgameplay/ncgameplay.h"
#include "ncfsm/ncfsm.h"
#endif
#include "ngpbasicaction/ngpbasicaction.h"
#include "ntrigger/ngameevent.h"

#include "entity/nentityobjectserver.h"
#include "util/nstream.h"

nNebulaClass(nGPActionManager, "nobject");

//------------------------------------------------------------------------------
nGPActionManager* nGPActionManager::Singleton = 0;

nMapTableTypes<int>::NString nGPActionManager::actionTypes;


//------------------------------------------------------------------------------
/**
    Constructor
*/
nGPActionManager::nGPActionManager()
{
    n_assert(0==Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPActionManager::~nGPActionManager()
{
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
	SetAction
*/
bool
nGPActionManager::SetAction (const char* action, int numarg, nArg* arglist, bool foreground, bool clearQueue)
{
    return this->SetActionArgs (action, numarg, arglist, foreground, clearQueue);
}


//------------------------------------------------------------------------------
/**
	SetActionArgs
*/
/**
    nArg args[4];
    args[0].SetFloat(12.2);
    args[1].SetBool(true);
*/
bool 
nGPActionManager::SetActionArgs (const char* action, int numargs, nArg* arglist, bool foreground, bool clearQueue)
{
    bool valid = false;

    // create action
    nObject* actionObject = nKernelServer::Instance()->New(action);
    n_assert(actionObject);

    if ( actionObject )
    {
#ifndef __ZOMBIE_EXPORTER__
        // get entity for basic action and gameplay 
        nEntityObject* entity = (nEntityObject*) arglist[0].GetO();
        n_assert(entity);

        ncGameplay * gameplay = 0;
        if( entity )
        {
            // tell the action which entity is executing it
            // most native actions already do this step in its init method, but this
            // is needed to not impose this step to designers who implement scripted
            // actions
            static_cast<nGPBasicAction*>(actionObject)->SetOwnerEntity( entity );

            gameplay = entity->GetComponentSafe<ncGameplay>();
        }

        valid = ( 0 != gameplay ) && ( 0 != entity );

        if( valid )
        {
            // get flags for basic action
            UINT flags = 0;

            flags |= foreground ? ncGameplay::BA_FOREGROUND : ncGameplay::BA_BACKGROUND;

            if( clearQueue )
            {
                flags |= ncGameplay::BA_CLEARQUEUE;
            }

            // clear the old basic action
            gameplay->AbortCurrentAction (flags);

            // Init the basic action with arguments 
            nArg outParam;
            actionObject->CallArgs("init", 1, &outParam, numargs, arglist);

            valid = outParam.GetB(); 

            if ( valid )
            {
                gameplay->SetBasicAction ((nGPBasicAction*)actionObject, flags);
            }
        }

        if ( !valid )
        {
            ncFSM* fsm = entity->GetComponent<ncFSM>();
            if (fsm)
            {
                fsm->OnActionEvent( nGameEvent::ACTION_FAIL );
            }
            actionObject->Release();
        }
#endif // !__ZOMBIE_EXPORTER__
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
	QueueAction
*/
void
nGPActionManager::QueueAction (const char* action, int numArg, nArg* arglist, bool foreground)
{
    n_assert(arglist);

    #ifndef __ZOMBIE_EXPORTER__
    nEntityObject* entity = (nEntityObject*)arglist[0].GetO();
    n_assert(entity);

    if ( entity )
    {
        ncGameplay* gameplay = entity->GetComponent<ncGameplay>();
        n_assert(gameplay);

        if ( gameplay )
        {
            gameplay->AddBasicAction (action, numArg, arglist, foreground);
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    @param action basic action to check the flags
    @param flags flags to check
    @returns true if action has flags
*/
bool
nGPActionManager::CheckExecuteFlags( const char * action , const int flags )const
{
    int * type = nGPActionManager::actionTypes[ action ];
    if( type )
    {
        return ( ( *type & flags ) != 0 );
    }

    return false;
}

//------------------------------------------------------------------------------
/**
	SetBasicActionType
*/
void
nGPActionManager::AddBasicActionType( const char * action, int new_type )
{
    int * type = nGPActionManager::actionTypes[ action ];
    if( type )
    {
        *type = (*type) | new_type;
    }
    else
    {
        nGPActionManager::actionTypes.Add( action, &new_type );
    }
}

//------------------------------------------------------------------------------
