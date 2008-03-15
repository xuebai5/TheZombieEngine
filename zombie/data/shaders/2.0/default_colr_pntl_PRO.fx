//------------------------------------------------------------------------------
//  2.0/default_colr_pntl_FULL.fx
//  
//  pixel lighting only with 1 light
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
shared float4x4 Model;
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAmbient;
float4   LightDiffuse;
float4   LightSpecular;
float4   ModelLightPos;
float4 MatAmbient;
float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel;

texture DiffMap0;
texture BumpMap0;

texture CubeMap0;
texture envMaskMap;
float   envMaskFactor;

//if (hasFading)
float MinDist;
float MaxDist;

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

#include "shaders:../lib/environmentsampler.fx"
#include "shaders:../lib/environmentmask.fx"

struct VsInput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT; 
    float3 binormal : BINORMAL;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 normal   : TEXCOORD2;
    float3 modelPos : TEXCOORD1;
    float4 tangent  : TEXCOORD3;
    //if (hasFading)
    float smooth    : TEXCOORD4;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    float3 tangent  = vsIn.tangent;
    float3 binormal  = vsIn.binormal;
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    vsOut.normal = normal;
    vsOut.modelPos = position.xyz;
    vsOut.tangent.xyz = tangent;
    vsOut.tangent.w = dot(normal, cross(tangent, binormal)) > 0 ? 1.0f : -1.0f;
    //if (hasFading)
    vsOut.smooth = smooth(position, MinDist, MaxDist, ModelView);

    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{   
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 normal = normalize(psIn.normal);
    float3 tangent = normalize(psIn.tangent.xyz);
    float3 binormal = psIn.tangent.w * cross(normal, tangent.xyz); // The w is -1 or 1 , is direction
    float3x3 tangentSpaceMatrix = float3x3(tangent, binormal, normal);
    float3 eyeVec = normalize(ModelEyePos - psIn.modelPos);
    // surface normal in tangent space
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    //----
    //if (hasFading)
    tangentSurfaceNormal = lerp(float3(0.0f, 0.0f, 1.0f), tangentSurfaceNormal, psIn.smooth);
    //----
    tangentSurfaceNormal = normalize(tangentSurfaceNormal);
    // surface normal in model space
    float3 modelSurfaceNormal = mul(tangentSurfaceNormal, tangentSpaceMatrix);
    // light vector in model space
    float3 lightVec0;
    lightVec0 = normalize(ModelLightPos - psIn.modelPos);
    float3 halfVec0;
    modelSpaceLightHalfVector(psIn.modelPos, ModelLightPos, ModelEyePos, lightVec0, halfVec0);
    // reflection vector in model space
    float3 reflectEye = reflect(-eyeVec, modelSurfaceNormal);
    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    // per-pixel diffuse lighting
    float3 modelLightVec = normalize(lightVec0);
    float diffIntensity = saturate(dot(modelSurfaceNormal, modelLightVec));
    float4 diffuse = diffIntensity * LightDiffuse * MatDiffuse;
    dstColor += diffuse; 
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    // per pixel environment mapping
    float envLerp = envMaskFactor; 
    envLerp *= tex2D(EnvMask, psIn.uv0); 
    float3 refl = mul(reflectEye, Model);
    dstColor.xyz = lerp(dstColor.xyz, texCUBE(EnvironmentSampler, refl), envLerp);
    // per pixel specular lighting
    float specIntensity = pow(saturate(dot(reflectEye, lightVec0)), MatSpecularPower);
    //float3 modelHalfVec = normalize(halfVec0);
    //float specIntensity = pow(saturate(dot(modelSurfaceNormal, modelHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular * MatSpecular; 
    float4 levelColor = MatLevel;
    dstColor += specular * levelColor * psIn.smooth; //if (hasFading)

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
