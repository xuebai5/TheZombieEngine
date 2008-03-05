
//-----------------------------------------------------------------------------
//  ncphyterrain_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "kernel/npersistserver.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    nphysicsworld

    @cppclass
    nPhysicsWorld

    @superclass
    nObject

    @classinfo
    An abstract physics world.
*/
NSCRIPT_INITCMDS_BEGIN(nPhysicsWorld)
    NSCRIPT_ADDCMD('DDDO', void, Add, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('DETS', void, SetSpace, 1, (nPhySpace*), 0, ());
    NSCRIPT_ADDCMD('DNBL', void, Enable, 0, (), 0, ());
    NSCRIPT_ADDCMD('DIBL', void, Disable, 0, (), 0, ());
    NSCRIPT_ADDCMD('DGRV', void, SetGravity, 1, (const vector3&), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhysicsWorld::SaveCmds(nPersistServer* ps)
{
    if( !nObject::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DDDO'));

    n_assert2( cmd, "Error command not found" );

    int Size(this->containerPhysicsObj.Size());

    for( int index(0); index < Size; ++index )
    {
        nCmd* cmd(ps->GetCmd( this, 'DDDO'));

        cmd->In()->SetO( containerPhysicsObj.GetElementAt( index ) );

        ps->PutCmd(cmd);
    }

    cmd = ps->GetCmd( this, 'DETS');

    n_assert2( cmd, "Error command not found" );

    if( this->worldSpace != NoValidID )
    {
        cmd->In()->SetO( this->worldSpace );

        ps->PutCmd(cmd);
    }

    if( this->IsEnabled() )
        cmd = ps->GetCmd( this, 'DNBL');
    else
        cmd = ps->GetCmd( this, 'DIBL');

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DGRV');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->gravityWorld.x );
    cmd->In()->SetF( this->gravityWorld.y );
    cmd->In()->SetF( this->gravityWorld.z );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
