//------------------------------------------------------------------------------
//  2.0/default_colr_spot.fx
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libtrag.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient;
float4   LightDiffuse;
float4   LightSpecular;
float4   ModelLightPos;
float4x4 ModelLightProjection;

float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

texture DiffMap0;
texture BumpMap0;
texture LightModMap;

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

struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float3 tangent  : TANGENT;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 uvlight       : TEXCOORD2;
    float3 primLightVec  : TEXCOORD3;
    float3 primHalfVec   : TEXCOORD4;
};

VsOutput vsMain(const VsInput vsIn
)
{
    VsOutput vsOut = (VsOutput)0;
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

float4 psMain(const VsOutput psIn) : COLOR
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

VertexShader vsArray[1]= {compile vs_2_0  vsMain()};
technique t0
{
    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        
        VertexShader = (vsArray[0]);
        PixelShader  = compile ps_2_0 psMain();
    }
}
