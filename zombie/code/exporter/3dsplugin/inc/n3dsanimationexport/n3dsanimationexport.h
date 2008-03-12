#ifndef N_3DS_ANIMATION_EXPORT_H
#define N_3DS_ANIMATION_EXPORT_H

//------------------------------------------------------------------------------
/**
    @class n3dsAnimationExport
    @brief Declaration of n3dsAnimationExport class.

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#pragma warning( push, 3 )
#include "IGame/IGame.h"
#pragma warning( pop )
#include "mathlib/bbox.h"
//------------------------------------------------------------------------------
#include "n3dsanimationexport/n3dsexportertypes.h"
#include "tools/nanimbuilder.h"
//------------------------------------------------------------------------------
struct n3dsBone;
class n3dsSkeleton;
class nEntityClass;
class ncRagDollClass;
class nJointGroup;
class ncSkeletonClass;

//------------------------------------------------------------------------------
class n3dsAnimationExport
{
public:
    /// default constructor
    n3dsAnimationExport();
    /// default destructor
    ~n3dsAnimationExport();

    /// Return a singleton of this class
    static n3dsAnimationExport* Instance();

    /// get skin pivot matrix, got from the fists skinned node found in scene
    Matrix3 GetSkinPivotMatrix();
    /// get skinned node, gets the first one, but can be used because there's only one skeleton in scene
    static IGameNode* GetSkinnedNode();

    /// export skeleton
    void ExportSkeleton( nEntityClass * entityClass, int lodLevel, const nArray<n3dsSkeleton*>& skeletons);

    /// export ragdoll skeleton
    void ExportRagdollSkeleton( nEntityClass * entityClass, const nArray<n3dsSkeleton*>& skeletons);

    /// get bone translation, rotation and scale
    void GetPosRotScale( Matrix3 localMatrix, int parentID, vector3& pos, quaternion& quat, vector3& scale);//(n3dsBone bone, vector3& pos, quaternion& quat, vector3& scale);
    /// create joint groups for human skeletons
    void CreateJointGroups(ncSkeletonClass *skeletonClass, int lodLevel, const n3dsSkeleton* skeleton );

    /// export skin animation
    void ExportSkinAnimation();
    /// create skin animation
    bool CreateSkinAnimation( nEntityClass * entityClass, nAnimBuilder &AnimBuilder, int lodLevel);
    /// create low lod animations (not used, little accum error because of matrix multiplication)
    bool CreateLowLodSkinAnimation(nEntityClass * entityClass, nAnimBuilder &AnimBuilder, int lodLevel);
        
    /// set skeleton state data (state, clip, ...)
    void SetSkeletonStateData( nEntityClass* entityClass, const nString& animPath, const nString& motionPath, int lodLevel);

    /// create blend animation
    //bool CreateBlendAnimation(IGameNode* Node, nEntityClass* entityClass);
    /// create blend animator
    //void CreateBlendAnimator();
    ///
    //bool FindBlendedNode(IGameNode* Node);

    /// create bounding box
    void CreateBoundingBox();
    /// find percepcion offset
    void FindPerceptionOffset();

    /// check if an animation exists in tha entityClass, oldAnim will be the state
    bool SkinAnimationExists( nEntityClass* entityClass, int& oldAnim );

    /// get animation settings
    const n3dsAnimationSettings GetAnimationSettings();

private:
    /// export human ragdoll skeleton
    void ExportHumanRagdollCorrespondence( ncRagDollClass * entityClass, const nArray<n3dsSkeleton*>& skeletons);
    /// export scavenger ragdoll skeleton
    void ExportScvRagdollCorrespondence( ncRagDollClass * entityClass, const nArray<n3dsSkeleton*>& skeletons);
    /// export strider ragdoll skeleton
    void ExportStriderRagdollCorrespondence( ncRagDollClass * entityClass, const nArray<n3dsSkeleton*>& skeletons);

    /// compute low level curves
    void  ComputeLowLevelCurves( nAnimBuilder::Group& animGroup, int completeSkParentBoneIdx, nJointGroup fullSkJointGroup, int completeSkBoneIndex);

    /// create motion file
    bool CreateMotionBuilder();
    /// set motion curve
    void SetMotionCurve( nAnimBuilder::Curve & AnimCurveTrans );

    static n3dsAnimationExport* animationServer;

    nArray<n3dsSkeleton*> skeletons;
    nAnimBuilder completeAnimBuilder;

    nAnimBuilder::Curve motionCurve;
    nAnimBuilder motionBuilder;

    int jointGroupIndex;
    bool canExportAnim;
    bool canExportMotion;

    bbox3 boundingBox;
    vector3 perceptionOffset;
    
    //MorphR3* morpher;

    n3dsAnimationSettings  animationSettings;
};

#endif
