//------------------------------------------------------------------------------
//  2.0/default_colr_pntl_mp.fx
//  
//  vertex+pixel shader with multipass lighting (1 light)
//  
//  (C) 2005 Tragnarion Studios
//------------------------------------------------------------------------------
#define NumLights 1

#include "default_colr_pntl_mp_lib.fx"

VertexShader vsArray[1]              = { compile vs_2_0 vsMain() };

VertexShader vsArray_diff[NumLights] = { compile vs_2_0 vsMain_diff(0) };

PixelShader psArray_diff[NumLights]  = { compile ps_2_0 psMain_diff(0) };

PixelShader psArray_spec[NumLights]  = { compile ps_2_0 psMain_spec(0) };

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
}
