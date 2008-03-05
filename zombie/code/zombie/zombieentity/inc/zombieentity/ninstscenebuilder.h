#ifndef N_INSTSCENEBUILDER_H
#define N_INSTSCENEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nInstSceneBuilder
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>
    @brief Builds an instanced version of a scene resource

    (C) 2005 Conjurer Services, S.A.
*/

class nSceneNode;
class nSurfaceNode;
//------------------------------------------------------------------------------
class nInstSceneBuilder
{
public:
    /// constructor 1
    nInstSceneBuilder();
    /// constructor 2
    nInstSceneBuilder(nSceneNode*);
    /// destructor
    ~nInstSceneBuilder();
    
    /// set original root node
    void SetRootNode(nSceneNode*);
    /// get original root node
    nSceneNode* GetRootNode();
    /// set instance stream name
    void SetInstanceStream(const char *);
    /// get instance stream name
    const char *GetInstanceStream();

    /// build instanced scene
    void BuildInstancedScene();
    /// destroy instanced scene
    void DestroyInstancedScene();

protected:

    /// build from current node
    void BuildFromNode(nSceneNode*);

    /// build instanced surface from current node
    nSurfaceNode* BuildFromSurface(nSurfaceNode*);

    nRef<nSceneNode> refInstSurface;//TEMP
    nRef<nSceneNode> refSceneRoot;
    nString instanceStreamName;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstSceneBuilder::SetRootNode(nSceneNode* sceneNode)
{
    this->refSceneRoot = sceneNode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nSceneNode*
nInstSceneBuilder::GetRootNode()
{
    return this->refSceneRoot.isvalid() ? this->refSceneRoot.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstSceneBuilder::SetInstanceStream(const char *name)
{
    this->instanceStreamName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nInstSceneBuilder::GetInstanceStream()
{
    return this->instanceStreamName.IsEmpty() ? 0 : this->instanceStreamName.Get();
}

//------------------------------------------------------------------------------
#endif /*N_INSTSCENEBUILDER_H*/
