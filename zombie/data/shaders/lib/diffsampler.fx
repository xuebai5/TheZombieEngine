#line 1 "diffsampler.fx"
//------------------------------------------------------------------------------
//  diffsampler.h
//  Declare the standard diffuse sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler DiffSampler = sampler_state
{
    Texture = <diffMap>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= <mipMapLodBias>;
    //mipMapLodBias = -0.75;
};
