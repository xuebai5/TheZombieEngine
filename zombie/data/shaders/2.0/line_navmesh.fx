#line 1 "line.fx"
//------------------------------------------------------------------------------
//  2.0/line_navmesh.fx
//
//  Used for debug visualization.
//  Same as line.fx but with configurable ztest/zwrite
//
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
shared float4x4 ModelViewProjection;    // the model*view*projection matrix

float4 MatDiffuse = {1.0f, 0.0f, 0.0f, 0.0f};
int ZEnable = false;
int ZWriteEnable = false;

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
        ZEnable             = <ZEnable>;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        ZWriteEnable        = <ZWriteEnable>;
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
