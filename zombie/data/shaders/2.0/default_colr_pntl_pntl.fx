//------------------------------------------------------------------------------
//  2.0/default_colr_pntl.fx
//  
//  vertex+pixel lighting with 2 lights and bump
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient[2];
float4   LightDiffuse[2];
float4   LightSpecular[2];
float4   ModelLightPos[2];
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
    float3 lightVec0     : TEXCOORD1;
    float3 halfVec0      : TEXCOORD2;
    float3 lightVec1     : TEXCOORD3;
    float3 halfVec1      : TEXCOORD4;
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
    float3 eyeDir = normalize(ModelEyePos - position);
    // lighting
    //float dist = distance(ModelLightPos[0], position);
    //float attenuation = 1 / (LightAttenuation.x  + LightAttenuation.y * dist + LightAttenuation.z * dist * dist);
    float attenuation = 1.0f;
    // tangent space lighting
    float3 lightVec[2];
    float3 halfVec[2];
    int i;
    for (i = 0; i < 2; ++i)
    {
    tangentSpaceLightHalfVector2(position, ModelLightPos[i], ModelEyePos, normal, binormal, tangent,
                                 lightVec[i], halfVec[i]);
    }
    vsOut.lightVec0 = lightVec[0];
    vsOut.halfVec0  = halfVec[0];
    vsOut.lightVec1 = lightVec[1];
    vsOut.halfVec1  = halfVec[1];
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    
    float3 lightVec[2];
    float3 halfVec[2];
    lightVec[0] = psIn.lightVec0;
    halfVec[0]  = psIn.halfVec0;
    lightVec[1] = psIn.lightVec1;
    halfVec[1]  = psIn.halfVec1;
    
    int i;
    for (i = 0; i < 2; ++i)
    {
    // ambient lighting
    dstColor += LightAmbient[i] * MatAmbient;
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(lightVec[i]);
    float diffIntensity = saturate(dot(tangentSurfaceNormal, tangentLightVec));
    float4 diffuse = diffIntensity * LightDiffuse[i] * MatDiffuse;
    dstColor += diffuse; 
    }
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    for (i = 0; i < 2; ++i)
    {
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(halfVec[i]);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), MatSpecularPower);
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
