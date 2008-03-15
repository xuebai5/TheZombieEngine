#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolprismarea_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
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
