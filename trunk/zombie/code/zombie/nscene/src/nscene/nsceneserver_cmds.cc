#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nsceneserver_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nsceneserver.h"

NSCRIPT_INITCMDS_BEGIN(nSceneServer)
    NSCRIPT_ADDCMD('SPRI', void, SetPriorityBySelectorType, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD('GPRI', int, GetPriorityBySelectorType, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::SetPriorityBySelectorType(const char *typeFourCC, int renderPri)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(typeFourCC);
    this->SetPriorityBySelectorType(fourcc, renderPri);
}

//------------------------------------------------------------------------------
/**
*/
int
nSceneServer::GetPriorityBySelectorType(const char *typeFourCC)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(typeFourCC);
    return this->GetPriorityBySelectorType(fourcc);
}
