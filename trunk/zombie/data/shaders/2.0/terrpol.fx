// this need for compapability , in application we set this variable, try to comment out
shared float4x4 ModelViewProjection;

//------------------------------------------------------------------------------
//  declare shader input/output parameters
//------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float4 diffuse  : COLOR0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse  : COLOR0;
};

//------------------------------------------------------------------------------
//  the vertex shader function
//------------------------------------------------------------------------------
VS_OUTPUT vsMain(const VS_INPUT vsIn)
{
    VS_OUTPUT vsOut;
    vsOut.position = mul(float4(vsIn.position, 1.0f), ModelViewProjection);
    vsOut.diffuse = vsIn.diffuse;
    return vsOut;
}

//------------------------------------------------------------------------------
//  Technique: VertexShader 1.1, Fixed Function Pixel Pipeline
//------------------------------------------------------------------------------
technique vs11_ffpp
{
    pass p0
    {

        VertexShader = compile vs_1_1 vsMain();
        PixelShader = 0;

        CullMode    = CCW;
        AlphaBlendEnable = False;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Diffuse;
        AlphaOp[0] = Modulate;
        
        ColorOp[1] = Disable;
        AlphaOp[1] = Disable;
        
//        FillMode = Solid;
    }
}
