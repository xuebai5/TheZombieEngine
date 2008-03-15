//------------------------------------------------------------------------------
//  2.0/default.fx
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"
#include "shaders:../lib/shadow.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4 LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4 LightAmbient;
float4 LightDiffuse;
float4 LightSpecular;
float4 ModelLightPos;
float4x4 ModelLightProjection;

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

texture DiffMap0;
texture BumpMap0;
texture LightModMap;

//------------------------------------------------------------------------------
/**
    texture samplers
*/

sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;    
};

sampler BumpSampler = sampler_state
{
    Texture   = <BumpMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias = -0.75;
};

#include "shaders:../lib/lightsampler.fx"

//------------------------------------------------------------------------------
/**
    input/output structures
*/

struct VsInput
{
    float4 position     : POSITION;
};

struct VsOutput
{
    float4 position     : POSITION;
};

struct VsInputColor
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
};

struct VsOutputColor
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
};

struct VsInputColor_Point
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT; 
};

struct VsOutputColor_Point
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 primLightVec : TEXCOORD1;
    float3 primHalfVec  : TEXCOORD2;
};

struct VsInputColor_Spot
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT; 
};

struct VsOutputColor_Spot
{
    float4 position     : POSITION;
    float2 uv0          : TEXCOORD0;
    float3 uvlight      : TEXCOORD1;
    float3 primLightVec : TEXCOORD2;
    float3 primHalfVec  : TEXCOORD3;
};

struct VsInputShadow
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv0      : TEXCOORD0;
};

//------------------------------------------------------------------------------
/**
*/
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut = (VsOutput) 0;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = float3(0.0f, 0.0f, 1.0f);
    
    return dstColor;
}

//------------------------------------------------------------------------------
/**
*/
VsOutputColor vsMainColor(const VsInputColor vsIn)
{
    VsOutputColor vsOut = (VsOutputColor) 0;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
float4 psMainColor(const VsOutputColor psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    dstColor = tex2D(DiffSampler, psIn.uv0);
    return dstColor;
}

//------------------------------------------------------------------------------
/**
*/
VsOutputColor_Point vsMainColor_Point(const VsInputColor_Point vsIn)
{
    VsOutputColor_Point vsOut = (VsOutputColor_Point) 0;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    float3 tangent  = vsIn.tangent;
    float3 binormal = cross(normal, tangent); 
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    float3 eyeDir = normalize(ModelEyePos - position);
    // lighting
    float dist = distance(ModelLightPos, position);
    float attenuation = 1 / (LightAttenuation.x  + LightAttenuation.y * dist + LightAttenuation.z * dist * dist);
    // tangent space lighting
    float3 primLightVec;
    float3 primHalfVec;
    tangentSpaceLightHalfVector2(position, ModelLightPos, ModelEyePos, normal, binormal, tangent,
                                 primLightVec, primHalfVec);
    vsOut.primLightVec = primLightVec;
    vsOut.primHalfVec  = primHalfVec;
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
float4 psMainColor_Point(const VsOutputColor_Point psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(psIn.primLightVec);
    float diffIntensity = saturate(dot(tangentSurfaceNormal, tangentLightVec));
    float4 diffuse = diffIntensity * LightDiffuse * MatDiffuse;
    dstColor += diffuse; 
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(psIn.primHalfVec);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular * MatSpecular; 
    float4 levelColor = MatLevel;
    dstColor += specular * levelColor;

    return dstColor;
}

//------------------------------------------------------------------------------
/**
*/
VsOutputColor_Spot vsMainColor_Spot(const VsInputColor_Spot vsIn)
{
    VsOutputColor_Spot vsOut = (VsOutputColor_Spot) 0;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    float3 tangent  = vsIn.tangent;
    float3 binormal = cross(normal, tangent);
    vsOut.position = mul(position, ModelViewProjection);
    
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    float4 uvlight = mul(position, ModelLightProjection);
    vsOut.uvlight = uvlight.xyz / uvlight.w;
    
    // lighting
    float dist = distance(ModelLightPos, position);
    float attenuation = 1 / (LightAttenuation.x  + LightAttenuation.y * dist + LightAttenuation.z * dist * dist);
    
    // tangent space lighting
    float3 primLightVec;
    float3 primHalfVec;
    tangentSpaceLightHalfVector2(position, ModelLightPos, ModelEyePos, normal, binormal, tangent,
                                 primLightVec, primHalfVec);
    vsOut.primLightVec = primLightVec;
    vsOut.primHalfVec  = primHalfVec;
    
    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
float4 psMainColor_Spot(const VsOutputColor_Spot psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    float4 lightColor = texCUBE(LightSampler, psIn.uvlight);
    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(psIn.primLightVec);
    float diffIntensity = saturate(dot(tangentSurfaceNormal, tangentLightVec));
    float4 diffuse = diffIntensity * LightDiffuse * MatDiffuse;
    diffuse *= lightColor;
    dstColor += diffuse; 
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    // per vertex specular lighting
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(psIn.primHalfVec);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular * MatSpecular; 
    float4 levelColor = MatLevel;
    specular *= lightColor;
    dstColor += specular * levelColor;

    return dstColor;
}

//------------------------------------------------------------------------------
/**
*/
VsOutput vsMainShadow(const VsInputShadow vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal = vsIn.normal;
    float3 tangent = vsIn.tangent;
    
    position = vsExtrudeShadowVolume(position, normal, ModelLightPos, 50.0, 0.01);
    vsOut.position = mul(position, ModelViewProjection);

    return vsOut;
}

//------------------------------------------------------------------------------
/**
*/
float4 psMainShadow(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(1.0f, 0.0f, 0.0f, 0.3f);
    return dstColor;
}

//------------------------------------------------------------------------------
/**
    techniques for shader: default
*/
VertexShader vsArrayDepth[1] = { compile vs_2_0 vsMain() };

VertexShader vsArrayColor[1] = { compile vs_2_0 vsMainColor() };

VertexShader vsArrayColor_Point[1] = { compile vs_2_0 vsMainColor_Point() };

VertexShader vsArrayColor_Spot[1] = { compile vs_2_0 vsMainColor_Spot() };

VertexShader vsArrayEmissive[1] = { compile vs_2_0 vsMain() };

VertexShader vsArrayShadow[1] = { compile vs_2_0 vsMainShadow() };

technique tDepth
{
    pass p0
    {
        CullMode        = CW;
        AlphaTestEnable = False;
        
        VertexShader    = (vsArrayDepth[0]);
        PixelShader     = compile ps_2_0 psMain();
    }
}

technique tColor
{
    pass p0
    {
        CullMode        = CW;
        AlphaTestEnable = False;
        
        VertexShader    = (vsArrayColor[0]);
        PixelShader     = compile ps_2_0 psMainColor();
    }
}

technique tColor_Point
{
    pass p0
    {
        CullMode        = CW;
        AlphaTestEnable = False;
        
        VertexShader    = (vsArrayColor_Point[0]);
        PixelShader     = compile ps_2_0 psMainColor_Point();
    }
}

technique tColor_Spot
{
    pass p0
    {
        CullMode        = CW;
        AlphaTestEnable = False;
        
        VertexShader    = (vsArrayColor_Spot[0]);
        PixelShader     = compile ps_2_0 psMainColor_Spot();
    }
}

technique tEmissive
{
    pass p0
    {
        CullMode        = CW;
        AlphaTestEnable = False;
        
        VertexShader    = (vsArrayEmissive[0]);
        PixelShader     = compile ps_2_0 psMain();
    }
}

technique tShadow
{
    pass p0
    {
        // depth-fail
        StencilFail      = Keep;
        StencilPass      = Keep;
        StencilZFail     = Incr;
#ifndef REVERSE_SHADOW_EXTRUSION
        CullMode         = CCW; // Back faces
#else
        CullMode         = CW; // Front faces
#endif
        VertexShader    = (vsArrayShadow[0]);
        PixelShader     = compile ps_2_0 psMainShadow();
    }

    pass p1
    {
        // depth-fail
        StencilFail      = Keep;
        StencilPass      = Keep;
        StencilZFail     = Decr;

#ifndef REVERSE_SHADOW_EXTRUSION
        CullMode         = CW; // Front faces
#else
        CullMode         = CCW; // Back faces
#endif
    }
}
