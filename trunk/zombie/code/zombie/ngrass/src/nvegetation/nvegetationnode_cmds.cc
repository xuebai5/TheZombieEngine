#include "precompiled/pchngrass.h"
#include "nvegetation/nvegetationnode.h"
/*-----------------------------------------------------------------------------
    @file nvegetationnode_cmds.cc
    @ingroup NebulaGrass

    @author Cristobal Castillo Domingo 

    @brief ngeomipmapnode Nebula class persistence and scripting

    (C) 2004 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
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

