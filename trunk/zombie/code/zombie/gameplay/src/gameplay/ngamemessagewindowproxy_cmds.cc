//-----------------------------------------------------------------------------
//  ngamemessagewindowproxy_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"
#include "gameplay/ngamemessagewindowproxy.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nGameMessageWindowProxy )

    NSCRIPT_ADDCMD('ESGM', void, SendGameMessage, 4, (nEntityObject*, const char*, int, bool), 0, () );

    cl->BeginSignals(1);
    N_INITCMDS_ADDSIGNAL( OnGameMessage )
    cl->EndSignals();

NSCRIPT_INITCMDS_END()
