//------------------------------------------------------------------------------
//  2.0/materials/mat0001_alph.fx
//  
//  Material description:
//      pro=notculling
//      hasalpha=true
//      map=colormap
//      map=illuminationmap
//  Material case:
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"


shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;
shared float4x4 TextureTransform0;

int funcDestBlend = 6; // = InvSrcAlpha
float4 matAmbient;
float mipMapLodBias = 0.0f;
texture diffMap;
texture illuminationMap;

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/emissivesampler.fx"
struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = mul(float3(vsIn.uv0, 1.0f), (float3x3) TextureTransform0);
    return vsOut;
}

float4 psMain_pass0(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 uv0 = psIn.uv0;
    float4 vertexMatAmbient = matAmbient;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // illumination map
    ambient += tex2D(EmissiveSampler, uv0);
    // ambient lighting
    dstColor += ambient * vertexMatAmbient;
    // restore alpha value
    dstColor.a = 1.0;
    // diffuse map
    dstColor *= tex2D(DiffSampler, uv0);
    return dstColor;
}

technique t0
{
    pass p_CW_DIFF
    {
        CullMode         = None;
        SrcBlend         = SrcAlpha;
        DestBlend        = <funcDestBlend>;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias  = 0.0f;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain_pass0();
    }
}
