#include "precompiled/pchrnsstates.h"
/**
   @file rnsphysicsstate_main.cc
   @author Luis Jose Cabellos Gomez
   @brief 
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "rnsstates/rnsphysicsstate.h"

#include "napplication/napplication.h"

#include "nscene/nscenegraph.h"
#include "nscene/nsceneserver.h"
#include "nscene/ntransformnode.h"

#include "misc/nconserver.h"

#include "input/ninputserver.h"

#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"

nNebulaClass( RnsPhysicsState, "nappstate" );
//------------------------------------------------------------------------------
/**
*/
RnsPhysicsState::RnsPhysicsState():
    renderContext(0),
    lightContext(0),
    viewContext(0),
    viewerPos( 0,0,0 )
{
}

//------------------------------------------------------------------------------
/**
*/
RnsPhysicsState::~RnsPhysicsState()
{
    this->shot->Release();
}

//------------------------------------------------------------------------------
/**
    @param application application that create the state
*/
void
RnsPhysicsState::OnCreate( nApplication * application )
{
    nAppState::OnCreate( application );

    this->shot = static_cast<nPhyGeomRay*>( nObject::kernelServer->New( "nphygeomray" ) );
    this->shot->SetCategories( 0 );
    this->shot->SetCollidesWith( -1 );

    this->refRootNode = (nTransformNode*) nKernelServer::ks->Lookup( "/usr/scene/objects" );
    this->refSceneGraph = (nSceneGraph *) nSceneServer::Instance()->NewSceneGraph(0);
}

//------------------------------------------------------------------------------
/**
    @param prevState previous state
*/
void
RnsPhysicsState::OnStateEnter( const nString & /*prevState*/ )
{
    nSceneServer::Instance()->AddGraph(this->refSceneGraph.get());
    this->viewMatrix.ident();
}

//------------------------------------------------------------------------------
/**
    @param nextState next state
*/
void
RnsPhysicsState::OnStateLeave( const nString & /*nextState*/ )
{
    nSceneServer::Instance()->RemoveGraph(this->refSceneGraph.get());
}

//------------------------------------------------------------------------------
/**
*/
void
RnsPhysicsState::OnFrame()
{
    this->HandleInput(static_cast<float>( this->app->GetFrameTime() ));

    // Shot
    vector3 viewPosition(this->ConvertScreenToViewSpace( vector2(0.5, 0.5) ));

    matrix44 invProj(nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection));            
    invProj.invert();

    vector3 position( this->viewMatrix.pos_component());
    this->shotDirection = this->viewMatrix * viewPosition;
    this->shotDirection = this->shotDirection - position;

    this->shot->SetLength( 1000 );
    this->shot->SetDirection( this->shotDirection );
    this->shot->SetPosition( position );
    nPhyCollide::nContact contact;
    int num( nPhysicsServer::Instance()->Collide( this->shot, 1, &contact ) );

    if( num )
    {
        this->shotObj = contact.GetGeometryIdB();
        contact.GetContactPosition( this->shotPosition );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RnsPhysicsState::OnRender3D()
{
    this->viewContext->GetComponent<ncScene>()->Render(this->refSceneGraph.get());
    this->lightContext->GetComponent<ncScene>()->Render(this->refSceneGraph.get());
    this->renderContext->GetComponent<ncScene>()->Render(this->refSceneGraph.get());
}

//------------------------------------------------------------------------------
/**
*/
void
RnsPhysicsState::OnRender2D()
{
#ifndef NGAME
    nPhysicsServer::Instance()->Draw( nGfxServer2::Instance() );
#endif
}

//------------------------------------------------------------------------------
/**
    @param frameTime time from last call
*/
void
RnsPhysicsState::HandleInput(const float frameTime)
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

    this->viewMatrix.ident();
    this->viewMatrix.rotate_x( this->viewerAngles.theta );
    this->viewMatrix.rotate_y( this->viewerAngles.rho );

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

    this->viewMatrix.translate( this->viewerPos );

    //this->viewContext->SetTransform( this->viewMatrix );
}

//------------------------------------------------------------------------------
/**
    @param screenCoord position in the screen
    @returns position in the space
*/
vector3 
RnsPhysicsState::ConvertScreenToViewSpace( const vector2& screenCoord )
{
   // get current inverted projection matrix
   matrix44 invProj = nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection);
   invProj.invert();

   vector3 screenCoord3D((screenCoord.x - 0.5f) * 2.0f, -(screenCoord.y - 0.5f) * 2.0f, 1.0f);
   vector3 viewCoord = invProj * screenCoord3D;

   // get near plane
   float nearZ = nGfxServer2::Instance()->GetCamera().GetNearPlane();
   return viewCoord * nearZ * 1.1f;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
