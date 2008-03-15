//------------------------------------------------------------------------------
//  zombie_phases.fx
//
//  Common states shared by all shaders within a renderpath phase
//
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

technique tPhaseDepth
{
    pass p0
    {
        ColorWriteEnable    = 0;
    }
    
    pass pRestore
    {
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
    }
}

technique tPhaseOpaque
{
    pass p0
    {
        ZWriteEnable        = False;
        ZFunc               = Equal;
    }
    
    pass pRestore
    {
        ZWriteEnable        = True;
        ZFunc               = LessEqual;
        AlphaBlendEnable    = False;
    }
}

technique tPhaseLaser
{
    pass p0
    {
        ZWriteEnable        = False;
        AlphaBlendEnable    = True;
        SrcBlend            = SrcAlpha;
        DestBlend           = One;
    }

    pass pRestore
    {
        ZWriteEnable        = True;
        AlphaBlendEnable    = False;
    }
}

technique tPhaseAlpha
{
    pass p0
    {
        ZWriteEnable        = False;
        AlphaBlendEnable    = True;
    }

    pass pRestore
    {
        ZWriteEnable        = True;
        AlphaBlendEnable    = False;
    }
}

technique tPhaseSky
{
    pass p0
    {
        ZWriteEnable        = False;
    }

    pass pRestore
    {
        ZWriteEnable        = True;
    }
}

technique tPhaseEmissive
{
    pass p0
    {
        ZWriteEnable        = False;
    }

    pass pRestore
    {
        ZWriteEnable        = True;
    }
}

technique tPhaseOverlay
{
    pass p0
    {
        ZEnable             = False;
        FogEnable           = False;
        CullMode            = None;
        AlphaBlendEnable    = True;
    }
    
    pass pRestore
    {
        ZEnable             = True;
        CullMode            = CW;
        AlphaBlendEnable    = False;
    }
}

technique tPhaseShadowDepth
{
    pass p0
    {
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
        ZWriteEnable        = True;
        ZEnable             = True;
        ZFunc               = LessEqual;
        NormalizeNormals    = True;
        StencilEnable       = False;
        DepthBias           = 0.0f;
        AlphaBlendEnable    = False;
        FogEnable           = false;
    }
}
