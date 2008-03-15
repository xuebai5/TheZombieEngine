//------------------------------------------------------------------------------
//  2.0/default_colr_pntl.fx
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

int CullMode = 2;//CW

texture DiffMap0;

sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};

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
    VsOutput vsOut = (VsOutput)0;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    dstColor = tex2D(DiffSampler, psIn.uv0);
    return dstColor;
}

VertexShader vsArray[1]= {compile vs_2_0  vsMain()};
technique t0
{
    pass p0
    {
        CullMode         = <CullMode>;
        AlphaTestEnable  = False;
        
        VertexShader = (vsArray[0]);
        PixelShader  = compile ps_2_0 psMain();
    }
}
