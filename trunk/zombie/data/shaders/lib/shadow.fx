#line 1 "shadow.fx"
//------------------------------------------------------------------------------
//  lib/shadow.fx
//
//  Support functions for shadow library.
//
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#define REVERSE_SHADOW_EXTRUSION

//------------------------------------------------------------------------------
/**
    shadow()
    
    Compute the shadow modulation color.
    
    @param  shadowPos           position in shadow space
    @param  noiseSampler        sampler with a noise texture
    @param  shadowMapSampler    sampler with shadow map
    @param  shadowModSampler    shadow modulation sampler to fade shadow color in/out
    @return                     a shadow modulation color
*/
float4
shadow(const float4 shadowPos, float distOffset, sampler shadowMapSampler, sampler shadowModSampler, sampler noiseSampler)
{
    // get projected position in shadow space
    float3 projShadowPos = shadowPos.xyz / shadowPos.w;
#if 0 // filter shadow map
    // jitter shadow map position using noise texture lookup
    projShadowPos.xy += tex2D(noiseSampler, projShadowPos.xy * 1234.5f).xy * 0.0005f;
#endif
    // sample shadow depth from shadow map
    float4 shadowDepth = tex2D(shadowMapSampler, projShadowPos.xy) + distOffset;
    
    // in/out test
    float4 shadowModulate;
    if ((projShadowPos.x < 0.0f) || 
        (projShadowPos.x > 1.0f) ||
        (projShadowPos.y < 0.0f) ||
        (projShadowPos.y > 1.0f))
    {
        // outside shadow projection
        shadowModulate = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if ((shadowDepth.x > projShadowPos.z) || (shadowPos.z > shadowPos.w))
    {
        // not in shadow
        shadowModulate = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {   
#if 1
        //shadowModulate = float4(0.5f, 0.5f, 0.5f, 1.0f);
        shadowModulate = float4(0.0f, 0.0f, 0.0f, 1.0f);
#else
        // in shadow
        shadowModulate = tex2D(shadowModSampler, projShadowPos.xy);
        //float4 shadowColor = tex2D(shadowModSampler, projShadowPos.xy);
        //float4 blendColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        //float relDist = saturate((projShadowPos.z - shadowDepth.x) * 20.0f);
        //shadowModulate = lerp(shadowColor, blendColor, relDist);
#endif
    }
    return shadowModulate;
}

//------------------------------------------------------------------------------
//  vsExtrudeShadowVolume
//
//  Vertex shader support function for simpler per-pixel lighting.
//
//  @param  pos                 [in] vertex position
//  @param  normal              [in] vertex normal
//  @param  modelLightPos       [in] light position in model space
//  @param  extrudeDistance     [in] extrusion distance (TODO use light range)
//  @param  amountInset         [in] offset into the model to remove artifacts
//                                   it is ignored with reverse extrusion.
//------------------------------------------------------------------------------
float4
vsExtrudeShadowVolume(in const float4 position,
                      in const float3 normal,
                      in const float4 modelLightPos,
                      in const float extrudeDistance,
                      in const float amountInset)
{
    float fExtrudeDistance = extrudeDistance;
#ifndef REVERSE_SHADOW_EXTRUSION
    float fAmountInset = -amountInset;
#else
    float fAmountInset = 0.0f;
#endif
    
    // compute vector from the light to the vertex
    float4 vNormLightToVertex = normalize(position - modelLightPos);
    // compute N * L
    float fNdotL = dot(normal, -vNormLightToVertex);
    // inset position along normal vector to create smaller 
    // model to minimize popping of shadowed areas
    float4 insetPos;
    insetPos.xyz = normal * fAmountInset + position.xyz;
    insetPos.w = position.w;
    
    float4 vExtrusion = vNormLightToVertex * fExtrudeDistance;
    
#ifndef REVERSE_SHADOW_EXTRUSION
    float away = (fNdotL < 0) ? 1.0f : 0.0f;
#else
    float away = (fNdotL > 0) ? 1.0f : 0.0f; 
#endif
    
    return vExtrusion * away + insetPos;
}
