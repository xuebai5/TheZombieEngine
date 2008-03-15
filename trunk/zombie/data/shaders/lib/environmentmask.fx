#line 1 "environmentmask.fx"
//------------------------------------------------------------------------------
//  environmentmask.fx
//  An environment mask sampler.
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
sampler EnvMask = sampler_state
{
    Texture = <envMaskMap>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};
