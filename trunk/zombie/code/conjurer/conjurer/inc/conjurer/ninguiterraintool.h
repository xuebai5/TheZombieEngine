#ifndef N_INGUI_TERRAIN_TOOL_H
#define N_INGUI_TERRAIN_TOOL_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintool.h
    @class nInguiTerrainTool
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitool.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nterrainline.h"
#include "tools/nlinedrawer.h"
#include "nphysics/nphycollide.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nFloatMap;
class nAppViewport;
class nPhyGeomRay;

//------------------------------------------------------------------------------
class nInguiTerrainTool: public nInguiTool
{
    public:

    // constructor
    nInguiTerrainTool();
    // destructor
    virtual ~nInguiTerrainTool();

    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Draw the helper in the screen
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    /// Set outdoor to edit
    virtual void SetOutdoor(nEntityObject * entityObject);

    /// Set apply diameter
    void SetDiameter(float);

    /// Get apply diameter
    float GetDiameter ();

    /// Get last map coordinates picked on the terrain
    bool GetLastTerrainCoords(int&, int&);

    /// Set 'always use picking' flag
    void SetAlwaysUsePickingFlag(bool);

    /// Get 'always use picking' flag
    bool GetAlwaysUsePickingFlag();

    /// Sets shape to be squared or round
   virtual void SetShapeSquared(bool);

    /// Gets shape to be squared or round
   virtual bool GetShapeSquared ();

    /// Get draw resolution multiplier
    virtual float GetDrawResolutionMultiplier();

protected:

    // Draw the round brush, clamped to the boundaries of terrain
    void DrawRoundBrush( bool snapped );

    // Draw current cursor line
    void DrawCursorLine( vector3 extrude, bool closed );

    /// reference to the 
    nRef<nEntityObject> outdoor;
    // Reference to heightmap
    nRef<nFloatMap> heightMap;

    // Tells if picking is done always directly, not using a plane in the following (not first) pickings
    bool alwaysUsePicking;

    // Last integer coordinates in the terrain
    int lastXMousePos, lastZMousePos;

    // Line for visualization of area covered by the terrain paintbrush (kind of a 3D pointer)
    nTerrainLine* cursor3DLine;

    // Line drawer
    nLineDrawer lineDrawer;

    /// Geometry ray for picking
    nPhyGeomRay* phyRay;

    // Array of intersections to make picking with the terrain
    nArray<nPhyCollide::nContact> contacts;

    // Color of 3d cursor
    vector4 cursorColor;

    /// Flag to indicate drawing shape: squared or round
    bool shapeSquared;

private:

    /// Diameter of placing area
    float diameter;

};

//------------------------------------------------------------------------------
/**
    @brief Get last map coordinates picked on the terrain
    @param x If returned true, the x coordinate.
    @param z If returned true, the z coordinate.
    @return Success. If returned false, there is no previus valid picking coordinates
*/
inline bool
nInguiTerrainTool::GetLastTerrainCoords(int& x, int& z)
{
    x = this->lastXMousePos;
    z = this->lastZMousePos;
    return true;
}
//------------------------------------------------------------------------------
/**
    @brief Set 'always use picking' flag
    @Param The flag

    This flag tells if the terrain picking is done always with the line of sight (true), or in the
    plane defined by the first picking (false)
*/
inline void
nInguiTerrainTool::SetAlwaysUsePickingFlag( bool flag )
{
    alwaysUsePicking = flag;
}
//------------------------------------------------------------------------------
/**
    @brief Get 'always use picking' flag   
    @return The flag

    This flag tells if the terrain picking is done always with the line of sight (true), or in the
    plane defined by the first picking (false)
*/
inline bool
nInguiTerrainTool::GetAlwaysUsePickingFlag( void )
{
    return alwaysUsePicking;
}

//------------------------------------------------------------------------------
/**
    @brief Sets shape to be squared or round
    @param The flag. True means squared, false means rounded.
*/
inline
void
nInguiTerrainTool::SetShapeSquared( bool shapeSquared )
{
    this->shapeSquared = shapeSquared;
}

//------------------------------------------------------------------------------
/**
    @brief Gets shape to be squared or round
    @return The flag. True means squared, false means rounded.
*/
inline
bool
nInguiTerrainTool::GetShapeSquared()
{
    return this->shapeSquared;
}

//------------------------------------------------------------------------------
#endif
