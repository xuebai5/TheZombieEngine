//-----------------------------------------------------------------------------
//  nwatcherdirserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "file/nwatcherdirserver.h"
#include "kernel/ncmd.h"

#ifdef __WIN32__

NSCRIPT_INITCMDS_BEGIN ( nWatcherDirServer )
    NSCRIPT_ADDCMD('FIAS', bool, IsEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('FWSE', void, Enable, 0, (), 0, ());
    NSCRIPT_ADDCMD('FWSD', void, Disable, 0, (), 0, ());
    NSCRIPT_ADDCMD('FSSA', void, SetAssign, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FSGA', nString, GetAssign, 0, (), 0, ());
NSCRIPT_INITCMDS_END ()

#endif