#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;

float4 MatDiffuse = float4( 1.f, 0.f, 0.f, 1.f );

int MinDist;

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
};

VsOutput vsMain(const VsInput IN, const uniform int passIndex)
{
    VsOutput OUT;
    float4 position = IN.position;
    float offset = .005f * passIndex;
    position.xyz += offset * IN.normal;
    OUT.position = mul(position, ModelViewProjection);
    OUT.uv0 = IN.uv0;
    return OUT;
}

float4 psMain(const VsOutput IN) : COLOR
{
    return tex2D(ColorSampler, IN.uv0) * MatDiffuse;
}
                           
technique t0
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable    = False;
        ZEnable         = True;

        AlphaBlendEnable = True;
        BlendOp         = Add;
        SrcBlend        = SrcAlpha;
        DestBlend       = Zero;
        
        FillMode        = <FillMode>;
        CullMode        = CW;

        VertexShader    = compile vs_1_1 vsMain(0);
        PixelShader     = compile ps_1_1 psMain();
    }

    pass p1
    {
        DestBlend       = One;
        VertexShader    = compile vs_1_1 vsMain(1);
    }
    pass p2
    {
        VertexShader    = compile vs_1_1 vsMain(2);
    }
    pass p3
    {
        VertexShader    = compile vs_1_1 vsMain(3);
    }
    pass p4
    {
        VertexShader    = compile vs_1_1 vsMain(4);
    }
    pass p5
    {
        VertexShader    = compile vs_1_1 vsMain(5);
    }
    pass p6
    {
        VertexShader    = compile vs_1_1 vsMain(6);
    }
    pass p7
    {
        VertexShader    = compile vs_1_1 vsMain(7);
    }
    pass p8
    {
        VertexShader    = compile vs_1_1 vsMain(8);
    }
    pass p9
    {
        VertexShader    = compile vs_1_1 vsMain(9);
    }
    pass p10
    {
        VertexShader    = compile vs_1_1 vsMain(10);
    }
    pass p11
    {
        VertexShader    = compile vs_1_1 vsMain(11);
    }
}
