//------------------------------------------------------------------------------
//  2.0/materials/mat0017_alph_2_pntl.fx
//  
//  Material description:
//      deform=wave
//      hasalpha=true
//      pro=fresnel
//      env=ambientlighting
//      env=diffuselighting
//      env=specularlighting
//      map=bumpmap
//      map=colormap
//      map=controlmap
//      map=environmentmap3d
//      map=levelmap
//      map=levelmapasenvmaskmap
//  Material case:
//      LightType=pntl
//  
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libtrag.fx"

float4   LightDiffuse;
float4   LightAmbient;
float4   LightSpecular;
float4   LightAttenuation;
float4   ModelLightPos;
int      LightType;
float4 matDiffuse;
float4 matSpecular;
float  matShininess;
float  matLevel;


shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

shared float4x4 Model;
int funcDestBlend = 6; // = InvSrcAlpha
float4 matAmbient;
float mipMapLodBias = 0.0f;
texture diffMap;
texture controlMap;
texture diffMap2;
texture CubeMap0;
float   envMaskFactor;
texture levelMap;
shared float Time;
float Amplitude;
float Frequency;
float4 Velocity;
float FresnelBias;
float FresnelPower;

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/controlsampler.fx"
#include "shaders:../lib/diffSecSampler.fx"
#include "shaders:../lib/environmentsampler.fx"
#include "shaders:../lib/levelsampler.fx"
struct VsInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float2 uv2      : TEXCOORD2;
    float3 color    : COLOR;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 uvenv         : TEXCOORD1;
    float3 eyeVec        : TEXCOORD2;
    float2 uv2           : TEXCOORD3;
    float3 lightVec      : TEXCOORD4;
    float3 halfVec       : TEXCOORD5;
};

VsOutput vsMain(const VsInput vsIn, uniform int lightType, uniform int lightStatic)
{
    VsOutput vsOut;
    float4 position = float4(vsIn.position, 1.0f);
    float3 normal   = vsIn.normal;
    float3 tangent = vsIn.tangent;
    float3 binormal  = vsIn.binormal;
    float3 eyePos = ModelEyePos;
    position = transformWave(position, vsIn.color, Frequency, Amplitude, Time, normal, tangent, binormal);
    vsOut.position = mul(position, ModelViewProjection);
    float3 eyeVec = normalize(eyePos - position);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv2 = vsIn.uv2;
    //the reflect invert the direction eyedir
    float3 refl = reflect(-eyeVec, normal);
    vsOut.uvenv = mul(refl, Model);
    vsOut.eyeVec = mul(float3x3(tangent, binormal, normal), eyeVec);
    // lighting
    float4 lightPos = ModelLightPos;
    float3 lightVec;
    float3 halfVec;
    if (lightType == LIGHT_DIR) {
        tangentSpaceLightHalfVectorDir(position, lightPos, eyePos, normal, binormal, tangent,
                                       lightVec, halfVec);
    } else {
        tangentSpaceLightHalfVector2(position, lightPos, eyePos, normal, binormal, tangent,
                                     lightVec, halfVec);
        lightVec *= distance(position, lightPos);
    }
    vsOut.lightVec = lightVec;
    vsOut.halfVec = halfVec;
    return vsOut;
}

float4 psMain_pass0(const VsOutput psIn, uniform int lightType, uniform bool lightStatic) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 uv0 = psIn.uv0;
    float3 tangentSurfaceNormal = float3(0.0f, 0.0f, 1.0f);
    tangentSurfaceNormal = normalize(tangentSurfaceNormal);
    float lightAtt = 1.0f;
    if (lightType == LIGHT_BULB) {
        float dist = length(psIn.lightVec);
        lightAtt = 1.0f - smoothstep(LightAttenuation.x, LightAttenuation.y, dist);
    }
    float facing = 1.0f - max(dot(normalize(psIn.eyeVec), tangentSurfaceNormal), 0.0f);
    float4 vertexMatAmbient = matAmbient;
    float4 vertexMatDiffuse = matDiffuse;
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(psIn.lightVec);
    float diffIntensity = dot(tangentSurfaceNormal, tangentLightVec);
    diffIntensity = saturate(diffIntensity);
    diffIntensity *= lightAtt;
    float4 diffuse = diffIntensity * LightDiffuse;
    // tangent space diffuse lighting
    diffuse *= vertexMatDiffuse;
    dstColor = diffuse;
    float4 ambient = LightAmbient;
    // ambient lighting
    dstColor += ambient * vertexMatAmbient;
    // restore alpha value
    dstColor.a = vertexMatAmbient.a;
    // blend map1 1 with map2 by controlmap, and apply
    dstColor *= lerp(tex2D(DiffSecSampler, uv0), tex2D(DiffSampler, uv0), tex2D(ControlSampler, psIn.uv2));
    float envLerp = envMaskFactor;
    envLerp *= tex2D(LevelSampler, uv0);
    // diffuse environmentmap3d
    dstColor.xyz = lerp(dstColor.xyz, texCUBE(EnvironmentSampler, psIn.uvenv), envLerp) ;
    float fresnel = FresnelBias + (1.0 - FresnelBias) * pow(facing, FresnelPower);
    dstColor *= fresnel;
    dstColor.a = fresnel * 0.5 + 0.5;
    return dstColor;
}

float4 psMain_pass1(const VsOutput psIn, uniform int lightType, uniform bool lightStatic) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 uv0 = psIn.uv0;
    float3 tangentSurfaceNormal = float3(0.0f, 0.0f, 1.0f);
    tangentSurfaceNormal = normalize(tangentSurfaceNormal);
    float lightAtt = 1.0f;
    if (lightType == LIGHT_BULB) {
        float dist = length(psIn.lightVec);
        lightAtt = 1.0f - smoothstep(LightAttenuation.x, LightAttenuation.y, dist);
    }
    float facing = 1.0f - max(dot(normalize(psIn.eyeVec), tangentSurfaceNormal), 0.0f);

    float4 levelColor = matLevel * tex2D(LevelSampler, uv0);
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(psIn.halfVec);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), matShininess);
    float4 specular = specIntensity * LightSpecular;
    specular *= matSpecular;
    dstColor += specular * levelColor;
    float fresnel = FresnelBias + (1.0 - FresnelBias) * pow(facing, FresnelPower);
    dstColor *= fresnel;
    dstColor.a = fresnel * 0.5 + 0.5;
    return dstColor;
}

VertexShader vsArray[4] = {
             compile vs_2_0 vsMain(LIGHT_POINT, true),
             compile vs_2_0 vsMain(LIGHT_SPOT,  false),
             compile vs_2_0 vsMain(LIGHT_DIR,   true),
             compile vs_2_0 vsMain(LIGHT_BULB,  false)
};
PixelShader  psArray_pass0[4] = {
             compile ps_2_0 psMain_pass0(LIGHT_POINT, true),
             compile ps_2_0 psMain_pass0(LIGHT_SPOT,  false),
             compile ps_2_0 psMain_pass0(LIGHT_DIR,   true),
             compile ps_2_0 psMain_pass0(LIGHT_BULB,  false)
};
PixelShader  psArray_pass1[4] = {
             compile ps_2_0 psMain_pass1(LIGHT_POINT, true),
             compile ps_2_0 psMain_pass1(LIGHT_SPOT,  false),
             compile ps_2_0 psMain_pass1(LIGHT_DIR,   true),
             compile ps_2_0 psMain_pass1(LIGHT_BULB,  false)
};

technique t0
{
    pass p_CW_DIFF
    {
        CullMode         = CW;
        SrcBlend         = SrcAlpha;
        DestBlend        = <funcDestBlend>;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias        = 0.0f;
        
        VertexShader = (vsArray[LightType]);
        PixelShader  = (psArray_pass0[LightType]);
    }
    pass p_CW_SPEC
    {
        CullMode         = CW;
        SrcBlend         = One;
        DestBlend        = One;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias        = 0.0f;
        
        VertexShader = (vsArray[LightType]);
        PixelShader  = (psArray_pass1[LightType]);
    }
}
