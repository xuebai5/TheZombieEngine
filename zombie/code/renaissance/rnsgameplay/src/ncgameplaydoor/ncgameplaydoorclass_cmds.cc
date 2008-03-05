//-----------------------------------------------------------------------------
//  ncgameplaydoorclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchrnsgameplay.h"

#include "ncgameplaydoor/ncgameplaydoorclass.h"

#include "entity/nentity.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGamePlayDoorClass)
    NSCRIPT_ADDCMD_COMPCLASS('DGTP', const ncGamePlayDoorClass::type, GetType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSTP', void, SetType, 1, (const ncGamePlayDoorClass::type), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGTA', const int, GetAttributes, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSTA', void, SetAttributes, 1, (const int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGGO', const float, GetOpeningAngle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSSO', void, SetOpeningAngle, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGSS', const float, GetSliddingSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSSS', void, SetSliddingSpeed, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGSL', const float, GetSliddingLenght, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSSL', void, SetSliddingLenght, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGWT', const float, GetWaitingTime, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSWT', void, SetWaitingTime, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGAS', const float, GetAngleSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSAS', void, SetAngleSpeed, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSCP', void, SetConnectToPortal, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGCP', bool, GetConnectToPortal, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    object persistency
*/
bool ncGamePlayDoorClass::SaveCmds(nPersistServer *ps)
{
    nCmd* cmd(0);

    // set door type
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSTP');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( int(this->GetType()) );

    ps->PutCmd(cmd);        
   
    // set door attributes
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSTA');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetAttributes() );

    ps->PutCmd(cmd);        
   
    // set door opening angle
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSSO');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetOpeningAngle() );

    ps->PutCmd(cmd);        

    // set door's slidding speed
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSSS');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetSliddingSpeed() );

    ps->PutCmd(cmd);        

    // set door's slidding length
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSSL');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetSliddingLenght() );

    ps->PutCmd(cmd);        

    // set door's waiting time
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSWT');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetWaitingTime() );

    ps->PutCmd(cmd);        

    // set door's angle speed
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSAS');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetAngleSpeed() );

    ps->PutCmd(cmd);        

    // set the connect to portal flag
    cmd = ps->GetCmd( this->GetEntityClass(), 'RSCP');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetB( this->GetConnectToPortal() );

    ps->PutCmd(cmd); 

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
