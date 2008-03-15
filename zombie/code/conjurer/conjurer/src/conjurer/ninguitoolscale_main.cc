#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolscale_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolscale.h"
#include "napplication/nappviewport.h"

nNebulaScriptClass(nInguiToolScale, "ninguitooltransform");

//------------------------------------------------------------------------------
/**
*/
nInguiToolScale::nInguiToolScale():
    scale( 0.0f ),
    scaleFactor( 3.0f )
{
    nInguiToolTransform::hasCombinedAxisHandlers = false;
    label.Set("Scale");
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolScale::~nInguiToolScale()
{
    // empty
}

//------------------------------------------------------------------------------   
/**
*/
float
nInguiToolScale::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{
    this->lastFramePicked = true;

    float t = nInguiToolTransform::Pick( vp, mp, ray );
    if ( t > 0.0f)
    {
        matrix44 matrix0scaled = this->matrix0;
        matrix0scaled.scale( vector3(this->size, this->size, this->size ) );

        // picking is done with a plane parallel to the viewport
        vector3 planeNormal = GetAxisVector( nInguiToolTransform::axisX, matrix0scaled );

        vector3 arrow = this->translation0 + planeNormal;
        vector3 vLast = this->lastPos - arrow;
        vector3 axisNormalDir1 = vp->GetViewMatrix().y_component();

        this->scale = min( 10000.0f, pow( this->scaleFactor, ( axisNormalDir1 % vLast ) / this->size ) );

        // If snap value is not 0, apply it
        if ( this->snapValue != 0.0f )
        {
            this->scale = snapValue * int( this->scale / this->snapValue );
            if ( this->scale < this->snapValue )
            {
                this->scale = this->snapValue;
            }
        }

        // If it's the first pick, store the initial scale
        if ( this->state <= nInguiTool::Inactive )
        {
            if ( abs( this->scale ) < TINY )
            {
                return -1.0f;
            }
            this->initialScale = this->scale;
        }
        this->scale /= this->initialScale;

        // Update matrices

        vector3 sv(1.0f, 1.0f, 1.0f);
        if ( this->transformObject && this->transformObject->GetEntity() &&
            this->transformObject->GetEntity()->IsA("neoccluder"))
        {
            if ( this->lastPickedAxis == axisX )
            {
                sv.x *= this->scale / this->matrix0.x_component().len();
            }
            else if ( this->lastPickedAxis == axisY )
            {
                sv.y *= this->scale / this->matrix0.y_component().len();
            }
            else if ( this->lastPickedAxis == axisZ )
            {
                sv.z *= this->scale / this->matrix0.z_component().len();
            }
        }
        else
        {
            float s = this->matrix0.x_component().len();
            n_assert( s > TINY );
            sv *= this->scale / s;
        }

        this->relMatrix.setscale( sv );

//        this->objMatrix = this->matrix0;
//        this->objMatrix.scale( sv );

        this->matrix1 = this->objMatrix;
        this->matrix1.scale( vector3(1.0f, 1.0f, 1.0f) * this->size );

        this->objMatrix.set_translation( this->translation0 );
        this->matrix1.set_translation( this->translation0 );

        return true;
    }

    return -1.0f;
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiToolScale::Draw( nAppViewport* vp, nCamera2* camera )
{
    //@todo, implement this with more good looking graphics
    nInguiToolTransform::Draw(vp, camera);
}
//------------------------------------------------------------------------------
float
nInguiToolScale::GetCurrentScaling()
{
    return this->scale;
}
//------------------------------------------------------------------------------
void
nInguiToolScale::SetScaleFactor( float s )
{
   this->scaleFactor = s;
}
//------------------------------------------------------------------------------
/**
    @brief Get relative transform matrix for a vector
    @param f Factor for the transform, if snap is not used
    @param v Vector describing the transform in some axis
    @param m Matrix to fill with the transform
*/
void
nInguiToolScale::GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType, transform44& t )
{
    // Scale with snap value or factor
    if ( this->snapValue > 0.0f )
    {
        if ( f < 0.0f )
        {
            f = 1 / ( this->snapValue + 1.0f );
        }
        else
        {
            f = 1.0f + this->snapValue;
        }
    }
    else
    {
        if ( f < 0.0f )
        {
            f = - 1 / f;
        }
    }
    t.setscale( vector3(f, f, f) );
}
//------------------------------------------------------------------------------