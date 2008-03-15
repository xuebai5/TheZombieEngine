#line 1 "default_fog.fx"
//------------------------------------------------------------------------------
//  ps2.0/default.fx
//
//  Default Volume Fog shader
//
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
//#include "shaders:../lib/fog.fx"

shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float3   ModelEyePos;

//float4 FogColor = {0.5f, 0.5f, 0.5f, 0.0f};
float4 FogDistances = {0.66f, 32.0f, 0.0f, 0.0f};
float4 FogColor = {1.0f, 1.0f, 1.0f, 0.0f};

struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = vsIn.position;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = FogColor;
    return dstColor;
}

technique t0
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable     = False;
        ZEnable          = False;
        ZFunc            = LessEqual;
        CullMode         = 0;//CW;

        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        BlendOp          = Add;
        SrcBlend         = DestAlpha;
        DestBlend        = InvDestAlpha;
              
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
