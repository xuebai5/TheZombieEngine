#include "precompiled/pchrnsstates.h"
//------------------------------------------------------------------------------
//  rnsvehiclesstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "rnsstates/rnsvehiclesstate.h"

#include "napplication/napplication.h"

#include "nscene/nscenegraph.h"
#include "nscene/nsceneserver.h"
#include "nscene/ntransformnode.h"

#include "misc/nconserver.h"

#include "input/ninputserver.h"

nNebulaClass( RnsVehiclesState, "nappstate" );
//------------------------------------------------------------------------------
/**
*/
RnsVehiclesState::RnsVehiclesState():
    renderContext(0),
    lightContext(0),
    viewContext(0),
    viewerPos( 0,0,0 )
{
}

//------------------------------------------------------------------------------
/**
*/
RnsVehiclesState::~RnsVehiclesState()
{
}

//------------------------------------------------------------------------------
/**
    @param application application that create the state
*/
void
RnsVehiclesState::OnCreate( nApplication * application )
{
    nAppState::OnCreate( application );

    this->refRootNode = (nTransformNode*) nKernelServer::ks->Lookup( "/usr/scene/objects" );
    this->refSceneGraph = (nSceneGraph *) nSceneServer::Instance()->NewSceneGraph(0);
}

//------------------------------------------------------------------------------
/**
    @param prevState previous state
*/
void
RnsVehiclesState::OnStateEnter( const nString & /*prevState*/ )
{
    nSceneServer::Instance()->AddGraph(this->refSceneGraph.get());
}

//------------------------------------------------------------------------------
/**
    @param nextState next state
*/
void
RnsVehiclesState::OnStateLeave( const nString & /*nextState*/ )
{
    nSceneServer::Instance()->RemoveGraph(this->refSceneGraph.get());
}

//------------------------------------------------------------------------------
/**
*/
void
RnsVehiclesState::OnFrame()
{
    this->HandleInput(static_cast<float>( this->app->GetFrameTime() ));
}

//------------------------------------------------------------------------------
/**
*/
void
RnsVehiclesState::OnRender3D()
{
    this->viewContext->GetComponent<ncScene>()->Render(this->refSceneGraph.get());
    this->lightContext->GetComponent<ncScene>()->Render(this->refSceneGraph.get());
    this->renderContext->GetComponent<ncScene>()->Render(this->refSceneGraph.get());
}

//------------------------------------------------------------------------------
/**
*/
void
RnsVehiclesState::OnRender2D()
{
}

//------------------------------------------------------------------------------
/**
    @param frameTime time from last call
*/
void
RnsVehiclesState::HandleInput(const float frameTime)
{
    nInputServer* inputServer = nInputServer::Instance();

    if( inputServer->GetButton("exit") )
    {
        this->app->SetQuitRequested(true);
    }

    // toggle console
    if( inputServer->GetButton("console") )
    {
        nConServer::Instance()->Toggle();
    }

    float lookHori = 0.0f;
    float lookVert = 0.0f;

    lookHori = inputServer->GetSlider("left") - inputServer->GetSlider("right");
    lookVert = inputServer->GetSlider("down") - inputServer->GetSlider("up");

    const float lookVelocity = 0.25f;
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    matrix44 viewMatrix;
    viewMatrix.ident();
    viewMatrix.rotate_x( this->viewerAngles.theta );
    viewMatrix.rotate_y( this->viewerAngles.rho );

    vector3 dirMove = viewMatrix * vector3( 0, 0, 1 );
    vector3 dirStep = viewMatrix * vector3( 1, 0 ,0 );

    if( inputServer->GetButton("Forward") )
    {
        this->viewerPos -= dirMove*frameTime*10.0f;
    }

    if( inputServer->GetButton("Backward") )
    {
        this->viewerPos += dirMove*frameTime*10.0f;
    }

    if( inputServer->GetButton("StrafeLeft") )
    {
        this->viewerPos -= dirStep*frameTime*10.0f;
    }

    if( inputServer->GetButton("StrafeRight") )
    {
        this->viewerPos += dirStep*frameTime*10.0f;
    }

    viewMatrix.translate( this->viewerPos );

    //this->viewContext->SetTransform( viewMatrix );
    this->viewContext->GetComponent<ncTransform>()->SetPolar(this->viewerAngles);
    this->viewContext->GetComponent<ncTransform>()->SetPosition(this->viewerPos);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
