#line 1 "shape.fx"
//------------------------------------------------------------------------------
//  fixed/shape.fx
//
//  Used for debug visualization.
//  
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
//shared float4x4 Model;
//shared float4x4 View;                   // the view matrix
//shared float4x4 Projection;             // the projection matrix
//shared float4x4 ModelView;
shared float4x4 ModelViewProjection;    // the model*view*projection matrix

float4 MatDiffuse = {1.0f, 0.0f, 0.0f, 0.0f};

//------------------------------------------------------------------------------
//  shader input/output declarations
//------------------------------------------------------------------------------
struct VsInput
{
    float4 position : POSITION;
    float4 diffuse  : COLOR;
};

struct VsOutput
{
    float4 position : POSITION;
    float4 diffuse  : COLOR;
};

VsOutput vsMain(const VsInput vsIn)
{
    VsOutput vsOut;
    vsOut.position = mul(vsIn.position, ModelViewProjection);
    vsOut.diffuse = vsIn.diffuse;
    return vsOut;
}

float4 psMain(const VsOutput psIn) : COLOR 
{
    return psIn.diffuse;
}

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        ZEnable             = False;
        ZWriteEnable        = False;
        ZFunc               = LessEqual;
    
        VertexShader = compile vs_1_1 vsMain();
        PixelShader = compile ps_1_1 psMain();

        CullMode    = CCW;
        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        FogEnable           = False;
        StencilEnable       = False;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Diffuse;
        AlphaOp[0] = Modulate;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
        
        FillMode = Solid;    
    }
}
