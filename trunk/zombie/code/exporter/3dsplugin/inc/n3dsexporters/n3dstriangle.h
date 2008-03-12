#ifndef N_3DS_TRIANGLE_H
#define N_3DS_TRIANGLE_H
//------------------------------------------------------------------------------
/**
*/
//------------------------------------------------------------------------------

#pragma warning( push, 3 )
#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"
#pragma warning( pop )
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
class n3dsSkeleton;

class n3dsTriangle
{
public:
    /// default constructor
    n3dsTriangle();
    /// constructor
    n3dsTriangle(IGameMesh *Mes3ds,int idxTriangle);
    /// set mesh and triangle
    void Set(IGameMesh *Mes3ds,int idxTriangle);
    /// default destructor
    ~n3dsTriangle();

    /// get material id
    int GetMaterialId();

    /// get vertex
    void GetVertex(nMeshBuilder::Vertex  vertex[],int component, IGameNode *iNode, n3dsSkeleton* pSkeleton =0);

    /// return the real components of igamemesh
    static int GetMeshComponents(IGameMesh* mesh);

private:
    IGameMesh* mesh;
    FaceEx* face3ds;
    int index;
};

//------------------------------------------------------------------------------
/**
*/
inline 
n3dsTriangle::n3dsTriangle() : 
    index(-1),
    face3ds(0),
    mesh(0)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsTriangle:: n3dsTriangle(IGameMesh *Mesh3ds,int idxTriangle)
{
    index=idxTriangle;
    mesh=Mesh3ds;
    face3ds=Mesh3ds->GetFace(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3dsTriangle::Set(IGameMesh *Mesh3ds,int idxTriangle)
{
    index=idxTriangle;
    mesh=Mesh3ds;
    face3ds=Mesh3ds->GetFace(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsTriangle::~n3dsTriangle()
{
    //empty
}

#endif