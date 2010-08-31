#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

float Frequency; //noise_frequency
float Noise;     //noise_amplitude

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid
int CullMode = 2; //1=NoCull, 2=CW, 3=CCW

texture DiffMap0;
texture NoiseMap0;

sampler2D SplineSampler = sampler_state
{
    Texture = <DiffMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

sampler2D NoiseSampler = sampler_state
{
    Texture = <NoiseMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 texNoise : TEXCOORD0;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.texNoise = position.xy * Frequency;
    return vsOut;
}

float3 snoise(float2 texNoise)
{
    return 2.f * tex2D(NoiseSampler, texNoise) - 1.f;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 color = (float) (-2.f * snoise(psIn.texNoise) + 0.75f);
    return color;
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
        
        VertexShader = compile vs_1_1 vsMain();
        PixelShader = compile ps_1_1 psMain();
    }
}
