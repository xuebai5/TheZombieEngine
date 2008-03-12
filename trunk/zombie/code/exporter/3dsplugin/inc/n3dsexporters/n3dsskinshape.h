#ifndef N_3DS_SKIN_H
#define N_3DS_SKIN_H
//------------------------------------------------------------------------------
/**
    @class n3dsSkeleton
    @ingroup n3dsMaxAnimation
    @brief n3dsExporter skin shape export data

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#pragma warning( push, 3 )
#include "IGame/IGame.h"
#pragma warning( pop )

//------------------------------------------------------------------------------
#include "util/nstring.h"
#include "mathlib/bbox.h"
#include "n3dsanimationexport/n3dsexportertypes.h"
#include "n3dsanimationexport/n3dsskeleton.h"
#include "n3dsexportsettings/n3dsExportSettings.h"

//------------------------------------------------------------------------------
const int maxJointPaletteSize = 72;

//------------------------------------------------------------------------------
class nMeshBuilder;
class nAnimBuilder;
class n3dsSkeleton;
class nEntityClass;

//------------------------------------------------------------------------------
class n3dsSkinShape
{
public:
    /// default constructor
    n3dsSkinShape();
    /// constructor
    n3dsSkinShape(IGameNode* Node, nEntityClass* entityClass);
    /// constructor
    n3dsSkinShape(IGameNode* Node , const n3dsAssetInfo& asset, const n3dsAssetInfo& ragAsset, nEntityClass* entityClass);
    /// default destructor
    ~n3dsSkinShape();

    /// create skinned mesh
    bool CreateMesh();
    /// create ragdoll mesh
    bool RagdollData(int material, int uniqueId , int subMaterialId = 0);

    /// gets skeleton from scene
    n3dsSkeleton* GetSkeleton(int lodLevel); 
    
    /// export attachments
    void ExportAttachments();
    /// export ragdoll attachments
    void ExportRagdollAttachments();
    /// place attachements
    void PlaceAttachment(IGameNode* parentNode, vector3 &trans, quaternion &quat, vector3 &scale);
    
    /// get bounding box
    bbox3& GetBBox();
    
    /// Return the shadowMesh
    nMeshBuilder* GetShadowMesh();
    /// Return the shadowMesh
    nMeshBuilder* GetRagShadowMesh();

    /// return if it is a shadow emitter
    bool IsShadowEmitter();
    /// Save the mesh a shadow mesh
    static void SaveShadowMesh( nMeshBuilder* shadowMesh, const nString& assetPath);

protected:
    IGameNode* iNode;
    IGameMesh* mesh;

    nEntityClass* entityClass;

    n3dsAssetInfo assetInfo;
    n3dsAssetInfo ragAssetInfo;
    //nString assetPathMeshes;
    //nString ragAssetPathMeshes;

    nMeshBuilder* skinMesh;
    nMeshBuilder* skinShadowMesh;
    nMeshBuilder* ragMesh;
    nMeshBuilder* skinRagShadowMesh;

    nString Name;
    nString CurrentName;
    matrix44 transMatrixInv;
    bbox3 box;

    n3dsSkeleton* skeleton;
    nArray<n3dsMeshFragment> fragmentsArray;
    /// bool is shadowEmitter
    bool shadowEmitter;

    // init
    void Init(IGameNode* Node);

    /// save mesh
    void SaveMesh(int material);
    void SaveRagdollMesh(int material);

    ///
    void CreateRagdollMesh();

    /// save scene file
    void SaveSceneFile(int material, int uniqueId,int subMaterialId = 0);
    /// save scene file for ragdoll
    void SaveRagdollSceneFile(int material, int uniqueId , int subMaterialId = 0);

    /// Convert To shadowMesh
    void ConvertToShadowMesh( nMeshBuilder* shadowMesh);
    /// set current name
    void SetCurrentName(int uniqueId);

    /// create mesh if node has only one material
    void CreateSkinnedMeshOneMtl();
    /// cretae mesh if node has more than one material
    void CreateSkinnedMeshMultiMtl();
    ///
    static void splitMesh( nMeshBuilder* mesh, nArray<n3dsMeshFragment>& fragmentsArray);
    /// transform local
    void TransformLocal(nMeshBuilder *localMesh);

    /// get components for export
    int GetComponents(int matId);

    /// material data
    typedef struct
    {
        int material;
        int index;
    } Map;
    static int __cdecl SortedByMatID(const void* elm0, const void* elm1);

};


//------------------------------------------------------------------------------
/**
*/
inline
bbox3& 
n3dsSkinShape::GetBBox()
{
    return box;
}

#endif
