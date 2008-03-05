//------------------------------------------------------------------------------
//  ncgameplay_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgameplay.h"
#include "ngpbasicaction/ngpbasicaction.h"
#include "ncfsm/ncfsm.h"
#include "ncaimovengine/ncaimovengine.h"
#include "entity/nentityobjectserver.h"
#include "ntrigger/ngameevent.h"
#include "nworldinterface/nworldinterface.h"
#include "ngpactionmanager/ngpactionmanager.h"

#include "ncnetwork/ncnetwork.h"
#include "nnetworkmanager/nnetworkmanager.h"

#include "nclogicanimator/nclogicanimator.h"

#ifndef NGAME
#include "ndebug/ndebugserver.h"
#include "gfx2/ngfxserver2.h"
#endif//NGAME

#ifdef __NEBULA_STATS__
#include "kernel/nprofiler.h"

namespace
{
    nProfiler profGPrun;
}
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplay,nComponentObject);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplay::ncGameplay() : 
    foregroundAction(0),
	backgroundAction(0),
    movEngine(0),
    valid(true)
{   
#ifdef __NEBULA_STATS__
    if ( !profGPrun.IsValid() )
    {
        profGPrun.Initialize( "profGPrun", true );
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplay::~ncGameplay()
{
    this->movEngine = 0;
    this->DestroyBasicAction (BA_BACKGROUND | BA_FOREGROUND);
	this->ClearQueue();
}

//------------------------------------------------------------------------------
/**
    DestroyBasicAction
*/
void
ncGameplay::DestroyBasicAction (UINT flags)
{
	bool foreground = (flags & BA_FOREGROUND) != 0;
	bool background = (flags & BA_BACKGROUND) != 0;

    if ( foreground && this->foregroundAction )
    {
        this->foregroundAction->Release();
		this->foregroundAction = 0;
    }

	if ( background && this->backgroundAction )
	{
		this->backgroundAction->Release();
		this->backgroundAction = 0;
	}
}

//------------------------------------------------------------------------------
/**
    SetBasicAction
*/
void
ncGameplay::SetBasicAction (nGPBasicAction* basicAction, UINT flags)
{
	bool clearQueue = (flags & BA_CLEARQUEUE) != 0;
	bool foreground = (flags & BA_FOREGROUND) != 0;

    this->DestroyBasicAction (flags);

	if ( foreground )
	{
		this->foregroundAction = basicAction;

        if ( clearQueue )
        {
            this->ClearQueue();
        }
	}
	else
	{
		this->backgroundAction = basicAction;
	}
};


//------------------------------------------------------------------------------
/**
    InitInstance
*/
void
ncGameplay::InitInstance (nObject::InitInstanceMsg initType )
{
    n_assert(this->entityObject);

    if ( this->entityObject )
    {
        this->movEngine = this->entityObject->GetComponent <ncAIMovEngine>();        
    }

    // Make sure to clear any previous basic action if this component is being reused
    if ( initType == nObject::ReloadedInstance )
    {
        this->DestroyBasicAction( BA_BACKGROUND | BA_FOREGROUND );
        this->ClearQueue();
    }

#ifndef NGAME
    // When the entity isn't being loaded, give it a default name with the format <class name>_<low ip>_<count>
    if ( initType == nObject::NewInstance )
    {
        this->name = this->GetEntityObject()->GetEntityClass()->GetName();
        this->name += "_";
        this->name += nString( int( nEntityObjectServer::Instance()->GetHighId() >> 24 ) );
        this->name += "_";

        // Look for the first free count index available for the same class name and low ip
        const int MaxCount = 10000;
        int count = 0;
        while ( count < MaxCount ) // Put some boundary, just in case something goes wrong and it hangs
        {
            nString desiredName( name + nString(count) );
            if ( !nWorldInterface::Instance()->GetGameEntity( desiredName.Get() ) )
            {
                this->name = desiredName;
                break;
            }
            ++count;
        }
        n_assert2( count < MaxCount, "An unknown error occurred while assigning a new default name to the entity" );
    }
#endif
}


//------------------------------------------------------------------------------
/**
    Run
    @param deltaTime time from last update
*/
void
ncGameplay::Run ( const float deltaTime )
{
    // Run movement
    if ( this->movEngine )
    {
        this->movEngine->Run(deltaTime);
    }

#ifdef __NEBULA_STATS__
    profGPrun.StartAccum();
#endif
    // Run current basic action
    if ( this->foregroundAction )
    {
        nArg outArg;
        this->foregroundAction->Call ("run", 1, &outArg);
        if ( outArg.GetB() )
        {
            this->DestroyBasicAction (BA_FOREGROUND);
            this->SignalActionDone();

#ifdef __NEBULA_STATS__
            profGPrun.StopAccum();
#endif
            ncNetwork * netcomp = this->GetComponent<ncNetwork>();
            if( netcomp )
            {
                netcomp->UpdateState();
            }
#ifdef __NEBULA_STATS__
            profGPrun.StartAccum();
#endif
        }
    }

    // Run the background action
	if ( this->backgroundAction )
	{
		nArg outArg;
		this->backgroundAction->Call ("run", 1, &outArg);
		if ( outArg.GetB() )
		{
			this->DestroyBasicAction (BA_BACKGROUND);
			this->SignalActionDone();
		}
	}

    // By now, this only works for foreground basic actions
    if ( !this->foregroundAction )
    {
        this->GetQueuedAction();
    }
#ifdef __NEBULA_STATS__
    profGPrun.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    AbortCurrentAction
*/
void
ncGameplay::AbortCurrentAction (UINT flags)
{
    this->DestroyBasicAction (flags);
}

//------------------------------------------------------------------------------
/**
    SignalActionDone
*/
void
ncGameplay::SignalActionDone()
{
    nNetworkManager * network = nNetworkManager::Instance();

    if( network && network->IsServer() )
    {
        // @todo When the single action is updated to an actions queue,
        //       do NOT signal an action done event for each action,
        //       but only when the actions queue is empty.
        ncFSM* fsm = this->GetComponent<ncFSM>();
        if ( fsm )
        {
            fsm->OnActionEvent( nGameEvent::ACTION_DONE );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param actionName name of the action to check
    @returns true if the action is in the entity
*/
bool
ncGameplay::HasAction (const nString& actionName, UINT flags) const
{
    nGPBasicAction* basicAction = 0;
	bool foreground = (flags & BA_FOREGROUND) != 0;

    basicAction = foreground ? this->foregroundAction : this->backgroundAction;

    if ( basicAction && basicAction->GetClass() )
    {
        if( basicAction->GetClass()->GetName() == actionName )
        {
            return true;
        }
    }

    return false;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    DebugDraw
*/
void 
ncGameplay::DebugDraw (nGfxServer2* const gfxServer)
{
    bool debugTexts = nDebugServer::Instance()->GetFlagEnabled( "rnsview", "debugtexts" );
    if( debugTexts )
    {
        nString text;
        if( this->foregroundAction )
        {
            text = "Foreground Action : ";
            text.Append( this->foregroundAction->GetClass()->GetName() );
            gfxServer->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), -0.9f, 0.3f );
        }
        if( this->backgroundAction )
        {
            text = "Background Action : ";
            text.Append( this->backgroundAction->GetClass()->GetName() );
            nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), -0.9f, 0.4f );
        }
        ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
        if( animator )
        {
            text = "1 Animation : ";
            int index = animator->GetFirstPersonStateIndex();
            if( index != -1 )
            {
                text.Append( animator->GetFirstPersonStateName( index ) );
                text.Append( " [ " );
                text.AppendFloat( animator->GetRemainingTime( index, true ) );
                text.Append( " / " );
                text.AppendFloat( animator->GetStateDuration( index, true ) );
                text.Append( " ] " );
            }
            else
            {
                text.Append( "???" );
            }
            nGfxServer2::Instance()->Text( text.Get(), vector4( 0.8f, 1, 0.8f, 0.8f), -0.9f, 0.5f );

            text = "3 animation (lower): ";
            index = animator->GetStateIndex();
            if( index != -1 )
            {
                text.Append( animator->GetStateName( index ) );
                text.Append( " [ " );
                text.AppendFloat( animator->GetRemainingTime( index, false ) );
                text.Append( " / " );
                text.AppendFloat( animator->GetStateDuration( index, false ) );
                text.Append( " ] " );
            }
            else
            {
                text.Append( "???" );
            }
			nGfxServer2::Instance()->Text( text.Get(), vector4( 0.8f, 1, 0.8f, 0.8f), -0.9f, 0.55f );

			text = "3 animation (upper): ";
            index = animator->GetUpperStateIndex();
            if( index != -1 )
            {
                text.Append( animator->GetStateName( index ) );
                text.Append( " [ " );
                text.AppendFloat( animator->GetRemainingTime( index, false ) );
                text.Append( " / " );
                text.AppendFloat( animator->GetStateDuration( index, false ) );
                text.Append( " ] " );
            }
            else
            {
                text.Append( "???" );
            }
            nGfxServer2::Instance()->Text( text.Get(), vector4( 0.8f, 1, 0.8f, 0.8f), -0.9f, 0.60f );
        }
    }
}
#endif//!NGAME

//------------------------------------------------------------------------------
/**
	ClearQueue
*/
void
ncGameplay::ClearQueue()
{
	while ( this->queueActions.Size() > 0 )
	{
		this->PopBasicAction();
	}
}

//------------------------------------------------------------------------------
/**
	AddBasicAction
*/
void
ncGameplay::AddBasicAction (const char* name, int numArgs, nArg* args, bool foreground)
{
	actionsSpec* spec = n_new (actionsSpec);

	if ( spec )
	{
		spec->name = n_new_array (char, strlen(name)+1);
		strcpy (spec->name, name);
		spec->args = n_new_array (nArg, numArgs);        
		
        for ( int i=0; i<numArgs; i++ )
        {
            // Using the copy constructor
            spec->args[i] = args[i];
        }

		spec->params = numArgs;
		spec->foreground = foreground;

		this->queueActions.PushBack (spec);
	}
}

//------------------------------------------------------------------------------
/**
	PopBasicAction
*/
void
ncGameplay::PopBasicAction()
{
	if ( this->queueActions.Size() > 0 )
	{
		actionsSpec* spec = this->queueActions[0];
		n_assert(spec);

		n_delete_array (spec->name);
		n_delete_array (spec->args);
		n_delete (spec);

        this->queueActions.Erase(0);
	}
}


//------------------------------------------------------------------------------
/**
	GetTopQueue
*/
ncGameplay::actionsSpec*
ncGameplay::GetTopQueue() const
{
	actionsSpec* spec = 0;

	if ( this->queueActions.Size() > 0 )
	{
		spec = this->queueActions[0];
	}

	return spec;
}

//------------------------------------------------------------------------------
/**
	GetQueuedAction
*/
void
ncGameplay::GetQueuedAction()
{
	if ( this->queueActions.Size() > 0 )
	{
		nGPActionManager* actionManager = nGPActionManager::Instance();
		n_assert(actionManager);

		if ( actionManager )
		{
			actionsSpec* action = this->GetTopQueue();
			n_assert(action);

			actionManager->SetAction (action->name, action->params, action->args, action->foreground, false);
			this->PopBasicAction();
		}
	}
}

//------------------------------------------------------------------------------
