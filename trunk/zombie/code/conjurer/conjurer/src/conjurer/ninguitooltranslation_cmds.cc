#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitooltranslation_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitooltranslation.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolTranslation )
    NSCRIPT_ADDCMD('GCTR', vector3, GetCurrentTranslation, 0, (), 0, ());
    NSCRIPT_ADDCMD('GCWP', vector3, GetCurrentWorldPosition, 0, (), 0, ());
    NSCRIPT_ADDCMD('SETG', void, SetGridPath, 1, (nString), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
