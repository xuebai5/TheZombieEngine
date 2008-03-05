//------------------------------------------------------------------------------
//  ps2.0/gmmt_dett.fx
//
//  Geomipmap terrain, detail texturing
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------

#include "../lib/gmmt.fx"

//------------------------------------------------------------------------------
//  global vars
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;
shared float3   EyePos;             // camera position
int             CullMode = 2;       // CW

float4 TerrainGrassPalette[16];
float Scale;


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
    float4 position : POSITION;
    float2 uv0 : TEXCOORD0;
};

//------------------------------------------------------------------------------
//  Texture samplers
//------------------------------------------------------------------------------
#include "../lib/gmmt_edit_sampler.fx"

//------------------------------------------------------------------------------
//  The vertex shader.
//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    
    // get projected position
    vsOut.position = mul(float4(vsIn.position,1), ModelViewProjection);
    
    CalculateGMMWeightUV( vsIn.position, TerrainCellPosition, TerrainSideSizeInv, vsOut.uv0.xy );
    return vsOut;
}

//------------------------------------------------------------------------------
//  The pixel shader.
//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{

    float4 color = tex2D( GrassEditionSampler , psIn.uv0 );
    
    int index = color.r*256;
    color = TerrainGrassPalette[index ];
    
    float invScale = 1.0 / Scale;
    
    // Draw grid
    float xmod = fmod(psIn.uv0.x , invScale);
    if ( xmod < (invScale /32 ) )
    {
        color = float4 ( 1.0 , 1.0 ,1.0, 0.3 );
    }
    
    float ymod = fmod(psIn.uv0.y , invScale);
    if ( ymod < (invScale /32 ) )
    {
        color = float4 ( 1.0 , 1.0 ,1.0, 0.3 );
    }
    
    return color;
}

//------------------------------------------------------------------------------
//  The technique.
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        // pass_color.fx
        DepthBias           = 0.0f;
        FogEnable           = False;
        
        ZWriteEnable        = False;
        ZEnable             = True;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        //AlphaBlendEnable    = True;
        AlphaBlendEnable    = True;
        SrcBlend            =  SrcAlpha;
        DestBlend           = InvSrcAlpha;
        
        
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = compile ps_2_0 psMain();
    }
}
