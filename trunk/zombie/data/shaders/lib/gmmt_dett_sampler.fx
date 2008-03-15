//------------------------------------------------------------------------------
//  gmmt_dett_sampler.fx
//  Geomipmap terrain detail texturing sampler
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

shared float TerrainSideSizeInv;    // 1 / block distance (weight map texture scale factor (scale UV))

float3 TerrainCellPosition;         // terrain cell position

texture TerrainCellWeightMap;       // mini weight texture
texture TerrainCellMaterial0;       // blend texture 0
texture TerrainCellMaterial1;       // blend texture 1
texture TerrainCellMaterial2;       // blend texture 2
texture TerrainCellMaterial3;       // blend texture 3
texture TerrainCellMaterial4;       // blend texture 4

float4 TerrainCellTransform0U;      // texture transform for material 0 coord. U
float4 TerrainCellTransform1U;      // texture transform for material 1 coord. U
float4 TerrainCellTransform2U;      // texture transform for material 2 coord. U
float4 TerrainCellTransform3U;      // texture transform for material 3 coord. U
float4 TerrainCellTransform4U;      // texture transform for material 4 coord. U

float4 TerrainCellTransform0V;      // texture transform for material 0 coord. V
float4 TerrainCellTransform1V;      // texture transform for material 1 coord. V
float4 TerrainCellTransform2V;      // texture transform for material 2 coord. V
float4 TerrainCellTransform3V;      // texture transform for material 3 coord. V
float4 TerrainCellTransform4V;      // texture transform for material 4 coord. V

texture lightMap;

//------------------------------------------------------------------------------
sampler MiniWeightMapSampler = sampler_state
{
    Texture = <TerrainCellWeightMap>;
    AddressU  = Clamp;
    AddressV  = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler BlendTexture0Sampler = sampler_state
{
    Texture = <TerrainCellMaterial0>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture1Sampler = sampler_state
{
    Texture = <TerrainCellMaterial1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture2Sampler = sampler_state
{
    Texture = <TerrainCellMaterial2>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture3Sampler = sampler_state
{
    Texture = <TerrainCellMaterial3>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture4Sampler = sampler_state
{
    Texture = <TerrainCellMaterial4>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler LightmapSampler = sampler_state
{
    Texture = <lightMap>;
    AddressU  = clamp;
    AddressV  = clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;//None;
};
