#ifndef N_INGUI_TERRAIN_TOOL_SMOOTH_H
#define N_INGUI_TERRAIN_TOOL_SMOOTH_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolsmooth.h
    @class nInguiTerrainToolSmooth
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools - Smooth terrain

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class nInguiTerrainToolSmooth: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolSmooth();
    // destructor
    virtual ~nInguiTerrainToolSmooth();

    /// Apply the tool
    virtual bool Apply( nTime dt );

protected:
private:
};

//------------------------------------------------------------------------------

#endif
