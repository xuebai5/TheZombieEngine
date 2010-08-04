#line 1 "target2d.fx"
//------------------------------------------------------------------------------
//  ps2.0/default.fx
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
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position     : POSITION;         // position in projection space
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
    VsOutput vsOut = (VsOutput)0;
   /* float4 position = mul( float4(0.0f, 0.0f, 0.0f, 1.0f) , ModelViewProjection );
    position = mul( position, ModelViewProjection );
    
    float2 pos = vsIn.position.xy;
    
    
    //Copy texture coordinates
    vsOut.uv0 = vsIn.uv0;
    position.xy += vsIn.position.xy*position.z*128/float2(1024,768);
    vsOut.position = position;
    */
    float4 position = float4(0.0f, 0.0f, 0.0f, 1.0f);
    position = transformBillBoard( position, vsIn.position.xy*Scale, InvModelView, ModelEyePos );

    //Calculate the projection position
    vsOut.position = mul( position, ModelViewProjection );
    vsOut.uv0 = vsIn.uv0;
    
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return tex2D( DiffSampler, psIn.uv0 );
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZWriteEnable = false;
        ColorWriteEnable = RED|GREEN|BLUE;       
        ZEnable          = true;
        ZFunc            = LessEqual;
        CullMode         = none;

        AlphaBlendEnable = true;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        
        AlphaTestEnable  = false;
        AlphaFunc        = GreaterEqual;
		fogEnable = false;

        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
