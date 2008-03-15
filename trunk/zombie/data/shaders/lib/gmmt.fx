//------------------------------------------------------------------------------
//  gmmt.fx
//  Geomipmap terrain library
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "shaders:../lib/libzombie.fx"

//------------------------------------------------------------------------------
/**
    Calculate the UV tex coords for the global texture. The global texture
    comprehends the whole terrain.
*/
void CalculateGMMGlobalUV( 
    in const float3 position,               // vertex position
    in const float TerrainGlobalMapScale,   // terrain map scale
    out float2 uv0                          // global texture uv
)
{
    // uv0 used for global texture
    uv0.xy = position.xz * TerrainGlobalMapScale;
}

//------------------------------------------------------------------------------
/**
    Calculate the UV tex coords used when texturing the terrain in detail:
    weightmap tex coords, and all the material tex coords.
*/
void CalculateGMMDetailUV( 
    in const float3 position,                   // vertex position
    in const float3 TerrainCellPosition,        // terrain cell position
    in const float TerrainSideSizeInv,          // 1 / (size of a terrain side)
    in const float4 TerrainCellTransform[10],   // terrain cell transform UV, for mat 0-4
    out float2 uv1,             // weight texture
    out float2 uv2,             // uv for material 0
    out float2 uv3,             // uv for material 1
    out float2 uv4,             // uv for material 2
    out float2 uv5,             // uv for material 3
    out float2 uv6              // uv for material 4
)
{
    // relative xz    
    float2 relxz = position.xz - TerrainCellPosition.xz;
    
    // uv1 used for weight texture
    uv1.xy = relxz * TerrainSideSizeInv;
    
    // uv for each texture layer
    uv2.x = dot(position, TerrainCellTransform[0]);
    uv2.y = dot(position, TerrainCellTransform[1]);

    uv3.x = dot(position, TerrainCellTransform[2]);
    uv3.y = dot(position, TerrainCellTransform[3]);

    uv4.x = dot(position, TerrainCellTransform[4]);
    uv4.y = dot(position, TerrainCellTransform[5]);

    uv5.x = dot(position, TerrainCellTransform[6]);
    uv5.y = dot(position, TerrainCellTransform[7]);

    uv6.x = dot(position, TerrainCellTransform[8]);
    uv6.y = dot(position, TerrainCellTransform[9]);
}

//------------------------------------------------------------------------------
/**
    Calculate the UV tex coords used for weightmap tex coords
*/
void CalculateGMMWeightUV( 
    in const float3 position,                   // vertex position
    in const float3 TerrainCellPosition,        // terrain cell position
    in const float TerrainSideSizeInv,          // 1 / (size of a terrain side)
    out float2 uv1								// weight texture
)
{
    // relative xz    
    float2 relxz = position.xz - TerrainCellPosition.xz;
    
    // uv1 used for weight texture
    uv1.xy = relxz * TerrainSideSizeInv;
}

//------------------------------------------------------------------------------
/**
    Calculate the UV tex coords used when texturing the terrain in detail:
    weightmap tex coords, and all the material tex coords.
*/
void CalculateGMMMaterialUV( 
    in const float3 position,                   // vertex position
    in const float4 TerrainCellTransformU,      // terrain cell transform U
    in const float4 TerrainCellTransformV,      // terrain cell transform U
	out float2 uv								// uv for material
)
{
    // uv for each texture layer
    uv.x = dot(position, TerrainCellTransformU);
    uv.y = dot(position, TerrainCellTransformV);
}

//------------------------------------------------------------------------------
/**
    Calculate the blend factor from detail to global texturing.
    It used xz distance from vertex position and eye position.
    The blending uses a quadratic function, instead of a linear function, 
    in order to show more detail texturing on more space.
*/
void CalculateGMMBlend(
    in const float3 position,       // vertex position
    in const float3 EyePos,         // camera position
    in const float TerrainCellDistSquaredInv, // 1.0f / max dist squared
    out float blend                 // blend factor
)
{
    // vector from vertex to the camera    
    float2 dxz = EyePos.xz - position.xz;
    
    // calculate blend factor =
    // distance squared from vertex to the camera multiplied by factor
    float blendval = dot(dxz,dxz) * TerrainCellDistSquaredInv;
    //blend = float2(saturate(blendval), 1.0f - blendval);    
    //blend = float2(saturate(blendval), blendval);    
    blend = saturate(blendval);
}

//------------------------------------------------------------------------------
/**
    Process material weights for the terrain.
    It calculates the implicit weight for the terrain material 5.
    And it calculates the weight for empty material.
    The original material weights can be modified when there the empty material
    appears.
*/
void ProcessGMMMaterialWeights(
    inout float4 matWeights,            // 4 main material weights
    out float weightmat5,               // weight for material 5
    out float weightempty               // weight for empty material
)
{
    float sumWeights = matWeights.x + matWeights.y + matWeights.z + matWeights.w;
    matWeights = matWeights / sumWeights;
    float sumWeights2 = matWeights.x + matWeights.y + matWeights.z + matWeights.w;
    weightmat5 = 1.0f - sumWeights2;
#ifndef NGAME    
    if ((1.0f - sumWeights) < -2.0f)
    {
        matWeights.xyzw = float4(0.0f, 0.0f, 0.0f, 0.0f);
        weightmat5 = 0.0f;
        weightempty = 1.0f;
    }
#endif    
}

//------------------------------------------------------------------------------
/**
    Process material weights for the terrain.
    And it calculates the weight for empty material.
*/
void ProcessGMMMaterialWeights4(
    inout float4 matWeights,            // 4 main material weights
    out float weightempty               // weight for empty material
)
{
    float sumWeights = matWeights.x + matWeights.y + matWeights.z + matWeights.w;
    matWeights /= sumWeights;
    weightempty = 0.0f;
#ifndef NGAME    
    if (sumWeights > 2.0f)
    {
        matWeights.xyzw = float4(0.0f, 0.0f, 0.0f, 0.0f);
        weightempty = 1.0f;
    }
#endif    
}

//------------------------------------------------------------------------------
/**
*/
float4 GetGMMGlobalColor(
    in const sampler GlobalSampler,
    in const float2 uvglobal
)
{
    return tex2D(GlobalSampler, uvglobal);
}

//------------------------------------------------------------------------------
/**
*/
float4 GetGMMDetailColor(
    in const float4 matWeights,
    in const float weightempty,
    in const float2 uvmat0,
    in const float2 uvmat1,
    in const float2 uvmat2,
    in const float2 uvmat3,
    in const sampler BlendTexture0Sampler,
    in const sampler BlendTexture1Sampler,
    in const sampler BlendTexture2Sampler,
    in const sampler BlendTexture3Sampler
)
{
    float4 detailColor;
    
    detailColor  = matWeights.x * tex2D(BlendTexture0Sampler, uvmat0);
    detailColor += matWeights.y * tex2D(BlendTexture1Sampler, uvmat1);
    detailColor += matWeights.z * tex2D(BlendTexture2Sampler, uvmat2);
    detailColor += matWeights.w * tex2D(BlendTexture3Sampler, uvmat3);
#ifndef NGAME    
    detailColor +=  weightempty * float4(1.0f, 0.0f, 1.0f, 1.0f);
#endif    
    
    return detailColor;
}

//------------------------------------------------------------------------------
/**
*/
float4 GetGMMDetailColorLayer(
    in const float weight,
    in const float2 uvmat,
    in const sampler BlendTextureSampler
)
{
    float4 detailColor;
    
    detailColor  = weight * tex2D(BlendTextureSampler, uvmat);
    
    return detailColor;
}

//------------------------------------------------------------------------------
/**
*/
float4 GetGMMDetailEmptyLayer(
    in const float weight
)
{
    float4 detailColor;
    
    detailColor =  weight * float4(1.0f, 0.0f, 1.0f, 1.0f);
    
    return detailColor;
}

//------------------------------------------------------------------------------
/**
*/
float4 BlendGMMGlobalDetail(
    in const float4 globalColor,
    in const float4 detailColor,
    in const float blend
)
{
    return lerp(detailColor, globalColor, blend); 
}

//------------------------------------------------------------------------------
/**
*/
float4 ModulateGMMColor(
    in const float4 baseColor,
    in const sampler ModulationSampler,
    in const float2 uv,
    in const float TerrainModulationFactor
)
{
    float4 control = tex2D(ModulationSampler, uv);    
#if 0
    return baseColor * control;
#elif 1
    // old cheap way:
    return lerp(baseColor, baseColor * control, TerrainModulationFactor);
#elif 0
    // tofol's way
    float4 unitv = float4(1.0f, 1.0f, 1.0f, 1.0f);
    control = control * 0.8f;
    return control * lerp(unitv, control, (-4.0f * baseColor + 2.0f));
#else
    // overlay photoshop operation (http://www.pegtop.net/delphi/blendmodes/#overlay)
    // after optimization
    float4 x1 = 2.0f * control * baseColor;
    float4 x2 = 1.0f - 2.0f * (1.0f - control) * (1.0f - baseColor);
    float4 stepcontrol = step(0.5f, control);
    return lerp(x2 , x1, stepcontrol);
#endif    
}

//------------------------------------------------------------------------------
/**
*/
void vsLightingPointGMM(
    in const float3 position,
    in const float3 normal,
    in const float4 matDiffuse,
    in const float4 matSpecular,
    in const float  matShininess,    
    in const float3 ModelEyePos,
    in const float4 ModelLightPos,
    in const float4 LightAttenuation,
    in const float4 LightDiffuse,
    in const float4 LightSpecular,
    in const int    LightType,
    out float4 diffuseLighting,
    out float4 specularLighting
)
{
    float3 normalizedNormal = normalize(normal);
    // eye direction vector
    float3 eyeDir = normalize(ModelEyePos - position);

    // lighting
    float3 lightVec;
    float attenuation;
    if (LightType == LIGHT_DIR) {
        attenuation = 1;
        lightVec = normalize(ModelLightPos.xyz);
    } else {
        float dist = distance(ModelLightPos.xyz, position.xyz);
        attenuation = 1 / (LightAttenuation.x  + LightAttenuation.y * dist + LightAttenuation.z * dist * dist);
        lightVec = normalize(ModelLightPos.xyz - position);
    }

    // per-vertex diffuse lighting
    float diffuse = attenuation;
    diffuse *= saturate(dot(normalizedNormal, lightVec));
    diffuseLighting = diffuse * LightDiffuse * matDiffuse;

    // per-vertex specular lighting
    float3 halfVec = normalize(lightVec + eyeDir);
    float specular = pow(saturate(dot(normalizedNormal, halfVec)), matShininess);
    specular *= attenuation;
    specularLighting = specular * LightSpecular * matSpecular;
}

//------------------------------------------------------------------------------
/**
*/
float4 psLightingPointGMM(
    in const float4 diffuseColor,
    in const float4 matAmbient,
    in const float  matLevel,
    in const float4 diffuseLighting,
    in const float4 specularLighting,
    in const float4 LightAmbient,
    in const float4 lightmapColor)
{
    // ambient lighting
    float4 dstColor = LightAmbient * matAmbient;
    
    // per vertex diffuse lighting
    dstColor += diffuseLighting;

    // diffuse lightmap
    dstColor.rgb += lightmapColor;

    // diffuse map
    dstColor *= diffuseColor;
    
    // per vertex specular lighting
    dstColor += specularLighting * matLevel * lightmapColor.a;
    
    return dstColor;
}

//------------------------------------------------------------------------------
/**
    Draw a grid.
    Cristobal piece of code.
*/
float4 DrawGrid (
    in const sampler LightmapSampler,
    in const float2 uvmat)    
{
   float4 retColor;
   retColor = tex2D(LightmapSampler, uvmat);
   retColor.gb = retColor.aa;
   //retColor *= LightDiffuse;

   // Draw grid
   float xmod = fmod(uvmat, 1.0f);
   if ( xmod < ( 1.0 / (512) ) )
   {
       retColor = float4 ( 1.0 , 0.0 ,0.0, 1.0f );
   }
   
   float ymod = fmod(uvmat, 1.0f);
   if ( ymod < ( 1.0 / (512) ) )
   {
       retColor = float4 ( 0.0 , 1.0 ,0.0, 1.0f );
   }
      
   /*
   float xmod =  512.f*fmod(psIn.uv1.x , 1.0f);
   float ymod =  512.f*fmod(psIn.uv1.y , 1.0f);
   int x1 = floor(xmod);
   int y1 = floor(ymod);
   if ( (( x1+y1) %2 ) == 0 )
   {
    dstColor.r = 1.0f;
   }
   */
   
   return retColor;
}
