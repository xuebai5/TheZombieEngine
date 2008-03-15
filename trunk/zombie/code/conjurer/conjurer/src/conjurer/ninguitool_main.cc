#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitool_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitool.h"
#include "kernel/nkernelserver.h"
#include "napplication/nappviewport.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTool, "nroot");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nInguiTool, RefreshInfo );

//------------------------------------------------------------------------------
/// Standard. framerate when applying a tool
const nTime nInguiTool::StdFrameRate = 1.0 / 60.0;

//------------------------------------------------------------------------------
/**
*/
nInguiTool::nInguiTool():
    previousViewport( 0 ),
    state( NotInited ),
    drawEnabled( false ),
    pickWhileIdle( false ),
    canApplyWhenRunningPhysics( false )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nInguiTool::~nInguiTool()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
    @param vp Viewport in wich the mouse pointer is located
    @return true if any input (keyboard, mouse or other) was handled

    Default implementation of HandleInput just returns false
*/
bool
nInguiTool::HandleInput( nAppViewport* /*vp*/ )
{
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
    @param vp Viewport in wich the mouse pointer is located
    @param ray The ray to do the pick, in world space
    @return The ray parameter of picking position (for ordering intersections), or -1 if couldn't pick.

    Default implementation of Pick just returns -1. When a pick was successful, a derived class must return
    a value >= 0

    Important: Every derived class should actualize the previousViewport field to the vp parameter at the end
    of the function, or simply call this class implementation ( nInguiTool::Pick() ), wich does basically that.
*/
float
nInguiTool::Pick( nAppViewport* vp, vector2 /*mp*/, line3 /*ray*/)
{
    this->previousViewport = vp;
    return -1.0f;
}

//------------------------------------------------------------------------------
/**
    @brief Apply the tool, given a world ray
    @param ray The ray
    
    Base class doesn't do anything. Subclasses may depend on more state data to apply the tool.
*/
bool
nInguiTool::Apply( nTime /*dt*/ )
{
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Draw the helper, given a viewport and a camera
    @param vp The viewport
    @param camera The camera
*/
void
nInguiTool::Draw( nAppViewport* /*vp*/, nCamera2* /*camera*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Get tool label
    @return Tool label
*/
nString
nInguiTool::GetLabel()
{
    return this->label;
}
//------------------------------------------------------------------------------
/**
    @brief Convert a mouse position to a world ray, given a viewport
    @param vp Viewport in wich the mouse pointer is located
    @param mousePos Device normalized mouse position (-1 to 1 in both axes)
    @param line The returned ray in world space
*/
void
nInguiTool::Mouse2Ray( nAppViewport* vp, vector2 mousePos, line3& ray)
{
    // Get camera transformation
    nCamera2 cam = vp->GetCamera();
    matrix44 cam2worldMatrix = vp->GetViewMatrix();

    // Calculate pos and dir of ray in world space
    vector3 pos = cam2worldMatrix.pos_component();
    
    if ( cam.GetType() == nCamera2::Perspective )
    {
        cam2worldMatrix.m[3][0] = 0.0f;
        cam2worldMatrix.m[3][1] = 0.0f;
        cam2worldMatrix.m[3][2] = 0.0f;

        vector3 dir = vector3( mousePos.x, mousePos.y, -1.0f);
        dir = cam.GetInvProjection() * dir;
        dir = cam2worldMatrix.transform_coord(dir);

        ray.set( pos, pos + dir );
    }
    else
    {
        vector3 dir = vector3( mousePos.x, mousePos.y, 0.0f);
        vector3 dir2 = dir;
        dir2.z = -1.0f;
        dir = cam.GetInvProjection() * dir;
        dir = cam2worldMatrix.transform_coord(dir);
        dir2 = cam.GetInvProjection() * dir2;
        dir2.z = -1.0f;
        dir2 = cam2worldMatrix.transform_coord(dir2);
        ray.set( dir, dir2 );
    }
}
//------------------------------------------------------------------------------
/**
    @brief Tell the size an object would have given its position and size in the screen
    @param vp Viewport
    @param pos World position of the object
    @param screenSize Object size in the screen (fraction) the user wants for the object. 
    @returns the size of the object

    The screenSize parameter is w.r. to the X or Y direction depending on the fov type of the camera (vertical / horizontal)
*/
float
nInguiTool::Screen2WorldObjectSize( nAppViewport* vp, vector3 pos, float screenSize )
{
    const nCamera2& cam = vp->GetCamera();
    if ( cam.GetType() == nCamera2::Perspective )
    {
        const matrix44& m = vp->GetViewMatrix();
        float c = 2.0f * tan( n_deg2rad( cam.GetAngleOfView() / 2.0f) );
        float d = ( pos - m.pos_component() ) % m.z_component();
        
        return abs( screenSize * c * d );
    }
    else
    {
        if ( cam.GetFovType() == nCamera2::Hor )
        {
            return cam.GetWidth() * screenSize;
        }
        else
        {
            return cam.GetHeight() * screenSize;
        }
    }
}
//------------------------------------------------------------------------------
/**
    @brief Tells if can be used when running physics simulation
*/
bool
nInguiTool::CanApplyWhenRunningPhysics()
{
    return this->canApplyWhenRunningPhysics;
}

//------------------------------------------------------------------------------
/**
    @brief Called when tool has been selected
*/
void
nInguiTool::OnSelected()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Called when tool has been unselected
*/
void
nInguiTool::OnDeselected()
{
    // empty
}
