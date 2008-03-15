#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection[3];
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient;
float4   LightDiffuse;
float4   LightSpecular;
float4   ModelLightPos;
float4 matDiffuse;

float4 matAmbient;
float mipMapLodBias = 0.0f;

struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 instIndex : TEXCOORD1;
};

struct VsOutput
{
    float4 position      : POSITION;
    float4 diffuse       : COLOR0;
    float4 specular      : COLOR1;
};

VsOutput vsMain(const VsInput vsIn
)
{
    VsOutput vsOut = (VsOutput)0;
    float4 position = vsIn.position;
    float3 normal   = vsIn.normal;
    vsOut.position = mul(position, ModelViewProjection[vsIn.instIndex.x]);
    // texture coordinates
    // lighting
    float dist = distance(ModelLightPos, position);
    float attenuation = 1 / (LightAttenuation.x  + LightAttenuation.y * dist + LightAttenuation.z * dist * dist);
    float3 lightVec = normalize(ModelLightPos - position);
    // per-vertex diffuse lighting
    float diffuse = attenuation;
    diffuse *= saturate(dot(normal, lightVec));
    vsOut.diffuse = diffuse * LightDiffuse * matDiffuse;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 tangentSurfaceNormal = float3(0.0f, 0.0f, 1.0f);
    // ambient lighting
    dstColor = LightAmbient * matAmbient;
    // per vertex diffuse lighting
    float4 diffuse = psIn.diffuse;
    dstColor += diffuse;

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
