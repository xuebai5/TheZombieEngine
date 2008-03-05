#include "shaders:../lib/lib.fx"


shared float4x4 ModelViewProjection;
texture clipMap;
float mipMapLodBias = 0.0f;
int CullMode = 2;

#include "shaders:../lib/clipsampler.fx"

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;    
};

struct VsOutput
{
    float4 position      : POSITION;
    float  depth         : TEXCOORD0;
    float2 uv0           : TEXCOORD1;    
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    
    float4 pos = mul( position, ModelViewProjection );
    vsOut.position = pos;
    vsOut.uv0 = vsIn.uv0;
    
    // pass z / w to the pixel shader
    vsOut.depth = (pos.z + 0.0001f) / pos.w;
    
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    // alpha test clipping
    float clipValue = tex2D(ClipSampler, psIn.uv0).r;

    float4 result= psIn.depth;
    result.a = clipValue;
    
    return result;
}

technique t0
{
    pass p0
    {
        CullMode         = <CullMode>;
        AlphaTestEnable  = True;
        AlphaRef         = 50;
        AlphaFunc        = GREATER;        
        StencilEnable    = false;
        VertexShader     = compile vs_2_0 vsMain();
        PixelShader      = compile ps_2_0 psMain();
    }
}
