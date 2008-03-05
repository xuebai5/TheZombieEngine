//-----------------------------------------------------------------------------
//  nphygeomheightmap_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomheightmap.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomheightmap

    @cppclass nPhyGeomHeightMap

    @superclass
    nPhysicsGeom

    @classinfo
    Represents a geometry made trough a heigh map.
*/
void
n_initcmds_nPhyGeomHeightMap(nClass* cl)
{
    cl->BeginCmds();

    // registering script commands.
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
