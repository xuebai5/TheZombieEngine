#line 1 "error.fx"
//------------------------------------------------------------------------------
//  error.fx
//
//  Error shaders used to show you that a shape has no assigned material
//  
//  (c) 2004 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelView;             // the modelview matrix
shared float4x4 ModelViewProjection;   // the modelview*projection matrix
matrix<float,4,3> JointPalette[72];

float  time;
texture diffMap ;


//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------

struct VSIN_IN_SKIN
{
    float4 position     : POSITION;
    float4 weights      : BLENDWEIGHT;
    float4 indices      : BLENDINDICES;
};

struct VS_INPUT
{
    float3 position : POSITION;

};

struct VS_OUTPUT
{
    float4 position  : POSITION;
    float4 color:      TEXCOORD1;
    float2 uv0  :      TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
};

sampler ErrorMap = sampler_state
{
    Texture = <diffMap>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    float4 pos= transformStatic(vsIn.position, ModelViewProjection);
    vsOut.position = pos;
    vsOut.color = pos;
    vsOut.uv0   = float2(4,-3)*pos.xy/pos.w;
    
    return vsOut;
}

VS_OUTPUT vsMainSkin(const VSIN_IN_SKIN vsIn)
{
    VS_OUTPUT vsOut;
    
    float4 skinPos     = skinnedPosition(vsIn.position, vsIn.weights, vsIn.indices, JointPalette);
    float4 pos= mul(skinPos, ModelViewProjection);
    vsOut.position = pos;
    vsOut.color = pos;
    vsOut.uv0   = float2(4,-3)*pos.xy/pos.w;
    //vsOut.uv0.y   = -vsOut.uv0.y;
    return vsOut;
}
//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
PS_OUTPUT psMain(const VS_OUTPUT psIn)
{
    PS_OUTPUT psOut;
    float4 color;

   // color = psIn.color;
//    color=color *64 %1;
    color.x=cos(psIn.color.x);
    color.y=cos(psIn.color.y);
    color.z=cos(color.x*color.y);
    color.w=1.0;
	
	
    
    float4 color2 = tex2D(ErrorMap, psIn.uv0);
    
    if (color2.x+color2.y+color2.z == 0)
    {
    	psOut.color = color;
    } else
    {
    	psOut.color = color2;
    }
    //psOut.color = fmod(chnTime*5.0,1.0)*float4(1.0,0.8,0.0,1.0);
    return psOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1
//------------------------------------------------------------------------------


VertexShader vsArray[2] = {compile vs_1_1 vsMain(),
                           compile vs_1_1 vsMainSkin()
                           };
int isSkinned = 0;
                          
technique vs20_ps20
{
    pass p0
    {
        AlphaTestEnable    = False;
        DepthBias          = 0.0f;

        //VertexShader = compile vs_1_1 vsMain();
        VertexShader = (vsArray[isSkinned]);
        PixelShader  = compile ps_2_0 psMain();
    }
}
