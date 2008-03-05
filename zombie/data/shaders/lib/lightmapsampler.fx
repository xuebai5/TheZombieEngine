#line 1 "lightmapsampler.fx"
//------------------------------------------------------------------------------
//  lightmapsampler.h
//  Declare the standard light map sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler LightmapSampler = sampler_state
{
    Texture = <lightMap>;
    AddressU  = Wrap;//Clamp;
    AddressV  = Wrap;//Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;//None;
};
