//------------------------------------------------------------------------------
//  2.0/default_colr_pntl.fx
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 Model;
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAmbient;
float4   LightDiffuse;
float4   LightSpecular;
float4   ModelLightPos;
float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

texture DiffMap0;
texture BumpMap0;

texture CubeMap0;
texture envMaskMap;
float   envMaskFactor;

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

#include "shaders:../lib/environmentsampler.fx"
#include "shaders:../lib/environmentmask.fx"

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : NORMAL;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 reflect  : TEXCOORD1;
    float4 diffuse  : COLOR0;
    float4 specular : COLOR1;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    float3 lightVec = normalize(ModelLightPos - position);
    float3 eyeVec = normalize(ModelEyePos - position);
    float3 halfVec = normalize(lightVec + eyeVec);
    // vertex lighting
    float diffuse = saturate(dot(normal, lightVec));
    vsOut.diffuse = diffuse * LightDiffuse * MatDiffuse;
    float specular = pow(saturate(dot(normal, halfVec)), MatSpecularPower);
    vsOut.specular = specular * LightSpecular * MatSpecular;
    // environment mapping
    vsOut.reflect = mul(reflect(-eyeVec, normal), Model);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    float4 diffuse = psIn.diffuse;
    dstColor += diffuse; 
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    // diffuse environmentmap3d
    float envLerp = envMaskFactor;
    envLerp *= tex2D(EnvMask, psIn.uv0);
    dstColor.xyz = lerp(dstColor.xyz, texCUBE(EnvironmentSampler, psIn.reflect), envLerp) ;
    // per pixel specular lighting
    float4 specular = psIn.specular;
    float4 levelColor = MatLevel;
    dstColor += specular * levelColor;

    return dstColor;
}

VertexShader vsArray[1]= {compile vs_2_0  vsMain()};
technique t0
{
    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        
        VertexShader = (vsArray[0]);
        PixelShader  = compile ps_2_0 psMain();
    }
}
