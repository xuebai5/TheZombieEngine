//------------------------------------------------------------------------------
//  ps2.0/gmmt_gen_glot.fx
//
//  Shader for geomipmapped terrains global texture generation.
//  The detail texture is a blending between 5 layers.
//  For non detailed terrain a global texture is used.
//  This shader does the blending from detailed texturing
//  to global texturing, using a quadratic blending parameter
//  based on distance to the eye position.
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"

shared float4x4 ModelViewProjection;

int  CullMode = 2;                   // CW

float4 TexGenS;
float4 TexGenT;
float4 TexGenR;
float MinDist;                      // block distance
float MaxDist;                      // texel size in word coord.

texture TerrainCellWeightMap;       // mini weight texture
texture TerrainCellMaterial0;       // blend texture 0
texture TerrainCellMaterial1;       // blend texture 1
texture TerrainCellMaterial2;       // blend texture 2
texture TerrainCellMaterial3;       // blend texture 3
texture TerrainCellMaterial4;       // blend texture 4

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION0;
    float2 uv0 : TEXCOORD0;
    //float  height   : POSITION1;
};

struct VsOutput
{
    float4 position     : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
    float2 uv2 : TEXCOORD2;
    float2 uv3 : TEXCOORD3;
    float2 uv4 : TEXCOORD4;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler MiniWeightMapSampler = sampler_state
{
    Texture = <TerrainCellWeightMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler BlendTexture0Sampler = sampler_state
{
    Texture = <TerrainCellMaterial0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture1Sampler = sampler_state
{
    Texture = <TerrainCellMaterial1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture2Sampler = sampler_state
{
    Texture = <TerrainCellMaterial2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture3Sampler = sampler_state
{
    Texture = <TerrainCellMaterial3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMainTexGen(const VsInput vsIn)
{
    VsOutput vsOut;

    // make an offset of 1.5 texels to make match the texels and pixels
    // this is very strange, in theory must be 0.5 texels
    vsOut.position = float4(vsIn.position.x, vsIn.position.y, 0.0f, 1.0f);
    
    vsOut.uv0.xy = vsIn.uv0.xy;
    float2 p = vsIn.uv0.xy * MinDist;
        
    vsOut.uv1.xy = p.xy * TexGenS.zw;
    vsOut.uv2.xy = p.xy * TexGenT.xy;
    vsOut.uv3.xy = p.xy * TexGenT.zw;
    vsOut.uv4.xy = p.xy * TexGenR.xy;
    
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMainTexGen(const VsOutput psIn) : COLOR
{
    // process material weights
    float4 matWeights;
    float weightEmpty;
    matWeights = tex2D(MiniWeightMapSampler, psIn.uv0);
    ProcessGMMMaterialWeights4(matWeights, weightEmpty);

    // get detail texturing color
    float4 detailColor = GetGMMDetailColor( matWeights, weightEmpty,
        psIn.uv1, psIn.uv2, psIn.uv3, psIn.uv4,
        BlendTexture0Sampler, BlendTexture1Sampler, BlendTexture2Sampler, BlendTexture3Sampler );
    
    return detailColor;
}                                     
 
//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        // pass_color.fx
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        ZEnable             = True;
        ZWriteEnable        = True;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        FogEnable           = False;
        AlphaBlendEnable    = False;        
        AlphaTestEnable     = False;

        CullMode     = 2;
        //CullMode      = <CullMode>;
        //AlphaRef   = <AlphaRef>;
        VertexShader = compile vs_2_0 vsMainTexGen();
        PixelShader  = compile ps_2_0 psMainTexGen();
    }
}
