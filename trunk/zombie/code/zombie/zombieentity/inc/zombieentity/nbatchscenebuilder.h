#ifndef N_BATCHSCENEBUILDER_H
#define N_BATCHSCENEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nBatchSceneBuilder
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>
    @brief Builds a batch-optimized version of a scene resource
    It batches together geometry for a shared depth pass.
    Etc.

    (C) 2005 Conjurer Services, S.A.
*/

class nSurfaceNode;
class nShapeNode;
//------------------------------------------------------------------------------
class nBatchSceneBuilder
{
public:
    /// constructor
    nBatchSceneBuilder();
    /// destructor
    ~nBatchSceneBuilder();

    /// set original root node
    void SetRootNode(nSceneNode*);
    /// get original root node
    nSceneNode* GetRootNode();

    /// build scene batched for depth
    nSceneNode* BuildBatchedDepthScene();

    /// build scene batched by material
    nSceneNode* BuildBatchedSceneByMaterial();

    /// build fake lod scene
    nSceneNode* BuildFakeBatchLevel();

    /// build scene batched for depth pass
    nSceneNode* BuildBatchedDepthSceneByCell();

protected:
    /// batch shapes for depth pass starting at root node
    nSceneNode* BatchDepthShapesFromNode(nSceneNode* parentNode, nArray<nShapeNode*>& shapeNodes);//nArray<nString>& shapeMeshes);
    /// build custom surface for rendering depth pass
    nSceneNode* BuildDepthSurface();

    /// traverse down a scene hierarchy, group geometry nodes by material
    void CollectShapesByMaterial(nSceneNode* sceneRoot);

    struct MaterialEntry
    {
        MaterialEntry() :
            material(0)
        {
            // empty
        }
        MaterialEntry(nSurfaceNode* mat) :
            material(mat)
        {
            // empty
        }
        ~MaterialEntry()
        {
            // empty
        }
        nSurfaceNode* material;
        nArray<nShapeNode*> shapes;//meshes to batch

        bool operator==(const MaterialEntry& entry)
        {
            return this->material == entry.material;
        }
    };

    nArray<MaterialEntry> shapesByMaterial;

    nRef<nSceneNode> refSceneRoot;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nBatchSceneBuilder::SetRootNode(nSceneNode* sceneNode)
{
    this->refSceneRoot = sceneNode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSceneNode*
nBatchSceneBuilder::GetRootNode()
{
    return this->refSceneRoot.isvalid() ? this->refSceneRoot.get() : 0;
}

//------------------------------------------------------------------------------
#endif /*N_BATCHSCENEBUILDER_H*/
