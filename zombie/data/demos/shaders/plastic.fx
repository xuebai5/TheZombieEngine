//------------------------------------------------------------------------------
//  2.0/zombie/color.fx
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 Model;
shared float3 EyePos;

float4 MatDiffuse;

int mipMapLodBias;

int FillMode = 3; //1=Point, 2=Wireframe, 3=Solid
int CullMode = 2; //1=NoCull, 2=CW, 3=CCW

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
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 view     : TEXCOORD1;
    float3 normal   : TEXCOORD2;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    float4 worldPos = mul(vsIn.position, Model);
    vsOut.uv0 = vsIn.uv0;
    vsOut.normal = mul(vsIn.normal, Model);
    vsOut.view = EyePos - worldPos;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float3 norm = normalize (psIn.normal);
    float v = 0.5f * (1 + dot (norm, normalize(psIn.view)));
    return MatDiffuse * v;
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
        
        VertexShader    = compile vs_2_0 vsMain();
        PixelShader     = compile ps_2_0 psMain();
    }
}
