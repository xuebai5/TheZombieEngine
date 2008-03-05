#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguimode_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiMode persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/neditorgrid.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nEditorGrid )
    NSCRIPT_ADDCMD('STLC', void, SetLightColor, 1, (vector4), 0, ());
    NSCRIPT_ADDCMD('STDC', void, SetDarkColor, 1, (vector4), 0, ());
    NSCRIPT_ADDCMD('STSP', void, SetSpacing, 1, (float), 0, ());
    NSCRIPT_ADDCMD('STCR', void, SetCameraRange, 1, (float), 0, ());
    NSCRIPT_ADDCMD('STNS', void, SetNSubdivision, 1, (int), 0, ());
    NSCRIPT_ADDCMD('STSE', void, SetSnapEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('STDE', void, SetDrawEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GTLC', vector4, GetLightColor, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTDC', vector4, GetDarkColor, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTSP', float, GetSpacing, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTCR', float, GetCameraRange, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTNS', int, GetNSubdivision, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTSE', bool, GetSnapEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('GTDE', bool, GetDrawEnabled, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
