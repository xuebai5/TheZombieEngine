#ifndef N_WRAPPERMESH_H
#define N_WRAPPERMESH_H
//------------------------------------------------------------------------------
/**
    @class nWrapperMesh
    @ingroup NebulaSpatialSystem
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Wrapper mesh for indoor cells.

    This class encapsulates both a mesh builder (required to test a point
    agains a set of triangles) and a mesh for debug visualizations. This
    class is not to be used in actual game to determine whether a point is
    inside an indoor cell, but just in exporter and editor to statically 
    place entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/nref.h"
#include "tools/nmeshbuilder.h"

class nMeshBuilder;
//------------------------------------------------------------------------------
class nWrapperMesh
{
public:
    /// constructor
    nWrapperMesh();
    /// destructor
    ~nWrapperMesh();
    /// set mesh from resource file
    void SetMeshFile(const char *meshFile);
    /// get mesh from resource file
    const char *GetMeshFile();
    /// load mesh resource and internal structure
    bool LoadResources();
    /// unload resources
    void UnloadResources();

    /// set mesh from builder
    void SetMeshBuilder(nMeshBuilder& meshBuilder);
    /// set mesh from builder
    nMeshBuilder& GetMeshBuilder();
    /// set transform for builder
    void SetTransform(const matrix44& transform);
    /// check whether a point is contained in the mesh
    bool Contains(vector3 const& pos) const;
    /// returns the distance of the minimum positive intersection
    float GetIntersectionDist(const vector3 &point, const vector3 &direction) const;

    /// render the wrapper mesh for debug visualization
    void RenderDebug();

private:
    nString resourceFile;
    nRef<nMesh2> refMesh;
    nMeshBuilder meshBuilder;
    matrix44 transform;
    matrix44 invTransform;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nWrapperMesh::SetMeshFile(const char *fileName)
{
    this->resourceFile = fileName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nWrapperMesh::GetMeshFile()
{
    return this->resourceFile.IsEmpty() ? 0 : this->resourceFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nWrapperMesh::SetTransform(const matrix44& transform)
{
    this->transform = transform;
    this->invTransform = transform;
    this->invTransform.invert();
}

//------------------------------------------------------------------------------
#endif // N_WRAPPERMESH_H
