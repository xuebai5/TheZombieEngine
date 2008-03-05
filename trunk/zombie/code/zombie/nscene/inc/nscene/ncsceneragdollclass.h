#ifndef NC_SCENERAGDOLLCLASS_H
#define NC_SCENERAGDOLLCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSceneRagdollClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component class for entities with ragdoll.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncscenelodclass.h"

class nEntityClass;
//------------------------------------------------------------------------------
class ncSceneRagdollClass : public ncSceneLodClass
{

    NCOMPONENT_DECLARE(ncSceneRagdollClass,ncSceneLodClass);

public:
    /// constructor
    ncSceneRagdollClass();
    /// destructor
    virtual ~ncSceneRagdollClass();
    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// get number of assigned ragdoll scene resources
    void ClearRagdollLevels();
    /// get number of assigned ragdoll scene resources
    int GetNumRagdollLevels();

    /// set entity class for ragdoll level
    void SetRagdollRoot(int, nSceneNode*);
    /// get entity class for ragdoll level
    nSceneNode*  GetRagdollRoot(int, int);
    /// get entity class for ragdoll level
    int  GetNumRagdollRoots(int);

private:

    enum
    {
        NumRagdolls = 8,
    };

    /// correspondences from ragdoll index to lod index
    nArray<int> ragdollIndices;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneRagdollClass::ClearRagdollLevels()
{
    this->ragdollIndices.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneRagdollClass::GetNumRagdollLevels()
{
    return this->ragdollIndices.Size();
}

//------------------------------------------------------------------------------
#endif
