#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplayvehicle_cmds.cc
//  (C) Conjurer Services, S.A. 2005
//------------------------------------------------------------------------------

#include "ncgameplayvehicle/ncgameplayvehicle.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayVehicle)
    NSCRIPT_ADDCMD_COMPOBJECT('ISVH', void, SetHealth, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGVH', int, GetHealth, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTS', void, SetSeat, 2, (const unsigned int,const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGTS', nEntityObject*, GetSeat, 1, (const unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESVB', void, SetBattery, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGVB', const bool, GetBattery, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplayVehicle::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- health
        ps->Put (this->entityObject, 'ISVH', this->health);
        // -- battery
        ps->Put (this->entityObject, 'ESVB', this->battery);
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
