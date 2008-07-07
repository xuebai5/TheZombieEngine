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
texture bumpMap;

#include "shaders:../lib/diffsampler.fx"
#include "shaders:../lib/bumpsampler.fx"

struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv0      : TEXCOORD0;
    float4 weights  : BLENDWEIGHT;
    float4 indices  : BLENDINDICES;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float3 lightVec : TEXCOORD1;
    float3 halfVec  : TEXCOORD2;
};

VsOutput vsMain(const VsInput vsIn, const uniform int geomType)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    float3 normal = vsIn.normal;
    float3 tangent = vsIn.tangent;
    float3 binormal = cross(normal, tangent);
    float3x3 tangentMatrix = float3x3(tangent, binormal, normal);
    if (geomType == GEOMETRY_SKINNED)
    {
        position = skinnedPosition(position, vsIn.weights, vsIn.indices, JointPalette);
    }
    vsOut.position = mul(position, ModelViewProjection);
    vsOut.uv0 = vsIn.uv0;
    //tangent space
    float3 lightVec = ModelLightPos - position;
    float3 eyeVec = ModelEyePos - position;
    float3 halfVec = lightVec + eyeVec;
    vsOut.lightVec = mul( tangentMatrix, lightVec );
    vsOut.halfVec = mul( tangentMatrix, halfVec );
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float3 lightVec = normalize( psIn.lightVec );
    float3 halfVec = normalize( psIn.halfVec );
    //float3 normal = normalize( 2.f * tex2D( BumpSampler, psIn.uv0 ) - 1.f );
    float3 normal = textureToNormal( tex2D( BumpSampler, psIn.uv0 ) ); 
    float4 ambient = LightAmbient * matAmbient;
    float4 diffuse = LightDiffuse * matDiffuse * saturate( dot( normal, lightVec ) );
    float4 specular = LightSpecular * matSpecular * pow( saturate( dot( normal, halfVec ) ), matShininess );
    float4 color = tex2D( DiffSampler, psIn.uv0 );
    return (ambient + diffuse) * color + specular * matLevel;
}

technique tBumpStatic
{
    pass p0
    {
        VertexShader    = compile vs_2_0 vsMain(GEOMETRY_DEFAULT);
        PixelShader     = compile ps_2_0 psMain();
    }
}

technique tBumpSkinned
{
    pass p0
    {
        VertexShader    = compile vs_2_0 vsMain(GEOMETRY_SKINNED);
        PixelShader     = compile ps_2_0 psMain();
    }
}
