//-----------------------------------------------------------------------------
//  ncareatrigger_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ncareatrigger.h"
#include "ntrigger/nctriggershape.h"
#include "ntrigger/ntriggerserver.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncAreaTrigger,ncTrigger);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAreaTrigger)
    NSCRIPT_ADDCMD_COMPOBJECT('ESII', void, SetInternalTriggerById, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JRST', void, ResetTrigger, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSSS', void, SetCheckSameSpace, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGSS', bool, GetCheckSameSpace, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSSC', void, SetCheckSameCell, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGSC', bool, GetCheckSameCell, 0, (), 0, ());

    cl->BeginSignals(1);
    NCOMPONENT_ADDSIGNAL( OnEnterArea )
    NCOMPONENT_ADDSIGNAL( OnExitArea )
    cl->EndSignals();

NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
NSIGNAL_DEFINE( ncAreaTrigger, OnEnterArea );
NSIGNAL_DEFINE( ncAreaTrigger, OnExitArea );

//-----------------------------------------------------------------------------
#ifdef __NEBULA_STATS__
namespace
{
    nProfiler profAreaTriggers;
}
#endif

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncAreaTrigger::ncAreaTrigger():
    checkSameSpace( false ),
    checkSameCell( false )
{
#ifdef __NEBULA_STATS__
    if ( !profAreaTriggers.IsValid() )
    {
        profAreaTriggers.Initialize( "profAIAreaTriggers", true );
    }
#endif
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncAreaTrigger::~ncAreaTrigger()
{
    // Empty
}

//-----------------------------------------------------------------------------
void 
ncAreaTrigger::InitInstance(nObject::InitInstanceMsg initType)
{
    ncTrigger::InitInstance( initType );

    if (initType == nObject::ReloadedInstance)
    {
        this->ResetTrigger();
    }
}

//-----------------------------------------------------------------------------
/**
    Set the trigger used to handle internal events (kept for backwards compatibility)

    This method does nothing, the internal trigger is obsolote. Instead, the
    ncTriggerOutput component is used to handle internally generated events.
*/
void
ncAreaTrigger::SetInternalTriggerById( nEntityObjectId /*entityId*/ )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Answers to an event, first doing additional culling if necessary

    Checks if the event is inside or outside of the trigger shape and for each
    case checks if the event was already inside or not previously. For a new
    event being inside an 'on enter' event is generated and for an old event
    being outside an 'on exit' event is generated.
*/
bool
ncAreaTrigger::HandleEvent( nGameEvent* event )
{
#ifdef __NEBULA_STATS__
    profAreaTriggers.StartAccum();
#endif

    n_assert( event );

    // Check if the event is inside the shape (intersecting is considered inside)
    ncTriggerShape* shape = this->GetComponent<ncTriggerShape>();
    n_assert2( shape, "It's needed a trigger shape component to know the trigger activation area" );
    nEntityObject* emitterEntity = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
    n_assert( emitterEntity );
    bool inside( shape->IsInside( emitterEntity->GetComponentSafe<ncTransform>()->GetPosition() ) );

    if ( this->checkSameSpace )
    {
        ncSpatial * spatComp = emitterEntity->GetComponent<ncSpatial>();
        if ( spatComp && spatComp->GetSpace() != this->GetComponentSafe<ncSpatial>()->GetSpace() )
        {
            inside = false;
        }
        
        if ( this->checkSameCell )
        {
            if ( spatComp && spatComp->GetCell() != this->GetComponentSafe<ncSpatial>()->GetCell() )
            {
                inside = false;
            }
        }
    }

    // If it's inside and previously it wasn't, throw the 'enter' event
    bool eventHandled( false );
    if ( inside )
    {
        if ( this->entitiesAlreadyInside.FindIndex( event->GetEmitterEntity() ) == -1 )
        {
            this->entitiesAlreadyInside.Append( event->GetEmitterEntity() );
            NLOG( trigger, (nTriggerServer::GeneralLog | 1,
                "Event %d entering area trigger %d", event->GetId(), this->GetEntityObject()->GetId()) );
            this->SignalOnEnterArea( this->GetEntityObject(), event );
            this->OnInternalEvent();
            eventHandled = true;
        }
    }
    // If it isn't inside and previously it was, throw the 'exit' event
    else
    {
        int index = this->entitiesAlreadyInside.FindIndex( event->GetEmitterEntity() );
        if ( index != -1 )
        {
            this->entitiesAlreadyInside.Erase(index);
            n_assert2( this->entitiesAlreadyInside.FindIndex( event->GetEmitterEntity() ) == -1,
                "The same entity has been registered to be inside the trigger area twice" );
            NLOG( trigger, (nTriggerServer::GeneralLog | 1,
                "Event %d exiting area trigger %d", event->GetId(), this->GetEntityObject()->GetId()) );
            this->SignalOnExitArea( this->GetEntityObject(), event );
            this->OnInternalEvent();
            eventHandled = true;
        }
    }

#ifdef __NEBULA_STATS__
    profAreaTriggers.StopAccum();
#endif

    // Tell the trigger system if the event has been handled
    return eventHandled;
}

//-----------------------------------------------------------------------------
/**
    Lose track of any entity inside the trigger shape
*/
void
ncAreaTrigger::ResetTrigger()
{
    this->entitiesAlreadyInside.Clear();
}

//-----------------------------------------------------------------------------
/**
    Set same space flag
*/
void
ncAreaTrigger::SetCheckSameSpace(bool checkSameSpace)
{
    this->checkSameSpace = checkSameSpace;
}

//-----------------------------------------------------------------------------
/**
    Get same space flag
*/
bool
ncAreaTrigger::GetCheckSameSpace()
{
    return this->checkSameSpace;
}

//-----------------------------------------------------------------------------
/**
    Set same cell flag
*/
void
ncAreaTrigger::SetCheckSameCell(bool checkSameCell)
{
    this->checkSameCell = checkSameCell;
}

//-----------------------------------------------------------------------------
/**
    Get same cell flag
*/
bool
ncAreaTrigger::GetCheckSameCell()
{
    return this->checkSameCell;
}