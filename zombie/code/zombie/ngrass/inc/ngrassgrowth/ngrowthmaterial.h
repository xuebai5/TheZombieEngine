#ifndef N_GROUTHMATERIAL_H
#define N_GROUTHMATERIAL_H

/*-----------------------------------------------------------------------------
    @file ngrowthmaterial.h
    @class nGrowthMaterial
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo

    @brief nGrowthMaterial Class is the material of define a group of brushes
    This material is only use in edition time. It's use for a generate random vegetation,

    (C) 2004 Conjurer Services, S.A.

*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngrassgrowth/ngrowthbrush.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Class nGrowthMaterial is a rootnode that contains the brushes
    for generating the grass
*/
class nGrowthMaterial : public nRoot
{
public:
    virtual bool SaveCmds(nPersistServer* ps);

    /// @name Script interface
    //@{
    /// Set color
    void SetColor(const vector3 &);
    /// Get color
    const vector3 & GetColor() const;
    /// Append Brush and return it
    nGrowthBrush* CreateGrowthBrush(const nString&);
    /// Append Brush and return it
    int GetNumberOfGrowthBrushes();

    nGrowthBrush* GetGrowthBrush(int) const;
    ///  Delete material
    void DeleteGrowthBrush(int) ;

    //@}

    /// Compute necessary data
    void BeginBuild();
    /// Set the random seed
    void SetRandomSeed(int randomSeed);
    /// Return a random brush
    nGrowthBrush* GetRandomBrush();
    /// Discard a last randomBrush
    void DiscardLastRandomBrush();
    /// free necessary data for build
    void EndBuild();

private:
    vector3 color;
    int random;
    int previousRandom;
    float sumProbability;
    /// Cache of children , only use when building grass
    nArray<nGrowthBrush*> listBrush;
};

#endif