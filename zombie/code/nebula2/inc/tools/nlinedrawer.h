#ifndef N_LINEDRAWER_H
#define N_LINEDRAWER_H
//------------------------------------------------------------------------------
/**
    @class nLineDrawer
    @ingroup NebulaGraphicsSystem

    Class encapsulation of 3d line drawing functions using polygons
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "resource/nresource.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ndynamicmesh.h"
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
class nLineDrawer
{
public:
    /// constructor
    nLineDrawer();
    /// destructor
    ~nLineDrawer();

    /// Set path of shader
    void SetShaderPath( const nString& shPath );
    /// Get path of shader
    const nString& GetShaderPath( void );
    
    /// Draw a line 3d extruded
    void DrawExtruded3DLine(vector3 *points, int numPoints, vector4 *colors, int numColors, vector3 extrude, bool closed);
    /// Draw a line 3d extruded on a plane
    void DrawExtruded3DLineOnPlane(vector3 *points,  int np, vector3 planeNormal, float size);
    /// Draw a rect 3d in absolute coordinates
    void DrawRectangle2D(const rectangle& rect, const vector4& color);
    /// Draw a rect 3d in absolute coordinates with a border
    void DrawRectangle2DRel(const rectangle& rect, const vector4& color);
    /// Draw a rect 3d in relative coordinates with a border
    void DrawRectangle2DRelBorder(const rectangle& rect, int innerWidth, int outerWidth, const vector4& color);

private:
    nString shaderPath;
    nDynamicMesh dynMesh;
    nRef<nShader2> refShader;

};
//------------------------------------------------------------------------------
#endif
