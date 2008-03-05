#ifndef N_BATCHMESHLOADER_H
#define N_BATCHMESHLOADER_H
//------------------------------------------------------------------------------
/**
    @class nBatchMeshLoader
    @ingroup SceneNodes
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A batch resource loader performs the setup of a batched geometry
    resource in several forms, including insertion of instance indices, 
    pretransforming of positions, skipping nonunsed components, etc. Also, it
    is responsible for checking maximum number of vertices.
       
    (C) 2006 Conjurer Services, S.A.
*/

#include "resource/nresourceloader.h"

class nInstShapeNode;
class nStaticBatchNode;
class nStreamGeometryNode;

//------------------------------------------------------------------------------
class nBatchMeshLoader : public nResourceLoader
{
public:
    /// constructor
    nBatchMeshLoader();
    /// destructor
    ~nBatchMeshLoader();

    /// singleton
    static nBatchMeshLoader* Instance();

    /// Primary load method, the one called by an nResource.
    virtual bool Load(const char *sFilename, nResource *callingResource);

private:
    /// load an instanced mesh from a single mesh
    bool LoadInstancedMesh(nInstShapeNode* batchNode, nMesh2* mesh);
    /// load a batch mesh from multiple meshes
    bool LoadStaticBatchMesh(nStaticBatchNode* batchNode, nMesh2* mesh);
    /// load a stream mesh from multiple meshes
    bool LoadStreamBatchMesh(nStreamGeometryNode* batchNode, nMesh2* mesh);

    /// load a source mesh from file
    nMesh2* LoadMesh(const char *filename);

    static nBatchMeshLoader *Singleton;
};

//------------------------------------------------------------------------------
#endif //N_BATCHMESHLOADER_H
