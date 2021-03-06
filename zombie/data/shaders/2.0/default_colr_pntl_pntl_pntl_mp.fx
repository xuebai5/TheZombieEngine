//------------------------------------------------------------------------------
//  2.0/default_colr_pntl_pntl_pntl_mp.fx
//  
//  vertex+pixel shader with multipass lighting (3 lights)
//  
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#define NumLights 3

#include "default_colr_pntl_mp_lib.fx"

VertexShader vsArray_diff[NumLights] = { compile vs_2_0 vsMain_diff(0),
                                         compile vs_2_0 vsMain_diff(1),
                                         compile vs_2_0 vsMain_diff(2) };

VertexShader vsArray[1]              = { compile vs_2_0 vsMain() };

PixelShader psArray_diff[NumLights]  = { compile ps_2_0 psMain_diff(0),
                                         compile ps_2_0 psMain_diff(1),
                                         compile ps_2_0 psMain_diff(2) };

PixelShader psArray_spec[NumLights]  = { compile ps_2_0 psMain_spec(0),
                                         compile ps_2_0 psMain_spec(1),
                                         compile ps_2_0 psMain_spec(2) };

technique t0
{
    pass p0_diff
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One; 

        VertexShader     = (vsArray_diff[0]);
        PixelShader      = (psArray_diff[0]);
    }

    pass p1_diff
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One; 

        VertexShader     = (vsArray_diff[1]);
        PixelShader      = (psArray_diff[1]);
    }
    
    pass p2_diff
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One; 

        VertexShader     = (vsArray_diff[2]);
        PixelShader      = (psArray_diff[2]);
    }

    pass p0
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = Zero;
        DestBlend        = SrcColor;
        
        VertexShader     = (vsArray[0]);
        PixelShader      = compile ps_2_0 psMain();
    }

    pass p0_spec
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One;
        
        VertexShader     = (vsArray_diff[0]);
        PixelShader      = (psArray_spec[0]);
    }
    
    pass p1_spec
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One;
        
        VertexShader     = (vsArray_diff[1]);
        PixelShader      = (psArray_spec[1]);
    }
    
    pass p2_spec
    {
        CullMode         = CW;
        AlphaTestEnable  = False;
        AlphaBlendEnable = True;
        SrcBlend         = One;
        DestBlend        = One;
        
        VertexShader     = (vsArray_diff[2]);
        PixelShader      = (psArray_spec[2]);
    }
}
