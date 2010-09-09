#include "ambient/ambientapp.h"

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
void AmbientApp::Init()
{
    this->bWireframe = false;
    this->bShowDiffMap = false;
}

//------------------------------------------------------------------------------
/**
*/
bool AmbientApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    //models
    N_REF_LOAD_MESH(model.shapes[0].refMesh, "mesh_00000870.0", "proj:models/Soldier/mesh_00000870.0.nvx2")
    N_REF_LOAD_TEXTURE(model.shapes[0].refDiffMap, "ns_forces_COLOR", "proj:models/Soldier/ns_forces_COLOR.DDS")
    N_REF_LOAD_TEXTURE(model.shapes[0].refSpecMap, "ns_forces_SPEC", "proj:models/Soldier/ns_forces_SPEC.DDS")
    N_REF_LOAD_SHADER(model.shapes[0].refShader, "ambient", "proj:shaders/ambient.fx")

    N_REF_LOAD_MESH(model.shapes[1].refMesh, "mesh_00000870.1", "proj:models/Soldier/mesh_00000870.1.nvx2")
    N_REF_LOAD_TEXTURE(model.shapes[1].refDiffMap, "head_01A_COLOR", "proj:models/Soldier/head_01A_COLOR.DDS")
    N_REF_LOAD_TEXTURE(model.shapes[1].refSpecMap, "head_01A_SPEC", "proj:models/Soldier/head_01A_SPEC.DDS")
    N_REF_LOAD_SHADER(model.shapes[1].refShader, "ambient", "proj:shaders/ambient.fx")

    //meshes
    N_REF_LOAD_MESH(this->refSphereMesh, "sphere", "proj:meshes/sphere.n3d2")
    N_REF_LOAD_SHADER(this->refCubeShader, "ambient_cube", "proj:shaders/ambient_cube.fx")

    //textures
    N_REF_LOAD_TEXTURE(this->refCubeTexture, "grace_cube", "proj:textures/grace_cube.dds")
    N_REF_LOAD_TEXTURE(this->refDiffCubeTexture, "grace_diffuse_cube", "proj:textures/grace_diffuse_cube.dds")

    return true;
}

//------------------------------------------------------------------------------

void AmbientApp::Close()
{
    for (int index=0; index<2; index++)
    {
        N_REF_RELEASE(model.shapes[index].refMesh);
        N_REF_RELEASE(model.shapes[index].refShader);
        N_REF_RELEASE(model.shapes[index].refDiffMap);
        N_REF_RELEASE(model.shapes[index].refSpecMap);
    }

    N_REF_RELEASE(this->refSphereMesh);
    N_REF_RELEASE(this->refCubeShader);

    N_REF_RELEASE(this->refCubeTexture);
    N_REF_RELEASE(this->refDiffCubeTexture);
}

//------------------------------------------------------------------------------

void AmbientApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();

/// --- copied from camerasapp ---
    if (inputServer->GetButton("wireframe"))
        this->bWireframe = !this->bWireframe;

    float mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
    float mouse_y = (inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down"));

    float angleSpace = n_deg2rad(360.f) * 5.f * fTimeElapsed;//=angleSpeed
    float moveSpace = 5.f * fTimeElapsed;//=cameraSpeed

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
    if (inputServer->GetButton("toggle"))
        this->bShowDiffMap = !this->bShowDiffMap;
}

//------------------------------------------------------------------------------

void AmbientApp::Render()
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

    //Draw Cube
    this->BeginDraw( this->refCubeShader, this->refSphereMesh );
    this->BeginPass( this->refCubeShader, 0 );
    this->refCubeShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refCubeShader->SetTexture( nShaderState::CubeMap0, this->bShowDiffMap ? this->refDiffCubeTexture : this->refCubeTexture );
    this->Draw( vector3(0,0,0), vector3( 10.f, 10.f, 10.f ) );
    this->EndPass( this->refCubeShader );
    this->EndDraw( this->refCubeShader );

    //DrawModel(Model*)
    for (int index=0; index<2; index++)
    {
        nTexture2* diffMap = this->model.shapes[index].refDiffMap;
        nTexture2* specMap = this->model.shapes[index].refSpecMap;
        nShader2* shader = this->model.shapes[index].refShader;
        nMesh2* mesh = this->model.shapes[index].refMesh;
        int numPasses = this->BeginDraw( shader, mesh );
        for (int passIndex = 0; passIndex < numPasses; passIndex++)
        {
            this->BeginPass( shader, passIndex );
            shader->SetTexture(nShaderState::DiffMap0, diffMap );
            shader->SetTexture(nShaderState::SpecMap0, specMap );
            shader->SetTexture(nShaderState::CubeMap0, this->refCubeTexture );
            shader->SetTexture(nShaderState::CubeMap1, this->refDiffCubeTexture );
            this->Draw( matWorld );
            this->EndPass( shader );
        }
    }
}
