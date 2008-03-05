#ifndef N_INGUI_TERRAIN_TOOL_POLYLINE_H
#define N_INGUI_TERRAIN_TOOL_POLYLINE_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolpolyline.h
    @class nInguiTerrainToolPolyLine
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool Base tool for generating a polyline, closed or not

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintool.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nterrainline.h"
#include "gfx2/nlineserver.h"

class nAppViewport;

//------------------------------------------------------------------------------
class nInguiTerrainToolPolyLine : public nInguiTerrainTool
{
public:

    // constructor
    nInguiTerrainToolPolyLine();

    // destructor
    virtual ~nInguiTerrainToolPolyLine();

    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Handle input in a viewport.
    virtual bool HandleInput( nAppViewport* vp );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Draw the helper in the screen
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    /// Set outdoor to edit
    virtual void SetOutdoor(nEntityObject * entityObject);

   virtual  void EndLine ();
    void CancelLine ();

    /// Marks a rectangle as dirty 
    void MakeRectangleDirtyGeometry(int x0, int z0, int x1, int z1);

    /// return true if the passed point close the poly
    bool PointClosePoly( vector3& firstPoint );

protected:

    /// Tool must close the polyline when finished
    bool mustClose;

    /// Margin for automatic close poly when user inserts a point near the first one
    float closePolyMargin;

    /// True if a the last picked point close the poly
    bool pickPointClosePoly;
    
    /// Current line being added
    nTerrainLine* currentLine;
    
    /// Display color 
    vector4 displayColor;

    // Shader for drawing axes
    nShader2 *sphereShader; 

private:
    // Draw a sphere in the a specific vertex if exist
    void DrawVertexHelper( int numVertex, vector4 color, float size );

    // Flag to add a point in this frame
    bool addPoint;

    // Line drawing
    nLineHandler lineHandler;
};
//------------------------------------------------------------------------------
#endif
