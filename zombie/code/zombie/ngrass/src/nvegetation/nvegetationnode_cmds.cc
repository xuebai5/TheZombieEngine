#include "precompiled/pchngrass.h"
//------------------------------------------------------------------------------
//  nvegetationnode_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nvegetation/nvegetationnode.h"
#include "nvegetation/nvegetationnode.h"
#include "kernel/npersistserver.h"

//---------------------------------------------------------------------------
/**
*/

NSCRIPT_INITCMDS_BEGIN( nVegetationNode  )
NSCRIPT_INITCMDS_END()

//---------------------------------------------------------------------------
/**
*/
bool
nVegetationNode::SaveCmds(nPersistServer * ps)
{
    if (nGeometryNode::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------

