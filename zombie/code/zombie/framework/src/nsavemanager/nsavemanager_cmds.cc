//-----------------------------------------------------------------------------
//  nsavemanager_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchframework.h"
#include "nsavemanager/nsavemanager.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN( nSaveManager )
    NSCRIPT_ADDCMD('BBSE', nObject *, BeginStatenEntityObject, 4, (const char *, const char *, const char *, nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('BBSR', nObject *, BeginStatenRoot, 4, (const char *, const char *, const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('BBSO', nObject *, BeginStatenObject, 3, (const char *, const char *, const char *), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
