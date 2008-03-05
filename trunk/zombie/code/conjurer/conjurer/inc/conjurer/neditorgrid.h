#ifndef N_EDITOR_GRID_H
#define N_EDITOR_GRID_H
//------------------------------------------------------------------------------
/**
    @file neditorgrid.h
    @class nEditorGrid
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Class that implements a 3D editor grid

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nlineserver.h"

class nAppViewport;

//------------------------------------------------------------------------------
class nEditorGrid: public nRoot
{
    public:

    // Constructor
    nEditorGrid();

    // Destructor
    virtual ~nEditorGrid();

    void SetLightColor(vector4);
    void SetDarkColor(vector4);
    void SetSpacing(float);
    void SetCameraRange(float);
    void SetNSubdivision(int);
    void SetSnapEnabled(bool);
    void SetDrawEnabled(bool);

    vector4 GetLightColor();
    vector4 GetDarkColor();
    float GetSpacing();
    float GetCameraRange();
    int GetNSubdivision();
    bool GetSnapEnabled();
    bool GetDrawEnabled();

    vector3 SnapPosition(vector3 pos);

    void Draw( nAppViewport* vp );

protected:
private:

    /// Grid Origin. X and Z coordinates must be less than spacing
    vector3 origin;

    /// Grid spacing
    float spacing;

    /// Grid light color
    vector4 lightColor;

    /// Grid dark color
    vector4 darkColor;

    /// Camera range for grid drawing
    float cameraRange;

    /// Grid major lines: every numSub number of lines spaced with the 'spacing' variable is darker
    int numSub;

    /// Snap activation
    bool snapEnabled;

    /// Drawing activation
    bool drawEnabled;

    // Line handler for drawing the grid
    nLineHandler lineHandler;

    // Internal function for snapping
    vector3 Snap(vector3 pos, float sp = 0.0f);
};
//------------------------------------------------------------------------------
#endif
