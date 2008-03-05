#include "shaders:../lib/lib.fx"

shared float4x4 ModelViewProjection;
shared float4x4 Model;
shared float3 ModelEyePos;

float4 FogDistances;// = {5.0f, 100.0f, 0.0f, 0.0f}; // {FogTop, FogEnd, FogRange, NA}
float4 FogColor     = {1.0f, 1.0f, 1.0f, 0.0f};

texture DiffMap0;

int CullMode = 2; // CW

struct VsInput
{
    float4 position : POSITION; 
    float2 uv0      : TEXCOORD0;
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float fog       : TEXCOORD1;
};

#include "shaders:../lib/diffsampler.fx"

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    float4 position = vsIn.position;
    
    vsOut.position = mul( position, ModelViewProjection );  
    vsOut.uv0 = vsIn.uv0;
    
    // Get fog parameter
    float fFogTop   = FogDistances.x;
    float fFogEnd   = FogDistances.y;
    float fFogRange = FogDistances.z;
    
    // Calculate the world position
    float4 worldPos = mul( position, Model );
    float4 eyePos = mul( float4(ModelEyePos, 1.0f), Model );
    
    // Project both points into the x-z plane
    float4 vCameraProj, vWorldProj;
    vCameraProj   = eyePos;
    vCameraProj.y = 0;
    vWorldProj    = worldPos;
    vWorldProj.y  = 0;
    
    // Scaled distance calculation in x-z plane
    float fDeltaD = distance(vCameraProj, vWorldProj) / fFogEnd * 2.0f;
    
    // Height based calculations
    float fDeltaY, fDensityIntegral;
    
    // camera above fog
    if (eyePos.y > fFogTop)
    {
        if (worldPos.y < fFogTop)
        {
            // camera above the fog, vertex under
            fDeltaY = (fFogTop - worldPos.y) / fFogRange * 2;
            fDensityIntegral = abs(fDeltaY * fDeltaY * 0.5f);
        }
        else
        {
            // both camera and vertex above fog
            fDeltaY = 0.0f;
            fDensityIntegral = 0.0f;
        }
    }
    else
    {
        if (worldPos.y < fFogTop)
        {
            // both camera and vertex under fog
            float fDeltaA = (fFogTop - eyePos.y) / fFogRange * 2.0f;
            float fDeltaB = (fFogTop - worldPos.y) / fFogRange * 2.0f;
            fDeltaY = abs(fDeltaA - fDeltaB);
            fDensityIntegral = abs((fDeltaA * fDeltaA * 0.5f) - (fDeltaB * fDeltaB * 0.5f));
        }
        else
        {
            // camera under, vertex above
            fDeltaY = abs(fFogTop - eyePos.y) / fFogRange * 2;
            fDensityIntegral = abs(fDeltaY * fDeltaY * 0.5f);
        }
    }
    
    float fDensity;
    if (fDeltaY != 0.0f)
    {
        fDensity = (sqrt(1.0f + ((fDeltaD / fDeltaY) * (fDeltaD / fDeltaY)))) * fDensityIntegral;
    }
    else
    {
        fDensity = 0.0f;
    }
    
    // Set the fog value
    vsOut.fog = exp(-fDensity);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 srcColor;
    float4 dstColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
    dstColor = tex2D(DiffSampler, psIn.uv0);  
    srcColor = FogColor;
    dstColor = lerp( srcColor, dstColor, psIn.fog );
    return dstColor;
}

technique t0
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable     = true;
        ZEnable          = true;
        ZFunc            = LessEqual;
        CullMode         = <CullMode>;
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        FogEnable        = false;
        VertexShader = compile vs_1_1 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
