#line 1 "default.fx"
//------------------------------------------------------------------------------
//  ps2.0/default_shdv.fx
//  
//  Default shader for shadow volume extrusion in dx9 cards.
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/shadow.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;

float4 ModelLightPos;                       // the light position in model space

struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal = vsIn.normal;
    float3 tangent = vsIn.tangent;
    
    position = vsExtrudeShadowVolume(position, normal, ModelLightPos, 50.0, 0.01);
    
    vsOut.position = mul( position, ModelViewProjection );
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(1.0f, 0.0f, 0.0f, 0.3f);
    return dstColor;
}

technique t0
{
    pass p0
    {
        // depth-fail
        StencilFail      = Keep;
        StencilPass      = Keep;
        StencilZFail     = Incr;
#ifndef REVERSE_SHADOW_EXTRUSION
        CullMode         = CCW; // Back faces
#else
        CullMode         = CW; // Front faces
#endif
        VertexShader     = compile vs_2_0 vsMain();
        PixelShader      = compile ps_2_0 psMain();
    }

    pass p1
    {
        // depth-fail
        StencilFail      = Keep;
        StencilPass      = Keep;
        StencilZFail     = Decr;

#ifndef REVERSE_SHADOW_EXTRUSION
        CullMode         = CW; // Front faces
#else
        CullMode         = CCW; // Back faces
#endif
    }
}
