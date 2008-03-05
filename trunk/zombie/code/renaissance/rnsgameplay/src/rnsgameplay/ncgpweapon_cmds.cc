//------------------------------------------------------------------------------
//  ncgpweapon_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"
#include "rnsgameplay/ncgpweapon.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPWeapon)
    NSCRIPT_ADDCMD_COMPOBJECT('LG01', float, GetAccuracy , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LG09', float, GetRecoilDeviationX , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LG10', float, GetRecoilDeviationY , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LG11', float, GetRecoilTime , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LG16', float, GetRange , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LG18', float, GetDamage , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LG22', int, GetClipSize , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGAT', int, GetAmmoType , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSAM', void, SetAmmo, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGAM', int, GetAmmo , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSAX', void, SetAmmoExtra, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGAX', int, GetAmmoExtra , 0, (), 0, ());
/** ZOMBIE REMOVE
    NSCRIPT_ADDCMD_COMPOBJECT('LAWA', int, AddAddon, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LHWA', bool, HasAddon, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGWA', nEntityObject *, GetAddon, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LRWA', nEntityObject *, RemoveAddon, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LRAA', nEntityObject *, RemoveAddonAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LDEA', void, DeleteAllAddons , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LNLS', int, GetNumSlots , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGAA', nEntityObject *, GetAddonAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LIAL', bool, IsAllowed, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LIMN', bool, IsMounted, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LAWT', bool, HasTrait, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGSN', const char*, GetSlotName, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGSF', int, GetSlotFor, 1, (nEntityObject *), 0, ());
*/
    NSCRIPT_ADDCMD_COMPOBJECT('LLSS', void, ApplyAccuracyLoss , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LLRC', void, ApplyAccuracyRecover, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSIR', void, SetIronsight, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGIR', bool, GetIronsight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSMV', void, SetMoving, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGMC', bool, GetMoving , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSCR', void, SetCrouch, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGCR', bool, GetCrouch , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSPR', void, SetProne, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGPR', bool, GetProne , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGRF', float, GetRateOfFire , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSFM', void, SetFireMode, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGFM', int, GetFireMode , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGDO', float, GetDownOffset , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LHFA', bool, HasFullAmmo , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
