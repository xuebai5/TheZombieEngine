//------------------------------------------------------------------------------
//  2.0/materials/mat0001_dept.fx
//  
//  Material description:
//      env=ambientlighting
//      env=fogdisable
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

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position      : POSITION;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);

    float signw = sign(vsOut.position.w);
    vsOut.position.xy = signw * vsOut.position.xy / vsOut.position.w;
    vsOut.position.w = signw;
    vsOut.position.z = sign(vsOut.position.z);
    
    
    // texture coordinates
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
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
