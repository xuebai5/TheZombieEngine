#ifndef N_INGUI_TOOL_MEASURE_H
#define N_INGUI_TOOL_MEASURE_H
//------------------------------------------------------------------------------
/**
    @file ninguitoolmeasure.h
    @class nInguiToolMeasureArea
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool to obtain distance and slope info on the level

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolphypick.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nterrainline.h"
#include "tools/nlinedrawer.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nAppViewport;

//------------------------------------------------------------------------------
class nInguiToolMeasure: public nInguiToolPhyPick
{
    public:

    // constructor
    nInguiToolMeasure();
    // destructor
    virtual ~nInguiToolMeasure();

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Draw the helper in the screen
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

        /// Get distance over terrain and terrain slope angle
        float GetMeasureInfo(float&);

protected:

    /// Line for visualization of line on the terrain
    nTerrainLine* terrainLine;

    /// Line drawer
    nLineDrawer lineDrawer;

private:
    /// Temp. variable
    float terrainDistance;

    /// Temp. variable
    float slopeAngle;
};

#endif
