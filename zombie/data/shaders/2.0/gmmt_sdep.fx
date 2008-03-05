//------------------------------------------------------------------------------
//  ps2.0/gmmt_lmap.fx
//
//  Geomipmap terrain, detail texturing, lightmap generation
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#include "../lib/lib.fx"
#include "../lib/gmmt.fx"

// The textures arent neede but the geomimap has a harcode settexture
//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_glot_sampler.fx"
#include "../lib/gmmt_dett_sampler.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4 ShadowProjectionDeform;
shared float4x4 ModelView;
shared float4x4 Projection;

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float3 position : POSITION0;
    float3 normal   : NORMAL;
};

struct VsOutput
{
    float4 position      : POSITION;
    float depth     : TEXCOORD1;
};


//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
   float4x4 deformMatrix = { { ShadowProjectionDeform.x, ShadowProjectionDeform.y, 0.f, 0.f},
                              { ShadowProjectionDeform.z, ShadowProjectionDeform.w, 0.f, 0.f},
                              {                      0.f,                      0.f, 1.f, 0.f},
                               {                      0.f,                      0.f, 0.f, 1.f}};
    float4x4 ModelViewProjection = mul( mul( ModelView, deformMatrix), Projection);
    VsOutput vsOut;
    float4 position = float4( vsIn.position , 1.0f);

    float4 clipPos = mul(position, ModelViewProjection);
    vsOut.position = clipPos;
    // texture coordinates
    // pass z / w to the pixel shader
    vsOut.depth = (clipPos.z + 0.0001f) / clipPos.w;
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    //return EncodeToRGBA_DXT5(psIn.depth);
    return float4( 1.0,1.0,0.0,1.0); //Draw red color and depth maximun value
}


//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p1
    {
        FogEnable    = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        
        ZFunc            = Less;
        CullMode         = CW;
        DepthBias        = 0.0f;
        ZEnable          = True;
        ZWriteEnable     = True;
        StencilEnable    = False;

        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
