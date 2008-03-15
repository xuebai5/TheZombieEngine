//------------------------------------------------------------------------------
//  2.0/default_colr_pntl_pntl_ps.fx
//  
//  pixel lighting only with 2 lights and bump
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float3   ModelEyePos;
shared float3   EyePos;

#define NUM_LIGHTS 3

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient[NUM_LIGHTS];
float4   LightDiffuse[NUM_LIGHTS];
float4   LightSpecular[NUM_LIGHTS];
float4   ModelLightPos[NUM_LIGHTS];
float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

texture DiffMap0;
texture BumpMap0;

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
    float3 pos           : TEXCOORD1;
    float3 normal        : TEXCOORD2;
    float3 tangent       : TEXCOORD3;
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
    // parameters for tangent space computations
    vsOut.pos     = position.xyz;
    vsOut.normal  = vsIn.normal;
    vsOut.tangent = vsIn.tangent;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 normal = normalize(psIn.normal);
    float3 tangent = normalize(psIn.tangent);
    float3 binormal = cross(normal, tangent);

    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 

    // compute surface normal in tangent space
    float3 modelSurfaceNormal;
    modelSpaceSurfaceNormalVector(tangentSurfaceNormal, normal, binormal, tangent, modelSurfaceNormal);
    
    // compute tangent space light vectors
    float3 lightVec[NUM_LIGHTS];
    float3 halfVec[NUM_LIGHTS];
    int i;
    for (i = 0; i < NUM_LIGHTS; ++i)
    {
    modelSpaceLightHalfVector(psIn.pos, ModelLightPos[i], ModelEyePos, lightVec[i], halfVec[i]);
    }
    
    for (i = 0; i < NUM_LIGHTS; ++i)
    {
    // ambient lighting
    dstColor += LightAmbient[i] * MatAmbient;
    // per-pixel diffuse lighting
    float3 modelLightVec = normalize(lightVec[i]);
    float diffIntensity = saturate(dot(modelSurfaceNormal, modelLightVec));
    float4 diffuse = diffIntensity * LightDiffuse[i] * MatDiffuse;
    dstColor += diffuse; 
    }
    
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    for (i = 0; i < NUM_LIGHTS; ++i)
    {
    // per pixel specular lighting
    float3 modelHalfVec = normalize(halfVec[i]);
    float  specIntensity = pow(saturate(dot(modelSurfaceNormal, modelHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular[i] * MatSpecular; 
    float4 levelColor = MatLevel;
    dstColor += specular * levelColor;
    }

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
