#include "transform/transformapp.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nmeshgroup.h"
#include "util/nrandomlogic.h"

//------------------------------------------------------------------------------
/**
    run script that loads required resources, etc.
	and sets initial position of everything
*/
void TransformApp::Init()
{
    this->bWireframe = false;

    this->transformMode = Translate;

    this->vecEye.set(0,5,10);

    this->vecPosition.set( 0.f, 1.f, 0.f );
    this->vecRotation.set( 0.f, 0.f, 0.f );
    this->vecScale.set( 1.f, 1.f, 1.f );

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

    this->refShader = gfxServer->NewShader("diffuse");
    if (!this->LoadResource(this->refShader, "proj:shaders/diffuse.fx"))
        return false;

    nMeshGroup& group = this->refMesh->Group(0);
    this->bbox = group.GetBoundingBox();

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

    if (inputServer->GetButton("reset"))
        this->transformMode = ((int)this->transformMode + 1) % Max_TransformModes;

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

    //left click on the object to transform
    //1- transform the click (x,y) into a line in 3D
    //2- does the line go through the bounding box of the object
    //3- if so, select it, otherwise don't.

    //Once the object is selected
    //translate:
    //left-click + up/down, left/right: translate along the view plane
    //camera look around
    if (inputServer->GetButton("left_pressed"))
    {
        switch (this->transformMode)
        {
        case Translate:
            {
                //mouse_x, mouse_y contain the distance for the mouse *in clip space*
                //we need to convert these to the scale of the object
                this->vecPosition.y += mouse_y; //up/down: translate on Y
                vector3 x_axis = mat.x_component();
                x_axis *= -1.f;
                x_axis.y = 0.f;
                this->vecPosition += x_axis * mouse_x;
            }
            break;

        case Rotate:
            this->vecRotation.y += mouse_x * n_deg2rad(30) * -1.f;
            //TODO- x,z rotation
            break;

        case Scale:
            this->vecScale += vector3( 1.f,1.f,1.f ) * mouse_y * .5f;//scale factor
            break;
        }
    }
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
    this->matWorld.ident();
    this->matWorld.scale( this->vecScale );
    this->matWorld.rotate_x( this->vecRotation.x );
    this->matWorld.rotate_y( this->vecRotation.y );
    this->matWorld.rotate_z( this->vecRotation.z );
    this->matWorld.translate( this->vecPosition );//pitch

    this->BeginDraw( this->refShader, this->refMesh );
    this->BeginPass( this->refShader, 0 );
    this->refShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShader->SetTexture( nShaderState::DiffMap0, this->refTexture );
    this->Draw( matWorld );
    this->EndPass( this->refShader );
    this->EndDraw( this->refShader );

    //draw the object bounding box using 3d lines
    static nArray<vector3> vertices;
    vertices.Reset();
    vertices.Append( this->bbox.corner_point(0) );
    vertices.Append( this->bbox.corner_point(1) );
    vertices.Append( this->bbox.corner_point(2) );
    vertices.Append( this->bbox.corner_point(3) );
    vertices.Append( this->bbox.corner_point(0) );

    vertices.Append( this->bbox.corner_point(6) );
    vertices.Append( this->bbox.corner_point(7) );
    vertices.Append( this->bbox.corner_point(4) );
    vertices.Append( this->bbox.corner_point(5) );
    vertices.Append( this->bbox.corner_point(6) );

    //5,1,2,4,7,3
    vertices.Append( this->bbox.corner_point(5) );
    vertices.Append( this->bbox.corner_point(1) );
    vertices.Append( this->bbox.corner_point(2) );
    vertices.Append( this->bbox.corner_point(4) );
    vertices.Append( this->bbox.corner_point(7) );
    vertices.Append( this->bbox.corner_point(3) );

    gfxServer->BeginLines();
    gfxServer->DrawLines3d( vertices.Begin(), vertices.Size(), vector4(1.f,0.f,0.f,1.f) );
    gfxServer->EndLines();

    //draw the floor
    this->BeginDraw( this->refShader, this->refFloorMesh );
    this->BeginPass( this->refShader, 0 );
    this->refShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShader->SetTexture( nShaderState::DiffMap0, this->refFloorTexture );
    this->Draw( vector3( -5.f, 0.f, -5.f ), vector3( 10.f, 0.f, 10.f ) );
    this->EndPass( this->refShader );
    this->EndDraw( this->refShader );

    //draw text
    float rowheight = 32.f / gfxServer->GetDisplayMode().GetHeight();
    nString str;

    switch (this->transformMode)
    {
    case Translate:
        str = "Translate";
        break;
    case Rotate:
        str = "Rotate";
        break;
    case Scale:
        str = "Scale";
        break;
    }

    gfxServer->Text( str.Get(), vector4(1.f,1.f,0,1), -1.f, 1.f - rowheight );
}
