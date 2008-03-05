#line 1 "default_terrainhole.fx"
//------------------------------------------------------------------------------
//  ps2.0/default_terrainhole.fx
//  
//  Default shader for drawing terrain holes
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/shadow.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;

float4 ModelLightPos;                       // the light position in model space

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
    vsOut.position = mul( vsIn.position, ModelViewProjection );
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

technique t0
{
    // Mark all pixels of object in stecil as one
    pass p0
    {
        // The state is in pass
        VertexShader        = compile vs_1_1 vsMain();
        PixelShader         = compile ps_1_1 psMain();//0;
    }

}
