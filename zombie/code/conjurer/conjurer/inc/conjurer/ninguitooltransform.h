#ifndef N_INGUI_TOOL_TRANSFORM_H
#define N_INGUI_TOOL_TRANSFORM_H
//------------------------------------------------------------------------------
/**
    @file ninguitooltransform.h
    @class nInguiToolTransform
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for transforms

    (C) 2004 Conjurer Services, S.A.

    This is the parent class of three transform tools: translation, rotation and scale. The interface with the object to transform
    is done through an object derived from InguiObjectTransform, wich defines the target/s and the reference system for the transform (world
    or local)
*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitool.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/ntimeserver.h"
#include "mathlib/nmath.h"
#include "mathlib/plane.h"
#include "conjurer/inguiobjecttransform.h"

class nAppViewport;
class nMesh2;
class nShader2;
class nObjectEditorState;
class nInguiToolSelection;

//------------------------------------------------------------------------------
class nInguiToolTransform: public nInguiTool
{
public:

    /// Size of the arrows relative to the helper size
    #define ArrowSize 0.1f
    
    /// Size of the combined axis handlers relative to the helper size
    #define CombinedAxisHandlerSize 0.3f

    /// Size of picking axis limit (relative size)
    #define PickingAxisLimit 0.6f

    // Type of intersection handler picked ( one of the three arrows or one of the three combined axis handlers)
    enum PickAxisType
    {
        axisNone,
        axisX,
        axisY,
        axisZ,
        axisXY,
        axisYZ,
        axisXZ
    };

    // constructor
    nInguiToolTransform();
    // destructor
    virtual ~nInguiToolTransform();

    /// Handle input in a viewport.
    virtual bool HandleInput( nAppViewport* vp );

    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Apply the tool
    virtual bool Apply( nTime dt);

    /// Draw the visual elements of the tool
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    /// Set tool state
    virtual void SetState( int s );

    /// Draw axis
    static void DrawAxis( const matrix44& worldMatrix, float size = 1.0f );

    /// Draw combined axis
    void DrawCombinedAxis();

    /// Set transform object
    void SetTransformObject( InguiObjectTransform* t );

    /// Set reference to selection tool
    void SetSelectionTool( nInguiToolSelection *selTool );

    /// Set size of the helper
    void SetSize( float s );

    /// Set size of the helper
    float GetSize();

    /// Get last picked axis type
    PickAxisType GetLastPickedAxisType();

    /// Get transform object
    InguiObjectTransform* GetTransformObject( void );

    // Update auxiliary matrices from the InguiObjectTransform object
    void UpdateMatrices();

    float GetSnap();
    void SetSnap(float);

    /// Get a single axis vector
    vector3 GetAxisVector( nInguiToolTransform::PickAxisType axis, matrix44& matrix );

    void GetCombinedAxisPlane( nInguiToolTransform::PickAxisType combAxis, matrix44& matrix, vector3 origin, plane& p );

    /// Clamp a position to the selected axis
    void ClampPosition( vector3& p, nInguiToolTransform::PickAxisType axis );

    /// Get single axis projection of a point
    float GetPointProjectionSingleAxis( nInguiToolTransform::PickAxisType axis, vector3 p );

    /// Get combined axis projection of a point
    vector2 GetPointProjectionCombinedAxis( nInguiToolTransform::PickAxisType combAxis, vector3 p );

    /// Set World mode flag
    void SetWorldMode(bool worldMode);

    /// Get World mode flag
    bool GetWorldMode();

    /// Switch to selection tool, and will return to the previous tool after using the selection
    void SwitchToSelection( nAppViewport* vp, vector2 mp, line3 ray );

    /// Get relative transform matrix for a transform in an axis
    virtual void GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType, transform44& t );

protected:

    /// Flag for world axis aligned operation
    bool inWorldMode;

    /// Reference to transform object
    InguiObjectTransform* transformObject;

    /// Selection tool reference
    nRef<nInguiToolSelection> refToolSelection;

    /// Size of the helper
    float size;

    // Last picked axis handler type
    PickAxisType lastPickedAxis;

    /// Last valid axis under cursor
    PickAxisType lastAxisMouseOver;

    // Did it pick in the last first frame?
    bool lastFramePicked;

    // First picking position w. r. to the initial translation
    vector3 pickOffset;

    // Tells wether this type of tool has combined axis handlers
    bool hasCombinedAxisHandlers;    

    // Original transform matrix in a dragging action, without translation
    matrix44 matrix0;

    // Original translation
    vector3 translation0;

    // Last transformed matrix for picking 
    matrix44 matrix1;

    // Matrix for transforming objects
    matrix44 objMatrix;

    // Transform of the operation performed
    transform44 relMatrix;

    // Value for snapping
    float snapValue;

    // Mesh objects for drawing axes
    static nMesh2 *meshAxisX, *meshAxisY, *meshAxisZ, *meshOrigin;

    // Shader for drawing axes
    static nShader2 *axisShader;

    // left mouse button state
    bool leftButtonPressed;

private:
    bool IntersectSquare( const vector3 &b, const vector3 &e0, const vector3 &e1, const line3 &line, float &t );
    float PickFirstFrame(nAppViewport* vp, line3 ray );
    float PickNotFirstFrame(nAppViewport* vp, line3 ray );
};

//------------------------------------------------------------------------------
/**
    @brief Set world mode on/off for the tool (the operation is performed world axis-aligned) 
*/
inline void
nInguiToolTransform::SetWorldMode( bool worldMode )
{
    this->inWorldMode = worldMode;
}

//------------------------------------------------------------------------------
/**
    @brief Get world mode flag for the tool (the operation is performed world axis-aligned) 
*/
inline bool
nInguiToolTransform::GetWorldMode()
{
    return this->inWorldMode;
}

//------------------------------------------------------------------------------
/**
    @brief Set reference to transform object
*/
inline void
nInguiToolTransform::SetTransformObject( InguiObjectTransform* t )
{
    this->transformObject = t;

    if ( !t )
    {
        return;
    }

    // Calculate a first matrix0 and matrix1, valid until a pick is done
    transform44 m;

    this->transformObject->GetTransform( m );

    this->matrix0 = m.getmatrix();
    this->translation0 = this->matrix0.pos_component();
    this->matrix0.set_translation(vector3(0.0f,0.0f,0.0f));
    this->matrix1 = m.getmatrix();
    this->objMatrix = m.getmatrix();
}

//------------------------------------------------------------------------------
/**
    @brief Get reference to transform object
*/
inline InguiObjectTransform*
nInguiToolTransform::GetTransformObject()
{
    return this->transformObject;
}
//------------------------------------------------------------------------------

/// Set size of the helper
inline void
nInguiToolTransform::SetSize( float s )
{
    size = s;
}
//------------------------------------------------------------------------------
/// Set size of the helper
inline float
nInguiToolTransform::GetSize()
{
    return size;
}
//------------------------------------------------------------------------------
/// Get last picked axis type
inline nInguiToolTransform::PickAxisType
nInguiToolTransform::GetLastPickedAxisType()
{
    return this->lastPickedAxis;
}

//------------------------------------------------------------------------------
/**
    @brief Get a single axis vector
    @param axis The axis (must be axisX, Y or Z, nor a combined one)
    @param matrix The matrix to obtain axis from

    The function always uses the matrix0 matrix.
*/
inline vector3
nInguiToolTransform::GetAxisVector( nInguiToolTransform::PickAxisType axis, matrix44& matrix )
{
    n_assert( axis == axisX || axis == axisY || axis == axisZ );
    switch ( axis )
    {
        case axisX:
            return matrix.x_component();
            break;
        case axisY:
            return matrix.y_component();
            break;
        case axisZ:
            return matrix.z_component();
            break;
        default:
            return vector3(0.0f,0.0f,0.0f);
    }
}
//------------------------------------------------------------------------------
/**
    @brief Get a combined axis plane
    @param axis The combined axis (must be axisXY, XZ or YZ)
    @param origin Origin of the matrix, used insteaad of the matrix translation
    @param p The plane to fill

    The function always uses the matrix0 matrix.
*/
inline void
nInguiToolTransform::GetCombinedAxisPlane( nInguiToolTransform::PickAxisType combAxis, matrix44& matrix, vector3 origin, plane& p )
{
    n_assert( (combAxis == axisXY || combAxis == axisYZ || combAxis == axisXZ ) );
    switch ( combAxis )
    {
        case nInguiToolTransform::axisXY:
            p.set( origin, origin + matrix.x_component(), origin + matrix.y_component() );
            break;
        case nInguiToolTransform::axisYZ:
            p.set( origin, origin + matrix.y_component(), origin + matrix.z_component() );
            break;
        case nInguiToolTransform::axisXZ:
            p.set( origin, origin + matrix.x_component(), origin + matrix.z_component() );
            break;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set reference to selection tool
*/
inline void
nInguiToolTransform::SetSelectionTool( nInguiToolSelection *selTool )
{
    n_assert( selTool );
    this->refToolSelection = selTool;
}
    
#endif
