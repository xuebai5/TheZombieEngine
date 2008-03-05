//-----------------------------------------------------------------------------
//  ncgameplaydoor_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchrnsgameplay.h"

#include "ncgameplaydoor/ncgameplaydoor.h"

#include "entity/nentity.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGamePlayDoor)
    NSCRIPT_ADDCMD_COMPOBJECT('DGTP', const ncGamePlayDoorClass::type, GetType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTP', void, SetType, 1, (const ncGamePlayDoorClass::type), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGTA', const int, GetAttributes, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTA', void, SetAttributes, 1, (const int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DISL', const bool, IsLocked, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DLCK', const bool, Lock, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DUCK', const bool, Unlock, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DOPN', bool, Open, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DCLS', bool, Close, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DICL', const bool, IsClosed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LIOP', const bool, IsOpened, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DIBK', const bool, IsBroken, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDA', void, SetDoorEntityA, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGDA', nEntityObject*, GetDoorEntityA, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDB', void, SetDoorEntityB, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGDB', nEntityObject*, GetDoorEntityB, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DMNT', void, Mount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGSS', const float, GetSliddingSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSSS', void, SetSliddingSpeed, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGSL', const float, GetSliddingLenght, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSSL', void, SetSliddingLenght, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGAS', const float, GetAngleSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSAS', void, SetAngleSpeed, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGGO', const float, GetOpeningAngle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSSO', void, SetOpeningAngle, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGWT', const float, GetWaitingTime, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSWT', void, SetWaitingTime, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DMUT', void, UnMount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RCTP', bool, ConnectToPortal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RUPS', void, UpdatePortal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DDAC', const bool, DoAction, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSNE', void, SetNewState, 1, (const int), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    object persistency
*/
bool ncGamePlayDoor::SaveCmds(nPersistServer *ps)
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

    // set door state
    cmd = ps->GetCmd( this->GetEntityObject(), 'RSNE');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( static_cast<int>(this->doorState) );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
