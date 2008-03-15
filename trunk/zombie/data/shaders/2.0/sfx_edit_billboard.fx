//------------------------------------------------------------------------------
//  2.0/sfx_alph_billboard.fx
//  (C) 2006 Conjurer Services, S.A.
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
VsOutput vsMain(const VsInput vsIn)
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
float4 psMain(const VsOutput psIn) : COLOR0
{
    return tex2D(DiffSampler, psIn.uv0);
}
//------------------------------------------------------------------------------
// techniques
//------------------------------------------------------------------------------
technique tEditBillboard
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        CullMode         = None;
        StencilEnable    = False;

        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        
        AlphaTestEnable  = False;
        AlphaFunc        = GreaterEqual;
		FogEnable        = False;

        VertexShader     = compile vs_2_0 vsMain();
        PixelShader      = compile ps_2_0 psMain();
    }
}
