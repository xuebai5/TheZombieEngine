#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolrotation.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolrotation.h"
#include "napplication/nappviewport.h"
#include "kernel/nkernelserver.h"
#include "mathlib/triangle.h"

nNebulaScriptClass(nInguiToolRotation, "ninguitooltransform");

//------------------------------------------------------------------------------
/**
*/
nInguiToolRotation::nInguiToolRotation():
    angle( 0.0f ),
    rotationFactor( 100.0f )
{
    label.Set("Rotation");
    this->lastPickedAxis = axisX;
    nInguiTool::canApplyWhenRunningPhysics = true;
    nInguiToolTransform::hasCombinedAxisHandlers = false;
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolRotation::~nInguiToolRotation()
{
    // empty
}
//------------------------------------------------------------------------------   
/**
    @brief Picking of rotation tool

    It handles the picking when the tool is not inactive. If not, it calls the upper class Pick method.

    This one calculates an angle of rotation depending on the initial and current picking position.
    It has no combined axis handlers.

*/
float
nInguiToolRotation::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{

    float t = 1.1f;

    // The pick is done with matrix0 but we need to scale it
    matrix44 matrix0scaled;

    // Do picking to select arrow (rotation axis)
    if ( this->state <= nInguiTool::Inactive )
    {
        t = nInguiToolTransform::Pick( vp, mp, ray );

        if ( this->state <= nInguiTool::NotInited && t < 0.0f )
        {
            return -1.0f;
        }

        matrix0scaled = this->matrix0;
        matrix0scaled.scale( vector3(this->size, this->size, this->size ) );
    }

    if ( this->lastPickedAxis == axisNone )
    {
        return -1.0f;
    }

    // Do picking for the angle of rotation
    matrix0scaled = this->matrix0;
    matrix0scaled.scale( vector3(this->size, this->size, this->size ) );

    vector3 pos = this->matrix1.pos_component();

    vector3 intersPoint;

    plane p;

    vector3 arrow = pos + GetAxisVector( this->lastPickedAxis, matrix0scaled );
    vector3 vnorm = arrow - ray.b;
    vnorm.norm();
    p.set(vnorm.x, vnorm.y, vnorm.z, - vnorm % arrow);

    if ( !p.intersect( ray, t ) )
    {
        // Invert plane if facing backwards
        vector3 n = p.normal();
        p.set( n.x, n.y, n.z, n % pos);
        if ( !p.intersect( ray, t ) )
        {
            t = -1.0f;
        }
    }

    this->lastPos = ray.ipol ( t );


    if ( t > 0.0f && this->lastPickedAxis != axisNone)
    {
        vector3 pos = this->translation0;

        vector3 planeNormal = GetAxisVector( this->lastPickedAxis, matrix0scaled );

        // picking is done with a plane parallel to the viewport
        vector3 arrow = pos + planeNormal;
        vector3 vLast = this->lastPos - arrow;
        vector3 axisNormalDir1 = vp->GetViewMatrix().y_component();

        this->angle = ( axisNormalDir1 % vLast ) / this->size;

        /// Apply rotation factor
        this->angle *= this->rotationFactor;

        // If snap value is not 0, apply it
        if ( this->snapValue != 0.0f )
        {
            this->angle = snapValue * int( this->angle / snapValue );
        }

        // If it's the first pick, store the initial angle
        if ( this->state <= nInguiTool::Inactive )
        {
            this->initialAngle = this->angle;
            this->angle = 0.0f;
        }
        else
        {
            this->angle -= this->initialAngle;
        }

        // Once the angle is calculated, perform rotation
        matrix44 m;
        planeNormal.norm();
        m.rotate( planeNormal, n_deg2rad( this->angle ) );
        
        // Store matrix for transforming objects
        // The operation matrix
        this->relMatrix.setquatrotation( m.get_quaternion() );

        // The world matrix
        objMatrix = this->matrix0 * m;
        this->objMatrix.set_translation( this->translation0 );

        // Store matrix for pick & draw
        this->matrix1 = matrix0scaled * m;
        this->matrix1.set_translation( this->translation0 );
    }
    else
    {
        this->angle = 0.0f;
    }
    
    this->previousViewport = vp;

    return t;
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiToolRotation::Draw( nAppViewport* vp, nCamera2* camera )
{
    nInguiToolTransform::Draw(vp, camera);
}
//------------------------------------------------------------------------------
float
nInguiToolRotation::GetCurrentRotAngle()
{
    return this->angle;
}
//------------------------------------------------------------------------------
void
nInguiToolRotation::SetRotationFactor( float f )
{
    this->rotationFactor = f;
}
//------------------------------------------------------------------------------
/**
    @brief Get relative transform matrix for a vector
    @param f Factor for the transform, if snap is not used
    @param v Vector describing the transform in some axis
    @param m Matrix to fill with the transform
*/
void
nInguiToolRotation::GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType axis, transform44& t )
{
    // If snap value is not 0, apply it
    if ( this->snapValue != 0.0f )
    {
        if ( f < 0.0f )
        {
            f = - snapValue;
        }
        else
        {
            f = snapValue;
        }
    }

    vector3 planeNormal = GetAxisVector( axis, this->matrix0 );
    planeNormal.norm();
    matrix44 m;
    m.rotate( planeNormal, n_deg2rad( f ) );
    t.setquatrotation( m.get_quaternion() );    
}

//------------------------------------------------------------------------------