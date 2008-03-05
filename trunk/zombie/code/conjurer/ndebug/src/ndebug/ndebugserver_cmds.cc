//------------------------------------------------------------------------------
//  ndebugserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ndebugserver

    @cppclass
    nDebugServer

    @superclass
    nroot

    @classinfo
    A central manager of debug options.
*/
NSCRIPT_INITCMDS_BEGIN(nDebugServer)
    NSCRIPT_ADDCMD('GNDO', int, GetNumOptions, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GDOA', const char *, GetOptionAt, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD('SDOE', bool, SetFlagEnabled, 3, (const char *, const char *, bool), 0, ());
    NSCRIPT_ADDCMD('GDOE', bool, GetFlagEnabled, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('YSCV', bool, SetColourSettingValue, 3, (const char *, const char *, vector4), 0, ());
    NSCRIPT_ADDCMD('YGCV', vector4, GetColourSettingValue, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('GDOD', const char *, GetOptionDescription, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('YGOT', int, GetOptionType, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('YGOG', const char *, GetOptionGroupName, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
