//------------------------------------------------------------------------------
//  filter_overlay.fx
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
texture DiffMap0;
texture DiffMap1;
texture DiffMap2;
float mipMapLodBias;
float4 Color0 = float4(1.0f, 1.0f, 1.0f, 1.0f);
float3 SpriteSwingTranslate = float3(0.0f, 0.0f, 0.0f);
int funcDestBlend;
//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 color    : COLOR0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 color    : COLOR0;
};

#include "shaders:../lib/diff0sampler.fx"
#include "shaders:../lib/diff1sampler.fx"
#include "shaders:../lib/diff2sampler.fx"
//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain( const VS_INPUT vsIn )
{
    VS_OUTPUT vsOut;
    float4 position = float4( vsIn.position, 1.0f );
    position = position - float4(SpriteSwingTranslate, 0.0f);
    vsOut.position = position;
    vsOut.uv0 = vsIn.uv0;
    vsOut.color = vsIn.color;
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
float4 psMain(const VS_OUTPUT psIn) : COLOR
{
    float4 diffColor0 = tex2D(Diff0Sampler, psIn.uv0) * psIn.color.r * Color0.r;
    float4 diffColor1 = tex2D(Diff1Sampler, psIn.uv0) * psIn.color.g * Color0.g;
    float4 diffColor2 = tex2D(Diff2Sampler, psIn.uv0) * psIn.color.b * Color0.b;
    float4 diffColor = diffColor0 + diffColor1 + diffColor2;

    return diffColor;
    //return float4(1.0f, 0.0f, 0.0f, 0.0f);
}

//------------------------------------------------------------------------------
//  the technique
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        SrcBlend         = SrcAlpha;
        DestBlend        = <funcDestBlend>; 
        AlphaTestEnable  = False;
        CullMode         = None;
        VertexShader     = compile vs_1_1 vsMain();
        PixelShader      = compile ps_2_0 psMain();
    }
}
