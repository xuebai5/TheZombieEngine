//-----------------------------------------------------------------------------
//  ncagenttrigger_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ntrigger/ncagenttrigger.h"
#include "ncgpperception/ncgpsight.h"
#include "ncgpperception/ncgphearing.h"
#include "ncgpperception/ncgpfeeling.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncAgentTrigger,ncTrigger);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAgentTrigger)
NSCRIPT_INITCMDS_END()

NCREATELOGLEVEL( perception, "Perception System", false, 1 );

//-----------------------------------------------------------------------------
#ifndef NGAME
bool ncAgentTrigger::sightEnabled( true );
bool ncAgentTrigger::hearingEnabled( true );
bool ncAgentTrigger::feelingEnabled( true );
#endif

#ifdef __NEBULA_STATS__
namespace
{
    nProfiler profPerception;
    nProfiler profSight;
    nProfiler profHearing;
    nProfiler profFeeling;
}
#endif

//-----------------------------------------------------------------------------
/**
    Default contructor
*/
ncAgentTrigger::ncAgentTrigger()
{
#ifdef __NEBULA_STATS__
    if ( !profPerception.IsValid() )
    {
        profPerception.Initialize( "profAIPerception", true );
        profSight.Initialize( "profAIPerceptionSight", true );
        profHearing.Initialize( "profAIPerceptionHearing", true );
        profFeeling.Initialize( "profAIPerceptionFeeling", true );
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Initializes the instance
*/
void 
ncAgentTrigger::InitInstance(nObject::InitInstanceMsg initType)
{
    ncTrigger::InitInstance( initType );
}

//-----------------------------------------------------------------------------
/**
    UpdateCullingRadius

    @brief Update the culling radius ti the highest perception range
*/
void
ncAgentTrigger::UpdateCullingRadius()
{
    float sightRadius = this->GetComponentSafe<ncGPSight>()->GetSightRadius();
    float feelRadius = this->GetComponentSafe<ncGPFeeling>()->GetFeelingRadius();
    float hearRadius = this->GetComponentSafe<ncGPHearing>()->GetHearingRadius();

    float maxRadius = n_max( n_max(sightRadius, feelRadius), hearRadius );

    this->SetCullingRadius( maxRadius );
}

//-----------------------------------------------------------------------------
/**
    Answers to an event, culling by perception field first

    Delegate the event handling to the senses that may perceive it. Finish and
    return true as soon as some of the senses had generated an event towards
    the FSM. Oherwise, false is returned.
*/
bool
ncAgentTrigger::HandleEvent( nGameEvent* event )
{
#ifdef __NEBULA_STATS__
    profPerception.StartAccum();
#endif

    n_assert( event );

    // Give the event only to those senses that may perceive it
    // Handle the event from less to more expensive senses
    bool eventHandled( false );
    nGameEvent::Info::Sensation sensation = nGameEvent::GetEventSensation( event->GetType() );

    // Feeling
#ifdef __NEBULA_STATS__
    profFeeling.StartAccum();
#endif
    if ( sensation & nGameEvent::Info::TOUCHABLE )
    {
        ncGPFeeling* feeling = this->GetComponentSafe<ncGPFeeling>();
#ifndef NGAME
        if ( feeling && this->feelingEnabled )
#endif
        {
            if ( feeling->FeelEvent( event ) )
            {
                eventHandled = true;
                goto exit;
            }
        }
    }
#ifdef __NEBULA_STATS__
    profFeeling.StopAccum();
#endif

    // Hearing
#ifdef __NEBULA_STATS__
    profHearing.StartAccum();
#endif
    if ( sensation & nGameEvent::Info::AUDIBLE )
    {
        ncGPHearing* hearing = this->GetComponentSafe<ncGPHearing>();
#ifndef NGAME
        if ( hearing && this->hearingEnabled )
#endif
        {
            if ( hearing->HearEvent( event ) )
            {
                eventHandled = true;
                goto exit;
            }
        }
    }
#ifdef __NEBULA_STATS__
    profHearing.StopAccum();
#endif

    // Sight
#ifdef __NEBULA_STATS__
    profSight.StartAccum();
#endif
    if ( sensation & nGameEvent::Info::VISIBLE )
    {
        ncGPSight* sight = this->GetComponentSafe<ncGPSight>();
#ifndef NGAME
        if ( sight && this->sightEnabled )
#endif
        {
            if ( sight->SeeEvent( event ) )
            {
                eventHandled = true;
                goto exit;
            }
        }
    }
#ifdef __NEBULA_STATS__
    profSight.StopAccum();
#endif

exit:
    // Stop any active perception profiler
#ifdef __NEBULA_STATS__
    if ( profFeeling.IsStarted() )
    {
        profFeeling.StopAccum();
    }
    if ( profHearing.IsStarted() )
    {
        profHearing.StopAccum();
    }
    if ( profSight.IsStarted() )
    {
        profSight.StopAccum();
    }
    profPerception.StopAccum();
#endif

    // Tell to the trigger system if the event has been handled
    return eventHandled;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Enable/disable sight on agent entities
*/
void ncAgentTrigger::SetSightEnabled( bool enable )
{
    ncAgentTrigger::sightEnabled = enable;
}

//-----------------------------------------------------------------------------
/**
    Enable/disable hearing on agent entities
*/
void ncAgentTrigger::SetHearingEnabled( bool enable )
{
    ncAgentTrigger::hearingEnabled = enable;
}

//-----------------------------------------------------------------------------
/**
    Enable/disable feeling on agent entities
*/
void ncAgentTrigger::SetFeelingEnabled( bool enable )
{
    ncAgentTrigger::feelingEnabled = enable;
}
#endif

#endif