#line 1 "controlsampler.fx"
//------------------------------------------------------------------------------
//  controlsampler.h
//  Declare the standard control sampler for all shaders.
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
sampler ControlSampler = sampler_state
{
    Texture = <controlMap>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    mipMapLodBias = <mipMapLodBias>;
};