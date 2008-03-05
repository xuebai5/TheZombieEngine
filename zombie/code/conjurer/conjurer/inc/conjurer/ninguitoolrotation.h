#ifndef N_INGUI_TOOL_ROTATION_H
#define N_INGUI_TOOL_ROTATION_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolrotation.h
    @class nInguiToolRotation
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for rotations

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitooltransform.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;

//------------------------------------------------------------------------------
class nInguiToolRotation: public nInguiToolTransform
{
public:

    // constructor
    nInguiToolRotation();
    // destructor
    virtual ~nInguiToolRotation();

    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    virtual void Draw( nAppViewport* vp, nCamera2* camera );

        float GetCurrentRotAngle();
        void SetRotationFactor(float);

    /// Get relative transform matrix for a transform in an axis
    virtual void GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType, transform44& t );

protected:
    /// Factor that multiplies the rotation
    float rotationFactor;

    /// Current angle of rotation
    float angle;

    /// Initial angle of rotation (first pick offset)
    float initialAngle;

private:
};

//------------------------------------------------------------------------------

#endif
