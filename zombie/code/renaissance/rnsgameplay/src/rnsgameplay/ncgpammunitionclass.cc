//------------------------------------------------------------------------------
//  ncgpammunitionclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpammunitionclass.h"

nNebulaComponentClass(ncGPAmmunitionClass,nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPAmmunitionClass)
    NSCRIPT_ADDCMD_COMPCLASS('LSAT', void, SetAmmoType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGAT', int, GetAmmoType , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncGPAmmunitionClass::ncGPAmmunitionClass():
    ammoType(ncGPWeaponClass::AMMO_MELEE)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPAmmunitionClass::~ncGPAmmunitionClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPAmmunitionClass::SaveCmds( nPersistServer * ps )
{
    if( ! nComponentClass::SaveCmds(ps) )
    {
        return false;
    }

    ps->Put( this->GetEntityClass(), 'LSAT', this->GetAmmoType() );

    return true;
}

//------------------------------------------------------------------------------
