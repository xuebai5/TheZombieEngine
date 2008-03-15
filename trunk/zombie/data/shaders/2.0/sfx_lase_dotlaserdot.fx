#line 1 "dotshaderdot.fx"
//------------------------------------------------------------------------------
//  dotshaderdot.fx
//
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelView;             // the modelview matrix
shared float4x4 ModelViewProjection;   // the modelview*projection matrix
float4x4 ModelLightProjection;

float  time;
texture diffMap ;
sampler DiffSampler = sampler_state
{
    Texture = <diffMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= 0;
};

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------



struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position  : POSITION;
    float4 pos:      TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
};


//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    float4 pos= transformStatic(vsIn.position, ModelViewProjection);
    vsOut.position = pos;
    vsOut.pos = transformStatic(vsIn.position, ModelLightProjection);    
    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
PS_OUTPUT psMain(const VS_OUTPUT psIn)
{
    PS_OUTPUT psOut;
    
    float2 uv0 = psIn.pos.xy / psIn.pos.w;

    float4 color = tex2D( DiffSampler , uv0 );

    psOut.color = color;
    return psOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1
//------------------------------------------------------------------------------


                          
technique vs20_ps20
{
    pass p0
    {
        AlphaTestEnable    = False;
        DepthBias          = 0.0f;
        
        SrcBlend           = SrcAlpha; //additive blend
        DestBlend          = one;

        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
