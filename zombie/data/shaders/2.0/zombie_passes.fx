//------------------------------------------------------------------------------
//  zombie_passes.fx
//
//  Common states shared by all shaders within a renderpath phase
//
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

// Put the variables that is posible put the value in  rener pass
shared float4x4 View;           // the view matrix
shared float4x4 Projection;     // the projection matrix
shared float4 ShadowProjectionDeform;// the projection matrix

// variables to tecnique
int CullMode = 2; /*CW*/

int AlphaSrcBlend = 1; // 5 = SrcAlpha
int AlphaDstBlend = 2; // 6 = InvSrcAlpha

technique tPassColor
{
    pass p0
    {
        ZEnable             = True;
        ZWriteEnable        = True;
        StencilEnable       = False;
        CullMode            = CW;
        FogEnable           = False;
        AlphaBlendEnable    = False;
        AlphaTestEnable     = False;
        DepthBias           = 0.0f;
        //NormalizeNormals    = True;
        //ScissorTestEnable   = False;
    }
}

technique tPassShadow
{
    pass p0
    {
        ZWriteEnable        = False;
        ZFunc               = Less;
    #ifdef NGAME
        AlphaBlendEnable    = False;
    #else
        AlphaBlendEnable    = True;
        SrcBlend            = <AlphaSrcBlend>;
        DestBlend           = <AlphaDstBlend>;
    #endif
        StencilEnable       = True;
        StencilFunc         = Always;
        StencilRef          = 0;

        TwoSidedStencilMode = True;
        CullMode            = None;
        StencilFail         = Keep;
        StencilPass         = Keep;
        StencilZFail        = Incr;
        CCW_StencilFail     = Keep;
        CCW_StencilPass     = Keep;
        CCW_StencilZFail    = Decr;
    }
}

technique tPassTerrainShadow
{
    pass p0
    {
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        NormalizeNormals    = True;
        ZEnable             = True;
        ZWriteEnable        = False;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        FogEnable           = False;
        AlphaBlendEnable    = True;
        AlphaTestEnable     = True;
        AlphaFunc           = GreaterEqual;        
        SrcBlend            = One;
        DestBlend           = One;
        ScissorTestEnable   = True;
    }
}
