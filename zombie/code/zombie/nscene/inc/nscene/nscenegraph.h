#ifndef N_SCENEGRAPH_H
#define N_SCENEGRAPH_H
//------------------------------------------------------------------------------
/**
    @class nSceneGraph
    @ingroup SceneServers
    @brief The scene graph collects nSceneNode objects and then organizes 
    them in passes for rendering.

    The scene graph is rebuilt only at the right times when some external 
    culling system determines that some node hierarchy is made visible 
    within the space determined by a scene graph. The whole scene graph is
    re-rendered each frame just in the right order.
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "kernel/nprofiler.h"
#include "util/narray.h"
#include "misc/nwatched.h"
#include "mathlib/matrix.h"
#include "variable/nvariable.h"
#include "variable/nvariableserver.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/nsceneserver.h"
#include "zombieentity/nctransform.h"
#ifndef NGAME
#include "kernel/nprofiler.h"
#endif

class nEntityObject;
class nSceneNode;
class nGfxServer2;
class nShader2;
class nVariableServer;

//------------------------------------------------------------------------------
class nSceneGraph : public nRoot
{
public:
    /// constructor
    nSceneGraph();
    /// destructor
    virtual ~nSceneGraph();
    
    /// begin collecting scene nodes
    bool BeginAttach();
    /// attach the toplevel object of a scene node hierarchy to the scene
    void Attach(nEntityObject* entityObject);
    /// begin a group node and push it onto the stack
    void BeginGroup(const int passIndex, nSceneNode *sceneNode, nEntityObject *entityObject);
    /// create a new group node
    void AddGroup(const int passIndex, nSceneNode *sceneNode, nEntityObject *entityObject);
    /// end a group node and pop from stack
    void EndGroup();
    /// end collecting scene nodes
    void EndAttach();

    /// set transform for the node on top of the attach stack
    void SetTopModelTransform(const matrix44& model);
    
    /// set the current view transform at Attach
    void SetViewTransform(const matrix44& viewTransform);
    /// get the current view transform at Attach
    const matrix44& GetViewTransform();
    /// set max material level in current attach process
    void SetMaxMaterialLevel(const int level);
    /// get max material level in current attach process
    const int GetMaxMaterialLevel();

    /// make sure scene node resources are valid
    void ValidateNodeResources();
    /// sort pass indexes by specified value
    void SortPass(const int passIndex, int __cdecl SortHook(const int *, const int *));
    /// render groups for a pass (soon to be obsolete)
    void RenderPass(const int passIndex);
    
    /// select current pass and select first group as current
    bool BeginRender(const int passIndex);
    /// select next group in current pass
    bool Next();
    /// render current group
    bool RenderCurrent();
    /// end rendering a pass
    void EndRender();
    /// push a pass to stack
    void PushPass(const int passIndex);
    /// pop a pass from stack
    void PopPass();
    /// get current render pass
    const nFourCC GetCurrentPass();
    /// get index for current render pass
    const int GetCurrentPassIndex();
    /// get current active group
    const int GetCurrentIndex();
    /// set current active group
    void SetCurrentIndex(const int index);
    /// get scene node from current group's parent group
    const nSceneNode *GetCallingNode();
    /// get render context from current group's parent group
    const nEntityObject *GetCallingEntity();
    /// get scene node from current group
    const nSceneNode *GetCurrentNode();
    /// get render context from current group
    const nEntityObject *GetCurrentEntity();
    /// get lod index from current group
    const int GetCurrentLod();
    /// get current set of visible flags
    const int GetCurrentFlags();

    /// get number of registered groups in a pass
    const int GetPassNumGroups(const int index);
    
    /// get scene node from group by index (only when sorting)
    const nSceneNode *GetGroupNodeByIndex(const int groupIndex);
    /// get render context from group by index (only when sorting)
    const nEntityObject *GetGroupContextByIndex(const int groupIndex);
    /// get model transform from group by index (only when sorting)
    const matrix44& GetGroupModelTransformByIndex(const int groupIndex);

    /// set model matrix for current group
    void SetModelTransform(const matrix44& model);
    /// get parent model matrix
    const matrix44& GetModelTransform() const;
    /// set shader index for current group
    void SetShaderIndex(const int index);
    /// get shader index for current group
    int GetShaderIndex() const;

protected:
    bool inBeginAttach;
    bool inSortingPass;
    matrix44 viewTransform;

    // lod and state for current attach process
    int maxMaterialLevel;
    int attachLodIndex;
    
    class Group
    {
    public:
        int parentIndex; ///< index of parent in group array, or -1
        nEntityObject* entityObject;
        nSceneNode *sceneNode;
        matrix44 modelTransform;
        int shaderIndex;
        int lodIndex;
    };
    struct CurrentGroup
    {
        int passIndex;
        int groupIndex;
    };
    enum
    {
        MaxHierarchyDepth = 64,
        MaxPassDepth = 16,
        MaxPasses = 16,
    };
    
    static nSceneGraph* self;
    static vector3 viewerPos;
    
    int stackDepth;
    nArray<Group> groupArray;
    nFixedArray<int> groupStack;
    nArray<nArray<int> > passGroups;
    CurrentGroup passStack[MaxPassDepth];
    int currentPassIndex;
    int currentIndex;
    #if __NEBULA_STATS__
    //friend class nGeometryNode;
    //friend class nTransformNode;
    //friend class nSceneNode;
    //friend class nRenderPathNode;
    //friend class nCommonViewport;
    nProfiler profAttach;
    //nProfiler profAddGroup;
    nProfiler profSortNodes;
    //nProfiler profAttachSubnodes;
    //nProfiler profAttachTransform;
    //nProfiler profAttachGeometry;
    //nProfiler profAttachSceneComp;
    nProfiler profRenderGroups;
    nWatched dbgQueryNumAttachGroups;
    int statsNumAttachGroups;
    #endif
};

//------------------------------------------------------------------------------
/**
    set the current view transform at Attach.
*/
inline
void
nSceneGraph::SetViewTransform(const matrix44& viewTransform)
{
    this->viewTransform = viewTransform;
}

//------------------------------------------------------------------------------
/**
    get the current view transform at Attach
*/
inline
const matrix44&
nSceneGraph::GetViewTransform()
{
    return this->viewTransform;
}

//------------------------------------------------------------------------------
/**
    set the current view transform at Attach.
*/
inline
void
nSceneGraph::SetMaxMaterialLevel(const int level)
{
    this->maxMaterialLevel = level;
}

//------------------------------------------------------------------------------
/**
    get the current view transform at Attach
*/
inline
const int
nSceneGraph::GetMaxMaterialLevel()
{
    return this->maxMaterialLevel;
}

//------------------------------------------------------------------------------
/**
    Get current render pass.
*/
inline
const nFourCC
nSceneGraph::GetCurrentPass()
{
    n_assert(stackDepth > 0);
    return nSceneServer::Instance()->GetPassAt(this->currentPassIndex);
}

//------------------------------------------------------------------------------
/**
    Get index for current render pass.
*/
inline
const int
nSceneGraph::GetCurrentPassIndex()
{
    n_assert(stackDepth > 0);
    return this->currentPassIndex;
}

//------------------------------------------------------------------------------
/**
    Push current pass to stack.
*/
inline
void
nSceneGraph::PushPass(const int passIndex)
{
    this->passStack[this->stackDepth].passIndex = this->currentPassIndex;
    this->passStack[this->stackDepth].groupIndex = this->currentIndex;
    ++this->stackDepth;

    this->currentPassIndex = passIndex;
}

//------------------------------------------------------------------------------
/**
    pop last pass from stack.
*/
inline
void
nSceneGraph::PopPass()
{
    n_assert(this->stackDepth > 0);

    this->stackDepth--;
    this->currentIndex = this->passStack[this->stackDepth].groupIndex;
    this->currentPassIndex = this->passStack[this->stackDepth].passIndex;
}

//------------------------------------------------------------------------------
/**
    Get index for current group.
*/
inline
const int
nSceneGraph::GetCurrentIndex()
{
    return this->currentIndex;
}

//------------------------------------------------------------------------------
/**
    Set index for current group.
*/
inline
void
nSceneGraph::SetCurrentIndex(const int index)
{
    n_assert(this->stackDepth > 0);//inRender
    n_assert((int)index < this->passGroups[this->currentPassIndex].Size());
    this->currentIndex = index;
}

//------------------------------------------------------------------------------
/**
    Get scene node from current group.
*/
inline
const nSceneNode *
nSceneGraph::GetCurrentNode()
{
    n_assert(this->stackDepth > 0);//inRender
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    return this->groupArray[indexArray[this->currentIndex]].sceneNode;
}

//------------------------------------------------------------------------------
/**
    Get scene node from current group of previous pass.
*/
inline
const nSceneNode *
nSceneGraph::GetCallingNode()
{
    // get pass and node index from parent group in the calling hierarchy
    n_assert(this->stackDepth > 1);
    int callingIndex = this->passStack[this->stackDepth - 1].groupIndex;
    int callingPassIndex = this->passStack[this->stackDepth - 1].passIndex;
    nArray<int>& indexArray = this->passGroups[callingPassIndex];
    return this->groupArray[indexArray[callingIndex]].sceneNode;
}

//------------------------------------------------------------------------------
/**
    Get render context from current group of previous pass.
*/
inline
const nEntityObject *
nSceneGraph::GetCallingEntity()
{
    // get pass and node index from parent group in the calling hierarchy
    n_assert(this->stackDepth > 1);
    int callingIndex = this->passStack[this->stackDepth - 1].groupIndex;
    int callingPassIndex = this->passStack[this->stackDepth - 1].passIndex;
    nArray<int>& indexArray = this->passGroups[callingPassIndex];
    return this->groupArray[indexArray[callingIndex]].entityObject;
}

//------------------------------------------------------------------------------
/**
    Get render context from current group.
*/
inline
const nEntityObject *
nSceneGraph::GetCurrentEntity()
{
    n_assert(this->stackDepth > 0);//inRender
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    return this->groupArray[indexArray[this->currentIndex]].entityObject;
}

//------------------------------------------------------------------------------
/**
    Get render context from current group.
*/
inline
const int
nSceneGraph::GetCurrentLod()
{
    n_assert(this->stackDepth > 0);//inRender
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    return this->groupArray[indexArray[this->currentIndex]].lodIndex;
}

//------------------------------------------------------------------------------
/**
    Get render context from current group.
*/
inline
const int
nSceneGraph::GetCurrentFlags()
{
    n_assert(this->stackDepth > 0);//inRender
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    return this->groupArray[indexArray[this->currentIndex]].entityObject->GetComponent<ncScene>()->GetAttachFlags();
}

//------------------------------------------------------------------------------
/**
    Get scene node from group by index (only when sorting).
*/
inline
const nSceneNode *
nSceneGraph::GetGroupNodeByIndex(const int groupIndex)
{
    n_assert(this->inSortingPass);
    return this->groupArray[groupIndex].sceneNode;
}

//------------------------------------------------------------------------------
/**
    Get render context from group by index (only when sorting).
*/
inline
const nEntityObject *
nSceneGraph::GetGroupContextByIndex(const int groupIndex)
{
    n_assert(this->inSortingPass);
    return this->groupArray[groupIndex].entityObject;
}

//------------------------------------------------------------------------------
/**
    Get model transform from group by index (only when sorting).
*/
inline
const matrix44&
nSceneGraph::GetGroupModelTransformByIndex(const int groupIndex)
{
    n_assert(this->inSortingPass);
    Group& curGroup = this->groupArray[groupIndex];
    if (curGroup.parentIndex != -1)
    {
        return this->groupArray[curGroup.parentIndex].modelTransform;
    }
    else
    {
        return curGroup.entityObject->GetComponent<ncTransform>()->GetTransform();
    }
}

//------------------------------------------------------------------------------
/**
    Set shader object for current group.
*/
inline
void
nSceneGraph::SetShaderIndex(const int shaderIndex)
{
    n_assert(this->stackDepth > 0);//inRender
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    this->groupArray[indexArray[this->currentIndex]].shaderIndex = shaderIndex;
}

//------------------------------------------------------------------------------
/**
    Get shader object for current group.
*/
inline
int
nSceneGraph::GetShaderIndex() const
{
    n_assert(this->stackDepth > 0);//inRender
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    return this->groupArray[indexArray[this->currentIndex]].shaderIndex;
}

//------------------------------------------------------------------------------
#endif
