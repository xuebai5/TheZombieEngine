//------------------------------------------------------------------------------
//  2.0/default_colr_smap.fx
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 Model;
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

float4 TerrainCellMaxMinPos; //xmin,zmin,xmax,zmax
float4x4 ModelShadowProjection;

texture DiffMap0;
texture BumpMap0;
texture lightMap;
texture ShadowMap;

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

#include "shaders:../lib/lightmapsampler_clamp.fx"
#include "shaders:../lib/shadowmapsampler.fx"

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT; 
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 primLightVec  : TEXCOORD1;
    float3 primHalfVec   : TEXCOORD2;
    float2 uvlightmap    : TEXCOORD3;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut = (VsOutput)0;
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
    //float attenuation = 1 / (LightAttenuation.x  + LightAttenuation.y * dist + LightAttenuation.z * dist * dist);
    // tangent space lighting
    float3 primLightVec;
    float3 primHalfVec;
    tangentSpaceLightHalfVector2(position, ModelLightPos, ModelEyePos, normal, binormal, tangent,
                                 primLightVec, primHalfVec);
    vsOut.primLightVec = primLightVec;
    vsOut.primHalfVec  = primHalfVec;
 
    // NEW
    vsLightmap(position, TerrainCellMaxMinPos, Model, vsOut.uvlightmap);
    // ENDNEW
    
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    
    // NEW
    float lmapIntensity = tex2D(LightmapSampler, psIn.uvlightmap).a;
    // ENDNEW
    
    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(psIn.primLightVec);
    float diffIntensity = saturate(dot(tangentSurfaceNormal, tangentLightVec));
    float4 diffuse = diffIntensity * LightDiffuse * MatDiffuse;
    diffuse *= lmapIntensity;
    
    dstColor += diffuse; 
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    // per vertex specular lighting
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(psIn.primHalfVec);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular * MatSpecular; 
    // ***
    specular *= lmapIntensity;
    
    float4 levelColor = MatLevel;
    dstColor += specular * levelColor;
    
    //dstColor = lightmapIntensity;
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
