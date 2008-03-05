
struct VsInput
{
    float4 position : POSITION;
};

struct VsOutput
{
    float4 position : POSITION;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = float4( vsIn.position.xy, 0.0f, 1.0f );
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    float4 color = float4( 0.0f, 0.0f, 0.0f, 0.5f );
    return color;
}

technique t0
{
    pass p0
    {
        // set up the stencil test to draw the big shadow rectangle.
		// This test will draw a "shadow" everywhere that the stencil value is 1
        StencilEnable       = True;
        StencilFunc         = LessEqual;
        StencilPass         = Keep;
        StencilRef          = 1;
        DepthBias           = 0.0;
		// Turn off depth testing and culling for shadow mask
		// Allow for shadow transparency
        ZEnable             = False;
        ZWriteEnable        = False;
        CullMode            = None;
        AlphaBlendEnable    = True;
        SrcBlend            = SrcAlpha;
        DestBlend           = InvSrcAlpha;
        
        VertexShader        = compile vs_1_1 vsMain();
        PixelShader         = compile ps_1_1 psMain();//0;
    }
}
