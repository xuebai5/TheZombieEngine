#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialquadtreeclass.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialquadtreeclass.h"
#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSpatialQuadtreeClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialQuadtreeClass)
    NSCRIPT_ADDCMD_COMPCLASS('RSOB', void, SetOriginalBBox, 6, (float, float, float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGOB', void, GetOriginalBBox, 0, (), 2, (vector3&, vector3&));
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialQuadtreeClass::SaveCmds(nPersistServer *ps)
{
    //--- setoriginalbbox ---
    vector3 v0, v1;
    this->GetOriginalBBox(v0, v1);
    if (!ps->Put(this->GetEntityClass(), 'RSOB', v0.x, v0.y, v0.z, v1.x, v1.y, v1.z))
    {
        return false;
    }

    return true;
}

