#line 1 "lensflare.fx"
//------------------------------------------------------------------------------
//  ps2.0/lensflare.fx
//
//  The default shader for dx9 cards using vertex/pixel shader 2.0.
//
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;       // the model*view*projection matrix
shared float3   ModelEyePos;               // the eye position in model space
shared float3   ModelLightPos;             // the light position in model space
shared float4x4 InvModelView;


float  Scale;            // the material's specular power

texture DiffMap0;                   // 2d texture


//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float4 diffuse  : COLOR0;
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
    float4 diffuse      : COLOR0;
    float2 uv0          : TEXCOORD0;        // texture coordinate
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = none;
    MagFilter = linear;
    MipFilter = none;
};
//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
   
    vsOut.position = vsIn.position;
    vsOut.uv0      = vsIn.uv0;
    vsOut.diffuse  = vsIn.diffuse;
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    //return psIn.diffuse;
    float4 color = psIn.diffuse;
    color.a *= tex2D( DiffSampler, psIn.uv0 ).a;
    return color;
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable     = False;
        ColorWriteEnable = RED|GREEN|BLUE;       
        ZEnable          = True;
        ZFunc            = LessEqual;
        CullMode         = None;
        StencilEnable    = False;

        AlphaBlendEnable = True;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        
        AlphaTestEnable  = False;
        AlphaFunc        = GreaterEqual;
		fogEnable        = False;

        VertexShader     = compile vs_2_0 vsMain();
        PixelShader      = compile ps_2_0 psMain();
    }
}
