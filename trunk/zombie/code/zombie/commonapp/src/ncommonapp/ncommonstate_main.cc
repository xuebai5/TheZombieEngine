#include "precompiled/pchncommonapp.h"
//------------------------------------------------------------------------------
//  ncommonstate_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "ncommonapp/ncommonstate.h"

#include "zombieentity/nloaderserver.h"

#include "napplication/napplication.h"

#include "nspecialfx/nfxserver.h"

#ifndef __ZOMBIE_EXPORTER__
#include "gui/nguiserver.h"
#endif
#include "kernel/nlogclass.h"
#include "kernel/ntimeserver.h"

nNebulaScriptClass(nCommonState, "nappstate");

static const char * guiScriptsPath = "home:data/scripts/";

NCREATELOGLEVEL ( userLog, "User", true, 1 )

//------------------------------------------------------------------------------
/**
*/
nCommonState::nCommonState():
    onEnterFunctionProto(0),
    onLeaveFunctionProto(0),
    onFrameFunctionProto(0),
    onUpdateLogFunctionProto(0),
    oslUpdateInterval( 1.0f ),
    oslLastUpdateInterval( 0.0f ),
    loadEnabled(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCommonState::~nCommonState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonState::OnCreate(nApplication* application)
{
    nAppState::OnCreate(application);

    // Set scripts and function names
    onEnterFunction = "guionenter";
    onLeaveFunction = "guionleave";
    onFrameFunction = "guionframe";
    onUpdateLogFunction = "onupdatelog";
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonState::OnStateEnter(const nString& prevState)
{
    if ( !this->onEnterFunctionProto )
    {
        this->onEnterFunctionProto = this->GetClass()->FindCmdByName( this->onEnterFunction.Get() );
    }
    if ( this->onEnterFunctionProto )
    {
        nCmd * cmd = this->onEnterFunctionProto->NewCmd();
        if( cmd )
        {
            cmd->Rewind();
            cmd->GetProto()->Dispatch( (void*)this, cmd );
            this->onEnterFunctionProto->RelCmd( cmd );
        }
    }

    nAppState::OnStateEnter( prevState );
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonState::OnStateLeave(const nString& nextState)
{
    if ( ! this->onLeaveFunctionProto )
    {
        this->onLeaveFunctionProto = this->GetClass()->FindCmdByName( this->onLeaveFunction.Get() );
    }

    if ( this->onLeaveFunctionProto )
    {
        nCmd * cmd = onLeaveFunctionProto->NewCmd();
        if( cmd )
        {
            cmd->Rewind();
            cmd->GetProto()->Dispatch( (void*)this, cmd );
            onLeaveFunctionProto->RelCmd( cmd );
        }
    }

    nAppState::OnStateLeave( nextState );
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonState::OnFrame()
{
    if ( this->GetClass()->FindCmdByName( this->onFrameFunction.Get() ) )
    {
        this->Call( this->onFrameFunction.Get(), 0, 0 );
    }

    this->oslLastUpdateInterval = nTimeServer::Instance()->GetTime();

    if (this->loadEnabled)
    {
        nLoaderServer::Instance()->Trigger();
    }

    // update special effects after gameplay (to keep synchronized with attached entities)
    nFXServer::Instance()->Trigger();
}

//------------------------------------------------------------------------------
/**
    @brief Set min time interval to make the gui update in calls to UpdateOnScreenLog()
*/
void
nCommonState::SetOnScreenLogUpdateInterval(float updateTime)
{
    this->oslUpdateInterval = updateTime;
}

//------------------------------------------------------------------------------
/**
    @brief Respond to a call from a potentially long process, to repaint gui and
    send a message to a scripted method of this object.
    @param message A message describing the task
*/
void
nCommonState::UpdateOnScreenLog(const char * message )
{
    // Call to this:OnUpdateLog script method
    if ( ! this->onUpdateLogFunctionProto )
    {
        this->onUpdateLogFunctionProto = this->GetClass()->FindCmdByName( this->onUpdateLogFunction.Get() );
    }
    if ( this->onUpdateLogFunctionProto )
    {
        nCmd * cmd = this->onUpdateLogFunctionProto->NewCmd();
        if (cmd)
        {
            cmd->Rewind();
            // todo put parameters: 1 string
            cmd->GetProto()->Dispatch( (void*)this, cmd );
            cmd->In()->SetS( message );
            this->onUpdateLogFunctionProto->Dispatch((void*)this, cmd);
            this->onUpdateLogFunctionProto->RelCmd( cmd );
        }
    }

    nTime t = nTimeServer::Instance()->GetTime();

    static bool inGUIRender = false;
    if ( !inGUIRender && t - this->oslLastUpdateInterval > this->oslUpdateInterval )
    {
#ifndef __ZOMBIE_EXPORTER__
        inGUIRender = true;

        // repaint GUI
        nGfxServer2 * gfxServer = nGfxServer2::Instance();
        n_assert( gfxServer );

        if( gfxServer->IsDisplayOpen() ) // Test especial case when has log and the display is not open
        {

            bool inBeginFrame = gfxServer->InBeginFrame();
            if (!inBeginFrame)
            {
                gfxServer->BeginFrame();
            }
            if (gfxServer->InBeginFrame())
            {
                bool inBeginScene = gfxServer->InBeginScene();
                if (!inBeginScene)
                {
                    gfxServer->BeginScene();
                }
                if (gfxServer->InBeginScene())
                {
                    nGuiServer::Instance()->Render();
                    gfxServer->EndScene();
                }                

                gfxServer->EndFrame();

                // flip frame buffer
                gfxServer->PresentScene();

                if (inBeginFrame)
                {
                    gfxServer->BeginFrame();
                }
                if (inBeginScene)
                {
                    gfxServer->BeginScene();
                }
            }
        }

        inGUIRender = false;
#endif

        this->oslLastUpdateInterval = t;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonState::TestLog(const char * message)
{
    NLOG( userLog, (NLOGUSER | 0, message) );
}
