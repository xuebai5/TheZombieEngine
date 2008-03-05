#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nloaderserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nloaderserver.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nLoaderServer)
    NSCRIPT_ADDCMD('SLSP', void, SetLoadSpaces, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GLSP', bool, GetLoadSpaces, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLBT', void, SetLoadBatches, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GLBT', bool, GetLoadBatches, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLEN', void, SetLoadResources, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GLEN', bool, GetLoadResources, 0, (), 0, ());
    NSCRIPT_ADDCMD('SUCR', void, SetUnloadClassResources, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GUCR', bool, GetUnloadClassResources, 0, (), 0, ());
    NSCRIPT_ADDCMD('MCLA', nRoot*, CreateLoadArea, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('MDLA', void, DeleteLoadArea, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('SLAE', void, SetLoadAreasEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GLAE', bool, GetLoadAreasEnabled, 0, (), 0, ());    
NSCRIPT_INITCMDS_END()
