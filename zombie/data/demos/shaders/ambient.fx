#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 InvModel;

float4 MatDiffuse = float4( 1.f, 0.f, 0.f, 1.f );

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid

texture DiffMap0;

sampler2D ColorSampler = sampler_state
{
    Texture = <DiffMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

texture SpecMap0;

sampler2D SpecSampler = sampler_state
{
    Texture = <SpecMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

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

texture CubeMap1;

samplerCUBE DiffCubeSampler = sampler_state
{
    Texture = <CubeMap1>;
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
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : TEXCOORD1;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    vsOut.normal = mul(vsIn.normal, (float3x3) InvModel);

    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 color = tex2D( ColorSampler, psIn.uv0 );
    float spec = .5f * tex2D( SpecSampler, psIn.uv0 ) + .5f;
    float4 diff = texCUBE( DiffCubeSampler, psIn.normal );

    return spec * diff * color;
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
        PixelShader     = compile ps_2_0 psMain();
    }
}
