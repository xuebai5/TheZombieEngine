//------------------------------------------------------------------------------
//  2.0/default_colr_pntl_mp_lib.fx
//  
//  vertex+pixel shader with multipass lighting
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAmbient[NumLights];
float4   LightDiffuse[NumLights];
float4   LightSpecular[NumLights];
float4   ModelLightPos[NumLights];

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

texture DiffMap0;
texture BumpMap0;

sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};

sampler BumpSampler = sampler_state
{
    Texture   = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT; 
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
};

struct VsOutput_diff
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 lightVec0     : TEXCOORD1;
    float3 halfVec0      : TEXCOORD2;
};

VsOutput
vsMain(const VsInput vsIn)
{
    VsOutput vsOut = (VsOutput)0;
    float4 position = vsIn.position;
    // ...geometry bullshit
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    dstColor = tex2D(DiffSampler, psIn.uv0);
    return dstColor;
}

VsOutput_diff
vsMain_diff(const VsInput vsIn, uniform int lightIndex)
{
    VsOutput_diff vsOut;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    float3 tangent  = vsIn.tangent;
    float3 binormal = cross(normal, tangent); 
    // ...geometry bullshit goes here
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    // tangent space lighting
    tangentSpaceLightHalfVector2(position, ModelLightPos[lightIndex], ModelEyePos, normal, binormal, tangent,
                                 vsOut.lightVec0, vsOut.halfVec0);
    return vsOut;
}

float4
psMain_diff(const VsOutput_diff psIn, uniform int lightIndex) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    // ambient lighting
    dstColor = LightAmbient[lightIndex] * MatAmbient;
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(psIn.lightVec0);
    float diffIntensity = saturate(dot(tangentSurfaceNormal, tangentLightVec));
    float4 diffuse = diffIntensity * LightDiffuse[lightIndex] * MatDiffuse;
    dstColor += diffuse;
    return dstColor;
}

float4
psMain_spec(const VsOutput_diff psIn, uniform int lightIndex) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(psIn.halfVec0);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular[lightIndex] * MatSpecular; 
    float4 levelColor = MatLevel;
    dstColor += specular * levelColor;
    return dstColor;
}
