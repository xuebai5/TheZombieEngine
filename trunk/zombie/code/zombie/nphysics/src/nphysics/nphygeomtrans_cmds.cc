//-----------------------------------------------------------------------------
//  nphygeomtrans_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomtrans

    @cppclass
    nPhyGeomTrans

    @superclass
    nPhysicsGeom

    @classinfo
    A Geometry Transform is a geometry that encapsulates another geom.
*/
NSCRIPT_INITCMDS_BEGIN(nPhyGeomTrans)
    NSCRIPT_ADDCMD('DDDG', void, AddGeometry, 1, (nPhysicsGeom*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
nPhyGeomTrans::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nPhysicsGeom* geometry(this->GetGeometry());

    if( !geometry )
        return true;

    nCmd* cmd(ps->GetCmd( this, 'DDDG'));

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetO(geometry);

    /// Setting if the object it's enabled
    ps->PutCmd(cmd);
            
    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
