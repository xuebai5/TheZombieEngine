//------------------------------------------------------------------------------
//  nshaderstate.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "gfx2/nshaderstate.h"
#include <string.h>

/// the shader parameter type string table
static const char* TypeTable[nShaderState::NumTypes] = 
{
    "v",
    "b",
    "i",
    "f",
    "f4",
    "m44",
    "t"
};

/// the shader parameter name string table
static const char* StateTable[nShaderState::NumParameters] = 
{
    "Model",
    "InvModel",
    "View",
    "InvView",
    "Projection",
    "ModelView",
    "InvModelView",
    "ModelViewProjection",
    "ModelLightProjection",
    "ModelShadowProjection",
    "ShadowProjectionDeform",
    "EyePos",
    "ModelEyePos",
    "ModelLightPos",
    "ModelLightDir",
    "LightPos",
    "LightDir",
    "LightType",
    "LightRange",
    "LightAmbient",
    "LightDiffuse",
    "LightDiffuse1",
    "LightSpecular",
    "LightMapAmbientFactor",
    "LightMapDiffuseFactor",
    "MatAmbient",
    "MatDiffuse",
    "MatEmissive",
    "MatEmissiveIntensity",
    "MatSpecular",
    "MatSpecularPower",
    "MatTransparency",
    "MatFresnel",
    "MatLevel",
    "Scale",
    "Noise",
    "MatTranslucency",
    "AlphaRef",
    "CullMode",
    "FillMode",
    "DirAmbient",
    "FogDistances",
    "FogColor",
    "LightAttenuation",
    "EmissiveMap0",
    "DiffSecMap0",
    "ClipMap0",
    "ControlMap2",
    "DiffMap0",
    "DiffMap1",
    "DiffMap2",
    "DiffMap3",
    "SpecMap0",
    "SpecMap1",
    "SpecMap2",
    "SpecMap3",
    "AmbientMap0",
    "AmbientMap1",
    "AmbientMap2",
    "AmbientMap3",
    "BumpMap0",
    "BumpMap1",
    "BumpMap2",
    "BumpMap3",
    "CubeMap0",
    "CubeMap1",
    "CubeMap2",
    "CubeMap3",
    "NoiseMap0",
    "NoiseMap1",
    "NoiseMap2",
    "NoiseMap3",
    "LightModMap",
    "ShadowMap",
    "SpecularMap",
    "ShadowModMap",
    "JointPalette",
    "Time",
    "Wind",
    "Swing",
    "InnerLightIntensity",
    "OuterLightIntensity",
    "BoxMinPos",
    "BoxMaxPos",
    "BoxCenter",
    "MinDist",
    "MaxDist",
    "SpriteSize",
    "MinSpriteSize",
    "MaxSpriteSize",
    "SpriteSwingAngle",
    "SpriteSwingTime",
    "SpriteSwingTranslate",
    "DisplayResolution",
    "TexGenS",
    "TexGenT",
    "TexGenR",
    "TexGenQ",
    "TextureTransform0",
    "TextureTransform1",
    "TextureTransform2",
    "TextureTransform3",
    "SampleOffsets", 
    "SampleWeights",
    "VertexStreams",
    "VertexWeights1",
    "VertexWeights2",
    "AlphaBlendEnable",
    "AlphaSrcBlend",
    "AlphaDstBlend",
    "MipMapLodBias",
    "BumpScale",
    "FresnelBias",
    "FresnelPower",
    "Intensity0",
    "Intensity1",
    "Intensity2",
    "Intensity3",
    "Amplitude",
    "Frequency",
    "Velocity",
    "StencilFrontZFailOp",
    "StencilFrontPassOp",
    "StencilBackZFailOp",
    "StencilBackPassOp",
    "ZWriteEnable",
    "ZEnable",
    "ShadowIndex",
    "CameraFocus",
    "Color0",
    "Color1",
    "Color2",
    "Color3",
    "HalfPixelSize",
    "TerrainAmbientFactor",
    "TerrainDiffuseFactor",
    "TerrainSideSizeInv",
    "TerrainGlobalMap",
    "TerrainGlobalMapScale",
    "TerrainModulationMap",
    "TerrainModulationFactor",
    "TerrainCellDistSquaredInv",
    "TerrainCellPosition",
    "TerrainCellWeightMap",
    "TerrainCellMaterial0",
    "TerrainCellMaterial1",
    "TerrainCellMaterial2",
    "TerrainCellMaterial3",
    "TerrainCellMaterial4",
    "TerrainGrassEditionMap",
    "TerrainGrassPalette",
    "TerrainGrassMinDist",
    "TerrainGrassMaxDist",
    "TerrainCellTransform0U",
    "TerrainCellTransform1U",
    "TerrainCellTransform2U",
    "TerrainCellTransform3U",
    "TerrainCellTransform4U",
    "TerrainCellTransform0V",
    "TerrainCellTransform1V",
    "TerrainCellTransform2V",
    "TerrainCellTransform3V",
    "TerrainCellTransform4V",
    "TerrainCellMaxMinPos",
    "TerrainShadowMapInfo",
    "GlobalLightMap",
    "InstPositionPalette",
    "InstRotationPalette",
    //<OBSOLETE>
    "lightMap",
    "matLevel",
    "matShininess",
    "bumpMap",
    "clipMap",
    "controlMap",
    "envMaskMap",
    "envMaskFactor",
    "parallaxMap",
    "parallaxFactor",
    "parallaxFactorRatioUV",
    "diffMap",
    "diffMap2",
    "illuminationMap",
    "levelMap",
    "illuminationColor",
    "matAmbient",
    "matDiffuse",
    "matSpecular",
    "funcDestBlend",
    "funcSrcBlend",
    "mipMapLodBias",
    "fogColor",
    "fogDistances",
    "isSkinned",
    //</OBSOLETE>
    "ShadowProjection",
};

//------------------------------------------------------------------------------
/**
*/
const char*
nShaderState::TypeToString(nShaderState::Type t)
{
    n_assert((t >= 0) && (t < nShaderState::NumTypes));
    return TypeTable[t];
}

//------------------------------------------------------------------------------
/**
*/
nShaderState::Type
nShaderState::StringToType(const char* str)
{
    n_assert(str);
    int i;
    for (i = 0; i < nShaderState::NumTypes; i++)
    {
        if (0 == strcmp(str, TypeTable[i]))
        {
            return (nShaderState::Type) i;
        }
    }
    // fallthrough: state not found 
    return nShaderState::InvalidType;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nShaderState::ParamToString(nShaderState::Param p)
{
    n_assert((p >= 0) && (p < nShaderState::NumParameters));
    return StateTable[p];
}

//------------------------------------------------------------------------------
/**
*/
nShaderState::Param
nShaderState::StringToParam(const char* str)
{
    n_assert(str);
    int i;
    for (i = 0; i < nShaderState::NumParameters; i++)
    {
        if (0 == strcmp(str, StateTable[i]))
        {
            return (nShaderState::Param) i;
        }
    }
    // fallthrough: state not found 
    return nShaderState::InvalidParameter;
}
