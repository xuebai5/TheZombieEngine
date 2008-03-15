#include "precompiled/pchngrass.h"
//------------------------------------------------------------------------------
//  nvegetationmeshresourceloader_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nvegetation/nVegetationMeshResourceLoader.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nVegetationMeshResourceLoader)
    // Empty
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nVegetationMeshResourceLoader::SaveCmds(nPersistServer* ps)
{
    if ( nResourceLoader ::SaveCmds(ps))
    {
        // Empty
        return true;
    }
    return false;
}
