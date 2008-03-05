//-----------------------------------------------------------------------------
//  nphysicsserver_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nphysicsserver

    @cppclass
    nPhysicsServer

    @superclass
    nRoot

    @classinfo
    The physics server takes care of creating and managing the physics world.
*/
NSCRIPT_INITCMDS_BEGIN(nPhysicsServer)
    NSCRIPT_ADDCMD('DDDO', void, Add, 1, (nPhysicsWorld*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('DRPM', void, RemoveMaterial, 1, (nPhyMaterial*), 0, ());
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
nPhysicsServer::SaveCmds(nPersistServer* ps)
{
    return nPhyMaterial::PersistMaterials( ps );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
