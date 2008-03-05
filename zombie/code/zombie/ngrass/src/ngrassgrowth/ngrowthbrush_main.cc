#include "precompiled/pchngrass_conjurer.h"
#include "ngrassgrowth/ngrowthbrush.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGrowthBrush, "nroot");


//------------------------------------------------------------------------------
nGrowthBrush::nGrowthBrush() :
    spawnProbability(1.0f),
    minScale(1.0f),
    maxScale(1.0f),
    modulateByTerrain(1.0f),
    minDistanceSameBrush(1.0f),
    minDistanceOtherBrush(1.0f),
    assetName("")
{
}

//------------------------------------------------------------------------------
void
nGrowthBrush::SetSpawnProbability( float probability)
{
    this->spawnProbability = probability;
}


//------------------------------------------------------------------------------
float
nGrowthBrush::GetSpawnProbability() const
{
    return this->spawnProbability;
}

//------------------------------------------------------------------------------
void
nGrowthBrush::SetMinScale( float scale)
{
    this->minScale = n_max ( scale , 0.5f);
}


//------------------------------------------------------------------------------
float
nGrowthBrush::GetMinScale() const
{
    return this->minScale;
}

//------------------------------------------------------------------------------
void
nGrowthBrush::SetMaxScale( float scale)
{
    this->maxScale = n_min ( scale , 1.5f);
}


//------------------------------------------------------------------------------
float
nGrowthBrush::GetMaxScale() const
{
    return this->maxScale;
}

//------------------------------------------------------------------------------
void
nGrowthBrush::SetMinDistanceSameBrush( float distance)
{
    this->minDistanceSameBrush = n_max( 0.2f, distance);
}


//------------------------------------------------------------------------------
float
nGrowthBrush::GetMinDistanceSameBrush() const
{
    return this->minDistanceSameBrush;
}

//------------------------------------------------------------------------------
void
nGrowthBrush::SetMinDistanceOtherBrush( float distance)
{
    this->minDistanceOtherBrush = n_max( 0.2f , distance );
}


//------------------------------------------------------------------------------
float
nGrowthBrush::GetMinDistanceOtherBrush() const
{
    return this->minDistanceOtherBrush;
}


//------------------------------------------------------------------------------
void
nGrowthBrush::SetModulateByTerrain( bool v)
{
    this->modulateByTerrain = v;
}


//------------------------------------------------------------------------------
bool
nGrowthBrush::GetModulateByTerrain() const
{
    return this->modulateByTerrain;
}


//------------------------------------------------------------------------------
void
nGrowthBrush::SetAssetName(const nString& name)
{
    this->assetName = name;
}


//------------------------------------------------------------------------------
const nString&
nGrowthBrush::GetAssetName() const
{
    return this->assetName;
}

//------------------------------------------------------------------------------
void 
nGrowthBrush::SetMeshId(int id)
{
    this->meshId = id;
}

//------------------------------------------------------------------------------
int 
nGrowthBrush::GetMeshId()
{
    return this->meshId;
}
