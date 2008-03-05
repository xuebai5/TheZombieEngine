#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  npreviewviewport_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/npreviewviewport.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    npreviewviewport

    @cppclass
    nPreviewViewport

    @superclass
    nappviewport

    @classinfo
    A custom viewport for class preview for Conjurer.
*/
NSCRIPT_INITCMDS_BEGIN(nPreviewViewport)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nPreviewViewport::SaveCmds(nPersistServer* ps)
{
    if (nAppViewport::SaveCmds(ps))
    {
        //nCmd* cmd;

        return true;
    }
    return false;
}
