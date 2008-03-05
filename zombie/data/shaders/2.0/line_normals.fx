#line 1 "line.fx"
//------------------------------------------------------------------------------
//  2.0/line.fx
//
//  Used for debug visualization.
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;    // the model*view*projection matrix

float4 MatDiffuse = {1.0f, 0.0f, 0.0f, 0.0f};

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
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
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR
{
    return MatDiffuse;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        NormalizeNormals    = True;
        ZEnable             = True;
        ZFunc               = LessEqual;//Equal;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        ZWriteEnable        = False;//True;
        AlphaBlendEnable    = False;//True;
        AlphaTestEnable     = False;
        Lighting            = True;
        FogEnable           = False;
        CullMode            = None;
        //FillMode         = Wireframe;
        VertexShader        = compile vs_1_1 vsMain();
        PixelShader         = compile ps_1_1 psMain();
    }
}
