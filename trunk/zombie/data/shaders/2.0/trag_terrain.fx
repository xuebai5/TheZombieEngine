#line 1 "trag_terrain.fx"
//------------------------------------------------------------------------------
//  2.0/trag_terrain.fx
//
//  Set render states for terrain passes
//
//  (C) 2006 Tragnarion Studios
//------------------------------------------------------------------------------

technique tPhaseTerrain
{
    pass p0
    {
        ZFunc               = Less;
        AlphaBlendEnable    = False;
    }
    
    pass pRestore
    {
        ZFunc               = LessEqual;
    }
}

// Other form is write in z buffer but no write in color , after the pass of terrain use less Z Function
// But need write to end of scene. But if you write terrain the first then you need another pass after terrain.
// Put in all holes ZBUFFER = 1
technique tPhaseTerrainHole
{
    pass p0
    {
        ColorWriteEnable    = 0;
    }

    pass pRestore
    {
        ColorWriteEnable    = RED|GREEN|BLUE|ALPHA;
    }
}
