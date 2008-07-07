//------------------------------------------------------------------------------
//  2.0/zombie/shadowvolumes.fx
//  
//  Shaders for shadow volume extrusion
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

float4   ModelLightPos;

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

matrix<float,4,3> JointPalette[72]; 

float mipMapLodBias = 0.0f;

#include "shaders:../lib/shadow.fx"

struct VsInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
};

struct VsInput_skinned
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

float4 vsShadowStatic(const VsInput vsIn) : POSITION
{
    float4 position = float4(vsIn.position, 1.0f);
    float3 normal   = vsIn.normal;
    position = vsExtrudeShadowVolume(position, normal, ModelLightPos, 50.0, 0.01);
    return mul(position, ModelViewProjection);
}

float4 vsShadowSkinned(const VsInput_skinned vsIn) : POSITION
{
    float4 position = float4(vsIn.position, 1.0f);
    float3 normal   = vsIn.normal;
    position = skinnedPosition(position, vsIn.weights, vsIn.indices, JointPalette);
    normal   = skinnedNormal(normal, vsIn.weights, vsIn.indices, JointPalette);
    position = vsExtrudeShadowVolume(position, normal, ModelLightPos, 50.0, 0.01);
    return mul(position, ModelViewProjection);
} 

float4 psShadow(const float4 position: POSITION) : COLOR
{
    return float4(1.0f, 0.0f, 0.0f, 0.3f);
}

technique tShadowStatic
{
    pass pShadow
    {
        VertexShader = compile VS_CURRENT vsShadowStatic();
        PixelShader  = compile PS_CURRENT psShadow();
    }
}

technique tShadowSkinned
{
    pass pShadow
    {
        VertexShader = compile VS_CURRENT vsShadowSkinned();
        PixelShader  = compile PS_CURRENT psShadow();
    }
}
