//------------------------------------------------------------------------------
//  gmmt_glot_sampler.fx
//  Geomipmap terrain global texturing sampler
//  (C) 2004 Tragnarion Studios
//------------------------------------------------------------------------------

shared texture TerrainGlobalMap;        // global terrain map texture
shared float TerrainGlobalMapScale;     // scaling for global map tex coord
shared texture TerrainModulationMap;    // global modulation terrain map texture
shared float TerrainModulationFactor;   // factor to blend modulation map with terrain texturing
shared float TerrainCellDistSquaredInv; // distance where start to see only global texture

texture GlobalLightMap;
//------------------------------------------------------------------------------
sampler GlobalSampler = sampler_state
{
    Texture = <TerrainGlobalMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler ModulationSampler = sampler_state
{
    Texture = <TerrainModulationMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler GlobalLightmapSampler = sampler_state
{
    Texture = <GlobalLightMap>;
    AddressU  = Wrap;//Clamp;
    AddressV  = Wrap;//Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;//None;
};
