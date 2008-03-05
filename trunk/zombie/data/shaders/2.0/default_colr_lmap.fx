//------------------------------------------------------------------------------
//  2.0/default_colr_lmap.fx
//  
//  Material description:
//      env=ambientlighting
//      env=diffuselighting
//      env=specularlighting
//      map=colormap
//      map=lightmap
//  Material case:
//  
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libtrag.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float mipMapLodBias = 0.0f;
texture DiffMap0;
texture DiffMap1;

sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= <mipMapLodBias>;
};

sampler LightmapSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;//Clamp;
    AddressV  = Wrap;//Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;//None;
};

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float2 uv1           : TEXCOORD1;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut = (VsOutput)0;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    float3 eyeDir = normalize(ModelEyePos - position);
    vsOut.uv1 = vsIn.uv1;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	  float2 uv0 = psIn.uv0;
    float3 tangentSurfaceNormal = float3(0.0f, 0.0f, 1.0f);
    // diffuse lightmap
    dstColor += tex2D(LightmapSampler, psIn.uv1);
    // diffuse map
    dstColor *= tex2D(DiffSampler, uv0);

    return dstColor;
}

VertexShader vsArray[1]= {compile vs_2_0  vsMain()};
technique t0
{
    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        
        VertexShader = (vsArray[0]);
        PixelShader  = compile ps_2_0 psMain();
    }
}
