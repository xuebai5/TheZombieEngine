#include "precompiled/pchrnsscene.h"
//------------------------------------------------------------------------------
//  ndotlasernode_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ndotlaser/ndotlasernode.h"

NSCRIPT_INITCMDS_BEGIN(nDotLaserNode)
NSCRIPT_INITCMDS_END()
//---------------------------------------------------------------------------
/**
*/
bool
nDotLaserNode::SaveCmds(nPersistServer * ps)
{
    if (nLightNode::SaveCmds(ps))
    {
        return true;
    }
    return false;
}