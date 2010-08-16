#include "transform/transformapp.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "util/nrandomlogic.h"

//------------------------------------------------------------------------------
/**
    run script that loads required resources, etc.
	and sets initial position of everything
*/
void TransformApp::Init()
{
    this->bWireframe = false;

    this->vecEye.set(0,5,10);

    //FreeCam
    //Pitch: X-rot, Yaw: Y-rot, Roll: Z-rot
    this->vecRot.set(n_deg2rad(-30),0,0); //looking down 30 degrees
}

//------------------------------------------------------------------------------
/**
*/
bool TransformApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    this->refMesh = gfxServer->NewMesh("torus");
    if (!this->LoadResource(this->refMesh, "proj:meshes/torus.n3d2"))
        return false;

    this->refTexture = gfxServer->NewTexture("brick");
    if (!this->LoadResource(this->refTexture, "proj:textures/brick.bmp"))
        return false;

    this->refFloorMesh = gfxServer->NewMesh("plane");
    if (!this->LoadResource(this->refFloorMesh, "proj:meshes/plane_xz.n3d2"))
        return false;

    this->refFloorTexture = gfxServer->NewTexture("sidewalk");
    if (!this->LoadResource(this->refFloorTexture, "proj:textures/sidewalk.dds"))
        return false;

    this->refShader = gfxServer->NewShader("color");
    if (!this->LoadResource(this->refShader, "proj:shaders/color.fx"))
        return false;

    return true;
}

//------------------------------------------------------------------------------

void TransformApp::Close()
{
    this->refMesh->Release();
    this->refFloorMesh->Release();
    this->refTexture->Release();
    this->refFloorTexture->Release();
    this->refShader->Release();
}

//------------------------------------------------------------------------------

void TransformApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();

    if (inputServer->GetButton("wireframe"))
        this->bWireframe = !this->bWireframe;

    float mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
    float mouse_y = (inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down"));

    float angleSpace = n_deg2rad(360.f) * 10.f * fTimeElapsed;//=angleSpeed
    float moveSpace = 10.f * fTimeElapsed;//=cameraSpeed

    //camera look around
    if (inputServer->GetButton("left_pressed"))
    {
        this->vecRot.y += mouse_x * angleSpace;
        this->vecRot.x += mouse_y * angleSpace;
    }

    //camera move
    vector3 vecMove;
    if (inputServer->GetButton("forward"))
    {
        vecMove.z -= moveSpace;
    }
    if (inputServer->GetButton("backward"))
    {
        vecMove.z += moveSpace;
    }
    if (inputServer->GetButton("strafe_left"))
    {
        vecMove.x -= moveSpace;
    }
    if (inputServer->GetButton("strafe_right"))
    {
        vecMove.x += moveSpace;
    }

    //update camera position
    matrix44 mat;
    mat.rotate_x( this->vecRot.x );
    mat.rotate_y( this->vecRot.y );
    mat.translate( this->vecEye );

    //transform with vectors:
    this->vecEye = mat * vecMove;

    //transform with matrices:
    //matrix44 matMove;
    //matMove.translate(vecMove);//the transform for the movement
    //matMove = matMove * mat;
    //this->vecEye = matMove.pos_component();
}

//------------------------------------------------------------------------------

void TransformApp::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->Clear( nGfxServer2::AllBuffers, .3f, .3f, .3f, 1.f, 1.f, 0 );

    this->matView.ident();
    this->matView.rotate_x( this->vecRot.x );//pitch
    this->matView.rotate_y( this->vecRot.y );//yaw
    this->matView.translate( this->vecEye );
    this->matView.invert_simple();

    gfxServer->SetTransform( nGfxServer2::View, this->matView );

    nCamera2 cam;
    gfxServer->SetCamera( cam );

    //draw the torus
    this->BeginDraw( this->refShader, this->refMesh );
    this->BeginPass( this->refShader, 0 );
    this->refShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShader->SetTexture( nShaderState::diffMap, this->refTexture );
    vector3 vecScale( 1.f, 1.f, 1.f );
    vector3 vecPosition( 0.f, 1.f, 0.f );
    this->Draw( vecPosition, vecScale );
    this->EndPass( this->refShader );
    this->EndDraw( this->refShader );

    //draw the floor
    this->BeginDraw( this->refShader, this->refFloorMesh );
    this->BeginPass( this->refShader, 0 );
    this->refShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShader->SetTexture( nShaderState::diffMap, this->refFloorTexture );
    this->Draw( vector3( -5.f, 0.f, -5.f ), vector3( 10.f, 0.f, 10.f ) );
    this->EndPass( this->refShader );
    this->EndDraw( this->refShader );
}
