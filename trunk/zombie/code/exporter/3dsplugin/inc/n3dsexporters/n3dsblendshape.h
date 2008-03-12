#ifndef N_3DS_BLEND_H
#define N_3DS_BLEND_H
//------------------------------------------------------------------------------
/**
    @class n3dsblendshape
    @ingroup n3dsMaxAnimation
    @brief n3dsExporter blend shape export data

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#pragma warning( push, 3 )
#include "Max.h"
#include "IGame/IGame.h"
#include "n3dsexporters/wm3.h"
#pragma warning( pop )

//------------------------------------------------------------------------------
#include "util/nstring.h"
#include "mathlib/bbox.h"
#include "n3dsexportsettings/n3dsExportSettings.h"

//------------------------------------------------------------------------------
class IGameNode;

//------------------------------------------------------------------------------
class nMeshBuilder;
class nEntityClass;

//------------------------------------------------------------------------------
class n3dsBlendShape
{
public:
    ///default constructor
    n3dsBlendShape();
    /// constructor
    n3dsBlendShape(IGameNode* Node, nEntityClass* entityClass);
    /// constructor
    n3dsBlendShape(IGameNode* Node , const n3dsAssetInfo& asset, nEntityClass* entityClass );

    /// destructor
    ~n3dsBlendShape();

    /// create mesh
    bool CreateMesh();
    /// create animation
    bool CreateAnimation();
    /// create targets
    void CreateTargets();
    /// create i-th target
    void CreateTarget(int i,int components);
    /// create animator
    void CreateAnimator();

    /// is blended node?
    static bool IsBlendedNode(IGameNode* Node);

protected:
    nMeshBuilder* blendMesh;
    nEntityClass* entityClass; // fixme, not used, only added to have same structure than skinshape
    nString assetPathMeshes;

    nArray<nString> Names;
    int exportComponent;
    matrix44 localTransMatrix;
    matrix44 targetTransMatrix;
    bbox3 box;
    MorphR3* morpher;
    nArray<IGameNode*> targets;
    nArray<IGameMesh*> targetmesh;

    // init
    void Init(IGameNode* Node);

    /// save scene n2
    void SaveSceneFile(int material);
    /// save i-th target
    void SaveTarget(int index, int material);
    /// set current target name
    void SetCurrentTargetName(int index, int material); 

    /// transform to local coordinates
    void TransformLocal(nMeshBuilder *localMesh);

    /// get components
    int GetComponents(int matId);

    typedef struct
    {
        int material;
        int index;
    } Map;
    static int __cdecl SortedByMatID(const void* elm0, const void* elm1);

};

//------------------------------------------------------------------------------
/**
    default constructor
*/
inline
n3dsBlendShape::n3dsBlendShape():
	blendMesh(0)
{
    ;
}

//------------------------------------------------------------------------------
/**
    default destructor
*/
inline 
n3dsBlendShape::~n3dsBlendShape()
{
    //trNode->Release();    
}

#endif