#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguiterraintoolpaint_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiIerrainToolPaint persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolpaint.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainToolPaint )
    NSCRIPT_ADDCMD('GFHE', bool, GetFilterHeightEnabled , 0, (), 0, ());
    NSCRIPT_ADDCMD('GFSE', bool, GetFilterSlopeEnabled , 0, (), 0, ());
    NSCRIPT_ADDCMD('GMIH', float, GetMinFilterHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD('GMAH', float, GetMaxFilterHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD('GMIS', float, GetMinFilterSlope , 0, (), 0, ());
    NSCRIPT_ADDCMD('GMAS', float, GetMaxFilterSlope , 0, (), 0, ());
    NSCRIPT_ADDCMD('SFHE', void, SetFilterHeightEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('SFSE', void, SetFilterSlopeEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('SMIH', void, SetMinFilterHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD('SMAH', void, SetMaxFilterHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD('SMIS', void, SetMinFilterSlope, 1, (float), 0, ());
    NSCRIPT_ADDCMD('SMAS', void, SetMaxFilterSlope, 1, (float), 0, ());
    NSCRIPT_ADDCMD('SBLE', void, SetBlendIntensity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GBLE', float, GetBlendIntensity, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
