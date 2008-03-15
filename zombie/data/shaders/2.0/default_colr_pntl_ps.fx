//------------------------------------------------------------------------------
//  2.0/default_colr_pntl_ps.fx
//  
//  pixel lighting only with 1 light
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float4x4 ModelView;
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
    float3 modelPos      : TEXCOORD1;
    float3 normal        : TEXCOORD2;
    float3 tangent       : TEXCOORD3;
};

VsOutput vsMain(const VsInput vsIn
)
{
    VsOutput vsOut = (VsOutput)0;
    float4 position = vsIn.position;
    vsOut.position = mul(position, ModelViewProjection);
    // texture coordinates
    vsOut.uv0 = vsIn.uv0;
    vsOut.modelPos = position.xyz;
    vsOut.normal   = vsIn.normal;
    vsOut.tangent  = vsIn.tangent;
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

    // compute surface normal in model space
    float3 modelSurfaceNormal;
    modelSpaceSurfaceNormalVector(tangentSurfaceNormal, normal, binormal, tangent, modelSurfaceNormal);

    float3 lightVec0;
    float3 halfVec0;
    modelSpaceLightHalfVector(psIn.modelPos, ModelLightPos, ModelEyePos, lightVec0, halfVec0);

    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    // per-pixel diffuse lighting
    float3 modelLightVec = normalize(lightVec0);
    float diffIntensity = saturate(dot(modelSurfaceNormal, modelLightVec));
    float4 diffuse = diffIntensity * LightDiffuse * MatDiffuse;
    dstColor += diffuse; 
    // diffuse map
    dstColor *= tex2D(DiffSampler, psIn.uv0);
    // per pixel specular lighting
    float3 modelHalfVec = normalize(halfVec0);
    float  specIntensity = pow(saturate(dot(modelSurfaceNormal, modelHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular * MatSpecular; 
    float4 levelColor = MatLevel;
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
