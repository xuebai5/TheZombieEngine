#line 1 "envsampler.fx"
//------------------------------------------------------------------------------
//  environmentsampler.fx
//  An environment cube map sampler.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler EnvironmentSampler = sampler_state
{
    Texture = <CubeMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
