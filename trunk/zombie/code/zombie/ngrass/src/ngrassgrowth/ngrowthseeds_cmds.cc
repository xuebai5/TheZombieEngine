#include "precompiled/pchngrass.h"
#include "ngrassgrowth/ngrowthseeds.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nGrowthSeeds )
    // Empty
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nGrowthSeeds::SaveCmds(nPersistServer* ps)
{
    if (nResource::SaveCmds(ps))
    {
        // Empty
        return true;
    }
    return false;
}
