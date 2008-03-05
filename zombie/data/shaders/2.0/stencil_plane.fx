//------------------------------------------------------------------------------
//  stencil_plane.fx
//  Draws the shadow plane over the whole screen using stencil
//------------------------------------------------------------------------------
float4 MatDiffuse;

//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
};

//------------------------------------------------------------------------------
struct VsOutput
{
    float4 position : POSITION;
};

//------------------------------------------------------------------------------
VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = vsIn.position;
    return vsOut;
}

//------------------------------------------------------------------------------
float4 psMain(const VsOutput psIn) : COLOR
{
    return MatDiffuse;
}

//------------------------------------------------------------------------------
technique tShadowPlane
{
    pass p0
    {
        ZEnable         = False;
        SrcBlend        = SrcAlpha;
        DestBlend       = InvSrcAlpha;
        FogEnable       = False;
        StencilFunc     = NotEqual;
        StencilZFail    = Keep;
        StencilPass     = Keep;
        CullMode        = None;
        TwoSidedStencilMode = False;

        VertexShader    = compile vs_1_1 vsMain();
        PixelShader     = compile ps_1_1 psMain();
    }
    
    pass pRestore
    {
        ZWriteEnable    = True;
        ZFunc           = LessEqual;
        StencilEnable   = False;
        CullMode        = CW;
    }
}
