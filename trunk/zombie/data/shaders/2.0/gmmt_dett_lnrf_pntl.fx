//------------------------------------------------------------------------------
//  ps2.0/gmmt_dett_lnrf_pntl.fx
//
//  Geomipmap terrain, detail texturing, linear fog, point lighting
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"
#include "../lib/fog.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float3   ModelEyePos;
shared float3   EyePos;             // camera position
int             CullMode = 2;       // CW

float4 fogDistances;
float4 fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient = { 0.3f, 0.3f, 0.3f, 1.0f };
float4   LightDiffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
float4   LightSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
float4   ModelLightPos;
int      LightType = LIGHT_POINT;

float4 matAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 matDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 matSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
float  matShininess = 60.0f;
float  matLevel = 1.0f;
float TerrainAmbientFactor = 1.0f;
float TerrainDiffuseFactor = 1.0f;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION0;
    float3 normal   : NORMAL;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float3 uv1 : TEXCOORD1;     // weight map uv.xy, uv1.z is blend factor
    float2 uv2 : TEXCOORD2;
    float2 uv3 : TEXCOORD3;
    float2 uv4 : TEXCOORD4;
    float2 uv5 : TEXCOORD5;
    float4 diffuse : COLOR0;
    float4 specular : COLOR1;
    float fog : FOG;    
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_glot_sampler.fx"
#include "../lib/gmmt_dett_sampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // get projected position
    vsOut.position = mul(float4(vsIn.position,1), ModelViewProjection);
    
    // calculate global texture coordinates
    CalculateGMMGlobalUV( vsIn.position, TerrainGlobalMapScale, vsOut.uv0 );
    
    // calculate detail texture coordinates
    CalculateGMMWeightUV( vsIn.position, TerrainCellPosition, TerrainSideSizeInv, vsOut.uv1.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform0U, TerrainCellTransform0V, vsOut.uv2.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform1U, TerrainCellTransform1V, vsOut.uv3.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform2U, TerrainCellTransform2V, vsOut.uv4.xy );
	CalculateGMMMaterialUV( vsIn.position, TerrainCellTransform3U, TerrainCellTransform3V, vsOut.uv5.xy );

    // calculate blending factor
    CalculateGMMBlend( vsIn.position, EyePos, TerrainCellDistSquaredInv, vsOut.uv1.z );

    // point lighting per vertex
    vsLightingPointGMM(
        vsIn.position, vsIn.normal, 
        matDiffuse, matSpecular, matShininess,
        ModelEyePos, ModelLightPos, 
        LightAttenuation, LightDiffuse, LightSpecular, LightType,
        vsOut.diffuse, vsOut.specular);

    // fog   
    vsOut.fog = linearFog(vsIn.position, fogDistances, ModelView);

    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // process material weights
    float4 matWeights;
    float weightEmpty;
    matWeights = tex2D(MiniWeightMapSampler, psIn.uv1);
    ProcessGMMMaterialWeights4(matWeights, weightEmpty);
    
    // get global texturing color
    float4 globalColor = GetGMMGlobalColor( GlobalSampler, psIn.uv0 );
    
    // get detail texturing color
    float4 detailColor = GetGMMDetailColor( matWeights, weightEmpty,
        psIn.uv2, psIn.uv3, psIn.uv4, psIn.uv5,
        BlendTexture0Sampler, BlendTexture1Sampler, BlendTexture2Sampler, BlendTexture3Sampler );
    
    // blend global and detail texturing color
    float4 baseColor = BlendGMMGlobalDetail( globalColor, detailColor, psIn.uv1.z );

    // modulate color
    baseColor = ModulateGMMColor( baseColor, ModulationSampler, psIn.uv0, TerrainModulationFactor );

    // point lighting per pixel
    float lightDiffIntensity = tex2D(LightmapSampler, psIn.uv1).a;
    float4 lightMapColor = TerrainDiffuseFactor*lightDiffIntensity * LightDiffuse;
    float4 specular = lightDiffIntensity * psIn.specular;
    float4 retColor = psLightingPointGMM(
        baseColor, matAmbient, matLevel * detailColor.a,
        psIn.diffuse, specular, TerrainAmbientFactor * LightAmbient, lightMapColor);
              
    return retColor;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        //FogEnable           = True;
        FogColor            = <fogColor>;
        //FogVertexMode       = Linear;
        //FogTableMode        = None;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
