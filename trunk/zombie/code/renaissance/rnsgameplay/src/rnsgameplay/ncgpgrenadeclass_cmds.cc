//------------------------------------------------------------------------------
//  ncgpgrenadeclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpgrenadeclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPGrenadeClass)
    NSCRIPT_ADDCMD_COMPCLASS('SLPW', void, SetLaunchPower, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GLPW', float, GetLaunchPower , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SMLT', void, SetMaxLifeTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GMLT', float, GetMaxLifeTime , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPGrenadeClass::SaveCmds( nPersistServer * ps )
{
    if( ! this->ncGameplayClass::SaveCmds( ps ) )
    {
        return false;
    }

    ps->Put( this->GetEntityClass(), 'SLPW', this->GetLaunchPower() );
    ps->Put( this->GetEntityClass(), 'SMLT', this->GetMaxLifeTime() );

    return true;
}

//------------------------------------------------------------------------------
