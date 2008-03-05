#line 1 "normalsampler.fx"
//------------------------------------------------------------------------------
//  normalsampler.fx
//  Declare the standard bump sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler NormalSampler = sampler_state
{
    Texture   = <BumpMap3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= <mipMapLodBias>;
    //MipMapLodBias = -0.75;
};
