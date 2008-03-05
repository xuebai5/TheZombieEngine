#ifndef N_INGUI_TERRAIN_TOOL_HOLE_H
#define N_INGUI_TERRAIN_TOOL_HOLE_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolhole.h
    @class nInguiTerrainToolHole
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool Tool for terrain surface hole generation

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolpolyline.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;

//------------------------------------------------------------------------------
class nInguiTerrainToolHole : public nInguiTerrainToolPolyLine
{
public:

    // constructor
    nInguiTerrainToolHole();

    // destructor
    virtual ~nInguiTerrainToolHole();

    virtual void EndLine ();

protected:
private:
};
//------------------------------------------------------------------------------
#endif
