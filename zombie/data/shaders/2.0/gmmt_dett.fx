//------------------------------------------------------------------------------
//  ps2.0/gmmt_dett.fx
//
//  Geomipmap terrain, detail texturing
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float3   EyePos;             // camera position
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
    float3 uv1 : TEXCOORD1;     // weight map uv.xy, uv1.z is blend factor
    float2 uv2 : TEXCOORD2;
    float2 uv3 : TEXCOORD3;
    float2 uv4 : TEXCOORD4;
    float2 uv5 : TEXCOORD5;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_glot_sampler.fx"
#include "../lib/gmmt_dett_sampler.fx"

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

    // calculate detail texture coordinates
    CalculateGMMWeightUV( vsIn.position, TerrainCellPosition, TerrainSideSizeInv, vsOut.uv1.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform0U, TerrainCellTransform0V, vsOut.uv2.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform1U, TerrainCellTransform1V, vsOut.uv3.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform2U, TerrainCellTransform2V, vsOut.uv4.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform3U, TerrainCellTransform3V, vsOut.uv5.xy );

    // calculate blending factor
    CalculateGMMBlend( vsIn.position, EyePos, TerrainCellDistSquaredInv, vsOut.uv1.z );

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // process material weights
    float4 matWeights;
    float weightEmpty;
    matWeights = tex2D(MiniWeightMapSampler, psIn.uv1);
    ProcessGMMMaterialWeights4(matWeights, weightEmpty);
    
    // get global texturing color
    float4 globalColor = GetGMMGlobalColor( GlobalSampler, psIn.uv0 );
    
    // get detail texturing color
    float4 detailColor = GetGMMDetailColor( matWeights, weightEmpty,
        psIn.uv2, psIn.uv3, psIn.uv4, psIn.uv5,
        BlendTexture0Sampler, BlendTexture1Sampler, BlendTexture2Sampler, BlendTexture3Sampler );
        
    // blend global and detail texturing color
    float4 baseColor = BlendGMMGlobalDetail( globalColor, detailColor, psIn.uv1.z );

    // modulate color
    baseColor = ModulateGMMColor( baseColor, ModulationSampler, psIn.uv0, TerrainModulationFactor );
    
#ifndef LEVEL_EDITOR
    // modulate lightmap
    float4 lightmapColor = tex2D(LightmapSampler, psIn.uv1);
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
        // pass_color.fx
        //DepthBias           = 0.0f;
        //FogEnable           = False;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
