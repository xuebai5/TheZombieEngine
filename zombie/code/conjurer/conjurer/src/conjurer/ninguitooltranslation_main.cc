#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitooltranslation_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitooltranslation.h"
#include "kernel/nkernelserver.h"
#include "mathlib/triangle.h"

nNebulaScriptClass(nInguiToolTranslation, "ninguitooltransform");

//------------------------------------------------------------------------------
/**
*/
nInguiToolTranslation::nInguiToolTranslation()
{
    nInguiToolTransform::hasCombinedAxisHandlers = true;
    label.Set("Translation");
    nInguiTool::canApplyWhenRunningPhysics = true;
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolTranslation::~nInguiToolTranslation()
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
float
nInguiToolTranslation::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{

    bool firstPick = this->IsFirstPick( vp );

    float t = nInguiToolTransform::Pick( vp, mp, ray );
    if ( t > 0.0f)
    {
        // V is final translation in global coordinates
        vector3 v = this->lastPos;// - this->pickOffset;
        ClampPosition( v, this->lastPickedAxis );
        if ( this->refGrid.isvalid() )
        {
            v = this->refGrid->SnapPosition( v );

            // If first pick, convert also the first position
            if ( firstPick )
            {
                this->firstPos = this->refGrid->SnapPosition( this->firstPos );
            }

        }
        this->lastPos = v;

        //Update matrices
        this->matrix1.set_translation( v );
        this->objMatrix.set_translation( v );

        vector3 v2 = v - translation0;
        
        // This converts the global position v2 to matrix0 space
        vector3 x = matrix0.x_component();
        vector3 y = matrix0.y_component();
        vector3 z = matrix0.z_component();
        x.norm();
        y.norm();
        z.norm();
        v2 = x * (x % ( v2 ) ) + 
             y * (y % ( v2 ) ) + 
             z * (z % ( v2 ) );

        this->relMatrix.settranslation( v2 );

        return true;
    }
    else
    {
        return -1.0f;
    }
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiToolTranslation::Draw( nAppViewport* vp, nCamera2* camera )
{
    nInguiToolTransform::Draw(vp, camera);

    // If no transform object assigned, exit
    if ( ! this->transformObject )
    {
        return;
    }

    // Draw combined axis handlers
    this->DrawCombinedAxis();
}
//------------------------------------------------------------------------------
/**
    @brief Set path for editor grid object
*/
void
nInguiToolTranslation::SetGridPath( nString path )
{
    this->refGrid.set(path.Get());
}
//------------------------------------------------------------------------------
/**
*/
vector3
nInguiToolTranslation::GetCurrentTranslation()
{
    return this->lastPos - this->firstPos;
}
//------------------------------------------------------------------------------
/**
*/
vector3
nInguiToolTranslation::GetCurrentWorldPosition()
{
    return this->lastPos;
}

//------------------------------------------------------------------------------
/**
    @brief Get relative transform matrix for a vector
    @param f Factor for the transform, if snap is not used
    @param v Vector describing the transform in some axis
    @param m Matrix to fill with the transform
*/
void
nInguiToolTranslation::GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType axis, transform44& t )
{

    bool negative = f < 0.0f;

    vector3 v = this->GetAxisVector( axis, this->matrix0 ) + this->matrix0.pos_component();

    // Apply grid. If it's activated, multiply the vector v by the grid snap

    if ( this->refGrid->GetSnapEnabled() )
    {
        this->refGrid->SnapPosition( v );
    }
    else
    {
        // Apply the transform factor
        v*= abs( f );
    }

    if ( negative )
    {
        v = -v;
    }

    // Return transform with translation v
    t.settranslation( v );

}
//------------------------------------------------------------------------------
