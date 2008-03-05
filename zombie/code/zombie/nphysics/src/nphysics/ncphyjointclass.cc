//------------------------------------------------------------------------------
//  ncphyjointclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyjointclass.h"

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------
#ifndef NGAME
nNebulaComponentClass(ncPhyJointClass,ncPhysicsObjClass);
#else
nNebulaComponentClass(ncPhyJointClass,nComponentClass);
#endif

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyJointClass)
    NSCRIPT_ADDCMD_COMPCLASS('DSKJ', void, SetTypeJoint, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGKJ', int, GetTypeJoint, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 19-Aug-2005   David Reyes    created
*/
ncPhyJointClass::ncPhyJointClass() :
    jointType( nPhysicsJoint::Invalid )
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 19-Aug-2005   David Reyes    created
*/
ncPhyJointClass::~ncPhyJointClass()
{
    // empty
}


//-----------------------------------------------------------------------------
/**
    Sets the kind of the joint.

    @param type joint type

    history:
        - 19-Aug-2005   David Reyes    created
*/
void ncPhyJointClass::SetTypeJoint( int type )
{
    this->jointType = type;
}

//-----------------------------------------------------------------------------
/**
    Gets the kind of the joint.

    @return joint type

    history:
        - 19-Aug-2005   David Reyes    created
*/
int ncPhyJointClass::GetTypeJoint() const
{
    return this->jointType;
}

//-----------------------------------------------------------------------------
/**
    Object persistency.

    history:
        - 19-Aug-2005   David Reyes    created
*/
bool ncPhyJointClass::SaveCmds(nPersistServer* server)
{
#ifndef NGAME
    if( !ncPhysicsObjClass::SaveCmds( server ) )
    {
        return false;
    }
#endif

    if( server->GetSaveType() != nPersistServer::SAVETYPE_PERSIST )
    {
        return true;
    }

    nCmd *cmd(0);

    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DSKJ' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetI( this->GetTypeJoint() );

    server->PutCmd(cmd);        

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
