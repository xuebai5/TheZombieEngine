//vertex shader input
shared float4x4 ModelViewProjection;
shared float4x4 TextureTransform0;

//pixel shader input
float4 MatDiffuse; // xyz == Light Wood Color
float4 MatSpecular; // xyz == Dark Wood Color
float Frequency;

#define ringFreq Frequency
#define lightWood MatDiffuse
#define darkWood MatSpecular

texture DiffMap0;

int FillMode = 2;

sampler1D PulseTrainSampler = sampler_state
{
    Texture = <DiffMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
}; 

struct VS_OUTPUT
{
    float4 Pos : POSITION;
    float3 Pshade : TEXCOORD0;
};

VS_OUTPUT vsMain (float4 vPosition : POSITION )
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    // Transform position to clip space
    Out.Pos = mul (vPosition, ModelViewProjection);
    // Transform Pshade
    Out.Pshade = mul (vPosition, TextureTransform0);
    return Out;
}


float4 hlsl_rings (float4 Pshade : TEXCOORD0) : COLOR
{
    float scaledDistFromZAxis = sqrt(dot(Pshade.xy, Pshade.xy)) * ringFreq;
    float4 blendFactor = tex1D (PulseTrainSampler, scaledDistFromZAxis);
    return lerp (darkWood, lightWood, blendFactor);
}

technique t0
{
    pass p0
    {
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable    = True;
        ZEnable         = True;
        ZFunc           = LessEqual;
        
        FillMode        = <FillMode>;
        CullMode        = None;
        
        VertexShader    = compile vs_1_1 vsMain();
        PixelShader     = compile ps_2_0 hlsl_rings();
    }
}
