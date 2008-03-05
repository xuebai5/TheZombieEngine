//------------------------------------------------------------------------------
//  filter_sepia.fx
//
//  Frame compositing shader.
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
float  Intensity0;  // the color saturation value
float4 MatDiffuse;  // the color balance value
float4 MatAmbient = {-0.299, -0.587, -0.114, 1.0};  // the b&w conversion value (usually x=0.299, y=0.587, z=0.114, w=1.0)
//const float3 lum = float3( 0.30, 0.59, 0.11 );
texture DiffMap0;   // the offscreen surface

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv0      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
};


//------------------------------------------------------------------------------
//  Texture sampler definitions
//------------------------------------------------------------------------------
sampler RenderBuffer = sampler_state
{
    Texture = <DiffMap0>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain( const VS_INPUT vsIn )
{
    VS_OUTPUT vsOut;

    vsOut.position = float4( vsIn.position.xy, 0.0f, 1.0f );
    vsOut.uv0 = vsIn.uv0;

    return vsOut;
}

//------------------------------------------------------------------------------
//  the pixel shader function
//------------------------------------------------------------------------------
float4 psMain(const VS_OUTPUT psIn) : COLOR
{
    float4 imageColor = tex2D(RenderBuffer, psIn.uv0);
    float luminance = dot(imageColor.xyz, MatAmbient.xyz);
    imageColor = MatDiffuse * lerp(float4(luminance, luminance, luminance, luminance), imageColor, Intensity0);

    float y = (0.5 * imageColor.x + 0.3 * imageColor.y + 0.2 * imageColor.z);

    imageColor.r = y * 0.82 + 0.18;
    imageColor.g = y;
    imageColor.b = y * 0.68;
    imageColor.a = 1.0;

    return imageColor;
}

//------------------------------------------------------------------------------
//  the technique
//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        StencilEnable       = False;
        ZEnable             = False;
        ZWriteEnable        = False;
        StencilEnable       = False;
        cullMode            = None;
        AlphaBlendEnable    = False;
        VertexShader        = compile vs_1_1 vsMain();
        PixelShader         = compile ps_2_0 psMain();
    }
}
