//------------------------------------------------------------------------------
//  ps2.0/gmmt_glot.fx
//
//  Geomipmap terrain, global texturing
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
int             CullMode = 2;       // CW

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION0;
    float3 normal   : NORMAL;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_glot_sampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // get projected position
    vsOut.position = mul(float4(vsIn.position,1), ModelViewProjection);
    
    // calculate global texture coordinates
    CalculateGMMGlobalUV( vsIn.position, TerrainGlobalMapScale, vsOut.uv0 );
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // get global texturing color
    float4 globalColor = GetGMMGlobalColor( GlobalSampler, psIn.uv0 );

    // modulate color
    float4 baseColor = ModulateGMMColor( globalColor, ModulationSampler, psIn.uv0, TerrainModulationFactor );
    
#ifndef LEVEL_EDITOR
    float4 lightmapColor = tex2D(GlobalLightmapSampler, psIn.uv0);
    baseColor *= float4(lightmapColor.a, lightmapColor.a, lightmapColor.a, lightmapColor.a);
#endif

    return baseColor;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        //FogEnable           = False;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
