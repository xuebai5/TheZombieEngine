#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolwaypoint_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolgenwaypoint.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolGenWaypoint )
    NSCRIPT_ADDCMD('JSPN', void, SetPathName, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('JGPN', const char*, GetPathName , 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
