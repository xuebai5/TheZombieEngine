#line 1 "parallaxmap.fx"
//------------------------------------------------------------------------------
//  parallaxmap.fx
//  An environment mask sampler.
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
sampler parallaxMapSampler = sampler_state
{
    Texture = <parallaxMap>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
