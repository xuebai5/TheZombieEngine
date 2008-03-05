#line 1 "levelsampler.fx"
//------------------------------------------------------------------------------
//  levelsampler.h
//  Declare the standard level sampler for all shaders.
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------
sampler LevelSampler = sampler_state
{
    Texture = <levelMap>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    MipMapLodBias= <mipMapLodBias>;
    //mipMapLodBias = -0.75;
};
