#line 1 "pass_wireframe.fx"
//------------------------------------------------------------------------------
//  ps2.0/pass_wireframe.fx
//
//  Debug mode for drawing everything else in wireframe.
//
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------

int FillMode = 2; // wireframe

technique t0
{
    pass p0
    {
        FillMode = <FillMode>;
    }
}
