//------------------------------------------------------------------------------
//  ps2.0/gmmt_lmap.fx
//
//  Geomipmap terrain, detail texturing, lightmap generation
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
shared float3   EyePos;             // camera position
int             CullMode = 2;       // CW

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient = { 0.3f, 0.3f, 0.3f, 1.0f };
float4   LightDiffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
float4   LightSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
float4   ModelLightPos;
float4   ModelLightDir;

float4 matAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 matDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 matSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
float  matShininess = 60.0f;
float  matLevel = 1.0f;

texture ShadowMap;
float4x4 ShadowProjection;
float4 TerrainShadowMapInfo; // xy is  offset , z shadow map resolution, w gauss disk scale

static const int   MaxSamples  = 12;

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_glot_sampler.fx"
#include "../lib/gmmt_dett_sampler.fx"
#include "../lib/shadowmapsampler.fx"

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
    float2 uv0      : TEXCOORD0;
    float3 uv1      : TEXCOORD1; // weight map uv.xy, uv1.z is blend factor
    float2 uv2      : TEXCOORD2;
    float2 uv3      : TEXCOORD3;
    float2 uv4      : TEXCOORD4;
    float2 uv5      : TEXCOORD5;
};

struct VsOutputLight
{
    float4 position : POSITION;
    float4 uvlight  : TEXCOORD0;
    float4 modelPos : TEXCOORD1;
    float3 normal   : TEXCOORD2;
};

//------------------------------------------------------------------------------
/**
   UpdateTerrainOffset
    
    Update the sample offsets for multiple sampling to shadowmap
    This is normally executed in the pre-shader.
*/
void UpdateTerrainOffset( out float2 terrainOffset[MaxSamples] )
{
    float shadowMapResolution = TerrainShadowMapInfo.z;
    float diskSize  = TerrainShadowMapInfo.w;
    float scale = diskSize / shadowMapResolution;
   // 12 samples
    const float2 SampleOffsets[MaxSamples] = { // poisson disc
        {-0.326212f, -0.40581f },
        {-0.840144f, -0.073580f },
        {-0.695914f,  0.457137f},
        {-0.203345f,  0.620716f},
        { 0.96234f,  -0.194983f},
        { 0.473434f, -0.480026f},
        { 0.519456f,  0.767022f},
        { 0.185461f, -0.893124f},
        {0.507431f ,  0.064425f},
        {0.89642f,    0.412458f},
        {-0.32194f,  -0.932615f},
        {-0.791559f, -0.59771f }
        };
        
    int x;
    for ( x = 0; x < MaxSamples ; ++x)
    {
        //terrainOffset[x] = mul( lightmapPixelSize2*SampleOffsets[x] , terrainOffsetMatrix );
        terrainOffset[x] = scale *SampleOffsets[x]; // SHADOW MAP RESOLUTION
    }
}

//------------------------------------------------------------------------------
/**
    shadow()
*/
float
shadowGeneration(const float4 uvlight, const float4 modelPos,  sampler shadowMapSampler)
{
    float3 projShadowPos = uvlight.xyz / uvlight.w;
    projShadowPos.y = -projShadowPos.y;
    projShadowPos.xy = projShadowPos.xy*0.5f+0.5f;
    projShadowPos.xy +=  TerrainShadowMapInfo.xy / TerrainShadowMapInfo.z; //  .xy is the pixels offset  divide it by resloution.
   
    float2 terrainOffset[12];
    UpdateTerrainOffset( terrainOffset);
    
    float4 shadowMapVals0;
    float4 shadowMapVals1;
    float4 shadowMapVals2;
    float3 percentInLight;
    
    shadowMapVals0.r = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[0] ) );
    shadowMapVals0.g = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[1] ) );
    shadowMapVals0.b = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[2] ) );
    shadowMapVals0.a = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[3] ) );
    float4 inLight0 = ( projShadowPos.z < shadowMapVals0); // Compare depth for each component
    
    shadowMapVals1.r = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[4] ) );
    shadowMapVals1.g = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[5] ) );
    shadowMapVals1.b = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[6] ) );
    shadowMapVals1.a = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[7] ) );
    float4 inLight1 = ( projShadowPos.z < shadowMapVals1);     
    
    shadowMapVals2.r = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[8] ) );
    shadowMapVals2.g = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[9] ) );
    shadowMapVals2.b = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[10] ) );
    shadowMapVals2.a = DecodeFromRGBA( tex2D(shadowMapSampler, projShadowPos.xy + terrainOffset[11] ) );
    float4 inLight2 = ( projShadowPos.z < shadowMapVals2); 
    
    percentInLight.r = dot(inLight0, float4(0.25, 0.25, 0.25, 0.25) ); //average
    percentInLight.g = dot(inLight1, float4(0.25, 0.25, 0.25, 0.25) ); //average
    percentInLight.b = dot(inLight2, float4(0.25, 0.25, 0.25, 0.25) ); //average
    
    return dot(percentInLight, float3(0.33333f, 0.33334f, 0.33f) ); //average
}

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

    return vsOut;
}

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutputLight vsMainLight(const VsInput vsIn)
{
    VsOutputLight vsOut;

    // get projected position
    vsOut.position = mul(float4(vsIn.position,1), ModelViewProjection);

    // point light vectors in model space
    vsOut.modelPos = float4(vsIn.position, 1.0f);
    vsOut.normal   = normalize(vsIn.normal / vsIn.normal.y); //  always y  > 0 
    

    //find shadowmap uv
    vsOut.uvlight = mul(float4(vsIn.position,1), ShadowProjection);

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
    
    float4 dstColor = float4(baseColor.xyz, 0.0f);

    return dstColor;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMainLight(const VsOutputLight psIn) : COLOR
{
    float4 uvlight = mul( psIn.modelPos , ShadowProjection);
    float shadowColor = shadowGeneration(uvlight, psIn.modelPos , ShadowMapSampler);
    //float shadowColor = shadowGeneration(psIn.uvlight, ShadowMapSampler);

    // compute diffuse term by pixel
    float3 lightVec = normalize(ModelLightDir.xyz);
    float3 normalizedNormal = normalize(psIn.normal.xyz);
    float diffIntensity = saturate(dot(normalizedNormal, lightVec));

    diffIntensity *= shadowColor;

    return float4( 1.0f, 0.0f , 0.0f , diffIntensity);
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        FogEnable    = False;
        ZFunc         = LessEqual;
        AlphaBlendEnable = false;
        ColorWriteEnable    = ALPHA;
        SrcBlend  = One;
        DestBlend = One;    

        VertexShader = compile vs_2_0 vsMainLight();
        PixelShader  = compile ps_2_0 psMainLight();
    }
    
    pass p1
    {
        FogEnable    = False;
        AlphaBlendEnable = false;
        ColorWriteEnable    = RED|GREEN|BLUE;
        ZFunc         = LessEqual;
        DestBlend = One;   
        SrcBlend  = One;
        

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
