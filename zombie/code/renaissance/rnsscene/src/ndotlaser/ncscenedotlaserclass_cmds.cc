//------------------------------------------------------------------------------
//  ncscenedotlaserclass_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsscene.h"
#include "ndotlaser/ncscenedotlaserclass.h"
nNebulaComponentClass(ncSceneDotLaserClass,ncSceneLightClass);
//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneDotLaserClass)
    NSCRIPT_ADDCMD_COMPCLASS('CSLP', void, SetLaserProjection, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGLP', void, GetLaserProjection, 0, (), 4, (float&, float&, float&, float&) );
    NSCRIPT_ADDCMD_COMPCLASS('CSLV', void, SetLaserProjectionScript, 1, (const vector4&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSLE', void, SetLaserEpsilon, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGLE', float,GetLaserEpsilon, 0, (), 0, () );
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncSceneDotLaserClass::SaveCmds(nPersistServer* ps)
{
    if (ncSceneLightClass::SaveCmds(ps))
    {
            ps->Put(this->GetEntityClass(), 'CSLV', 
                    this->laserProjection.x,
                    this->laserProjection.y, 
                    this->laserProjection.z, 
                    this->laserProjection.w);
            ps->Put(this->GetEntityClass(), 'CSLE', this->epsilon);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
