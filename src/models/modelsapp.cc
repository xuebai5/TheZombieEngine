#include "models/modelsapp.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "util/nrandomlogic.h"

#include "models/textfile.h"

//------------------------------------------------------------------------------
/**
    run script that loads required resources, etc.
	and sets initial position of everything
*/
void ModelsApp::Init()
{
    this->bWireframe = false;
    this->bShowDiffMap = false;
}

//------------------------------------------------------------------------------
/**
*/
bool ModelsApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    //TEMP- shaders
    N_REF_LOAD_SHADER(this->refShader, "diffuse", "proj:shaders/diffuse.fx")

    this->models.SetFixedSize(2);
    //if (!this->LoadModel(&this->model, "wc:export/assets/aNsforces_01_lod2"))
    //if (!this->LoadModel(&this->model, "wc:export/assets/aNsforces_01_lod1"))

    if (!this->LoadModel(&this->models[0], "wc:export/assets/aNsforces_01_lod0")) return false;
    if (!this->LoadModel(&this->models[1], "wc:export/assets/aHead_00_lod0")) return false;

    //meshes
    N_REF_LOAD_MESH(this->refSphereMesh, "sphere", "proj:meshes/sphere.n3d2")
    N_REF_LOAD_SHADER(this->refCubeShader, "ambient_cube", "proj:shaders/ambient_cube.fx")

    //textures
    N_REF_LOAD_TEXTURE(this->refCubeTexture, "grace_cube", "proj:textures/grace_cube.dds")
    N_REF_LOAD_TEXTURE(this->refDiffCubeTexture, "grace_diffuse_cube", "proj:textures/grace_diffuse_cube.dds")

    return true;
}

//------------------------------------------------------------------------------

void ModelsApp::Close()
{
    for (int index=0; index<models.Size(); index++)
        this->ReleaseModel( &this->models[index] );

    N_REF_RELEASE(this->refShader);

    N_REF_RELEASE(this->refSphereMesh);
    N_REF_RELEASE(this->refCubeShader);

    N_REF_RELEASE(this->refCubeTexture);
    N_REF_RELEASE(this->refDiffCubeTexture);
}

//------------------------------------------------------------------------------

void ModelsApp::Tick( float fTimeElapsed )
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

void ModelsApp::Render()
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

    //Draw model
    this->DrawModels();
}

//------------------------------------------------------------------------------
///---move to Model.cc---

bool ModelsApp::LoadModel(Model* model, const char* path)
{
    //nGfxServer2* gfxServer = nGfxServer2::Instance();

//    model->shapes.SetFixedSize(2);//temp

    //N_REF_LOAD_MESH(model->shapes[0].refMesh, "mesh_00000870.0", "proj:models/Soldier/mesh_00000870.0.nvx2")
    //N_REF_LOAD_TEXTURE(model->shapes[0].refDiffMap, "ns_forces_COLOR", "proj:models/Soldier/ns_forces_COLOR.DDS")
    //N_REF_LOAD_TEXTURE(model->shapes[0].refSpecMap, "ns_forces_SPEC", "proj:models/Soldier/ns_forces_SPEC.DDS")
    //N_REF_LOAD_SHADER(model->shapes[0].refShader, "ambient", "proj:shaders/ambient.fx")

    //N_REF_LOAD_MESH(model->shapes[1].refMesh, "mesh_00000870.1", "proj:models/Soldier/mesh_00000870.1.nvx2")
    //N_REF_LOAD_TEXTURE(model->shapes[1].refDiffMap, "head_01A_COLOR", "proj:models/Soldier/head_01A_COLOR.DDS")
    //N_REF_LOAD_TEXTURE(model->shapes[1].refSpecMap, "head_01A_SPEC", "proj:models/Soldier/head_01A_SPEC.DDS")
    //N_REF_LOAD_SHADER(model->shapes[1].refShader, "ambient", "proj:shaders/ambient.fx")

    //read the scene file
    nString sceneFile(path);
    sceneFile.StripTrailingSlash();
    nString filename = sceneFile.ExtractFileName();
    sceneFile.Append("/scene/");
    sceneFile.Append(filename.Get());
    sceneFile.Append(".n2");

    return LoadScene(model, sceneFile.Get());
}

//------------------------------------------------------------------------------

bool ModelsApp::LoadScene(Model* model, const char* path)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    int numShapes = 0;

    TextFile text(path);
    while (!text.Eof())
    {
        text.ReadLine();
        if (text.Contains("nshapenode"))
            numShapes++;

        if (text.Contains("setmesh"))
        {
            nString filename;
            text.Extract("[[", "]]", filename);
            nMesh2* mesh = gfxServer->NewMesh(filename.Get());
            if (!this->LoadResource(mesh, filename))
                return false;

            model->shapes.At(numShapes-1).refMesh = mesh;
        }

        if (text.Contains("setsurface"))
        {
            nString filename;
            if (text.Extract("[[", "]]", filename))
                this->LoadMaterial(&model->shapes.At(numShapes-1), filename.Get());
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool ModelsApp::LoadMaterial(Shape* shape, const char* path)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    //use a default shader
    shape->refShader = this->refShader.get();

    nShaderState::Param param(nShaderState::InvalidParameter);
    nTexture2* texture;

    TextFile text(path);
    while (!text.Eof())
    {
        text.ReadLine();
        nString filename;

        //texture maps
        if (text.Contains("diffMap"))
            param=nShaderState::DiffMap0;
        //else if (text.Contains("bumpMap"))
        //    param=nShaderState::BumpMap0;
        //else if (text.Contains("levelMap"))
        //    param=nShaderState::SpecMap0;
        else
            continue;

        text.Extract("[[", "]]", filename);//skip the parameter
        text.Extract("[[", "]]", filename);
        texture = gfxServer->NewTexture(filename.Get());
        if (!this->LoadResource(texture, filename))
            return false;

        shape->shaderParams.SetArg(param, texture);
        //other parameters...
        //shape->shaderParams[param].SetTexture(texture);
    }
    return true;
}

//------------------------------------------------------------------------------

void ModelsApp::ReleaseModel(Model* model)
{
    int numShapes = model->shapes.Size();
    for (int index=0; index<numShapes; index++)
        N_REF_RELEASE(model->shapes[index].refMesh);

    int numTextures = model->textures.Size();
    for (int index=0; index<numTextures; index++)
        N_REF_RELEASE(model->textures[index]);
}

//------------------------------------------------------------------------------

void ModelsApp::DrawModels()
{
    for (int index=0; index<this->models.Size(); index++)
    {
        Model* model = &this->models[index];

        //draw model
        int numShapes = model->shapes.Size();
        for (int index=0; index<numShapes; index++)
        {
            Shape& shape = model->shapes[index];
            shape.shaderParams.SetArg(nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid);

            nShader2* shader = shape.refShader;
            nMesh2* mesh = shape.refMesh;
            int numPasses = this->BeginDraw( shader, mesh );
            for (int passIndex = 0; passIndex < numPasses; passIndex++)
            {
                this->BeginPass( shader, passIndex );
                shader->SetParams( shape.shaderParams );

                this->Draw( matWorld );
                this->EndPass( shader );
            }
        }
    }
}
