//------------------------------------------------------------------------------
//  ps2.0/gmmt_glot_pntl.fx
//
//  Geomipmap terrain, global texturing, point lighting
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
int             CullMode = 2;       // CW

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

float4 Color0 = { 0.0f, 0.0f, 0.0f, 0.0f };// just for testing

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
    float4 diffuse : COLOR0;
    float4 specular : COLOR1;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_glot_sampler.fx"

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
    
    // point lighting per vertex
    vsLightingPointGMM(
        vsIn.position, vsIn.normal, 
        matDiffuse, matSpecular, matShininess,
        ModelEyePos, ModelLightPos, 
        LightAttenuation, LightDiffuse, LightSpecular, LightType,
        vsOut.diffuse, vsOut.specular);
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    // get global texturing color
    float4 globalColor = GetGMMGlobalColor( GlobalSampler, psIn.uv0 );

    // modulate color
    float4 baseColor = ModulateGMMColor( globalColor, ModulationSampler, psIn.uv0, TerrainModulationFactor );
    
    // point lighting per pixel
    float lightDiffIntensity = tex2D(GlobalLightmapSampler, psIn.uv0).a;
    float4 lightMapColor = TerrainDiffuseFactor*lightDiffIntensity * LightDiffuse;
    float4 specular = lightDiffIntensity * psIn.specular;
    float4 retColor = psLightingPointGMM(
        baseColor, matAmbient, matLevel * baseColor.a,
        psIn.diffuse, specular, TerrainAmbientFactor * LightAmbient, lightMapColor );

    return retColor;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        //FogEnable           = False;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
