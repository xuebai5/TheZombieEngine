//------------------------------------------------------------------------------
//  2.0/zombie/instanced.fx
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
//shared float3   ModelEyePos;
//shared float3   EyePos;

float4 InstPositionPalette[72];
float4 InstRotationPalette[72];

float4 matAmbient;
float4 matDiffuse;
float4 matSpecular;
float  matShininess;
float  matLevel;

texture diffMap;
float mipMapLodBias;

#include "shaders:../lib/diffsampler.fx"

struct VsInput
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float4 instIndex    : BLENDINDICES;
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
    position = transformInstanced(position, InstPositionPalette[vsIn.instIndex.x], InstRotationPalette[vsIn.instIndex.x]);
    vsOut.position = mul(position, ModelViewProjection);
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

technique tDefaultInstanced
{
    pass p0
    {        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader = compile ps_2_0 psMain();
    }
}
