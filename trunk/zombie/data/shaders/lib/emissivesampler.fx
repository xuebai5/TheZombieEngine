#line 1 "emissivesampler.fx"
//------------------------------------------------------------------------------
//  emissivesampler.h
//  Declare the standard self-illumination map sampler for all shaders.
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------
sampler EmissiveSampler = sampler_state
{
    Texture = <illuminationMap>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    mipMapLodBias = <mipMapLodBias>;
};
