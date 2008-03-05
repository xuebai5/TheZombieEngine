#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file nInguiToolPrismArea_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiToolPrismArea persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolprismarea.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolPrismArea )
    NSCRIPT_ADDCMD('JSBH', void, SetHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JGBH', float, GetHeight , 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
