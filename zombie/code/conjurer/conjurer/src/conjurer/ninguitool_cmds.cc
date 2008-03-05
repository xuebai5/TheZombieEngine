#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguimode_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiMode persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguitool.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTool )

    NSCRIPT_ADDCMD('SETS', void, SetState, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GETS', int/*nInguiTool::ToolState*/, GetState, 0, (), 0, ());
    NSCRIPT_ADDCMD('SDRE', void, SetDrawEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GDRE', bool, GetDrawEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('GFPO', bool, GetFirstPosition, 0, (), 1, (vector3&));
    NSCRIPT_ADDCMD('GLPO', bool, GetLastPosition, 0, (), 1, (vector3&));
    NSCRIPT_ADDCMD('GLAB', nString, GetLabel , 0, (), 0, ());


    cl->BeginSignals( 1 );
    N_INITCMDS_ADDSIGNAL( RefreshInfo );
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
