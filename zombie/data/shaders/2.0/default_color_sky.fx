//------------------------------------------------------------------------------
//  2.0/materials/mat0001_colr.fx
//  
//  Material description:
//      env=ambientlighting
//      env=fogdisable
//      map=colormap
//      map=lightmap
//  Material case:
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

float4   LightAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4 matAmbient;
float mipMapLodBias = 0.0f;
texture diffMap;
texture lightMap;

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/lightmapsampler.fx"
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
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    vsOut.uv1 = vsIn.uv1;
    

    float signw = sign(vsOut.position.w);
    vsOut.position.xy = signw * vsOut.position.xy / vsOut.position.w;
    vsOut.position.w = signw;
    vsOut.position.z = sign(vsOut.position.z);

    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 uv0 = psIn.uv0;
    // diffuse lightmap
    float4 lightMapColor = tex2D(LightmapSampler, psIn.uv1);
    float4 vertexMatAmbient = matAmbient;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    ambient += lightMapColor;
    // ambient lighting
    dstColor += ambient * vertexMatAmbient;
    // diffuse map
    dstColor *= tex2D(DiffSampler, uv0);
    //dstColor.g = 1.0f;
    return dstColor;
}

technique t0
{
    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias  = 0.0f;
        Clipping = False;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
