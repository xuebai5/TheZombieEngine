#ifndef N_INGUI_TOOL_SCALE_H
#define N_INGUI_TOOL_SCALE_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolscale.h
    @class nInguiToolScale
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for scale transforms

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitooltransform.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;

//------------------------------------------------------------------------------
class nInguiToolScale: public nInguiToolTransform
{
public:

    // constructor
    nInguiToolScale();
    // destructor
    virtual ~nInguiToolScale();

    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    float GetCurrentScaling();
    void SetScaleFactor(float);

    /// Get relative transform matrix for a transform in an axis
    virtual void GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType, transform44& t );

protected:

    /// Current scaling
    float scale;

    /// Scale factor ( degrees / movement of half of the screen)
    float scaleFactor;

    /// Initial scaling in the current operation
    float initialScale;

private:
};
//------------------------------------------------------------------------------

#endif
