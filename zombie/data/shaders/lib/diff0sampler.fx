#line 1 "diff0sampler.fx"
//------------------------------------------------------------------------------
//  diff0sampler.h
//  Declare the standard diffuse sampler for all shaders.
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
sampler Diff0Sampler = sampler_state
{
    Texture = <DiffMap0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

