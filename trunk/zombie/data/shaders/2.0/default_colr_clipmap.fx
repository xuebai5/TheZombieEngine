//------------------------------------------------------------------------------
//  2.0/default_colr_clipmap.fx
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4 matAmbient;
float4 matDiffuse;
float4 matSpecular;
float  matShininess;
float  matLevel;

int CullMode = 2;//CW

texture diffMap;
texture clipMap;

float mipMapLodBias = 0.0f;

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/clipsampler.fx"

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut = (VsOutput)0;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    dstColor = tex2D(DiffSampler, psIn.uv0);
    float clipValue = tex2D(ClipSampler, psIn.uv0).r;
    dstColor.a = clipValue;
    return dstColor;
}

technique t0
{
    pass p0
    {
        CullMode         = <CullMode>;
        //AlphaTestEnable  = False;
        AlphaTestEnable  = True;
        AlphaRef         = 50;
        AlphaFunc        = GREATER;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
