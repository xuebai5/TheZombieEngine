#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolflatten_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolflatten.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainToolFlatten )

    NSCRIPT_ADDCMD('SHEI', void, SetHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GHEI', float, GetHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD('SADI', void, SetAdaptiveIntensity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GADI', float, GetAdaptiveIntensity, 0, (), 0, ());


    cl->BeginSignals( 1 );
    N_INITCMDS_ADDSIGNAL( RefreshFlattenHeight );
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
