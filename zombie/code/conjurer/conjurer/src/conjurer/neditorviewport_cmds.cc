#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  neditorviewport_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditorviewport.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    neditorviewport

    @cppclass
    nEditorViewport

    @superclass
    nappviewport

    @classinfo
    An encapsulation of editor viewport for Conjurer.
*/
NSCRIPT_INITCMDS_BEGIN(nEditorViewport)
    NSCRIPT_ADDCMD('SDMN', void, SetDebugModuleName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GDMN', const char *, GetDebugModuleName, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nEditorViewport::SaveCmds(nPersistServer* ps)
{
    if (nCommonViewport::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setviewportdebugmodule ---
        cmd = ps->GetCmd(this, 'SDMN');
        cmd->In()->SetS(this->GetDebugModuleName());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
