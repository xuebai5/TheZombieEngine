//------------------------------------------------------------------------------
//  trag_sequences.fx
//
//  Common states shared by shaders for non-default surfaces
//
//  (C) 2006 Tragnarion Studios
//------------------------------------------------------------------------------

technique tSeqAlphaTest
{
    pass p0
    {
        AlphaTestEnable  = True;
        AlphaRef         = 128;
        AlphaFunc        = GREATER;
    }
    
    pass pRestore
    {
        AlphaTestEnable  = False;
    }
}

technique tSeqNoCull
{
    pass p0
    {
        CullMode = None;
    }
    
    pass pRestore
    {
        CullMode = CW;
    }
}

technique tSeqDepthBias
{
    pass p0
    {
        DepthBias = -0.00001f;
    }
    
    pass pRestore
    {
        DepthBias = 0.0f;
    }
}

technique tSeqLinearFog
{
    pass p0
    {
        FogEnable       = True;
        FogVertexMode   = Linear;
        FogTableMode    = None;
    }

    pass pRestore
    {
        FogEnable       = False;
    }
}

technique tSeqDepthBiasLinearFog
{
    pass p0
    {
        DepthBias       = -0.00001f;
        FogEnable       = True;
        FogVertexMode   = Linear;
        FogTableMode    = None;
    }
    
    pass pRestore
    {
        DepthBias       = 0.0f;
        FogEnable       = False;
    }
}

technique tSeqNoCullLinearFog
{
    pass p0
    {
        FogEnable       = True;
        FogVertexMode   = Linear;
        FogTableMode    = None;
        CullMode        = None;
    }
    
    pass pRestore
    {
        FogEnable       = False;
        CullMode        = CW;
    }
}

technique tSeqAlphaTestNoCull
{
    pass p0
    {
        AlphaTestEnable = True;
        AlphaRef        = 128;
        AlphaFunc       = GREATER;
        CullMode        = None;
    }

    pass pRestore
    {
        AlphaTestEnable = False;
        CullMode        = CW;
    }
}

technique tSeqAlphaTestLinearFog
{
    pass p0
    {
        AlphaTestEnable = True;
        AlphaRef        = 128;
        AlphaFunc       = GREATER;
        FogEnable       = True;
        FogVertexMode   = Linear;
        FogTableMode    = None;
    }

    pass pRestore
    {
        AlphaTestEnable = False;
        FogEnable       = False;
    }
}

technique tSeqAlphaTestNoCullLinearFog
{
    pass p0
    {
        AlphaTestEnable = True;
        AlphaRef        = 128;
        AlphaFunc       = GREATER;
        FogEnable       = True;
        FogVertexMode   = Linear;
        FogTableMode    = None;
        CullMode        = None;
    }

    pass pRestore
    {
        AlphaTestEnable = False;
        FogEnable       = False;
        CullMode        = CW;
    }
}
