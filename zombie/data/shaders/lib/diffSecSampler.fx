#line 1 "diffSecSampler.fx"
//------------------------------------------------------------------------------
//  diffSecSampler.h
//  Declare the standard control sampler for all shaders.
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------

sampler DiffSecSampler = sampler_state
{
    Texture = <diffMap2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    mipMapLodBias = <mipMapLodBias>;
};
