#line 1 "shape_map.fx"
//------------------------------------------------------------------------------
//  2.0/shape_map.fx
//
//  Map shader.
//  
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float4x4 ViewProjection;
shared float4x4 View;
shared float4x4 Projection;

float4  MatDiffuse;
texture DiffMap0;

int AlphaSrcBlend = 5;  // SrcAlpha
int AlphaDstBlend = 6;  // InvSrcAlpha

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position  : POSITION;  // the particle position in world space
    float2 uv0       : TEXCOORD0; // the particle texture coordinates
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
sampler DiffSampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{	
    VsOutput vsOut;

    float4 position = vsIn.position;
    
    // transform to projection space
    vsOut.position = mul(position, ModelViewProjection);//map is in world coordinates
    vsOut.uv0      = vsIn.uv0;
    vsOut.diffuse  = MatDiffuse;

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        TextureTransformFlags[0] = 0;
        CullMode        = None;
        ZEnable         = False;//TEMP

        AlphaBlendEnable = True;
        SrcBlend        = <AlphaSrcBlend>;
        DestBlend       = <AlphaDstBlend>;
                        
        Sampler[0]      = <DiffSampler>;
        VertexShader    = compile vs_2_0 vsMain();
        PixelShader     = 0;
                        
        ColorOp[0]      = Modulate;
        ColorArg1[0]    = Texture;
        ColorArg2[0]    = Diffuse;
        AlphaOp[0]      = Modulate;
        AlphaArg1[0]    = Texture;
        AlphaArg2[0]    = Diffuse;
                        
        ColorOp[1]      = Disable;
        AlphaOp[1]      = Disable;
    }
}

