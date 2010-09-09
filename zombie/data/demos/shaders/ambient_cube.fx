#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid

texture CubeMap0;

samplerCUBE CubeSampler = sampler_state
{
    Texture = <CubeMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
    float3 texcoord : TEXCOORD0;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.texcoord = position;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return texCUBE( CubeSampler, psIn.texcoord );
}
                           
technique t0
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable    = True;
        ZEnable         = True;
        ZFunc           = LessEqual;
        
        FillMode        = <FillMode>;
        CullMode        = None;

        VertexShader    = compile vs_1_1 vsMain();
        PixelShader     = compile ps_1_1 psMain();
    }
}
