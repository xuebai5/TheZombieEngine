#ifndef N_INGUI_TOOL_TRANSLATION_H
#define N_INGUI_TOOL_TRANSLATION_H
//------------------------------------------------------------------------------
/**
    @file ninguitooltranslation.h
    @class nInguiToolTranslation
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for translations

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitooltransform.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;
class nEditorGrid;

//------------------------------------------------------------------------------
class nInguiToolTranslation: public nInguiToolTransform
{
public:

    // constructor
    nInguiToolTranslation();
    // destructor
    virtual ~nInguiToolTranslation();

    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    // Set grid for snapping
    vector3 GetCurrentTranslation();
    vector3 GetCurrentWorldPosition();
    
    void SetGridPath(nString);

    /// Get relative transform matrix for a transform in an axis
    virtual void GetTransformMatrixForAxis( float f, nInguiToolTransform::PickAxisType, transform44& t );

protected:
private:
    nDynAutoRef<nEditorGrid> refGrid;
};
//------------------------------------------------------------------------------
#endif
