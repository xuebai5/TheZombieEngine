//------------------------------------------------------------------------------
//  ncgpexplosionclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpexplosionclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPExplosionClass)
    NSCRIPT_ADDCMD_COMPCLASS('SEXR', void, SetExplosionRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GEXR', float, GetExplosionRadius , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SEXP', void, SetExplosionPower, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GEXP', float, GetExplosionPower , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SEXD', void, SetExplosionDamage, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GEXD', float, GetExplosionDamage, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPExplosionClass::SaveCmds( nPersistServer * ps )
{
    ps->Put( this->GetEntityClass(), 'SEXR', this->GetExplosionRadius() );
    ps->Put( this->GetEntityClass(), 'SEXP', this->GetExplosionPower() );
    ps->Put( this->GetEntityClass(), 'SEXD', this->GetExplosionDamage() );

    return true;
}

//------------------------------------------------------------------------------
