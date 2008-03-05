#line 1 "lightmapsampler_clamp.fx"
//------------------------------------------------------------------------------
//  lightmapsampler_clamp.h
//  Declare the clamped light map sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler LightmapSampler = sampler_state
{
    Texture = <lightMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;//None;
};
