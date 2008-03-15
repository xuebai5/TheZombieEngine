//------------------------------------------------------------------------------
//  fx_alph_quad.fx
//
//  standard shader to set an overlay quad covering the screen.
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection; 
texture DiffMap0;
float mipMapLodBias;
int funcDestBlend;

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
#include "shaders:../lib/diff0sampler.fx"

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain( const VS_INPUT vsIn )
{
    VS_OUTPUT vsOut;
    vsOut.position = float4(vsIn.position, 1.0f);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
float4 psMain(const VS_OUTPUT psIn) : COLOR
{
    return tex2D(Diff0Sampler, psIn.uv0);
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
        VertexShader     = compile vs_1_1 vsMain();
        PixelShader      = compile ps_1_1 psMain();
    }
}
