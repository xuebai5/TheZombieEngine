#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguitoolselection_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiToolSelection persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolselection.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolSelection )
    NSCRIPT_ADDCMD('JGLM', bool, GetLockedMode , 0, (), 0, ());
    NSCRIPT_ADDCMD('JSLM', void, SetLockedMode, 1, (bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
