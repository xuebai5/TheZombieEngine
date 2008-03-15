//------------------------------------------------------------------------------
//  2.0/default_colr_lmap_pntl.fx
//  
//  Material description:
//      env=ambientlighting
//      env=diffuselighting
//      env=specularlighting
//      map=colormap
//      map=lightmap
//  Material case:
//      LightType=pntl
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/lib.fx"
#include "shaders:../lib/libzombie.fx"

shared float4x4 ModelViewProjection;
shared float3   ModelEyePos;
shared float3   EyePos;

float4   LightAttenuation = { 1.0f, 0.0f, 0.0f, 0.0f };
float4   LightAmbient;
float4   LightDiffuse;
float4   LightSpecular;
float4   ModelLightPos;
float    LightMapMod;//[NumLights]

float4 MatDiffuse;
float4 MatSpecular;
float  MatSpecularPower;
float  MatLevel = 1.0f;

float4 MatAmbient;
float mipMapLodBias = 0.0f;
texture DiffMap0;
texture DiffMap1;
texture BumpMap0;

sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= <mipMapLodBias>;
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

sampler LightmapSampler = sampler_state
{
    Texture = <DiffMap1>;
    AddressU  = Wrap;//Clamp;
    AddressV  = Wrap;//Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;//None;
};

struct VsInput
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT; 
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
};

struct VsOutput
{
    float4 position      : POSITION;
    float2 uv0           : TEXCOORD0;
    float2 uv1           : TEXCOORD1;
    float3 primLightVec  : TEXCOORD2;
    float3 primHalfVec   : TEXCOORD3;
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
    vsOut.uv1 = vsIn.uv1;
    // tangent space lighting
    float3 primLightVec;
    float3 primHalfVec;
    tangentSpaceLightHalfVector2(position, ModelLightPos, ModelEyePos, normal, binormal, tangent,
                                 primLightVec, primHalfVec);
    vsOut.primLightVec = primLightVec;
    vsOut.primHalfVec  = primHalfVec;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float2 uv0 = psIn.uv0;
	
	// bumpmapping
    float3 tangentSurfaceNormal = 2.0f * tex2D(BumpSampler, psIn.uv0) - 1.0f; 
    tangentSurfaceNormal = normalize(tangentSurfaceNormal); 
    
    // lightmapping
    float4 lightmapColor = tex2D(LightmapSampler, psIn.uv1);
    
    // ambient lighting
    dstColor = LightAmbient * MatAmbient;
    
    // per-pixel diffuse lighting
    float3 tangentLightVec = normalize(psIn.primLightVec);
    float diffIntensity = saturate(dot(tangentSurfaceNormal, tangentLightVec));
    float4 diffuse = diffIntensity * LightDiffuse * MatDiffuse;
    // no lightmap modulation
    //dstColor += diffuse; 
    // lightmap modulation
    dstColor += diffuse * lightmapColor * LightMapMod + lightmapColor * (1 - LightMapMod);
    
    // diffuse lightmap
    //dstColor += lightmapColor;// for the dynamic light
    //dstColor += lightmapColor * (1 - Intensity0);// for a static light
    
    // diffuse map
    dstColor *= tex2D(DiffSampler, uv0);
    
    // per pixel specular lighting
    float3 tangentHalfVec = normalize(psIn.primHalfVec);
    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), MatSpecularPower);
    float4 specular = specIntensity * LightSpecular * MatSpecular; 
    float4 levelColor = MatLevel;
    //dstColor += specular * levelColor;//no lightmap modulation
    dstColor += specular * levelColor * lightmapColor;// * Intensity0; // lightmap modulation
    //modulate LightMap diffuse
    //specular *= lightmapColor * Intensity0;// lightmap modulation
    //dstColor += specular * levelColor; 
    
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
