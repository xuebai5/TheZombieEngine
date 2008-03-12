#ifndef N_3DS_MESH_H
#define N_3DS_MESH_H

#pragma warning( push, 3 )
#include "IGame/IGame.h"
#pragma warning( pop )

#include "util/nstring.h"
#include "mathlib/bbox.h"

class nMeshBuilder;
class nEntityClass;

class n3dsMeshShape
{
public:
    n3dsMeshShape();
    n3dsMeshShape(IGameNode* Node , const nString& assetPath );
    n3dsMeshShape(IGameNode* Node, nEntityClass* entityClass);
    /// Create Mesh return false if it not has a vertex
    bool CreateMesh();
    ~n3dsMeshShape();
    const nString& GetSceneName();
    const bbox3& GetBBox();
    bool IsShadowEmitter();

private:
    void Init(IGameNode* Node);
    // static nKernelServer *ks;

    IGameNode* iNode;
    IGameMesh* mesh;
    //nEntityClass* entityClass;
    nMeshBuilder* myMesh;
    nString Name;
    nString CurrentName;
    int realComponent;
    matrix44 myMatrixInv;
    bbox3 box;
    /// The asset's path in disk
    nString assetPathMeshes;
    /// bool is shadowEmitter
    bool shadowEmitter;

    void Save(int material, int uniqueId, int subMaterialId = 0);
    /// Convert To shadowMesh
    void ConvertToShadowMesh( nMeshBuilder* shadowMesh);
    /// Save the mesh a shadow mesh
    void SaveShadowMesh( nMeshBuilder* shadowMesh);
    void SetCurrentName(int uniqueId);
    void CreateMesh_oneMtl();
    void CreateMesh_multiMtl();

    void TransformLocal(nMeshBuilder *localMesh);

    /// return components for export
    int GetComponents(int matId);

    typedef struct
    {
        int material;
        int index;
    } Map;
    static int __cdecl SortedByMatID(const void* elm0, const void* elm1);

};

inline
n3dsMeshShape::n3dsMeshShape():
    iNode(0),
    mesh(0),
	myMesh(0),
    shadowEmitter(false)
{
    ;
}



inline 
n3dsMeshShape::~n3dsMeshShape()
{
    //trNode->Release();   
}

inline
const nString& 
n3dsMeshShape::GetSceneName()
{
    return Name;
}

inline
const bbox3& 
n3dsMeshShape::GetBBox()
{
    return box;
}

#endif