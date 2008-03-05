#line 1 "default_fog.fx"
//------------------------------------------------------------------------------
//  ps2.0/default.fx
//
//  Default Volume Fog shader
//
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/fog.fx"

shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float3   ModelEyePos;

//float4 FogDistances = {0.66f, 32.0f, 0.0f, 0.0f};
//float4 FogColor = {0.5f, 0.5f, 0.5f, 0.0f};//{1.0f, 1.0f, 1.0f, 0.0f}

int  CullMode = 2; // CW

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
    position = mul(position, ModelViewProjection);
    vsOut.position = position;
    //float4 campos = mul(position, ModelView);
    //vsOut.depth = campos.z / 10.0;
    vsOut.depth = position.z / 1000.0f;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
    dstColor.a = psIn.depth;
    return dstColor;
}

technique t0
{
    pass p0
    {
        ZWriteEnable     = True;
        ColorWriteEnable = ALPHA;//RED|GREEN|BLUE|ALPHA;
        ZEnable          = True;
        ZFunc            = LessEqual;
        CullMode         = CW;
        AlphaTestEnable  = False;
        
        AlphaBlendEnable = True;
        BlendOp          = RevSubtract;
        SrcBlend         = One;
        DestBlend        = One;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
