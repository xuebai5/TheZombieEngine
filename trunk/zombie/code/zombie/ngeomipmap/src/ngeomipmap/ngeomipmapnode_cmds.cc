#include "precompiled/pchngeomipmap.h"
/*-----------------------------------------------------------------------------
    @file ngeomipmapnode_cmds.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief ngeomipmapnode Nebula class persistence and scripting

    (C) 2004 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "ngeomipmap/ngeomipmapnode.h"
#include "kernel/npersistserver.h"

//---------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN( nGeoMipMapNode )
NSCRIPT_INITCMDS_END()

//---------------------------------------------------------------------------
/**
*/
bool
nGeoMipMapNode::SaveCmds(nPersistServer * ps)
{
    if (nGeometryNode::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
