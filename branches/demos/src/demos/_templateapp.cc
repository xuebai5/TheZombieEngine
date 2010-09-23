#include "demos/_templateapp.h"

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
void _TemplateApp::Init()
{
    this->bWireframe = false;

    //model
    this->vecPosition.set( 0.f, 1.f, 0.f );
    this->vecRotation.set( 0.f, 0.f, 0.f );
    this->vecScale.set( 1.f, 1.f, 1.f );

    //camera
    this->vecEye.set(0,5,10);
    this->vecRot.set(n_deg2rad(-30),0,0); //looking down 30 degrees

    //light
    this->vecLightPos.set( 0.f ,5.f ,-10.f );

}

//------------------------------------------------------------------------------
/**
*/
bool _TemplateApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    N_REF_LOAD_MESH(this->refMesh, "torus", "proj:meshes/torus.n3d2");
    N_REF_LOAD_TEXTURE(this->refTexture, "brick", "proj:textures/brick.bmp");

    N_REF_LOAD_MESH(this->refMeshFloor, "plane", "proj:meshes/plane_xz.n3d2");
    N_REF_LOAD_TEXTURE(this->refTextureFloor, "sidewalk", "proj:textures/sidewalk.dds");

    N_REF_LOAD_SHADER(this->refShaderColor, "color", "proj:shaders/color.fx");
    N_REF_LOAD_SHADER(this->refShaderDiffuse, "diffuse", "proj:shaders/diffuse.fx");

    //shaders for lighting
        
    return true;
}

//------------------------------------------------------------------------------

void _TemplateApp::Close()
{
    N_REF_RELEASE(this->refMesh)
    N_REF_RELEASE(this->refTexture)

    N_REF_RELEASE(this->refMeshFloor)
    N_REF_RELEASE(this->refTextureFloor)

    N_REF_RELEASE(this->refShaderColor)
    N_REF_RELEASE(this->refShaderDiffuse)
}

//------------------------------------------------------------------------------

void _TemplateApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();

/// --- copied from shadersapp ---
    if (inputServer->GetButton("wireframe"))
        this->bWireframe = !this->bWireframe;

    float mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
    float mouse_y = (inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down"));

    float angleSpace = n_deg2rad(360.f) * 10.f * fTimeElapsed;//=angleSpeed
    float moveSpace = 10.f * fTimeElapsed;//=cameraSpeed

    //camera look around
    if (inputServer->GetButton("right_pressed"))
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

    if (inputServer->GetButton("light"))
        this->vecLightPos.set( this->vecEye );
/// --- copied from shadersapp ---
}

//------------------------------------------------------------------------------

void _TemplateApp::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->Clear( nGfxServer2::AllBuffers, 0.f, 0.f, 1.f, 1.f, 1.f, 0 );

    //view transform
    this->matView.ident();
    this->matView.rotate_x( this->vecRot.x );//pitch
    this->matView.rotate_y( this->vecRot.y );//yaw
    this->matView.translate( this->vecEye );

    this->matView.invert_simple();
    gfxServer->SetTransform( nGfxServer2::View, this->matView );

    //projection transform
    nCamera2 cam;
    gfxServer->SetCamera( cam );

    //draw the floor
    this->BeginDraw( this->refShaderDiffuse, this->refMeshFloor );
    this->BeginPass( this->refShaderDiffuse, 0 );
    this->refShaderDiffuse->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShaderDiffuse->SetTexture( nShaderState::DiffMap0, this->refTextureFloor );
    this->refShaderDiffuse->SetVector4( nShaderState::MatDiffuse, vector4(1.f,1.f,1.f,1.f) );
    this->Draw( vector3( -5.f, 0.f, -5.f ), vector3( 10.f, 0.f, 10.f ) );
    this->EndPass( this->refShaderDiffuse );
    this->EndDraw( this->refShaderDiffuse );


}
