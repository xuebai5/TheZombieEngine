//------------------------------------------------------------------------------
//  ncscenedotlaser_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsscene.h"
#include "ndotlaser/ncscenedotlaser.h"

nNebulaComponentObject(ncSceneDotLaser, ncSceneLight);
NSCRIPT_INITCMDS_BEGIN(ncSceneDotLaser)
NSCRIPT_INITCMDS_END()
//---------------------------------------------------------------------------
/**
*/
bool
ncSceneDotLaser::SaveCmds(nPersistServer * ps)
{
    if (ncSceneLight::SaveCmds(ps))
    {
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
