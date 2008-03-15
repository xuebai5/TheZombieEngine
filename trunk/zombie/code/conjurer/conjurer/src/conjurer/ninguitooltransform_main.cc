#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitooltransform_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitooltransform.h"
#include "kernel/nkernelserver.h"
#include "mathlib/triangle.h"
#include "conjurer/nobjecteditorstate.h"
#include "conjurer/ninguitoolselection.h"
#include "input/ninputserver.h"

nNebulaScriptClass(nInguiToolTransform, "ninguitool");


nMesh2* nInguiToolTransform::meshAxisX = 0;
nMesh2* nInguiToolTransform::meshAxisY = 0;
nMesh2* nInguiToolTransform::meshAxisZ = 0;
nMesh2* nInguiToolTransform::meshOrigin = 0;
nShader2* nInguiToolTransform::axisShader;
//------------------------------------------------------------------------------
/**
*/
nInguiToolTransform::nInguiToolTransform():
    size( 1.0f ),
    hasCombinedAxisHandlers( true ),
    snapValue( 0.0f ),
    inWorldMode( false ),
    lastFramePicked( false ),
    lastPickedAxis( axisY ),
    lastAxisMouseOver( axisNone ),
    leftButtonPressed( true )
{
    const char * nameAxisX = "wc:libs/system/meshes/AxisX.nvx2";
    const char * nameAxisY = "wc:libs/system/meshes/AxisY.nvx2";
    const char * nameAxisZ = "wc:libs/system/meshes/AxisZ.nvx2";
    const char * nameOrigin = "wc:libs/system/meshes/Origin.nvx2";
    const char * shaderPath = "home:data/shaders/2.0/axis.fx";

    // Load mesh objects for drawing
    meshAxisX = nGfxServer2::Instance()->NewMesh( nameAxisX );
    meshAxisX->SetFilename( nameAxisX );
    meshAxisX->Load();
    n_assert(meshAxisX->IsValid());

    meshAxisY = nGfxServer2::Instance()->NewMesh( nameAxisY );
    meshAxisY->SetFilename( nameAxisY );
    meshAxisY->Load();
    n_assert(meshAxisY->IsValid());

    meshAxisZ = nGfxServer2::Instance()->NewMesh( nameAxisZ );
    meshAxisZ->SetFilename( nameAxisZ );
    meshAxisZ->Load();
    n_assert(meshAxisZ->IsValid());
    
    meshOrigin = nGfxServer2::Instance()->NewMesh( nameOrigin );
    meshOrigin->SetFilename( nameOrigin );
    meshOrigin->Load();
    n_assert(meshOrigin->IsValid());

    // Load shader
    axisShader = nGfxServer2::Instance()->NewShader( shaderPath );
    axisShader->SetFilename( shaderPath );
    axisShader->Load();
    n_assert( axisShader->IsValid() );


    this->pickWhileIdle = true;
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolTransform::~nInguiToolTransform()
{
/*    meshAxisX->Release();
    meshAxisY->Release();
    meshAxisZ->Release();
    meshOrigin->Release();
    axisShader->Release();
*/
}
//------------------------------------------------------------------------------
/**
    @brief Get snap value
*/
float
nInguiToolTransform::GetSnap()
{
    return this->snapValue;
}
//------------------------------------------------------------------------------
/**
    @brief Set snap value
*/
void
nInguiToolTransform::SetSnap(float value)
{
    this->snapValue = value;
}
//------------------------------------------------------------------------------
/**
    @brief Clamp a position to the currently selected axis, respect to matrix0
    @param axis The single or combined axis to clamp to.
    @param p The position to be clamped.
*/
void
nInguiToolTransform::ClampPosition( vector3& p, nInguiToolTransform::PickAxisType axis )
{
    vector3 pos = p;
    if ( axis == axisX || axis == axisY || axis == axisZ )
    {
        // Single axis, clamp the point to that axis.
        pos = pos - this->translation0;
        vector3 axV = GetAxisVector( axis, this->matrix0 );
        axV.norm();
        pos = this->translation0 + ( axV * ( axV % pos ));
    }
    else
    {
        // Combined axis, clamp the point to the plane
        plane p0;
        GetCombinedAxisPlane( axis, this->matrix0, this->translation0, p0);
        vector3 n0 = p0.normal();
        n0.norm();
        pos = pos - this->translation0;
        pos = this->translation0 + ( pos - ( n0 * ( n0 % pos ) ) );
    }
    p = pos;
}
//------------------------------------------------------------------------------    
/**
    @brief Get single axis projection of a point
    @param axis The axis
    @param p the position
    @return Point projection along the axis

    The function always uses the matrix0 matrix.
*/
float
nInguiToolTransform::GetPointProjectionSingleAxis( nInguiToolTransform::PickAxisType axis, vector3 p )
{
    n_assert( this->state > Inactive && ( axis == axisX || axis == axisY || axis == axisZ ) );
    vector3 axV = GetAxisVector( axis, this->matrix0 );
    axV.norm();

    //RMV return axV % ( p - this->matrix0.pos_component() );
    return axV % ( p - this->translation0 );
}

/**
    @brief Get combined axis projection of a point
    @param axis The axis
    @param p the position
    @return Position of point projection in the combined axis plane

    The function always uses the matrix0 matrix.
*/
vector2
nInguiToolTransform::GetPointProjectionCombinedAxis( nInguiToolTransform::PickAxisType combAxis, vector3 p )
{
    n_assert( this->state > Inactive && (combAxis == axisXY || combAxis == axisYZ || combAxis == axisXZ ) );
    vector3 axV1, axV2;
    switch ( combAxis )
    {
        case nInguiToolTransform::axisXY:
            axV1 = matrix0.x_component();
            axV2 = matrix0.y_component();
            break;
        case nInguiToolTransform::axisYZ:
            axV1 = matrix0.y_component();
            axV2 = matrix0.z_component();
            break;
        case nInguiToolTransform::axisXZ:
            axV1 = matrix0.x_component();
            axV2 = matrix0.z_component();
            break;
    }
    axV1.norm();
    axV2.norm();
    //p = p - matrix0.pos_component();
    p = p - translation0;
    vector2 ret = vector2( axV1 % p, axV2 % p );

    return ret;
}

//------------------------------------------------------------------------------   
/**
    @brief Update auxiliary matrices from the InguiObjectTransform object
*/
void
nInguiToolTransform::UpdateMatrices()
{
    if ( this->transformObject == 0 )
    {
        // No selection was made
        return;
    }

    transform44 t;
    this->transformObject->GetTransform( t );

    // If in world mode, leave it aligned to world
    if ( this->inWorldMode )
    {
        this->matrix0.ident();
        this->matrix0.set_translation( t.gettranslation() );
    }
    else
    {
        this->matrix0 = t.getmatrix();
        this->matrix0.x_component().norm();
        this->matrix0.y_component().norm();
        this->matrix0.z_component().norm();
    }

    // Store matrix for transforming objects
    this->objMatrix = this->matrix0;

    // this->translation0 is the original translation
    this->translation0 = this->matrix0.pos_component();

    // this->matrix0 is the matrix without translation
    this->matrix0.set_translation(vector3());

    // Get a matrix scaled to the helper size and transformed with the transform object for picking. Store it in matrix1
    this->matrix1.ident();
    this->matrix1.scale( vector3(this->size, this->size, this->size ) );
    this->matrix1 = this->matrix1 * this->matrix0;
    this->matrix1.set_translation( this->translation0 );
}

//------------------------------------------------------------------------------   
/**
    @brief Switch to selection tool. Later, when user releases the mouse button, the previous tool is selected again
*/
void
nInguiToolTransform::SwitchToSelection( nAppViewport* vp, vector2 mp, line3 ray )
{
    n_assert( this->refToolSelection.isvalid() );

    nObjectEditorState* objEd = this->refToolSelection->refObjState;
    n_assert( objEd );
    objEd->SwitchToSelection( vp, mp, ray );
}
//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
    @param vp Viewport in wich the mouse pointer is located
    @return true if any input (keyboard, mouse or other) was handled

    Default implementation of HandleInput just returns false
*/
bool
nInguiToolTransform::HandleInput( nAppViewport* /*vp*/ )
{
    nInputServer * inputServer = nInputServer::Instance();
    this->leftButtonPressed = inputServer->GetButton("buton0") || inputServer->GetButton("buton0_ctrl") || inputServer->GetButton("buton0_alt") || inputServer->GetButton("buton0_shift");
    return true;
}

//------------------------------------------------------------------------------   
/**
    @brief Picking of transform tool

    This makes the picking over an edge or over two axis. In the first pick it tests intersection with arrows and
    combined axis handlers. In subsequent frames, it tests picked position respect to the selected axis or in the selected plane.
*/
float
nInguiToolTransform::Pick( nAppViewport* vp, vector2 mp, line3 ray )
{
    n_assert( ray.len() > 0.0f );

    // return value
    float tmin = -1.0f;

    // origin of object space
    vector3 pos = this->matrix1.pos_component();

    // Handle case of picking the first time
    if ( this->state <= nInguiTool::Inactive )
    {
        if ( leftButtonPressed )
        {
            n_assert( this->refToolSelection.isvalid() );

            // If there is no selection and not in locked mode, try to select. After the try to select, the previous tool will be selected again.
            nObjectEditorState* objEd = this->refToolSelection->refObjState;
            n_assert( objEd );
            if (  objEd->GetSelectionCount() == 0 && ! this->refToolSelection->GetLockedMode() )
            {
                this->SwitchToSelection( vp, mp, ray );
                return -1.0f;
            }

            if ( this->transformObject )
            {
                // The picking with arrows and combined axis
                tmin = PickFirstFrame(vp, ray);
            }

            // If did not pick them and not in locked mode, switch to selection tool
            if ( tmin < 0.0f && ! this->refToolSelection->GetLockedMode())
            {
                this->SwitchToSelection( vp, mp, ray );
            }

            if ( ! this->transformObject )
            {
                return -1.0f;
            }

            // Store first picking position
            if ( tmin > 0.0f )
            {
                vector3 pickPos = ray.ipol( tmin );
                this->firstPos = pos;
                this->lastPos = this->firstPos;
                this->pickOffset = pickPos - pos;
                this->lastFramePicked = true;
            }
            else
            {
                // When didn't pick in first frame, check if can use the previous axis for the transform
                if ( lastFramePicked )
                {
                    tmin = this->PickNotFirstFrame(vp, ray);
                    if ( tmin > 0.0f )
                    {
                        vector3 pickPos = ray.ipol( tmin );
                        this->firstPos = pos;
                        this->lastPos = this->firstPos;
                        this->pickOffset = pickPos - pos;
                        this->lastFramePicked = true;
                    }
                }
                else
                {
                    tmin = -1.0f;
                }
            }
        }
        else
        {
            // If mouse button not pressed, just detect if there's a picking axis under mouse
            if ( this->transformObject )
            {
                // The picking with arrows and combined axis
                tmin = PickFirstFrame(vp, ray);

                if ( tmin <= 0.0f )
                {
                    this->lastAxisMouseOver = axisNone;
                }
            }
        }
    }
    // Handle not-first picking frame
    else
    {
        tmin = PickNotFirstFrame(vp, ray);

        if ( tmin > 0.0f )
        {
            this->lastPos = ray.ipol( tmin ) - this->pickOffset;
        }
        else
        {
            tmin = -1.0f;
        }
    }

    this->previousViewport = vp;
    return tmin;
}
//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolTransform::Apply( nTime /*dt*/ )
{
    if ( !this->transformObject || this->lastPickedAxis == axisNone)
    {
        return false;
    }

    
    n_assert( this->refToolSelection.isvalid() );
    n_assert( this->refToolSelection->refObjState.get() );
    nObjectEditorState * objState = this->refToolSelection->refObjState.get();
    if ( objState->GetSelectionMode() == nObjectEditorState::ModeTerrainCell )
    {
        return false;
    }

    this->transformObject->SetTransform( this->relMatrix );

    return true;
}


//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolTransform::IntersectSquare( const vector3 &b, const vector3 &e0, const vector3 &e1, const line3 &line, float &t )
{
    plane pl(b,b+e0,b+e1);
    if (!pl.intersect(line,t)) return false;
    vector3 ip = line.ipol(t);
    vector3 v1 = e0;
    v1.norm();
    float p0 = v1 % (ip - b);
    if ( p0 < 0 || p0 > e0.len()) return false;

    vector3 v2 = e1;
    v2.norm();
    float p1 = v2 % (ip - b);
    if ( p1 < 0 || p1 > e1.len()) return false;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
float
nInguiToolTransform::PickFirstFrame(nAppViewport* /*vp*/, line3 ray )
{
    // return value
    float tmin = 1.1f;
    bool picked = false;

    matrix44 transformMatrix, m;

    // Update matrices from object transform
    this->UpdateMatrices();

    transformMatrix = this->objMatrix;
    m = this->matrix1;
    vector3 pos = this->translation0;

    // Intersect the ray with the arrows as an sphere. Get the closest one.

    // The sphere radius is a constant fraction of the helper size
    float sphereRadius = ArrowSize * this->matrix1.x_component().len();

    vector3 vecp = ray.m;
    float vecpLen = vecp.len();
    float vecpLen2 = vecpLen * vecpLen;
    if ( vecpLen < TINY ) return false;

    // X arrow
    vector3 arrowPos = pos + m.x_component() * ( 1.0f - ArrowSize );
    float dist = ray.distance( arrowPos );
    if ( dist < sphereRadius )
    {
        vector3 vo = arrowPos - ray.b;
        float t = ( vecp % vo ) / vecpLen2;
        if ( t > 0.0f )
        {
            tmin = t;
            picked = true;
            lastPickedAxis = axisX;
        }
    }

    // Y arrow
    arrowPos = pos + m.y_component() * ( 1.0f - ArrowSize );
    dist = ray.distance( arrowPos );
    if ( dist < sphereRadius )
    {
        vector3 vo = arrowPos - ray.b;
        float t = ( vecp % vo ) / vecpLen2;
        if ( t > 0.0f && t < tmin )
        {
            tmin = t;
            picked = true;
            lastPickedAxis = axisY;
        }
    }

    // Z arrow
    arrowPos = pos + m.z_component() * ( 1.0f - ArrowSize );
    dist = ray.distance( arrowPos );
    if ( dist < sphereRadius )
    {
        vector3 vo = arrowPos - ray.b;
        float t = ( vecp % vo ) / vecpLen2;
        if ( t > 0.0f && t < tmin )
        {
            tmin = t;
            picked = true;
            lastPickedAxis = axisZ;
        }
    }

    // Intersection with combined axis handlers
    float t;

    if ( this->hasCombinedAxisHandlers )
    {    
        // XY
        if ( this->IntersectSquare( pos,
                                    m.y_component() * CombinedAxisHandlerSize,
                                    m.x_component() * CombinedAxisHandlerSize,
                                    ray, t) )
        {
            if ( t > 0.0f && t < tmin )
            {
                tmin = t;
                picked = true;
                lastPickedAxis = axisXY;
            }
        }

        // YZ
        if ( this->IntersectSquare( pos,
                                    m.z_component() * CombinedAxisHandlerSize,
                                    m.y_component() * CombinedAxisHandlerSize,
                                    ray, t) )
        {
            if ( t > 0.0f && t < tmin )
            {
                tmin = t;
                picked = true;
                lastPickedAxis = axisYZ;
            }
        }

        // XZ
        if ( this->IntersectSquare( pos,
                                    m.z_component() * CombinedAxisHandlerSize,
                                    m.x_component() * CombinedAxisHandlerSize,
                                    ray, t) )
        {
            if ( t > 0.0f && t < tmin )
            {
                tmin = t;
                picked = true;
                lastPickedAxis = axisXZ;
            }
        }
    }
    if ( picked )
    {
        this->lastAxisMouseOver = this->lastPickedAxis;
        return tmin;
    }
    else
    {
        return -1.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
float
nInguiToolTransform::PickNotFirstFrame(nAppViewport* /*vp*/, line3 ray )
{
    vector3 pos = this->matrix1.pos_component();
    float t = 1.1f;
    float tmin = -1.0f;
    vector3 intersPoint;

    plane p;
    if ( this->lastPickedAxis == axisX || this->lastPickedAxis == axisY || this->lastPickedAxis == axisZ )
    {
        // Picking against single axis, get a plane for it
        vector3 elong = GetAxisVector( this->lastPickedAxis, this->matrix1 );
        vector3 vnorm = ( pos - ray.b ) * elong;
        vnorm.norm();
        p.set( pos, pos + elong, pos + vnorm );
    }
    else
    {
        // Picking against combined axis, get that plane
        GetCombinedAxisPlane( this->lastPickedAxis, this->matrix1, this->matrix1.pos_component(), p);
    }
    if ( p.intersect( ray, t ) )
    {
        if ( t < TINY )
        {
            return -1.0f;
        }
        tmin = t;
    }
    else
    {
        // Invert plane if facing backwards
        vector3 n = p.normal();
        p.set( n.x, n.y, n.z, n % pos);
        if ( p.intersect( ray, t ) )
        {
            if ( t < TINY )
            {
                return -1.0f;
            }
            tmin = t;
        }
    }

        // If picking single axis, picking position must be near the axis
        // SEE IF THIS OPTION IS NEEDED
/*        if ( picked && ( this->lastPickedAxis == axisX || this->lastPickedAxis == axisY || this->lastPickedAxis == axisZ ) )
        {
            float axisDist;
            vector3 pa = this->lastPos - pos;
            switch ( this->lastPickedAxis )
            {
                case axisX:
                    axisDist = sqrt( pa.y*pa.y + pa.z*pa.z );
                    break;
                case axisY:
                    axisDist = sqrt( pa.x*pa.x + pa.z*pa.z );
                    break;
                case axisZ:
                    axisDist = sqrt( pa.x*pa.x + pa.y*pa.y );
                    break;
            }
            if ( axisDist > this->size * PickingAxisLimit )
            {
                picked = false;
                tmin = -1.0f;
            }
        }
*/

    return tmin;
}

//------------------------------------------------------------------------------
/**
    @brief Draw three axis given a world matrix and an optional size
    @param worldMatrix Matrix defining the coord system of the axis to draw. Can be scaled
    @size Optional resizing of the axis
*/
void nInguiToolTransform::DrawAxis( const matrix44& worldMatrix, float size )
{

    // Get gfx server pointer
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // Set axis shader
    gfxServer->SetShader(nInguiToolTransform::axisShader);

    // Set transform
    matrix44 m;
    m.scale( vector3( size, size, size ) );
    m = m * worldMatrix;
    gfxServer->PushTransform( nGfxServer2::Model, m );

    // Draw origin ball
    axisShader->SetVector4(nShaderState::MatDiffuse , vector4(0.5f, 0.5f, 0.5f, 1.0f ));
    gfxServer->SetMesh(nInguiToolTransform::meshOrigin, nInguiToolTransform::meshOrigin);
    nMeshGroup& curGroupOrigin = nInguiToolTransform::meshOrigin->Group(0);
    gfxServer->SetVertexRange(curGroupOrigin.GetFirstVertex(), curGroupOrigin.GetNumVertices());
    gfxServer->SetIndexRange(curGroupOrigin.GetFirstIndex(), curGroupOrigin.GetNumIndices());
    gfxServer->DrawIndexed(nGfxServer2::TriangleList);

    // Draw X axis
    axisShader->SetVector4(nShaderState::MatDiffuse , vector4(0.0f, 0.0f, 1.0f, 1.0f ));
    gfxServer->SetMesh(nInguiToolTransform::meshAxisX, nInguiToolTransform::meshAxisX);
    nMeshGroup& curGroupX = nInguiToolTransform::meshAxisX->Group(0);
    gfxServer->SetVertexRange(curGroupX.GetFirstVertex(), curGroupX.GetNumVertices());
    gfxServer->SetIndexRange(curGroupX.GetFirstIndex(), curGroupX.GetNumIndices());
    gfxServer->DrawIndexed(nGfxServer2::TriangleList);

    // Draw Y axis
    axisShader->SetVector4(nShaderState::MatDiffuse , vector4(0.0f, 1.0f, 0.0f, 1.0f ));
    gfxServer->SetMesh(nInguiToolTransform::meshAxisY, nInguiToolTransform::meshAxisY);
    nMeshGroup& curGroupY = nInguiToolTransform::meshAxisY->Group(0);
    gfxServer->SetVertexRange(curGroupY.GetFirstVertex(), curGroupY.GetNumVertices());
    gfxServer->SetIndexRange(curGroupY.GetFirstIndex(), curGroupY.GetNumIndices());
    gfxServer->DrawIndexed(nGfxServer2::TriangleList);

    // Draw Z axis
    axisShader->SetVector4(nShaderState::MatDiffuse , vector4(1.0f, 0.0f, 0.0f, 1.0f ));
    gfxServer->SetMesh(nInguiToolTransform::meshAxisZ, nInguiToolTransform::meshAxisZ);
    nMeshGroup& curGroupZ = nInguiToolTransform::meshAxisZ->Group(0);
    gfxServer->SetVertexRange(curGroupZ.GetFirstVertex(), curGroupZ.GetNumVertices());
    gfxServer->SetIndexRange(curGroupZ.GetFirstIndex(), curGroupZ.GetNumIndices());
    gfxServer->DrawIndexed(nGfxServer2::TriangleList);

    gfxServer->PopTransform( nGfxServer2::Model );

    /*
    int numPasses = axisShader->Begin(false);
    for (int i = 0; i < numPasses; i++)
    {
        axisShader->BeginPass(i);
        //for ( ... cones ... )
        //{
        axisShader->SetVector4(nShaderState::MatDiffuse, vector4(1.0f, 1.0f, 1.0f, 1.0f));
        gfxServer->DrawIndexedNS(nGfxServer2::nTriangleList);
        //}

        axisShader->EndPass();
    }
    axisShader->End();
    */

}

//------------------------------------------------------------------------------
/**
    @brief Draw three combined axis squares
*/
void nInguiToolTransform::DrawCombinedAxis()
{
    // Get gfx server pointer
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    gfxServer->BeginShapes( nInguiToolTransform::axisShader );

    float combAxisSize = this->size * CombinedAxisHandlerSize;
    matrix44 m;

    // XY combined axis handler
    vector4 col2 = vector4(1.0f, 1.0f, 0.0f, 0.5f);
    if ( this->lastAxisMouseOver == axisXY )
    {
        col2 = vector4( 1.0f, 1.0f, 1.0f, 0.5f);
    }
    m.ident();
    m.scale( vector3( 1.0f, 1.0f, 0.005f ) * combAxisSize );
    m.set_translation( vector3( 1.0f, 1.0f, 0.0f ) * 0.5f * combAxisSize );
    m = m * this->objMatrix;
    gfxServer ->DrawShape( nGfxServer2::Box, m, col2 );

    // XZ combined axis handler
    col2 = vector4(1.0f, 0.0f, 1.0f, 0.5f);
    if ( this->lastAxisMouseOver == axisXZ )
    {
        col2 = vector4( 1.0f, 1.0f, 1.0f, 0.5f);
    }
    m.ident();
    m.scale( vector3( 1.0f, 0.005f, 1.0f ) * combAxisSize );
    m.set_translation( vector3( 1.0f, 0.0f, 1.0f ) * 0.5f * combAxisSize );
    m = m * this->objMatrix;
    gfxServer ->DrawShape( nGfxServer2::Box, m, col2 );

    // YZ combined axis handler
    col2 = vector4(0.0f, 1.0f, 1.0f, 0.5f);
    if ( this->lastAxisMouseOver == axisYZ )
    {
        col2 = vector4( 1.0f, 1.0f, 1.0f, 0.5f);
    }
    m.ident();
    m.scale( vector3( 0.005f, 1.0f, 1.0f ) * combAxisSize );
    m.set_translation( vector3( 0.0f, 1.0f, 1.0f ) * 0.5f * combAxisSize );
    m = m * this->objMatrix;
    gfxServer ->DrawShape( nGfxServer2::Box, m, col2 );

    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
*/
void
nInguiToolTransform::Draw( nAppViewport* vp, nCamera2* /*camera*/ )
{

    // If no transform object assigned, exit
    if ( ! this->transformObject )
    {
        return;
    }
    
    transform44 t;
    this->transformObject->GetInitialMatrix( t );
    this->SetSize( nInguiTool::Screen2WorldObjectSize( vp, t.gettranslation(), 0.1f ) );

    // Get gfx server pointer
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    this->DrawAxis(this->objMatrix, this->size );

    matrix44 m;

    gfxServer->BeginShapes( nInguiToolTransform::axisShader );

    if ( this->lastAxisMouseOver == axisX || this->lastAxisMouseOver == axisY || this->lastAxisMouseOver == axisZ )
    {
        m.ident();
        m.scale( vector3(1.0f,1.0f,1.0f) * ArrowSize * this->size );
        m = m * this->objMatrix;
        vector3 arrowPos = GetAxisVector( this->lastPickedAxis, this->objMatrix ) * ( 1.0f - ArrowSize ) * this->size;
        m.translate( arrowPos );
        gfxServer->DrawShape(nGfxServer2::Sphere, m, vector4(1.0f, 1.0f, 1.0f, 0.5f));
    }

    gfxServer->EndShapes();

}

//------------------------------------------------------------------------------
/**
    @brief Get tool state
    @return The tool state
*/
void
nInguiToolTransform::SetState( int s )
{
    nInguiTool::SetState( s );

    if ( nInguiTool::ToolState( s ) <= nInguiTool::NotInited )
    {
        this->lastFramePicked = false;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get relative transform matrix for a transform in an axis
    @param f magnitude of transform
    @param axis Transform axis
    @param m Matrix to fill with the transform
    @param scale Returned scale, separated from matrix
*/
void
nInguiToolTransform::GetTransformMatrixForAxis( float /*f*/, nInguiToolTransform::PickAxisType /*axis*/, transform44& /*t*/ )
{
    // empty
}
//------------------------------------------------------------------------------
