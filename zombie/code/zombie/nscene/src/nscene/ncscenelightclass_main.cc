#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscenelightenv_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscenelightclass.h"
#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSceneLightClass,ncSceneClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneLightClass)
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPCLASS('MSLY', void, SetLightType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLY', int, GetLightType, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSceneLightClass::ncSceneLightClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneLightClass::~ncSceneLightClass()
{
    // empty
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncSceneLightClass::SetLightType(int lightType)
{
    if (lightType == -1)
    {
        this->shaderOverrides.ClearArg(nShaderState::LightType);
    }
    else
    {
        this->shaderOverrides.SetArg(nShaderState::LightType, nShaderArg(lightType));
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneLightClass::GetLightType()
{
    if (this->shaderOverrides.IsParameterValid(nShaderState::LightType))
    {
        return this->shaderOverrides.GetArg(nShaderState::LightType).GetInt();
    }
    return -1;//NONE- this is to identify Fog and other things
}
#endif
