#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;

int CullMode = 2;

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
    float depth     : TEXCOORD0;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    
    float4 clipPos = mul( position, ModelViewProjection );
    vsOut.position = clipPos;
    
    // pass z / w to the pixel shader
    vsOut.depth = (clipPos.z + 0.0001f) / clipPos.w;
    
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return psIn.depth;
}

technique t0
{
    pass p0
    {
        //ZWriteEnable     = True;
        //ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        //ZEnable          = true;
        //ZFunc            = LessEqual;
        CullMode         = <CullMode>;
        //AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        //FogEnable        = false;
        StencilEnable    = false;
        VertexShader     = compile vs_2_0 vsMain();
        PixelShader      = compile ps_2_0 psMain();
    }
}
