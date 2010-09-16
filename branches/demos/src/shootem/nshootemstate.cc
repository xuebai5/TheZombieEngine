#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
//  nshooterstate_input.cc
//  (C) 2010 M.A. Garcias <ma@magarcias.com>
//------------------------------------------------------------------------------
#include "shootem/nshootemstate.h"

#include "ncommonapp/ncommonstate.h"

#include "napplication/napplication.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"
#include "input/ninputserver.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "zombieentity/nctransform.h"

nNebulaClass(nShootemState, "ncommonstate");

//------------------------------------------------------------------------------

nShootemState::nShootemState()
{
    this->playerSpeed = 5.f;

    this->cameraOffset.set(-5.f, 4.f, 1.f);//TEMP!
    this->cameraThreshold = 0.f;//1.f;
    this->cameraAngles.set(-0.19f, 4.69f);//TEMP!
}

//------------------------------------------------------------------------------

nShootemState::~nShootemState()
{
}

//------------------------------------------------------------------------------

void
nShootemState::OnStateEnter( const nString & prevState )
{
    // getting viewport
    this->refViewport = static_cast<nAppViewport*>( nKernelServer::ks->Lookup( "/usr/rnsview" ) );
    n_assert( this->refViewport.isvalid() );
    this->refViewport->SetVisible(true);

    // find the player entity
    nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
    this->refPlayerEntity.set(level->FindEntity("player"));
    if (this->refPlayerEntity.isvalid())
    {
        ncTransform* transform = this->refPlayerEntity->GetComponentSafe<ncTransform>();
        this->playerPos.set(transform->GetPosition());
        this->playerRot.set(transform->GetEuler());
    }

    this->cameraPos = this->playerPos + this->cameraOffset;

    nCommonState::OnStateEnter(prevState);
}

//------------------------------------------------------------------------------
/**
    @param nextState next state
*/
void
nShootemState::OnStateLeave( const nString & nextState )
{
    this->refViewport->SetVisible(false);

    nCommonState::OnStateLeave( nextState );
}

//------------------------------------------------------------------------------
/**
    @param nextState next state
*/
void
nShootemState::OnFrame()
{
    nInputServer* inputServer = nInputServer::Instance();
    nTime frameTime = this->app->GetFrameTime();

    float moveSpace = this->playerSpeed * float(frameTime);

    //camera move
    vector3 vecMove;
    if (inputServer->GetButton("Forward"))
    {
        vecMove.z += moveSpace;
    }
    if (inputServer->GetButton("Backward"))
    {
        vecMove.z -= moveSpace;
    }
    if (inputServer->GetButton("StrafeLeft"))
    {
        vecMove.x -= moveSpace;
    }
    if (inputServer->GetButton("StrafeRight"))
    {
        vecMove.x += moveSpace;
    }

    //update player position
    vector3 playerPos = this->playerPos + vecMove;
    this->playerPos = playerPos;

    //update camera position applying threshold
    matrix44 matWorld;
    matWorld.rotate_x(this->playerRot.x);
    matWorld.rotate_y(this->playerRot.y);
    matWorld.invert_simple();
    vector3 cameraOffset = matWorld * this->cameraOffset;
    vector3 eyePos = this->playerPos + cameraOffset;
    //if ((eyePos.z - this->cameraPos.z) > this->cameraThreshold)
    //{
    //    this->cameraPos.z = eyePos.z - this->cameraThreshold;
    //}
    //else if ((this->cameraPos.z - eyePos.z) > this->cameraThreshold)
    //{
    //    this->cameraPos.z = eyePos.z + this->cameraThreshold;
    //}
    //this->cameraPos.x = eyePos.x;
    this->cameraPos = eyePos;
    
    //update entities
    ncTransform* transform = this->refPlayerEntity->GetComponentSafe<ncTransform>();
    transform->SetPosition(this->playerPos);

    this->refViewport->SetViewerPos(this->cameraPos);
    this->refViewport->SetViewerAngles(this->cameraAngles);

    //viewport logic
    this->refViewport->SetFrameId(this->app->GetFrameId());
    this->refViewport->SetFrameTime(this->app->GetFrameTime());
    this->refViewport->SetTime(this->app->GetTime());
    this->refViewport->Trigger();

    nCommonState::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
nShootemState::OnRender3D()
{
    this->refViewport->OnRender3D();
}
