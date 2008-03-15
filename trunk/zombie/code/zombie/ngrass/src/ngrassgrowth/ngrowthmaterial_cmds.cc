#include "precompiled/pchngrass_conjurer.h"
//------------------------------------------------------------------------------
//  ngrowthmaterial_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngrassgrowth/ngrowthmaterial.h"
#include "ngrassgrowth/ngrowthbrush.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nGrowthMaterial )
    NSCRIPT_ADDCMD('CSCL', void, SetColor, 1, (const vector3 &), 0, ());
    NSCRIPT_ADDCMD('CGCL', const vector3 &, GetColor, 0, (), 0, ());
    NSCRIPT_ADDCMD('CCBR', nGrowthBrush*, CreateGrowthBrush, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('CGNB', int, GetNumberOfGrowthBrushes, 0, (), 0, ());
    NSCRIPT_ADDCMD('CGGB', nGrowthBrush*, GetGrowthBrush, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CDGB', void, DeleteGrowthBrush, 1, (int), 0, ()) ;
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nGrowthMaterial::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        ps->Put( this, 'CSCL' , this->color.x , this->color.y , this->color.z );
        //ps->Put(this, 'CSCL', this->GetColor());
        return true;
    }
    return false;
}

