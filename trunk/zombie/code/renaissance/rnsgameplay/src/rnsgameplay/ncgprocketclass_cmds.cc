//------------------------------------------------------------------------------
//  ncgprocketclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgprocketclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPRocketClass)
    NSCRIPT_ADDCMD_COMPCLASS('SIPW', void, SetInitialThrustPower, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GIPW', float, GetInitialThrustPower , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SMTT', void, SetMaxThrustTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GMTT', float, GetMaxThrustTime , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SMLT', void, SetMaxLifeTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GMLT', float, GetMaxLifeTime , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SMMS', void, SetMaxMeters, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GMMS', float, GetMaxMeters , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPRocketClass::SaveCmds( nPersistServer * ps )
{
    if( ! this->ncGameplayClass::SaveCmds( ps ) )
    {
        return false;
    }

    ps->Put( this->GetEntityClass(), 'SIPW', this->GetInitialThrustPower() );
    ps->Put( this->GetEntityClass(), 'SMTT', this->GetMaxThrustTime() );
    ps->Put( this->GetEntityClass(), 'SMLT', this->GetMaxLifeTime() );
    ps->Put( this->GetEntityClass(), 'SMMS', this->GetMaxMeters() );

    return true;
}

//------------------------------------------------------------------------------
