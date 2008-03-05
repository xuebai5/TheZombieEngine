#line 1 "particle.fx"
//------------------------------------------------------------------------------
//  fixed/particle.fx
//
//  Particle system shader.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection; // the model*view*projection matrix
shared float4x4 InvModelView;       // the inverse model*view matrix

texture diffMap;
float mipMapLodBias;

int funcSrcBlend = 5;  // SrcAlpha
int funcDestBlend = 6;  // InvSrcAlpha

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct vsInputParticle2Color
{
    float4 position : POSITION;    // the particle position in world space
    float3 velocity : NORMAL;      // the particle coded uv and corners,rotation and scale
    float2 uv       : TEXCOORD0;   // the particle coded uv and corners,rotation and scale
    float4 data     : COLOR0;      // the particle coded uv and corners,rotation and scale
};

struct vsOutputParticle2Color
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/diffsampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
vsOutputParticle2Color vsParticle2Color(const vsInputParticle2Color vsIn)
{
    float code     = vsIn.data[0];
    float rotation = vsIn.data[1];
    float size     = vsIn.data[2];
    float colorCode  = vsIn.data[3];

    // build rotation matrix
    float sinAng, cosAng;
    sincos(rotation, sinAng, cosAng);
    float3x3 rot = {
        cosAng, -sinAng, 0.0f,
        sinAng,  cosAng, 0.0f,
        0.0f,    0.0f,   1.0f,
    };

    // decode color data
    float4 rgba;
    rgba.z = modf(colorCode / 256.0f, colorCode);
    rgba.y = modf(colorCode / 256.0f, colorCode);
    rgba.x = modf(colorCode / 256.0f, colorCode);
    rgba.w = modf(code / 256.0f, code);
    rgba *= 256.0f / 255.0f;
    
    float4 position = vsIn.position;

    // the corner offset gets calculated from the velocity
    float3 extrude = mul(InvModelView,vsIn.velocity);
    if (code != 0.0f) 
    {
        extrude = normalize(extrude);
        float vis = abs(extrude.z);
        size *= cos(vis * 3.14159f * 0.5f);
        rgba.w *= cos(vis * 3.14159f * 0.5f);
    };

    extrude.z = 0.0f;
    extrude = normalize(extrude);    

    extrude *= size;
    extrude = mul(rot, extrude);
    extrude = mul(extrude, (float3x3) InvModelView);
    position.xyz += extrude.xyz;

    vsOutputParticle2Color vsOut;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0      = vsIn.uv;
    vsOut.diffuse  = rgba;

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader
//------------------------------------------------------------------------------
float4 psParticleColor(const vsOutputParticle2Color psIn) : COLOR
{
    float4 finalColor = tex2D(DiffSampler, psIn.uv0) * psIn.diffuse;
    return finalColor;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        CullMode     = None;
        SrcBlend     = <funcSrcBlend>;
        DestBlend    = <funcDestBlend>;
        
        VertexShader = compile VS_PROFILE vsParticle2Color();
        PixelShader  = compile PS_PROFILE psParticleColor();
    }
}
