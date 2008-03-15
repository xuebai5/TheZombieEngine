//------------------------------------------------------------------------------
//  gmmt_dett_sampler.fx
//  Geomipmap terrain detail texturing sampler
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

shared float TerrainSideSizeInv;    // 1 / block distance (weight map texture scale factor (scale UV))

float3 TerrainCellPosition;         // terrain cell position
float TerrainCellDistSquaredInv;    // distance where start to see only global texture

texture TerrainGrassEditionMap;       // mini weight texture


//------------------------------------------------------------------------------
sampler GrassEditionSampler = sampler_state
{
    Texture = <TerrainGrassEditionMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Point;
    MipFilter = None;
};