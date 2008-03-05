#ifndef N_SKINGEOMETRYNODE_H
#define N_SKINGEOMETRYNODE_H
//------------------------------------------------------------------------------
/**
    @class nSkinGeometryNode
    @ingroup SceneNodes
    
    @brief A smooth skinned shape node.
    
    It requires being created as a child to a regular 
    ngeometrynode-derived node (nshapenode, nblendshapenode) that
    implements actual geometry rendering, from where it reuses the
    Render implementation.
    
    It allows attaching one or several animators that implement
    animation for each of the states enumerated in this node, after
    which it computes and render a joint palette for the skeleton.
    
    See also @ref N2ScriptInterface_nskingeometrynode
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/ngeometrynode.h"
#include "ncharacter/ncharjointpalette.h"

class nCharSkeleton;
class nSkinGeometryAnimator;

//------------------------------------------------------------------------------
class nSkinGeometryNode : public nGeometryNode
{
public:
    /// constructor
    nSkinGeometryNode();
    /// destructor
    virtual ~nSkinGeometryNode();
    /// object persistency
    virtual void Initialize();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// perform pre-instance geometry setup
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// render per-instance geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// invoke skin animators (published to allow access from decorator skinshadowshapenode)
    virtual void InvokeAnimators(nEntityObject* entityObject);

    /// get embedded joint palette
    nCharJointPalette& GetJointPalette();
    /// begin defining the joint palette of a fragment
    void BeginJointPalette(int numJoints);
    /// add up to 8 joint indices to the fragments joint palette
    void SetJointIndices(int paletteIndex, int ji0, int ji1, int ji2, int ji3, int ji4, int ji5, int ji6, int ji7);
    /// add a single joint index to the fragments joint palette
    void SetJointIndex(int paletteIndex, int jointIndex);
    /// finish adding joints to the joint palette
    void EndJointPalette();
    /// get number of joints in a fragment's joint palette
    int GetJointPaletteSize();
    /// get a joint index from a fragment's joint palette
    int GetJointIndex(int paletteIndex);

    /// set level to retrieve the updated skeleton from
    void SetSkeletonLevel(int level);
    /// get level to retrieve the updated skeleton from
    int GetSkeletonLevel();

    /// set channel name which delivers the current anim state index
    void SetStateChannel(const char* name);
    /// get anim state channel name 
    const char* GetStateChannel();
    /// get anim state variable handle (for nSkinGeometryAnimator only!)
    nVariable::Handle GetAnimStateVarHandle();
    
private:
    nRef<nGeometryNode> refParentNode;
    nDynAutoRef<nSkinGeometryAnimator> refGeometryAnimator;
    nCharJointPalette jointPalette;         /// (static) joint palette definition
    const nCharSkeleton* extCharSkeleton;   /// (dynamic) animated skeleton
    // animation state is specified per-shape
    nVariable::Handle animStateVarHandle;
    int skeletonLevel;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharJointPalette&
nSkinGeometryNode::GetJointPalette()
{
    return this->jointPalette;
}

//------------------------------------------------------------------------------
#endif
