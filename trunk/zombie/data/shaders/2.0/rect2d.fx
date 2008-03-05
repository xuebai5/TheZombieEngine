//------------------------------------------------------------------------------
//  rect2d.fx
//
//  A simple 2d rect shader for debug purposes.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float3   ModelEyePos;               // the eye position in model space
shared float3   ModelLightPos;             // the light position in model space

float4 MatDiffuse;                  // material diffuse color

texture DiffMap0;

#include "../lib/diff0sampler.fx"

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    //return MatDiffuse;
    return tex2D(Diff0Sampler, psIn.uv0);
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode        = None;
        AlphaTestEnable = False;
        DepthBias       = 0.0f;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
