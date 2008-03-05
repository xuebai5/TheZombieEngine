#line 1 "bumpsampler.fx"
//------------------------------------------------------------------------------
//  bumpsampler.fx
//  Declare the standard bump sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler BumpSampler = sampler_state
{
    Texture   = <bumpMap>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= <mipMapLodBias>;
    //MipMapLodBias = -0.75;
};
