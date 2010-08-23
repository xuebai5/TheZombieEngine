#include "shaders/shadersapp.h"

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
void ShadersApp::Init()
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
bool ShadersApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

/// --- copied from camerasapp
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

    this->refFloorShader = gfxServer->NewShader("color");
    if (!this->LoadResource(this->refFloorShader, "proj:shaders/color.fx"))
        return false;
/// --- copied from camerasapp
    
    this->refShader = gfxServer->NewShader("phong_bump_reflect");
    if (!this->LoadResource(this->refShader, "proj:shaders/phong_bump_reflect.fx"))
        return false;

    this->refBumpTexture = gfxServer->NewTexture("default_bump_normal");
    if (!this->LoadResource(this->refBumpTexture, "proj:textures/default_bump_normal.dds"))
        return false;

    this->refCubeTexture = gfxServer->NewTexture("default_reflection");
    if (!this->LoadResource( this->refCubeTexture, "proj:textures/default_reflection.dds"))
        return false;

    //light sphere mesh and shader
    this->refSphereMesh = gfxServer->NewMesh("sphere");
    if (!this->LoadResource(refSphereMesh, "proj:meshes/sphere.n3d2"))
        return false;

    this->refColorShader = gfxServer->NewShader("default");
    if (!this->LoadResource(this->refColorShader, "proj:shaders/default.fx"))
        return false;

    return true;
}

//------------------------------------------------------------------------------

void ShadersApp::Close()
{
    N_REF_RELEASE(this->refMesh);
    N_REF_RELEASE(this->refTexture);
    N_REF_RELEASE(this->refShader);
    N_REF_RELEASE(this->refBumpTexture);
    N_REF_RELEASE(this->refCubeTexture);    

    N_REF_RELEASE(this->refFloorMesh);
    N_REF_RELEASE(this->refFloorTexture);
    N_REF_RELEASE(this->refFloorShader);

    N_REF_RELEASE(this->refSphereMesh);
    N_REF_RELEASE(this->refColorShader);
}

//------------------------------------------------------------------------------

void ShadersApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();

/// --- copied from camerasapp ---
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

/// --- copied from camerasapp ---
    if (inputServer->GetButton("light"))
        this->vecLightPos.set( this->vecEye );
}

//------------------------------------------------------------------------------

void ShadersApp::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->Clear( nGfxServer2::AllBuffers, .3f, .3f, .3f, 1.f, 1.f, 0 );

    //view transform
    this->matView.ident();
    this->matView.rotate_x( this->vecRot.x );//pitch
    this->matView.rotate_y( this->vecRot.y );//yaw
    this->matView.translate( this->vecEye );

    this->matView.invert_simple();

    gfxServer->SetTransform( nGfxServer2::View, this->matView );

    nCamera2 cam;
    gfxServer->SetCamera( cam );

    //Lighting
    //vector3 vecLightPos(0,5,-10);//(-0.5f,2.0f,1.25f
    vector4 vecLightDiffuse(1.f, 0.f, 0.f, 1.f);
    vector4 vecLightAmbient(.3f, .3f, .3f, 1.f);

    matrix44 matLight;
    matLight.translate( vecLightPos );
    gfxServer->SetTransform( nGfxServer2::Light, matLight );

    float fSpecular = .4f;
    float fSpecularPower = 64.f;
    float fBumpScale = 1.f;
    float fReflectionScale = .5f;

    //draw the torus
    this->matWorld.ident();
    this->matWorld.scale( this->vecScale );
    this->matWorld.rotate_x( this->vecRotation.x );
    this->matWorld.rotate_y( this->vecRotation.y );
    this->matWorld.rotate_z( this->vecRotation.z );
    this->matWorld.translate( this->vecPosition );//pitch

    this->BeginDraw( this->refShader, this->refMesh );
    this->BeginPass( this->refShader, 0 );
    //BEGIN- shader parameters (TODO- use ShaderParams)
    this->refShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShader->SetVector3( nShaderState::LightPos, vecLightPos );
    this->refShader->SetVector4( nShaderState::LightDiffuse, vecLightDiffuse );
    this->refShader->SetVector4( nShaderState::LightAmbient, vecLightAmbient );
    this->refShader->SetFloat( nShaderState::MatSpecular, fSpecular );
    this->refShader->SetFloat( nShaderState::MatSpecularPower, fSpecularPower );
    this->refShader->SetFloat( nShaderState::BumpScale, fBumpScale );
    this->refShader->SetFloat( nShaderState::MatLevel, fReflectionScale );

    this->refShader->SetTexture( nShaderState::DiffMap0, this->refTexture );
    this->refShader->SetTexture( nShaderState::BumpMap0, this->refBumpTexture );
    this->refShader->SetTexture( nShaderState::CubeMap0, this->refCubeTexture );
    //END- shader parameters
    this->Draw( matWorld );
    this->EndPass( this->refShader );
    this->EndDraw( this->refShader );

    //draw the light
    this->BeginDraw( this->refColorShader, this->refSphereMesh );
    this->BeginPass( this->refColorShader, 0 );
    this->refColorShader->SetVector4( nShaderState::matDiffuse, vecLightDiffuse );
    this->Draw( vecLightPos, vector3( .5f, .5f, .5f ) );
    this->EndPass( this->refColorShader );
    this->EndDraw( this->refColorShader );

    //draw the floor
    this->BeginDraw( this->refFloorShader, this->refFloorMesh );
    this->BeginPass( this->refFloorShader, 0 );
    this->refFloorShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refFloorShader->SetTexture( nShaderState::diffMap, this->refFloorTexture );
    this->Draw( vector3( -5.f, 0.f, -5.f ), vector3( 10.f, 0.f, 10.f ) );
    this->EndPass( this->refFloorShader );
    this->EndDraw( this->refFloorShader );
}
