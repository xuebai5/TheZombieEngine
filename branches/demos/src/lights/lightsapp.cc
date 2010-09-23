#include "lights/lightsapp.h"

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
void LightsApp::Init()
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
    this->vecLightDiffuse.set( 1.f ,1.f, 1.f, 1.f );
    this->vecLightAmbient.set(.3f, .3f, .3f, 1.f);
}

//------------------------------------------------------------------------------
/**
*/
bool LightsApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    N_REF_LOAD_MESH(this->refMesh, "torus", "proj:meshes/torus.n3d2");
    N_REF_LOAD_TEXTURE(this->refDiffMap, "brick", "proj:textures/brick.bmp");
    N_REF_LOAD_TEXTURE(this->refBumpMap, "bump", "proj:textures/bump.tga");
    N_REF_LOAD_TEXTURE(this->refCubeMap, "grace_cube", "proj:textures/grace_cube.dds");

    N_REF_LOAD_MESH(this->refMeshFloor, "plane", "proj:meshes/plane_xz.n3d2");
    N_REF_LOAD_TEXTURE(this->refTextureFloor, "sidewalk", "proj:textures/sidewalk.dds");

    N_REF_LOAD_SHADER(this->refShaderColor, "color", "proj:shaders/color.fx");
    N_REF_LOAD_SHADER(this->refShaderDiffuse, "diffuse", "proj:shaders/diffuse.fx");

    N_REF_LOAD_MESH(this->refMeshSphere, "sphere", "proj:meshes/sphere.n3d2");

    //shaders for lighting
    Material* material(0);

    material = &this->materials.PushBack( Material() );
    material->shaderParams.SetArg( nShaderState::DiffMap0, nShaderArg(this->refDiffMap) );//diffuse
    material->shaderParams.SetArg( nShaderState::MatDiffuse, vector4(1.f, .3f, 0.f, 1.f) );//color
    material->shaderParams.SetArg( nShaderState::MatSpecular, vector4(.7f, .6f, 0.4f, 1.f) );//gloss
    material->technique.Set("Phong");
    N_REF_LOAD_SHADER(material->refShader, "phong_pixel", "proj:shaders/phong_pixel.fx");

    material = &this->materials.PushBack( Material() );
    material->shaderParams.SetArg( nShaderState::DiffMap0, nShaderArg(this->refDiffMap) );//diffuse
    material->shaderParams.SetArg( nShaderState::BumpMap0, nShaderArg(this->refBumpMap) );//normal
    material->shaderParams.SetArg( nShaderState::MatDiffuse, vector4(1.f, .3f, 0.f, 1.f) );//color
    material->shaderParams.SetArg( nShaderState::MatSpecular, vector4(.7f, .6f, 0.4f, 1.f) );//gloss
    material->technique.Set("Phong_Bump");
    N_REF_LOAD_SHADER(material->refShader, "phong_pixel", "proj:shaders/phong_pixel.fx");

    material = &this->materials.PushBack( Material() );
    material->shaderParams.SetArg( nShaderState::DiffMap0, nShaderArg(this->refDiffMap) );//diffuse
    material->shaderParams.SetArg( nShaderState::BumpMap0, nShaderArg(this->refBumpMap) );//normal
    material->shaderParams.SetArg( nShaderState::CubeMap0, nShaderArg(this->refCubeMap) );//reflection
    material->shaderParams.SetArg( nShaderState::MatDiffuse, vector4(1.f, .3f, 0.f, 1.f) );//color
    material->shaderParams.SetArg( nShaderState::MatSpecular, vector4(.7f, .6f, 0.4f, 1.f) );//gloss
    material->technique.Set("Phong_Bump_Reflect");
    N_REF_LOAD_SHADER(material->refShader, "phong_pixel", "proj:shaders/phong_pixel.fx");

    this->curMaterialIndex = 0;

    return true;
}

//------------------------------------------------------------------------------

void LightsApp::Close()
{
    N_REF_RELEASE(this->refMesh)
    N_REF_RELEASE(this->refDiffMap)
    N_REF_RELEASE(this->refBumpMap)
    N_REF_RELEASE(this->refCubeMap)

    N_REF_RELEASE(this->refMeshFloor)
    N_REF_RELEASE(this->refTextureFloor)

    N_REF_RELEASE(this->refShaderColor)
    N_REF_RELEASE(this->refShaderDiffuse)

    N_REF_RELEASE(this->refMeshSphere)

    for (int index=0; index<this->materials.Size(); index++)
        N_REF_RELEASE(this->materials[index].refShader);

    this->materials.Clear();
}

//------------------------------------------------------------------------------

void LightsApp::Tick( float fTimeElapsed )
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

    if (inputServer->GetButton("toggle"))
        this->curMaterialIndex = (this->curMaterialIndex + 1) % this->materials.Size();
/// --- copied from shadersapp ---
}

//------------------------------------------------------------------------------

void LightsApp::Render()
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

    //draw the scene
    this->matWorld.ident();
    this->matWorld.scale( this->vecScale );
    this->matWorld.rotate_x( this->vecRotation.x );
    this->matWorld.rotate_y( this->vecRotation.y );
    this->matWorld.rotate_z( this->vecRotation.z );
    this->matWorld.translate( this->vecPosition );

    matrix44 matModel(matWorld); //Model matrix = inverse world matrix (from World->Model)
    matModel.invert_simple();

    Material& material = this->materials[this->curMaterialIndex];
    this->BeginDraw( material.refShader, this->refMesh, material.technique.Get() );
    this->BeginPass( material.refShader, 0 );
    material.shaderParams.SetArg( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    //light parameters
    material.shaderParams.SetArg( nShaderState::LightPos, this->vecLightPos );
    vector3 vecModelLightPos = matModel * vecLightPos;
    material.shaderParams.SetArg( nShaderState::ModelLightPos, vecModelLightPos );
    material.shaderParams.SetArg( nShaderState::LightDiffuse, vecLightDiffuse );
    material.shaderParams.SetArg( nShaderState::LightAmbient, vecLightAmbient );
    material.refShader->SetParams( material.shaderParams );
    this->Draw( matWorld );
    this->EndPass( material.refShader );
    this->EndDraw( material.refShader );

    //draw the lights
    this->BeginDraw( this->refShaderColor, this->refMeshSphere );
    this->BeginPass( this->refShaderColor, 0 );
    this->refShaderColor->SetVector4( nShaderState::MatDiffuse, this->vecLightDiffuse );
    this->Draw( this->vecLightPos, vector3( .5f, .5f, .5f ) );
    this->EndPass( this->refShaderColor );
    this->EndDraw( this->refShaderColor );

    //draw text
    float rowheight = 32.f / gfxServer->GetDisplayMode().GetHeight();
    nString str;
    str.Format("%s,%s", material.refShader->GetName(), material.technique.Get() );
    gfxServer->Text( str.Get(), vector4(1.f,1.f,0,1), -1.f, 1.f - rowheight );
}
