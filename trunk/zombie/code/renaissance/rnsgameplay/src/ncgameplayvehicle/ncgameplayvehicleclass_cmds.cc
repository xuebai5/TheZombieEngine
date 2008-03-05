#include "precompiled/pchrnsgameplay.h"

/**
    @file ncgameplayvehicleclass_cmds.cc

    (C) Conjurer Services, S.A. 2005
*/

#include "ncgameplayvehicle/ncgameplayvehicleclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayVehicleClass)
    NSCRIPT_ADDCMD_COMPCLASS('ISMH', void, SetMaxHealth, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGMH', int, GetMaxHealth, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSSP', void, SetSeat, 2, (const unsigned int, const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGSP', const nString&, GetSeat, 1, (const unsigned int), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplayVehicleClass::SaveCmds (nPersistServer* ps)
{
    if ( nComponentClass::SaveCmds(ps) == false )
    {
        return false;
    }

    nCmd* cmd(0);

    // persisting car's HPs
    cmd = ps->GetCmd( this->GetEntityClass(), 'ISMH');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetMaxHealth() );

    ps->PutCmd(cmd);        

    // persisting car's seats
    for( int index(0); index < NumberOfSeats; ++index )
    {
        cmd = ps->GetCmd( this->GetEntityClass(), 'DSSP');

        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetI( index );
        cmd->In()->SetS( this->GetSeat( index ).Get() );

        ps->PutCmd(cmd);        
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
