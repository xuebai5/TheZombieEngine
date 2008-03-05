#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscenelight_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscenelight.h"
#include "nscene/ncscenelightclass.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSceneLight,ncScene);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneLight)
    NSCRIPT_ADDCMD_COMPOBJECT('MSLS', void, SetLightStatic, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGLS', bool, GetLightStatic, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSDL', void, SetLightDynamic, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGDL', bool, GetLightDynamic, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('MSAC', void, SetLightAmbient, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGAC', void, GetLightAmbient, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSDC', void, SetLightDiffuse, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGDC', void, GetLightDiffuse, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSSC', void, SetLightSpecular, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGSC', void, GetLightSpecular, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('CSAF', void, SetLightMapAmbientFactor , 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CGAF', float, GetLightMapAmbientFactor, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CSDF', void, SetLightMapDiffuseFactor , 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CGDF', float, GetLightMapDiffuseFactor, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CSTF', void, SetTerrainAmbientFactor , 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CGTF', float, GetTerrainAmbientFactor, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CSTD', void, SetTerrainDiffuseFactor , 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CGTD', float, GetTerrainDiffuseFactor, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSFC', void, SetFogColor, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGFC', void, GetFogColor, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSPR', void, SetPointLightRadius, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGPR', void, GetPointLightRadius, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSCA', void, SetConeLightAngles, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGCA', void, GetConeLightAngles, 0, (), 2, (float&, float&));
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSceneLight::ncSceneLight() :
    lightFlags(AllLightFlags),
    lightPriority(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneLight::~ncSceneLight()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLight::Load()
{
    if (!ncScene::Load())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLight::Render(nSceneGraph* sceneGraph)
{
    this->lightTypes.Reset();

    ncScene::Render(sceneGraph);
}

//------------------------------------------------------------------------------
/**
    check if the entity is affected by this light. This is intended for
    subclasses to perform additional checks using visibility, collision, etc.
    @return whether the target entity is affected by this entity or not
*/
bool
ncSceneLight::AffectsEntity(nEntityObject* /*entityObject*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLight::SaveCmds(nPersistServer* ps)
{
    if (ncScene::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setlightstatic ---
        if (!this->GetLightStatic())
        {
            cmd = ps->GetCmd(this->GetEntityObject(), 'MSLS');
            cmd->In()->SetB(this->GetLightStatic());
            ps->PutCmd(cmd);
        }

        // --- setlightdynamic ---
        if (!this->GetLightDynamic())
        {
            cmd = ps->GetCmd(this->GetEntityObject(), 'MSDL');
            cmd->In()->SetB(this->GetLightDynamic());
            ps->PutCmd(cmd);
        }

        return true;
    }

    return false;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    helper methods for light properties inspector
*/
void
ncSceneLight::GetVectorOverride(nShaderState::Param param, float& x, float& y, float& z, float& w)
{
    nShaderParams& shaderParams = this->GetShaderOverrides();
    if (shaderParams.IsParameterValid(param))
    {
        const vector4& v = shaderParams.GetArg(param).GetVector4();
        x = v.x; y = v.y; z = v.z; w = v.w;
    }
    else
    {
        x = y = z = w = 0.0f;
    }
}

void
ncSceneLight::GetColorOverride(nShaderState::Param param, float& r, float& g, float& b)
{
    float unused;
    this->GetVectorOverride(param, r, g, b, unused);
}

void
ncSceneLight::SetLightAmbient(const vector3& color)
{
    this->GetShaderOverrides().SetArg(nShaderState::LightAmbient, nShaderArg(color));
}

void
ncSceneLight::GetLightAmbient(float& r, float& g, float& b)
{
    this->GetColorOverride(nShaderState::LightAmbient, r, g, b);
}

void
ncSceneLight::SetLightDiffuse(const vector3& color)
{
    this->GetShaderOverrides().SetArg(nShaderState::LightDiffuse, nShaderArg(color));
}

float
ncSceneLight::GetLightMapAmbientFactor()
{
    nShaderParams& shaderParams = this->GetShaderOverrides();
    if (shaderParams.IsParameterValid(nShaderState::LightMapAmbientFactor))
    {
        return shaderParams.GetArg(nShaderState::LightMapAmbientFactor).GetFloat();
    }
    else
    {
        return 0;
    }
}

void
ncSceneLight::SetLightMapAmbientFactor(float v )
{
    this->GetShaderOverrides().SetArg(nShaderState::LightMapAmbientFactor, nShaderArg(v) );
}


float
ncSceneLight::GetTerrainAmbientFactor()
{
    nShaderParams& shaderParams = this->GetShaderOverrides();
    if (shaderParams.IsParameterValid(nShaderState::TerrainAmbientFactor))
    {
        return shaderParams.GetArg(nShaderState::TerrainAmbientFactor).GetFloat();
    }
    else
    {
        return 0;
    }
}

void
ncSceneLight::SetTerrainAmbientFactor(float v )
{
    this->GetShaderOverrides().SetArg(nShaderState::TerrainAmbientFactor, nShaderArg(v) );
}

float
ncSceneLight::GetTerrainDiffuseFactor()
{
    nShaderParams& shaderParams = this->GetShaderOverrides();
    if (shaderParams.IsParameterValid(nShaderState::TerrainDiffuseFactor))
    {
        return shaderParams.GetArg(nShaderState::TerrainDiffuseFactor).GetFloat();
    }
    else
    {
        return 0;
    }
}

void
ncSceneLight::SetTerrainDiffuseFactor(float v )
{
    this->GetShaderOverrides().SetArg(nShaderState::TerrainDiffuseFactor, nShaderArg(v) );
}

float
ncSceneLight::GetLightMapDiffuseFactor()
{
    nShaderParams& shaderParams = this->GetShaderOverrides();
    if (shaderParams.IsParameterValid(nShaderState::LightMapAmbientFactor))
    {
        return shaderParams.GetArg(nShaderState::LightMapDiffuseFactor).GetFloat();
    }
    else
    {
        return 0;
    }
}

void
ncSceneLight::SetLightMapDiffuseFactor(float v )
{
    this->GetShaderOverrides().SetArg(nShaderState::LightMapDiffuseFactor, nShaderArg(v) );
}


void
ncSceneLight::GetLightDiffuse(float& r, float& g, float& b)
{
    this->GetColorOverride(nShaderState::LightDiffuse, r, g, b);
}

void
ncSceneLight::SetLightSpecular(const vector3& color)
{
    this->GetShaderOverrides().SetArg(nShaderState::LightSpecular, nShaderArg(color));
}

void
ncSceneLight::GetLightSpecular(float& r, float& g, float& b)
{
    this->GetColorOverride(nShaderState::LightSpecular, r, g, b);
}

void
ncSceneLight::SetFogColor(const vector3& color)
{
    this->GetShaderOverrides().SetArg(nShaderState::fogColor, nShaderArg(color));
}

void
ncSceneLight::GetFogColor(float& r, float& g, float& b)
{
    this->GetColorOverride(nShaderState::fogColor, r, g, b);
}

void
ncSceneLight::SetPointLightRadius(float innerRadius, float outerRadius)
{
    this->GetShaderOverrides().SetArg(nShaderState::LightAttenuation, nShaderArg(vector4(innerRadius, outerRadius, 0.0f, 0.0f)));
}

void
ncSceneLight::GetPointLightRadius(float& innerRadius, float& outerRadius)
{
    float unused0, unused1;
    this->GetVectorOverride(nShaderState::LightAttenuation, innerRadius, outerRadius, unused0, unused1);
}

void
ncSceneLight::SetConeLightAngles(float innerAngle, float outerAngle)
{
    this->GetShaderOverrides().SetArg(nShaderState::LightAttenuation, nShaderArg(vector4(0.0f, 0.0f, n_cos(innerAngle), n_cos(outerAngle))));
}

void
ncSceneLight::GetConeLightAngles(float& innerAngle, float& outerAngle)
{
    float unused0, unused1;
    this->GetVectorOverride(nShaderState::LightAttenuation, unused0, unused1, innerAngle, outerAngle);
}
#endif
