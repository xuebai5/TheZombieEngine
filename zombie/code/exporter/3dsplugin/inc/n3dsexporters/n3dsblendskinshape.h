#ifndef N_3DS_BLENDSKIN_H
#define N_3DS_BLENDSKIN_H
//------------------------------------------------------------------------------
/**
    @class n3dsblendskinshape
    @ingroup n3dsMaxAnimation
    @brief n3dsExporter blended-skinned data

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#include "n3dsexporters/n3dsblendshape.h"
#include "n3dsexporters/n3dsskinshape.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
class n3dsBlendSkinShape : public n3dsBlendShape, n3dsSkinShape
{
public:
    /// default constructor
    n3dsBlendSkinShape();
    /// constructor
    n3dsBlendSkinShape(IGameNode* Node, nEntityClass* entityClass);
    /// constructor
    n3dsBlendSkinShape(IGameNode* Node , const n3dsAssetInfo& asset, const n3dsAssetInfo& ragAsset, nEntityClass* entityClass );

    /// create mesh
    bool CreateMesh();

    /// get bounding box
    bbox3& GetBBox();

private:
    /// save scene file
    void SaveSceneFile(int material, int uniqueId,int subMaterialId = 0);
    /// create mesh multimaterial
    void CreateMeshMultiMtl(int matmorpherId, int nummorphvertexs);
};

//------------------------------------------------------------------------------
#endif