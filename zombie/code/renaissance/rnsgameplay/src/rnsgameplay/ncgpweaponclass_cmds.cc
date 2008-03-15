//------------------------------------------------------------------------------
//  ncgpweaponclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPWeaponClass)
    NSCRIPT_ADDCMD_COMPCLASS('LS31', void, SetRateOfFire, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG31', float, GetRateOfFire , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS32', void, SetRateOfTapFire, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG32', float, GetRateOfTapFire , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS34', void, SetCockTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG34', float, GetCockTime , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS35', void, SetDamageType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG35', int, GetDamageType , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS36', void, SetAmmoType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG36', int, GetAmmoType , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS37', void, SetTypeShape, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG37', int, GetTypeShape , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS38', void, SetAmmoChamber, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG38', int, GetAmmoChamber , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS39', void, SetAmmoReq, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG39', int, GetAmmoReq , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS40', void, SetWeaponType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG40', int, GetWeaponType , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS41', void, SetAddonOffset, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG41', const vector3&, GetAddonOffset , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS42', void, SetAddonOffsetBone, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG42', int, GetAddonOffsetBone , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS43', void, SetFireModes, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG43', int, GetFireModes , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LHAF', bool, HasSemiFireMode , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LHSF', bool, HasAutoFireMode , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPWeaponClass::SaveCmds( nPersistServer * ps )
{
    if( ! this->ncGPWeaponCharsClass::SaveCmds( ps ) )
    {
        return false;
    }

    ps->Put( this->GetEntityClass(), 'LS31', this->GetRateOfFire() );
    ps->Put( this->GetEntityClass(), 'LS32', this->GetRateOfTapFire() );
    ps->Put( this->GetEntityClass(), 'LS34', this->GetCockTime() );
    ps->Put( this->GetEntityClass(), 'LS35', this->GetDamageType() );
    ps->Put( this->GetEntityClass(), 'LS36', this->GetAmmoType() );
    ps->Put( this->GetEntityClass(), 'LS37', this->GetTypeShape() );
    ps->Put( this->GetEntityClass(), 'LS38', this->GetAmmoChamber() );
    ps->Put( this->GetEntityClass(), 'LS39', this->GetAmmoReq() );
    ps->Put( this->GetEntityClass(), 'LS40', this->GetWeaponType() );
    vector3 vector( this->GetAddonOffset() );
    ps->Put( this->GetEntityClass(), 'LS41', vector.x, vector.y, vector.z );
    ps->Put( this->GetEntityClass(), 'LS42', this->GetAddonOffsetBone() );
    ps->Put( this->GetEntityClass(), 'LS43', this->GetFireModes() );

    return true;
}

//------------------------------------------------------------------------------
