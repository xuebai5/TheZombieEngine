#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  neditorviewport_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditorviewport.h"
#include "conjurer/nviewportdebugmodule.h"
#include "input/ninputserver.h"
#include "nspatial/ncspatialcamera.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
/**
    Handle general input
*/
bool
nEditorViewport::HandleInput(nTime frameTime)
{   
    nInputServer* inputServer = nInputServer::Instance();

    // toggle perspective / isometric view
    if (inputServer->GetButton("toggle_isometric"))
	{
        this->SetIsometricView(!this->GetIsometricView());
    }
    
    // @todo with the three standard views: 
    // Calculate center of scene and extents (perhaps with spatial spaces) and set viewer position and zoom

    // set top view
    if (inputServer->GetButton("view_top"))
	{
        this->SetViewerAngles(-PI / 2.0f, 0.0f);
        this->SetIsometricView(true);
    }

    // set left view
    if (inputServer->GetButton("view_left"))
	{
        this->SetViewerAngles(0.0f, - PI / 2.0f);
        this->SetIsometricView(true);
    }

    // set right view
    if (inputServer->GetButton("view_right"))
	{
        this->SetViewerAngles(0.0f, PI / 2.0f);
        this->SetIsometricView(true);
    }

    // set front view
    if (inputServer->GetButton("view_front"))
	{
        this->SetViewerAngles(0.0f, 0.0f);
        this->SetIsometricView(true);
    }

    // set back view
    if (inputServer->GetButton("view_back"))
	{
        this->SetViewerAngles(0.0f, PI);
        this->SetIsometricView(true);
    }

    // mouse wheel (zoom/velocity)
    // zoom in
    if (inputServer->GetButton("zoom_in"))
	{
        nCamera2 camera = this->GetCamera();

        if (camera.GetType() == nCamera2::Perspective)
        {
            this->SetAngleOfView(this->GetAngleOfView() - 1.0f);
        }
        else
        {
            this->SetViewerZoom(this->GetViewerZoom() * 0.9f);
        }
	}

    // zoom out
    if (inputServer->GetButton("zoom_out"))
	{
        nCamera2 camera = this->GetCamera();

        if (camera.GetType() == nCamera2::Perspective)
        {
            this->SetAngleOfView(this->GetAngleOfView() + 1.0f);
        }
        else
        {
            this->SetViewerZoom(this->GetViewerZoom() * 1.1f);
        }
	}

    // velocity
    if (inputServer->GetButton("wheel_velocity_minus") || inputServer->GetButton("velocity_minus"))
    {
        this->SetViewerVelocity(this->GetViewerVelocity() / 1.5f);
        if ( this->refDebugModule.isvalid() )
        {
            this->refDebugModule->SetTimeToDrawCameraVelocity( 0.8f );
        }
    }

    if (inputServer->GetButton("wheel_velocity_plus") || inputServer->GetButton("velocity_plus"))
    {
        this->SetViewerVelocity(this->GetViewerVelocity() * 1.5f);
        if ( this->refDebugModule.isvalid() )
        {
            this->refDebugModule->SetTimeToDrawCameraVelocity( 0.8f );
        }
    }

    this->HandleInputGameCam(frameTime);

    this->refDebugModule->HandleInput(frameTime);

    if (this->refLightEntity.isvalid())
    {
        if (inputServer->GetButton("set_lightpos") || this->moveLight)
        {
            ncTransform *lightTransform = this->refLightEntity->GetComponentSafe<ncTransform>();
            lightTransform->SetPosition(this->GetViewerPos());
            lightTransform->SetPolar(this->GetViewerAngles());
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Handle input for the  Navigator Game Cam model.
*/
bool
nEditorViewport::HandleInputGameCam(nTime frameTime)
{
    nInputServer* inputServer = nInputServer::Instance();

    polar2 viewerAngles;
    this->GetViewerAngles(viewerAngles.theta, viewerAngles.rho);

    bool moved = false;
    bool checkAngles = false;

    if (frameTime <= 0.0001f)
    {
        frameTime = 0.0001f; // in seconds
    }

    float space = this->GetViewerVelocity() * (float) frameTime;
    float angularSpace = n_deg2rad(30.0f) * (float) frameTime; // turn 90 per second

    float Mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
    float Mouse_y = (inputServer->GetSlider("slider_up") - inputServer->GetSlider("slider_down"));
    Mouse_x *= this->mouseXfactor;
    Mouse_y *= this->mouseYfactor;

    if (inputServer->GetButton("buton1"))
	{
        viewerAngles.rho += Mouse_x * n_deg2rad(9.0f);
        viewerAngles.theta += Mouse_y * n_deg2rad(9.0f);
        checkAngles = true;
	}

    if (inputServer->GetButton("pitch_up"))
	{
        viewerAngles.theta -= angularSpace;
        checkAngles = true;
	}

    if (inputServer->GetButton("pitch_down"))
	{
        viewerAngles.theta += angularSpace;
        checkAngles = true;
	}

    if (inputServer->GetButton("turn_left"))
	{
        viewerAngles.rho += angularSpace;
        checkAngles = true;
	}

    if (inputServer->GetButton("turn_right"))
	{
        viewerAngles.rho -= angularSpace;
        checkAngles = true;
	}

    if (checkAngles)
    {
        while (viewerAngles.rho < 0.0f)
        {
            viewerAngles.rho += 2.0f * PI;
        }

        while (viewerAngles.rho > 2.0f * PI)
        {
            viewerAngles.rho -= 2.0f * PI;
        }

        if (viewerAngles.theta < n_deg2rad(-70.0f))
        {
            viewerAngles.theta = n_deg2rad(-70.0f);
        }
        if (viewerAngles.theta > n_deg2rad(70.0f))
        {
            viewerAngles.theta = n_deg2rad(70.0f);
        }
        moved = true;
    }

    // compute camera movement
    vector3 camera;

	if (inputServer->GetButton("buton2"))
	{
		camera += vector3(-Mouse_x * 0.01f, Mouse_y * 0.01f, 0.0f);
	}

    if (inputServer->GetButton("strafe_up"))
    {
        camera.y += space;        
	}

    if (inputServer->GetButton("strafe_down"))
    {
        camera.y -= space;        
	}

	if (inputServer->GetButton("up"))
	{
        camera.z -= space;
	}

	if (inputServer->GetButton("down"))
	{
		camera.z +=  space;
	}

	if (inputServer->GetButton("right"))
	{
		camera.x += space;
	}

	if (inputServer->GetButton("left"))
	{
		camera.x -= space;
	}

    matrix44 tmpMatrix;
    if (!camera.isequal(vector3(), 0.0f))
    {
        tmpMatrix.translate(camera);
        moved = true;
    }

    // update camera transform when moved
    // FIXME update position or angles ONLY
    if (moved)
    {
        vector3 oldPosition = this->GetViewerPos();
        vector3 newPosition;

        tmpMatrix = tmpMatrix * this->GetViewMatrix(); // matrix before update!
        newPosition = tmpMatrix.pos_component();
        
        this->SetViewerPos(newPosition);
        this->SetViewerAngles(viewerAngles.theta, viewerAngles.rho);
    }

    if (inputServer->GetButton("recalculate_campos") && this->refSpatialCamera.isvalid())
    {
        ncSpatialCamera* spatialCamera = this->refSpatialCamera->GetComponent<ncSpatialCamera>();
        if (spatialCamera && spatialCamera->IsOn())
        {
            // force recalculate the camera's position
            spatialCamera->SetCell(NULL);
        }
    }

    return moved;
}
