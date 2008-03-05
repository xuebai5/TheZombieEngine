#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nlevelmapviewport_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nlevelmapviewport.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlevelmapviewport

    @cppclass
    nLevelMapViewport

    @superclass
    nappviewport

    @classinfo
    A custom viewport for class preview for Conjurer.
*/
NSCRIPT_INITCMDS_BEGIN(nLevelMapViewport)
    NSCRIPT_ADDCMD('SBGC', void, SetBgColor, 1, (const vector4&), 0, ());
    NSCRIPT_ADDCMD('GBGC', const vector4&, GetBgColor, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nLevelMapViewport::SaveCmds(nPersistServer* ps)
{
    if (nAppViewport::SaveCmds(ps))
    {
        //nCmd* cmd;

        return true;
    }
    return false;
}
