#line 1 "dotshadershaft.fx"
//------------------------------------------------------------------------------
//  dotshadershaft.fx
//  
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"

shared float4x4 ModelView;             // the modelview matrix
shared float4x4 Model;             // the modelview matrix
shared float4x4 ModelViewProjection;   // the modelview*projection matrix
shared float4x4 View;                   // the view matrix
shared float4x4 InvView;                   // the view matrix
shared float4x4 Projection;             // the projection matrix
shared float Time;
shared float3   LightPos;               // light position in world space
float3 LightDir;                        // light direction
shared float3   EyePos;                 // the eye pos in world space
float4 LightDiffuse1;
float4 matAmbient;
float mipMapLodBias = 0.0f;

texture diffMap;
texture NoiseMap0;
#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/noisesampler.fx"

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
    float2 uv0  :      TEXCOORD0;
    float2 uv1  :      TEXCOORD1;
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
    
    float3 eyePos = InvView[3];
    float3 Vx = normalize(LightDir.xyz);
    float3 R = LightPos.xyz + LightDir.xyz;
    float3 ER = normalize ( R - eyePos.xyz );
    float3 PE = normalize ( eyePos.xyz - R );
    
    const float cos10 = cos(0.17453292519943295769236907684886); //10 deg
 
 /*
    if ( dot(Vx, ER) > cos10 ||dot(Vx, ER) > cos10 )
    {
    
        //degenerate triangle.
       vsOut.position = float4(  0 , 0, 1000, 1);
    } else */
    {    
        float3 pos = vsIn.position;
        pos.x *= length(LightDir);
        vsOut.uv0 = pos.xy;
        vsOut.uv1 = float2(pos.y+0.5 ,0.5);
        
        //LightDiffuse1 has the equation   radius = dist*LightDiffuse1.x + LightDiffuse1.y
        pos.y *= pos.x*LightDiffuse1.x + LightDiffuse1.y;
        
        float3 Vy = cross( ER , Vx);
        Vy = normalize(Vy);
        
        
        pos = pos.x * Vx + pos.y * Vy + LightPos;
        
        vsOut.position = mul(float4(pos, 1.0), mul( View ,Projection ));
    }

    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
PS_OUTPUT psMain(const VS_OUTPUT psIn)
{
    PS_OUTPUT psOut;
    psOut.color = tex2D(DiffSampler, psIn.uv1 ) ;
    psOut.color *= matAmbient; // Modulate the color and alpha
    
    float alpha = 1.f;
    
    float2 uv;
    uv.x = psIn.uv0.x - Time*0.25;
    uv.y = psIn.uv0.y + 0.5;
    
    alpha = 0.8*tex2D(NoiseSampler, uv );
    uv.x = psIn.uv0.x - Time;
    alpha += 0.2*tex2D(NoiseSampler, uv );
    
    psOut.color.a *= alpha;
    return psOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, PixelShader 1.1
//------------------------------------------------------------------------------


int isSkinned = 0;
                          
technique vs20_ps20
{
    pass p0
    {
        AlphaTestEnable    = False;
        DepthBias          = 0.0f;
        SrcBlend           = SrcAlpha; //additive blend
        DestBlend          = one;
        

        //VertexShader = compile vs_1_1 vsMain();
        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
