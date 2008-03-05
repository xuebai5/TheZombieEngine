#line 1 "env2dsampler.fx"
//------------------------------------------------------------------------------
//  env2dsampler.fx
//  An environment map sampler.
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------
sampler Env2DSampler = sampler_state
{
    Texture = <AmbientMap0>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
