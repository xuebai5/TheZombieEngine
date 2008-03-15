//------------------------------------------------------------------------------
//  ps2.0/gmmt_glot_lnrf.fx
//
//  Geomipmap terrain, global texturing, linear fog
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"
#include "../lib/fog.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
int             CullMode = 2;       // CW

float4 fogDistances;
float4 fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };

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
    float fog : FOG;
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
    
    // fog   
    vsOut.fog = linearFog(vsIn.position, fogDistances, ModelView);    
    
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

    return baseColor;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        //FogEnable           = True;
        fogColor            = <fogColor>;
        //FogVertexMode       = Linear;
        //FogTableMode        = None;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
