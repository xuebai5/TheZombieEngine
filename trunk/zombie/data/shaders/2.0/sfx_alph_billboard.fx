//------------------------------------------------------------------------------
//  2.0/sfx_alph_billboard.fx
//  (C) 2006 Tragnarion Studios
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/bill.fx"
//------------------------------------------------------------------------------
// parameters
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float4x4 InvModelView;
shared float4   ModelEyePos;
shared float4x4 TextureTransform0;

float Scale = 1.0f;

float4 matDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);

texture diffMap;

float mipMapLodBias = 0.0f;

//------------------------------------------------------------------------------
// i/o structs
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float2 uv3      : TEXCOORD3;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

//------------------------------------------------------------------------------
// texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
// vertex shader
//------------------------------------------------------------------------------
VsOutput
vsh_vs20_alpha(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    
    float3 extrude  = float3(vsIn.uv3, 0.0f) * Scale;
    extrude         = mul(extrude, (float3x3) InvModelView);
    position.xyz   += extrude.xyz; 
    
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = mul(float3(vsIn.uv0, 1.0f), (float3x3) TextureTransform0);
    return vsOut;
}
//------------------------------------------------------------------------------
// pixel shader
//------------------------------------------------------------------------------
float4
psh_ps20_alpha(const VsOutput psIn) : COLOR0
{
    float4 dstColor = tex2D(DiffSampler, psIn.uv0);
    dstColor.a = dstColor.r;//TEMP- while there is no alpha channel
    return dstColor;
}
//------------------------------------------------------------------------------
// techniques
//------------------------------------------------------------------------------
technique tAlphaBillboard
{
    pass p0
    {
        AlphaTestEnable     = False;
        CullMode            = None;
        FogEnable           = False;
        VertexShader        = compile vs_2_0 vsh_vs20_alpha();
        PixelShader         = compile ps_2_0 psh_ps20_alpha();
    }
}
