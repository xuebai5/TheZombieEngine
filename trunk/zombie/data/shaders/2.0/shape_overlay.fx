//------------------------------------------------------------------------------
//  fixed/shape_overlay.fx
//
//  Used for debug visualization of shapes on top of everything.
//
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
shared float4x4 Model;
shared float4x4 View;                   // the view matrix
shared float4x4 Projection;             // the projection matrix

float4x4 TextureTransform0 = {1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f };
float4 MatDiffuse;

texture DiffMap0;

#include "../lib/diff0sampler.fx"

//------------------------------------------------------------------------------
technique t0
{
    pass p0
    {
        WorldTransform[0]   = <Model>;
        TextureTransform[0] = <TextureTransform0>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;

        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        NormalizeNormals    = True;
        ZEnable             = False;
        ZFunc               = LessEqual;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        PixelShader         = 0;
        SpecularEnable      = False;

        Ambient             = {0.3, 0.3, 0.3, 0.0f};
        LightEnable[0]      = True;
        LightAmbient[0]     = {0.3, 0.3, 0.3, 0.0f};
        //Ambient           = {0.8, 0.8, 0.8, 1.0f};
        //LightEnable[0]    = False;
        //LightAmbient[0]   = {1.0, 1.0, 1.0, 1.0f};

        LightDiffuse[0]     = {1.0, 1.0, 1.0, 1.0f};
        LightSpecular[0]    = {1.0, 1.0, 1.0, 0.0f};
        LightPosition[0]    = {0.0, 0.0, 0.0};
        LightRange[0]       = 500000.0;
        LightAttenuation0[0] = 1.0;
        LightAttenuation1[0] = 0.0;
        LightAttenuation2[0] = 0.0;

        // LightType must be the last light state that is set!
        LightType[0]        = POINT;

        ZWriteEnable        = False;
        AlphaBlendEnable    = True;
        AlphaTestEnable     = False;
        Lighting            = True;
        VertexShader        = 0;
        FogEnable           = False;

        CullMode            = None;
        SrcBlend            =  SrcAlpha;
        DestBlend           = InvSrcAlpha;
        MaterialDiffuse     = <MatDiffuse>;
        MaterialAmbient     = <MatDiffuse>;

        //CullMode          = None;
        //SrcBlend          = SrcAlpha;
        //DestBlend         = InvSrcAlpha;
        //MaterialAmbient   = <MatDiffuse>;
        //MaterialDiffuse   = <MatDiffuse>;
        //MaterialSpecular  = { 0.0, 0.0, 0.0, 0.0 };

        FVF = XYZ | NORMAL | TEX1;

        Texture[0] = 0;
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = Count2;

        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Current;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = Current;
        ColorOp[1]   = Disable;
        AlphaOp[1]   = Disable;

        AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Diffuse;

        AlphaOp[1] = Disable;

        //FillMode         = Wireframe;
    }
}

