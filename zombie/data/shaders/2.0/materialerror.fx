//------------------------------------------------------------------------------
//  2.0/materialerror.fx
//
//  Error shader to replace invalid shaders
//
//  (C) 2005 Tragnarion Studios 
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float3   ModelEyePos;               // the eye position in model space
shared float3   ModelLightPos;             // the light position in model space

float4 MatDiffuse = float4(1.0f, 1.0f, 0.0f, 0.0f); // material diffuse color

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return MatDiffuse;
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
