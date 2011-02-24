#include "cameras/camerasapp.h"

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
void CamerasApp::Init()
{
    this->bWireframe = false;
    this->bShowAxes = false;

    this->cameraMode = FreeCam;

    this->vecEye.set(0,5,10);  

    //FreeCam
    //Pitch: X-rot, Yaw: Y-rot, Roll: Z-rot
    this->vecRot.set(n_deg2rad(-30),0,0); //looking down 30 degrees

    //LookAt
    this->vecAt.set(0,0,0);
    this->vecUp.set(0,1,0);
}

//------------------------------------------------------------------------------
/**
*/
bool CamerasApp::Open()
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
    if (!this->LoadResource(this->refShader, "proj:shaders/diffuse.fx"))
        return false;

    return true;
}

//------------------------------------------------------------------------------

void CamerasApp::Close()
{
    N_REF_RELEASE(this->refMesh);
    N_REF_RELEASE(this->refFloorMesh);
    N_REF_RELEASE(this->refTexture);
    N_REF_RELEASE(this->refFloorTexture);
    N_REF_RELEASE(this->refShader);
}

//------------------------------------------------------------------------------

void CamerasApp::Tick( float fTimeElapsed )
{
    nInputServer* inputServer = nInputServer::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    if (inputServer->GetButton("wireframe"))
        this->bWireframe = !this->bWireframe;

    if (inputServer->GetButton("axes"))
        this->bShowAxes = !this->bShowAxes;

    if (inputServer->GetButton("reset"))
    {
        this->cameraMode = (CameraMode)((this->cameraMode + 1) % Max_CameraModes);

        //hide the cursor in first person camera
        if (this->cameraMode == FirstPerson)
        {
            gfxServer->SetCursorVisibility( nGfxServer2::None );
        }
        else
        {
            gfxServer->SetCursorVisibility( nGfxServer2::System );
        }

        //initialize the offset to its current value
        if (this->cameraMode == ThirdPerson)
        {
            //FIXME- initialize the rotation and offset vector from the current offset
            this->vecRot.set(0,0,0);
            this->vecFrom = this->vecEye - this->vecAt;
        }
    }

    float mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
    float mouse_y = (inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down"));

    float angleSpace = n_deg2rad(360.f) * 10.f * fTimeElapsed;//=angleSpeed
    float moveSpace = 10.f * fTimeElapsed;//=cameraSpeed

    switch (cameraMode)
    {
    case FreeCam:
    case FirstPerson:
    case ThirdPerson:
        //camera look around
        if (cameraMode == FirstPerson || inputServer->GetButton("right_pressed"))
        {
            this->vecRot.y += mouse_x * angleSpace;
            this->vecRot.x += mouse_y * angleSpace;
        }
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

    if (cameraMode == LookAt)
    {
        if (inputServer->GetButton("right_pressed"))
        {
            vecMove.x += mouse_x * moveSpace * 20.f;
            vecMove.y -= mouse_y * moveSpace * 20.f;
        }
    }

    matrix44 mat;
    switch (cameraMode)
    {
    case FreeCam:
    case FirstPerson:

        //update camera position
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

        break;

    case ThirdPerson:
        mat.rotate_x( this->vecRot.x );
        mat.rotate_y( this->vecRot.y );
        mat.translate( this->vecAt );

        this->vecAt = mat * vecMove;
        this->vecAt.y = 0.f;

        this->vecEye = this->vecAt + this->vecFrom;
        
        break;

    case LookAt:
        mat.ident();
        mat.translate( this->vecEye );
        mat.lookatRh( this->vecAt, this->vecUp );

        this->vecEye = mat * vecMove;

        break;
    }
}

//------------------------------------------------------------------------------

void CamerasApp::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->Clear( nGfxServer2::AllBuffers, .3f, .3f, .3f, 1.f, 1.f, 0 );

    switch (this->cameraMode)
    {
    case LookAt:
    case ThirdPerson:
        this->matView.ident();
        this->matView.translate( this->vecEye );
        this->matView.lookatRh( this->vecAt, this->vecUp );
        this->matView.invert_simple();
        break;

    case FreeCam:
    case FirstPerson:
        this->matView.ident();
        this->matView.rotate_x( this->vecRot.x );//pitch
        this->matView.rotate_y( this->vecRot.y );//yaw
        this->matView.translate( this->vecEye );
        this->matView.invert_simple();
    }

    gfxServer->SetTransform( nGfxServer2::View, this->matView );

    nCamera2 cam;
    gfxServer->SetCamera( cam );

    //draw the torus
    this->BeginDraw( this->refShader, this->refMesh );
    this->BeginPass( this->refShader, 0 );
    this->refShader->SetInt( nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid );
    this->refShader->SetTexture( nShaderState::DiffMap0, this->refTexture );
    vector3 vecScale( 1.f, 1.f, 1.f );
    vector3 vecPosition( 0.f, 1.f, 0.f );
    vecPosition += this->vecAt; //for third person camera
    this->Draw( vecPosition, vecScale );
    this->EndPass( this->refShader );
    this->EndDraw( this->refShader );

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
    float textY = -1.f;
    nString str;
    str.Format("Position: (x=%.2f,y=%.2f,z=%.2f)", vecEye.x, vecEye.y, vecEye.z);
    gfxServer->Text(str.Get(), vector4(1.f,0,0,1), -1.f, textY );
    textY += rowheight;
    switch (this->cameraMode)
    {
    case FreeCam:
    case FirstPerson:
    case ThirdPerson:
        str.Format("Rotation: (pitch=%.2f,yaw=%.2f,roll=%.2f)", vecRot.x, vecRot.y, vecRot.z);
        gfxServer->Text(str.Get(), vector4(1.f,0,0,1), -1.f, textY );
        textY += rowheight;
        break;

    }

    switch (this->cameraMode)
    {
    case ThirdPerson:
    case LookAt:
        str.Format("LookAt: (x=%.2f,y=%.2f,z=%.2f)", vecAt.x, vecAt.y, vecAt.z);
        gfxServer->Text(str.Get(), vector4(1.f,0,0,1), -1.f, textY );
        textY += rowheight;
        str.Format("LookUp: (x=%.2f,y=%.2f,z=%.2f)", vecUp.x, vecUp.y, vecUp.z);
        gfxServer->Text(str.Get(), vector4(1.f,0,0,1), -1.f, textY );
        textY += rowheight;
        break;
    }

    switch (this->cameraMode)
    {
    case FreeCam:
        str = "Free Camera";
        break;
    case FirstPerson:
        str = "First Person Camera";
        break;
    case ThirdPerson:
        str = "Third Person Camera";
        break;
    case LookAt:
        str = "Look-at Camera";
        break;
    }

    gfxServer->Text( str.Get(), vector4(1.f,1.f,0,1), -1.f, 1.f - rowheight );

    //draw onscreen axes
    if (this->bShowAxes)
    {
        const nDisplayMode2& displayMode = gfxServer->GetDisplayMode();
        const matrix44& view = gfxServer->GetTransform(nGfxServer2::View);
        const matrix44& proj = gfxServer->GetTransform(nGfxServer2::Projection);
        const nViewport& viewport = gfxServer->GetViewport();

        static vector4 color[3] = { vector4(1.0f, 0.0f, 0.0f, 1.0f),
                                    vector4(0.0f, 1.0f, 0.0f, 1.0f),
                                    vector4(0.0f, 0.0f, 1.0f, 1.0f) };

        static char* buf[3] = { "x", "y" ,"z" };

        rectangle rect;

        matrix44 matModel;
        matModel.scale( vector3(20, -30, 0) );
        matModel.translate( vector3(20, viewport.height - 30, 0) );

        vector3 offset(0.f, 0.f, -2.01f); //projected origin, Z negative goes into the screen
        vector3 point = matModel * offset;

        vector2 points[2];
        points[0].set(point.x, point.y);

        gfxServer->BeginLines();

        for (int idx=0; idx<3; idx++)
        {
            vector3 axis( view.m[idx][0], view.m[idx][1], view.m[idx][2] ); //get axes from view matrix
            axis.norm();
            axis += offset;
            point = proj.transform_coord(axis); //transform vector, projecting back into w=1
            point = matModel * point;
            points[1].set(point.x, point.y);
            gfxServer->DrawLines2d(points, 2, color[idx]);

            rect.set(vector2((viewport.x + points[1].x) / displayMode.GetWidth(),
                             (viewport.y + points[1].y) / displayMode.GetHeight()),
                     vector2(1.0f, 1.0f));
            gfxServer->DrawText(buf[idx], color[idx], rect, 0);
        }
        gfxServer->EndLines();
    }
}
