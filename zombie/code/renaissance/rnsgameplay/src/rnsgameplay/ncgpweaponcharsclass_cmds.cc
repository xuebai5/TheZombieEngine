//------------------------------------------------------------------------------
//  ncgpweaponcharsclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponcharsclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPWeaponCharsClass)
    NSCRIPT_ADDCMD_COMPCLASS('LS01', void, SetAccuracyMin, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG01', float, GetAccuracyMin , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS02', void, SetAccuracyMax, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG02', float, GetAccuracyMax , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS03', void, SetAccuracyLoss, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG03', float, GetAccuracyLoss , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS04', void, SetAccuracyProne, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG04', float, GetAccuracyProne , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS05', void, SetAccuracyCrouch, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG05', float, GetAccuracyCrouch , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS06', void, SetAccuracyMove, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG06', float, GetAccuracyMove , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS07', void, SetAccuracyRun, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG07', float, GetAccuracyRun , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS08', void, SetAccuracyIronsight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG08', float, GetAccuracyIronsight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS09', void, SetAccuracyAngle, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG09', float, GetAccuracyAngle , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS10', void, SetRecoilDeviationX, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG10', float, GetRecoilDeviationX , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS11', void, SetRecoilDeviationY, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG11', float, GetRecoilDeviationY , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS12', void, SetRecoilTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG12', float, GetRecoilTime , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS13', void, SetRecoilDeviationProneX, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG13', float, GetRecoilDeviationProneX , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS14', void, SetRecoilDeviationProneY, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG14', float, GetRecoilDeviationProneY , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS15', void, SetAimSpeed, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG15', float, GetAimSpeed , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS16', void, SetAimSpeedProne, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG16', float, GetAimSpeedProne , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS17', void, SetRange, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG17', float, GetRange , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS18', void, SetMinRange, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG18', float, GetMinRange , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS19', void, SetDamage, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG19', float, GetDamage , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS20', void, SetDamageHalfRange, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG20', float, GetDamageHalfRange , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS22', void, SetShootVolume, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG22', float, GetShootVolume , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LS23', void, SetClipSize, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LG23', int, GetClipSize , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSTR', void, SetTraits, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGTR', int, GetTraits , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LATR', void, AddTrait, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LRTR', void, RemoveTrait, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LAWT', bool, AllowTrait, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSSL', void, SetSlots, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGSL', int, GetSlots , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LASL', void, AddSlot, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LAWA', bool, AllowType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LRMT', void, RemoveSlot, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPWeaponCharsClass::SaveCmds( nPersistServer * ps )
{
    if( ! this->ncGameplayClass::SaveCmds( ps ) )
    {
        return false;
    }

    ps->Put( this->GetEntityClass(), 'LS01', this->GetAccuracyMin() );
    ps->Put( this->GetEntityClass(), 'LS02', this->GetAccuracyMax() );
    ps->Put( this->GetEntityClass(), 'LS03', this->GetAccuracyLoss() );
    ps->Put( this->GetEntityClass(), 'LS04', this->GetAccuracyProne() );
    ps->Put( this->GetEntityClass(), 'LS05', this->GetAccuracyCrouch() );
    ps->Put( this->GetEntityClass(), 'LS06', this->GetAccuracyMove() );
    ps->Put( this->GetEntityClass(), 'LS07', this->GetAccuracyRun() );
    ps->Put( this->GetEntityClass(), 'LS08', this->GetAccuracyIronsight() );
    ps->Put( this->GetEntityClass(), 'LS09', this->GetAccuracyAngle() );
    ps->Put( this->GetEntityClass(), 'LS10', this->GetRecoilDeviationX() );
    ps->Put( this->GetEntityClass(), 'LS11', this->GetRecoilDeviationY() );
    ps->Put( this->GetEntityClass(), 'LS12', this->GetRecoilTime() );
    ps->Put( this->GetEntityClass(), 'LS13', this->GetRecoilDeviationProneX() );
    ps->Put( this->GetEntityClass(), 'LS14', this->GetRecoilDeviationProneY() );
    ps->Put( this->GetEntityClass(), 'LS15', this->GetAimSpeed() );
    ps->Put( this->GetEntityClass(), 'LS16', this->GetAimSpeedProne() );
    ps->Put( this->GetEntityClass(), 'LS17', this->GetRange() );
    ps->Put( this->GetEntityClass(), 'LS18', this->GetMinRange() );
    ps->Put( this->GetEntityClass(), 'LS19', this->GetDamage() );
    ps->Put( this->GetEntityClass(), 'LS20', this->GetDamageHalfRange() );
    ps->Put( this->GetEntityClass(), 'LS22', this->GetShootVolume() );
    ps->Put( this->GetEntityClass(), 'LS23', this->GetClipSize() );

    ps->Put( this->GetEntityClass(), 'LSTR', this->GetTraits() );
    ps->Put( this->GetEntityClass(), 'LSSL', this->GetSlots() );

    return true;
}

//------------------------------------------------------------------------------
