//------------------------------------------------------------------------------
//  2.0/materials/mat0002_colr_pntl.fx
//  
//  Material description:
//      env=grassfadeout
//      env=useterraincolor
//      map=terrainlightmap
//      deform=swinging
//      env=ambientlighting
//      env=diffuselighting
//      env=specularlighting
//      map=clipmap
//      map=colormap
//  Material case:
//      LightType=pntl
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

#include "shaders:../lib/bill.fx"
#include "shaders:../lib/gmmt.fx"
float4   LightAmbient;
float4   LightDiffuse;
float4   LightSpecular;
float  TerrainAmbientFactor;
float TerrainDiffuseFactor;
float  LightMapDiffuseFactor;
float4   ModelLightPos;
float4 matDiffuse;
float4 matSpecular;
float  matShininess;
float  matLevel;


shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

shared float4x4 Model;
float4 matAmbient;
shared float TerrainSideSizeInv;    // 1 / block distance (weight map texture scale factor (scale UV));
float3 TerrainCellPosition;         // terrain cell position;
float mipMapLodBias = 0.0f;
texture diffMap;
texture lightMap;
shared float Time;

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/lightmapsampler_clamp.fx"
struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 color  : COLOR;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float3 uv1           : TEXCOORD1;
    float4 diffuse       : COLOR0;
    float4 specular      : COLOR1;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    float3 eyePos = ModelEyePos;
    position = swing2(position, vsIn.color, Time);
    vsOut.position = mul(position, ModelViewProjection);
    float3 eyeVec = normalize(eyePos - position);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    float4 TerrainPos = mul(position, Model);
    CalculateGMMWeightUV( TerrainPos, TerrainCellPosition, TerrainSideSizeInv, vsOut.uv1.xy );
    vsOut.uv1.z = vsIn.uv1.x;
    // lighting
    float4 lightPos = ModelLightPos;
    float3 lightVec = normalize(lightPos - position);
    // per-vertex diffuse lighting
    float diffuse = saturate(dot(normal, lightVec));
    vsOut.diffuse = diffuse * LightDiffuse * matDiffuse;
    // per-vertex specular lighting
    float3 halfVec = normalize(lightVec + eyeVec);
    float specular = pow(saturate(dot(normal, halfVec)), matShininess);
    vsOut.specular = specular * LightSpecular * matSpecular;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 uv0 = psIn.uv0;
    // diffuse lightmap
    float4 lightMapValue = tex2D(LightmapSampler, psIn.uv1.xy);
    float4 TerrainColor = lerp( float4(1.0,1.0,1.0,1.0), lightMapValue , psIn.uv1.z );
    float4 lightMapColor = TerrainDiffuseFactor*LightDiffuse*float4(lightMapValue.a, lightMapValue.a , lightMapValue.a , 0.0f);

    float4 vertexMatAmbient = matAmbient;
    float4 vertexMatDiffuse = matDiffuse;
    // per vertex diffuse lighting
    //float4 diffuse = psIn.diffuse;
    //dstColor = diffuse;
    float4 ambient = LightAmbient;
    ambient *= TerrainAmbientFactor;
    ambient += lightMapColor;
    // ambient lighting
    dstColor = ambient * vertexMatAmbient;
    
    dstColor *= TerrainColor;
    
    // diffuse map
    dstColor *= tex2D(DiffSampler, uv0);

    float4 levelColor = matLevel;
    // per vertex specular lighting
    float4 specular = psIn.specular;

    // modulate LightMap diffuse
    specular *= 2.0 * lightMapColor;
    //dstColor += specular * levelColor;
    
    
    //return lightMapColor;
    //return LightDiffuse*lightMapAlpha;
    //return tex2D(LightmapSampler, psIn.uv1.xy)*(tex2D(LightmapSampler, psIn.uv1.xy).a*LightDiffuse + TerrainAmbientFactor*LightAmbient ) ;
    
    //return float4(0,1,0,1);
    return dstColor;
}

technique t0
{
    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        FogEnable        = False;
        DepthBias        = 0.0f;
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
