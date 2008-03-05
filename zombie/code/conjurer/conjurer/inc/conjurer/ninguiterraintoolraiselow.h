#ifndef N_INGUI_TERRAIN_TOOL_RAISELOW_H
#define N_INGUI_TERRAIN_TOOL_RAISELOW_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolraiselow.h
    @class nInguiTerrainToolRaiseLow
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools - Raise/lower terrain

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class nInguiTerrainToolRaiseLow: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolRaiseLow();
    // destructor
    virtual ~nInguiTerrainToolRaiseLow();

    virtual bool HandleInput( nAppViewport* vp );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Set Raise/lower flag
    void SetRaiseLower( bool rl );

    /// Set Raise/lower flag
    bool GetRaiseLower( void );

protected:

    // Raise or lower?
    bool raiseNotLower;

    // Do smooth instead of raise/low
    bool doSmooth;

private:
};
//------------------------------------------------------------------------------
/**
    @brief Set Raise/lower flag
    @param raiseNotLower Flag, true means raise, false lower
*/
inline void
nInguiTerrainToolRaiseLow::SetRaiseLower( bool raiseNotLower )
{
    this->raiseNotLower = raiseNotLower;
}

//------------------------------------------------------------------------------

#endif
