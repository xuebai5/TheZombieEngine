#line 1 "default_inst_impostor.fx"
//------------------------------------------------------------------------------
//  2.0/default_inst_impostor.fx
//
//  Default color shader for impostor geometry
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/fog.fx"
#include "shaders:../lib/libtrag.fx"

shared float4x4 ModelViewProjection;
shared float4x4 InvModelView;               // the inverse model*view matrix
shared float4x4 ModelView;                  // the model*view matrix

float4 LightAmbient;
float4 LightDiffuse;
float4 LightSpecular;
float4 LightPos;

texture diffMap;
texture bumpMap;

float mipMapLodBias = 0.0f;

float Scale; // scale the vertex extrude by this value

int AlphaRef = 50;

float4 fogDistances;
float4 fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/bumpsampler.fx"

struct VsInput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float2 extrude      : TEXCOORD1; // the impostor corner offset
};

struct VsOutput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 lightVec     : TEXCOORD1;
    float  fog          : FOG;
};

VsOutput vsMain(const VsInput vsIn, uniform bool fog, uniform bool light)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    //
    float3 extrude  = float3(vsIn.extrude, 0.0f) * Scale;
    extrude         = mul(extrude, (float3x3) InvModelView);
    position.xyz   += extrude.xyz;
    //
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    
    if (fog)
    {
        vsOut.fog = linearFog(position, fogDistances, ModelView);
    }
    else
    {
        vsOut.fog = 0.0f;
    }
    
    if (light)
    {
        float3 lightVec = normalize(LightPos - position);
        vsOut.lightVec = mul(lightVec, (float3x3) ModelView);
    }
    else
    {
        vsOut.lightVec = float3(0.0f, 0.0f, 0.0f);
    }
    
    return vsOut;
}

float4 psMain(const VsOutput psIn, uniform bool light) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    if (light)
    {
        dstColor = LightAmbient;
        float4 normal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f;
        float diffIntensity = saturate(dot(normal, normalize(psIn.lightVec)));
        dstColor += diffIntensity * LightDiffuse;
    }
    
    dstColor *= tex2D(DiffSampler, psIn.uv0);

    return dstColor;
}

technique tDefault
{
    pass p0
    {
        VertexShader    = compile vs_2_0 vsMain(false, false);
        PixelShader     = compile ps_2_0 psMain(false);
    }
}

technique tPointLight
{
    pass p0
    {
        VertexShader    = compile vs_2_0 vsMain(false, true);
        PixelShader     = compile ps_2_0 psMain(true);
    }
}

technique tLinearFog
{
    pass p0
    {
        FogColor        = <fogColor>;

        VertexShader    = compile vs_2_0 vsMain(true, false);
        PixelShader     = compile ps_2_0 psMain(false);
    }
}

technique tLinearFog_PointLight
{
    pass p0
    {
        FogColor        = <fogColor>;

        VertexShader    = compile vs_2_0 vsMain(true, true);
        PixelShader     = compile ps_2_0 psMain(true);
    }
}
