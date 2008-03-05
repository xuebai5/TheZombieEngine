#include "precompiled/pchrnsscene.h"
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