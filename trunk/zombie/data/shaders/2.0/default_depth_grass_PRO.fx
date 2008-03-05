//------------------------------------------------------------------------------
//  2.0/materials/material_grass_dept.fx
//  
//  Material description:
//      deform=swinging
//      map=lightmap
//      env=specularlighting
//      map=clipmap
//      map=colormap
//  Material case:
//  
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libtrag.fx"

#include "shaders:../lib/bill.fx"

shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float3   ModelEyePos;
shared float3   EyePos;

float mipMapLodBias = 0.0f;
texture clipMap;
texture NoiseMap0;
shared float Time;

float TerrainGrassMinDist;
float TerrainGrassMaxDist;

#include "shaders:../lib/clipsampler.fx"
#include "shaders:../lib/noisesampler.fx"

struct VsInput
{
    float4 position : POSITION;
    float3 color  : COLOR;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float  clipdis       : TEXCOORD1;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    position = swing2(position, vsIn.color, Time);
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    
    float4 pos  = mul(position, ModelView); 
    // The visivble z is 0.0 to  -FarPlane
    vsOut.clipdis =  ( -pos.z - TerrainGrassMinDist) / ( TerrainGrassMaxDist - TerrainGrassMinDist );
    
    
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 uv0 = psIn.uv0;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // alpha test clipping
    float clipValue = tex2D(ClipSampler, uv0).r;
    dstColor.a = clipValue;
    
    
    float val;    
    float perlin = tex2D(NoiseSampler, uv0).r;
    if ( psIn.clipdis < perlin )
    {
        val = 1.0f;
    } else
    {
        val = 0.0f;
    }
    
    dstColor.a*=val;
    
    
    return dstColor;
}

technique t0
{
    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = True;
        AlphaRef         = 50;
        AlphaFunc        = GREATER;
        FogEnable        = False;
        DepthBias        = 0.0f;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
