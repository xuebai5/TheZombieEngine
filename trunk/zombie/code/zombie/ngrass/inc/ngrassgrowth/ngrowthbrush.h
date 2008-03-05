#ifndef N_GROWTHBRUSH_H
#define N_GROWTHBRUSH_H

/*-----------------------------------------------------------------------------
    @file ngrowthbrush.h
    @class nGrowthBrush
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo

    @brief nGrowthBrush

    (C) 2004 Conjurer Services, S.A.

*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Class nGrowthBrush
*/
class nGrowthBrush : public nRoot
{
public:
    nGrowthBrush();
    virtual bool SaveCmds(nPersistServer* ps);

    /// @name Script interface
    //@{
    /// Set spawn probability
    void SetSpawnProbability(float);
    /// Get spawn probability
    float  GetSpawnProbability() const;
    /// Set minimun scale
    void SetMinScale(float);
    /// Get minimun scale
    float  GetMinScale() const;
    /// Set maximun scale
    void SetMaxScale(float);
    /// Get maximun scale
    float  GetMaxScale() const;

    /// Set mimimun distance betwen same brush
    void SetMinDistanceSameBrush(float);
    /// Get mimimun distance betwen same brush
    float  GetMinDistanceSameBrush() const;
    /// Set mimimun distance betwen same brush
    void SetMinDistanceOtherBrush(float);
    /// Get mimimun distance betwen same brush
    float  GetMinDistanceOtherBrush() const;
    /// Set mimimun distance betwen same brush
    void SetModulateByTerrain(bool);
    /// Get mimimun distance betwen same brush
    bool  GetModulateByTerrain() const;

    /// Sets the material name
    void SetAssetName(const nString&);
    /// Gets the material name
    const nString& GetAssetName() const;
    //@}

    /// set the mesh id;
    void SetMeshId(int id);
    /// return the mesh id
    int GetMeshId();

private:
    /// Probability
    float spawnProbability;
    /// min scale
    float minScale;
    /// maxScale
    float maxScale;
    /// indicate if the brush take the colour of terrain
    bool modulateByTerrain;
    /// The minimun distance to the same brush
    float minDistanceSameBrush;
    /// Minimun distnance to other brush
    float minDistanceOtherBrush;
    /// the mesh of asset
    nString assetName;
    ///
    int meshId;
};

#endif //N_GROWTHBRUSH_H