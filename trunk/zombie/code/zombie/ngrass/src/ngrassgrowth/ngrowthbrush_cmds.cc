#include "precompiled/pchngrass_conjurer.h"
//------------------------------------------------------------------------------
//  ngrowthbrush_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngrassgrowth/ngrowthbrush.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nGrowthBrush )
    NSCRIPT_ADDCMD('CSSP', void, SetSpawnProbability, 1, (float), 0, ());
    NSCRIPT_ADDCMD('CGSP', float , GetSpawnProbability, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSMI', void, SetMinScale, 1, (float), 0, ());
    NSCRIPT_ADDCMD('CGMI', float , GetMinScale, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSMA', void, SetMaxScale, 1, (float), 0, ());
    NSCRIPT_ADDCMD('CGMA', float , GetMaxScale, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSSB', void, SetMinDistanceSameBrush, 1, (float), 0, ());
    NSCRIPT_ADDCMD('CGSB', float , GetMinDistanceSameBrush, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSOB', void, SetMinDistanceOtherBrush, 1, (float), 0, ());
    NSCRIPT_ADDCMD('CGOB', float , GetMinDistanceOtherBrush, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSMT', void, SetModulateByTerrain, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('CGMT', bool , GetModulateByTerrain, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSAN', void, SetAssetName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('CGAN', const nString&, GetAssetName, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nGrowthBrush::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        ps->Put(this, 'CSSP', this->GetSpawnProbability());
        ps->Put(this, 'CSMI', this->GetMinScale());
        ps->Put(this, 'CSMA', this->GetMaxScale());
        ps->Put(this, 'CSSB', this->GetMinDistanceSameBrush());
        ps->Put(this, 'CSOB', this->GetMinDistanceOtherBrush());
        ps->Put(this, 'CSMT', this->GetModulateByTerrain());
        ps->Put(this, 'CSAN', this->GetAssetName().Get() );
        return true;
    }
    return false;
}
