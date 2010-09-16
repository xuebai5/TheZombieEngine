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
#include "ngeomipmap/ncterraingmmclass.h"

#include "gfx2/nshader2.h"
#include "gfx2/nmesh2.h"

nNebulaClass(nShootemState, "ncommonstate");

//------------------------------------------------------------------------------

nShootemState::nShootemState()
{
    this->playerSpeed = 5.f;
    this->turnSpeed = n_deg2rad(60.f);

    this->cameraOffset.set(0, 3.f, -5.f);
    this->cameraThreshold = 1.f;
    this->cameraAngles.set(n_deg2rad(-15), 0.f);

    this->fProjectileMaxTime = 2.f;
    this->fProjectileSpeed = 10.f;
}

//------------------------------------------------------------------------------

nShootemState::~nShootemState()
{
}

//------------------------------------------------------------------------------

void
nShootemState::OnCreate(nApplication* application)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    this->refShaderColor = gfxServer->NewShader("color");
    n_assert(this->refShaderColor.isvalid());
    this->refShaderColor->SetFilename("home:data/demos/shaders/color.fx");
    if (!this->refShaderColor->Load())
    {
        this->refShaderColor->Release();
        this->refShaderColor.invalidate();
    }

    this->refMeshCone = gfxServer->NewMesh("cone");
    n_assert(this->refMeshCone.isvalid());
    this->refMeshCone->SetFilename("home:data/demos/meshes/cone.nvx2");
    if (!this->refMeshCone->Load())
    {
        this->refMeshCone->Release();
        this->refMeshCone.invalidate();
    }

    nCommonState::OnCreate(application);
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

    //find the terrain entity
    nRefEntityObject refTerrain(level->FindEntity("outdoor"));
    if (refTerrain.isvalid())
    {
        ncTerrainGMMClass * terraincomp = refTerrain->GetClassComponent<ncTerrainGMMClass>();
        if (terraincomp)
        {
            this->refHeightMap = terraincomp->GetHeightMap();
        }
    }

    this->cameraPos = this->playerPos + this->cameraOffset;
    this->cameraAngles.set(n_deg2rad(-15.f), this->playerRot.y + n_deg2rad(180.f));

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
    nTime frameTime = this->app->GetFrameTime();

    //---HandleInput(frameTime)---
    nInputServer* inputServer = nInputServer::Instance();

    //player rotate
    float angleSpace = this->turnSpeed * float(frameTime);

    if (inputServer->GetButton("StrafeLeft"))
    {
        this->playerRot.y += angleSpace;
    }
    if (inputServer->GetButton("StrafeRight"))
    {
        this->playerRot.y -= angleSpace;
    }

    //player move
    float moveSpace = this->playerSpeed * float(frameTime);

    vector3 vecMove;
    if (inputServer->GetButton("Forward"))
    {
        vecMove.z += moveSpace;
    }
    if (inputServer->GetButton("Backward"))
    {
        vecMove.z -= moveSpace;
    }
    //if (inputServer->GetButton("StrafeLeft"))
    //{
    //    vecMove.x -= moveSpace;
    //}
    //if (inputServer->GetButton("StrafeRight"))
    //{
    //    vecMove.x += moveSpace;
    //}

    matrix44 matWorld;
    matWorld.rotate_y(this->playerRot.y);
    matWorld.translate(this->playerPos);

    //update player position
    this->playerPos = matWorld * vecMove;

    //adjust to terrain
    if (this->refHeightMap)
    {
        float height;
        if (this->refHeightMap->GetHeight(this->playerPos.x, this->playerPos.z, height))
        {
            this->playerPos.y=height;
        }
    }

    //update camera position applying threshold
    vector3 eyePos = matWorld * this->cameraOffset;
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
    this->cameraAngles.rho = this->playerRot.y + n_deg2rad(180);//can't figure this out!

    //shoot
    if (inputServer->GetButton("PrimaryAttack"))
    {
        this->AddProjectile();
    }

    this->TickProjectiles(float(frameTime));

    //update entities
    ncTransform* transform = this->refPlayerEntity->GetComponentSafe<ncTransform>();
    transform->SetPosition(this->playerPos);
    transform->SetEuler(this->playerRot);

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

//------------------------------------------------------------------------------
/**
*/
void
nShootemState::OnRender2D()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    
    //draw projectiles
    this->DrawProjectiles();

    //render text
    float rowheight = 32.f / gfxServer->GetDisplayMode().GetHeight();
    nString str;

    //debug text
    str.Format("Projectiles = %d", this->projectiles.Size());
    gfxServer->Text( str.Get(), vector4(1.f,1.f,0,1), -1.f, 1.f - rowheight );//lower-left corner
}
