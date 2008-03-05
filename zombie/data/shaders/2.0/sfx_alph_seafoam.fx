#line 1 "sfx_alph_seafoam.fx"
//------------------------------------------------------------------------------
//  sfx_alph_seafoam.fx
//  (C) 2006 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libtrag.fx"
#include "shaders:../lib/fog.fx"
float4   ModelLightPos;
float4 Velocity; // Encode the foam curve, x = cicling time, y = the climax time , z = then end time , w >0.5 smoth curve
float4 matAmbient; // Encode the  min uv translations and alpha  , x = u min , y = v min ,  w = alpha min
float4 matDiffuse; // Encode the max uv translations and alpha  , x = u max , y = v max,  w = alpha max
shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float Time;                      // the current global time
texture diffMap;
float mipMapLodBias = 0.0f;
float4 fogDistances;
float4 fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };

#include "shaders:../lib/diffsampler.fx"
struct VsInput
{
    float3 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 color    : COLOR;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float4 color         : COLOR0; // The green chanel is time offset
};

struct VsOutputFog
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float4 color        : COLOR0; // The green chanel is time offset
    float  fog          : FOG;
};

void  foamVs(const VsInput vsIn, out float4 position, out float2 uv0, out float4 color)
{
    position = float4(vsIn.position, 1.0f);
    position = mul(position, ModelViewProjection);
    
    //Velocity has  the foam curve, x = cicling time, y = the climax time , z = then end time,  , w >0.5 smoth curve
    float timeOffset = vsIn.color.g * Velocity.x;
    float factor = foamCurve( Time + timeOffset, Velocity.x, Velocity.y, Velocity.z, Velocity.w>0.5);
    
    // texture coordinates
    float2 uvMin = matAmbient.xy;
    float2 uvMax = matDiffuse.xy;
    uv0 = vsIn.uv0 + lerp( uvMin,  uvMax, float2(factor,factor) );
    
    //alpha
    color.a = lerp( matAmbient.a ,  matDiffuse.a , factor );
    color.rgb = float3( 0.f , 0.f , 0.f);
}

float4 foamPs(float2 uv0, float4 color) 
{
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    dstColor = tex2D(DiffSampler, uv0);
    dstColor.a *= color.a;
    return dstColor;
}

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    foamVs( vsIn, vsOut.position, vsOut.uv0, vsOut.color);
    return vsOut;
}

VsOutputFog vsMainFog(const VsInput vsIn)
{
    VsOutputFog vsOut;
    foamVs( vsIn, vsOut.position, vsOut.uv0, vsOut.color);
    vsOut.fog = linearFog(vsIn.position, fogDistances, ModelView);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 dstColor = foamPs(psIn.uv0,psIn.color) ;
    return dstColor;
}

float4 psMainFog(const VsOutputFog psIn) : COLOR
{
    float4 dstColor = foamPs(psIn.uv0,psIn.color) ;
    return dstColor;
}

technique tDefault
{
    pass p_CW_DIFF
    {
        CullMode         = CW;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias        = 0.0f;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}

technique tLinearFog
{
    pass p0
    {
        CullMode         = CW;
        SrcBlend         = SrcAlpha;
        DestBlend        = InvSrcAlpha;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias        = 0.0f;
        FogEnable        = True; 
        FogColor         = <fogColor>;
        FogVertexMode    = Linear; 
        FogTableMode     = None; 

        VertexShader     = compile vs_2_0 vsMainFog();
        PixelShader      = compile ps_2_0 psMainFog();
    }
}