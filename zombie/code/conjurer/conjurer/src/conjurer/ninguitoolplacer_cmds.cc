#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolplacer_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolplacer.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolPlacer )
    NSCRIPT_ADDCMD('SETY', void, SetInstanceClass, 1, (nString), 0, ());
    NSCRIPT_ADDCMD('GETY', nString, GetInstanceClass , 0, (), 0, ());
    NSCRIPT_ADDCMD('JSRR', void, SetRandomRotation, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JGRR', bool, GetRandomRotation, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSSV', void, SetSizeVariation, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JGSV', float, GetSizeVariation, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
