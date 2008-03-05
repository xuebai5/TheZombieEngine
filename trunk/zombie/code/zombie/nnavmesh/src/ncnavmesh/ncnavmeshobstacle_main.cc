//-----------------------------------------------------------------------------
//  ncnavmeshobstacle_main.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnnavmesh.h"
#include "ncnavmesh/ncnavmeshobstacle.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncNavMeshObstacle,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNavMeshObstacle)
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncNavMeshObstacle::ncNavMeshObstacle() :
    obstacle( NULL )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncNavMeshObstacle::~ncNavMeshObstacle()
{
    // Empty
}
