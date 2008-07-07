//------------------------------------------------------------------------------
//  2.0/zombie/phong.fx
//  Implement phong lighting using dot3(normal,light)
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos; 

matrix<float,4,3> JointPalette[72];

float4 LightAmbient;
float4 LightDiffuse;
float4 LightSpecular;
float4 ModelLightPos;

float4 matAmbient;
float4 matDiffuse;
float4 matSpecular;
float matShininess;
float matLevel;

int mipMapLodBias;

texture diffMap;

#include "shaders:../lib/diffsampler.fx"

struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv0      : TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
    float4 specular : COLOR1;
};

VsOutput vsMain(const VsInput vsIn, const uniform int geomType)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal = vsIn.normal;
    if (geomType == GEOMETRY_SKINNED)
    {
        position = skinnedPosition(position, vsIn.weights, vsIn.indices, JointPalette);
    }
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    //phong lighting
    float3 lightVec = normalize(ModelLightPos - position);
    float3 eyeVec = normalize(ModelEyePos - position);
    float3 halfVec = normalize(lightVec + eyeVec); 
    // vertex lighting
    float diffuse = saturate(dot(normal, lightVec));
    vsOut.diffuse = diffuse * LightDiffuse * matDiffuse;
    float specular = pow(saturate(dot(normal, halfVec)), matShininess);
    vsOut.specular = specular * LightSpecular * matSpecular;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 ambient = LightAmbient * matAmbient;
    float4 diffuse = psIn.diffuse;
    float4 specular = psIn.specular;
    float4 color = tex2D( DiffSampler, psIn.uv0 );
    return (ambient + diffuse) * color + specular * matLevel;
}

technique tPhongStatic
{
    pass p0
    {
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_DEFAULT);
        PixelShader     = compile ps_1_1 psMain();
    }
}

technique tPhongSkinned
{
    pass p0
    {
        VertexShader    = compile vs_1_1 vsMain(GEOMETRY_SKINNED);
        PixelShader     = compile ps_1_1 psMain();
    }
}
