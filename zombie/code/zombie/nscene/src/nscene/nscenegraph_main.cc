#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nscenegraph_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nscenegraph.h"
#include "nscene/nscenenode.h"

#include "kernel/nlogclass.h"

nNebulaClass(nSceneGraph, "nroot");

nSceneGraph* nSceneGraph::self = 0;
vector3 nSceneGraph::viewerPos;

NCREATELOGLEVEL(scenegraph, "Scene Graph", false, 3)
// log for scene graph attach
// 0- per-entity attach log
// 1- per-entity linked entities
// 2- detailed per-node attach log

//------------------------------------------------------------------------------
/**
*/
nSceneGraph::nSceneGraph() :
    inBeginAttach(false),
    inSortingPass(false),
    groupArray(256, 256),
    passGroups(16, 16),
    stackDepth(0),
    currentIndex(0),
    currentPassIndex(0),
    attachLodIndex(0),
    maxMaterialLevel(0)
    #if __NEBULA_STATS__
   ,statsNumAttachGroups(0),
    profAttach("profSceneAttach", true),
    profSortNodes("profSceneSortNodes", true),
    //profAddGroup("profSceneAddGroup", true),
    //profAttachSubnodes("profSceneAttachSubnodes", true),
    //profAttachTransform("profSceneAttachTransform", true),
    //profAttachGeometry("profSceneAttachGeometry", true),
    //profAttachSceneComp("profSceneAttachSceneComp", true),
    profRenderGroups("profSceneRenderGroups", true),
    dbgQueryNumAttachGroups("sceneNumAttachGroups", nArg::Int)
    #endif
{
    this->groupStack.SetSize(MaxHierarchyDepth);
    this->groupStack.Clear(0);
}

//------------------------------------------------------------------------------
/**
*/
nSceneGraph::~nSceneGraph()
{
    n_assert(!this->inBeginAttach);
}

//------------------------------------------------------------------------------
/**
    Begin building the scene. Must be called once before attaching 
    nSceneNode hierarchies using nSceneGraph::Attach().
*/
bool
nSceneGraph::BeginAttach()
{
    n_assert(!this->inBeginAttach);
    
    #if __NEBULA_STATS__
    this->profAttach.StartAccum();
    //this->profAddGroup.ResetAccum();
    //this->profAttachSubnodes.ResetAccum();
    //this->profAttachTransform.ResetAccum();
    //this->profAttachGeometry.ResetAccum();
    //this->profAttachSceneComp.ResetAccum();
    //this->profRenderGroups.ResetAccum();
    this->statsNumAttachGroups = 0;
    #endif

    NLOG(scenegraph, (0, "--------------------------------------------------------------------------------"))
        NLOG(scenegraph, (0, "nSceneGraph::BeginAttach( self: '%s' )", this->GetName()))

    #if __NEBULA_STATS__
    //this->profAddGroup.StartAccum();
    #endif

    for (int i = 0; i < this->passGroups.Size(); i++)
    {
        this->passGroups[i].Reset();
    }
    this->stackDepth = 0;
    this->groupStack.Clear(0);
    this->groupArray.Reset();
    this->inBeginAttach = true;
    
    #if __NEBULA_STATS__
    //this->profAddGroup.StopAccum();
    #endif

    return true;
}

//------------------------------------------------------------------------------
/**
    Attach a scene node to the scene. This will simply invoke 
    nSceneNode::Attach() on the scene node hierarchy's root object.
*/
void
nSceneGraph::Attach(nEntityObject* entityObject)
{
    n_assert(entityObject);
    ncScene *sceneComp = entityObject->GetComponent<ncScene>();
    nSceneNode* rootNode = sceneComp->GetRootNode();
    n_assert(rootNode);

    NLOG(scenegraph, (0, "nSceneGraph::Attach( self: '%s', entity: 0x%x (%s), rootNode: %s, frameId: %d )", 
         this->GetName(), entityObject->GetId(), entityObject->GetClass()->GetName(), rootNode->GetName(), sceneComp->GetFrameId()));

    N_IFDEF_NLOG(for (int linkIndex = 0; linkIndex < sceneComp->GetNumLinks(); ++linkIndex))
    {
        NLOG(scenegraph, (1, " - light link at: %d, entity: 0x%x (%s)", linkIndex,
             sceneComp->GetLinkAt(linkIndex)->GetId(),
             sceneComp->GetLinkAt(linkIndex)->GetClass()->GetName()))
    }

    this->attachLodIndex = sceneComp->GetAttachIndex();

    rootNode->Attach(this, entityObject);

    // reset lod index to default value for entities with no lod
    this->attachLodIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Finish building the scene.
*/
void
nSceneGraph::EndAttach()
{
    // make sure the modelview stack is clear
    n_assert(0 == this->stackDepth);
    this->inBeginAttach = false;

    NLOG(scenegraph, (0, "nSceneGraph::EndAttach( self: '%s' )", this->GetName()))

    #if __NEBULA_STATS__
    this->profAttach.StopAccum();
    this->dbgQueryNumAttachGroups->SetI(this->statsNumAttachGroups);
    #endif
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to notify the scene graph that a new hierarchy group starts.
*/
void
nSceneGraph::BeginGroup(const int passIndex, nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(this->stackDepth < MaxHierarchyDepth);
    this->AddGroup(passIndex, sceneNode, entityObject);

    // push group onto hierarchy stack
    this->groupStack[this->stackDepth] = this->groupArray.Size() - 1;
    ++this->stackDepth;
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to add a node to the current hierarchy group.
*/
void
nSceneGraph::AddGroup(int passIndex, nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);

    #if __NEBULA_STATS__
    ++this->statsNumAttachGroups;
    //this->profAddGroup.StartAccum();
    #endif

    char buf[5];
    nFourCC fourcc = nSceneServer::Instance()->GetPassAt(passIndex);
    NLOG(scenegraph, (2, "nSceneGraph::AddGroup(pass: %s, index: %i, nodeClass: %s, entityClass: %s, entityId: %08x)",
        nVariableServer::FourCCToString(fourcc, buf, sizeof(buf)), this->attachLodIndex,
        sceneNode->GetClass()->GetName(), entityObject->GetClass()->GetName(), entityObject->GetId()))

    // initialize new group node
    Group& group = this->groupArray.At(this->groupArray.Size());
    group.shaderIndex = -1;
    group.sceneNode = sceneNode;
    group.entityObject = entityObject;
    group.lodIndex = this->attachLodIndex;
    
    if (0 == this->stackDepth)
    {
        group.parentIndex = -1;
    }
    else
    {
        group.parentIndex = this->groupStack[this->stackDepth - 1];
    }
    
    // add group index to the pass array
    // @FIXME set of passes should be customized from script
    //this->AddPass(fourcc);
    //int passIndex = this->passes.FindIndex(fourcc);

    // FIXME ugly hack: grow passGroups array to ensure that passIndex
    while (this->passGroups.Size() < passIndex + 1)
    {
        nArray<int> newGroupArray(64,128);
        this->passGroups.Append(newGroupArray);
    }

    this->passGroups[passIndex].Append(this->groupArray.Size() - 1);

    #if __NEBULA_STATS__
    //this->profAddGroup.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    This method is called back by nSceneNode objects in their Attach() method
    to notify the scene graph that a hierarchy group ends.
*/
void
nSceneGraph::EndGroup()
{
    n_assert(this->stackDepth > 0);
    this->stackDepth--;
}

//------------------------------------------------------------------------------
/**
    Set model transform for the node on top of the attach stack.
    @todo this is an ugly hack to allow placing decals
*/
void
nSceneGraph::SetTopModelTransform(const matrix44& m)
{
    n_assert(this->inBeginAttach);
    n_assert(this->stackDepth > 0);
    this->groupArray[this->groupStack[this->stackDepth - 1]].modelTransform = m;
}

//------------------------------------------------------------------------------
/**
    Set model transform for current group.
*/
void
nSceneGraph::SetModelTransform(const matrix44& m)
{
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    this->groupArray[indexArray[this->currentIndex]].modelTransform = m;
}

//------------------------------------------------------------------------------
/**
    Get model transform from parent group of current.
*/
const matrix44&
nSceneGraph::GetModelTransform() const
{
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    Group& curGroup = this->groupArray[indexArray[this->currentIndex]];
    if (curGroup.parentIndex != -1)
    {
        return this->groupArray[curGroup.parentIndex].modelTransform;
    }
    else
    {
        ncTransform* transform = curGroup.entityObject->GetComponentSafe<ncTransform>();
        return transform->GetTransform();
    }
}

//------------------------------------------------------------------------------
/**
    This makes sure that all attached shape and light nodes have
    loaded their resources.
    This method is available as a convenience method for subclasses.
*/
void
nSceneGraph::ValidateNodeResources()
{
    uint i;
    uint num = this->groupArray.Size();
    for (i = 0; i < num; i++)
    {
        const Group& group = this->groupArray[i];
        if (!group.sceneNode->AreResourcesValid())
        {
            group.sceneNode->LoadResources();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Sorts groups for a pass.

    history:
        - 02-Aug-2005   MA Garcias  Removed check for valid pass index.
*/
void
nSceneGraph::SortPass(const int passIndex, int __cdecl SortHook(const int *, const int *))
{
    #if __NEBULA_STATS__
    this->profSortNodes.StartAccum();
    #endif

    if (passIndex < this->passGroups.Size())
    {
        uint *indexPtr = (uint *) this->passGroups[passIndex].Begin();
        uint numGroups = this->passGroups[passIndex].Size();
        if (numGroups > 0)
        {
            this->inSortingPass = true;
            qsort(indexPtr, numGroups, sizeof(uint), (int(__cdecl *)(const void *, const void *)) SortHook);
            this->inSortingPass = false;
        }
    }

    #if __NEBULA_STATS__
    this->profSortNodes.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    Render all nodes from a pass.
*/
void
nSceneGraph::RenderPass(const int passIndex)
{
    // FIXME crashes if there aren't that many passes
    if (passIndex >= this->passGroups.Size())
    {
        return;
    }
    int numGroups = this->passGroups[passIndex].Size();
    if (numGroups == 0)
    {
        return;
    }

    #if __NEBULA_STATS__
    this->profRenderGroups.StartAccum();
    #endif

    this->PushPass(passIndex);
    nArray<int>& indexArray = this->passGroups[passIndex];
    for (int i = 0; i < numGroups; i++)
    {
        this->currentIndex = i;
        Group& curGroup = this->groupArray[indexArray[i]];

        #if __NEBULA_STATS__
        this->profRenderGroups.StopAccum();
        #endif

        curGroup.sceneNode->Render(this, curGroup.entityObject);

        #if __NEBULA_STATS__
        this->profRenderGroups.StartAccum();
        #endif
    }
    this->PopPass();

    #if __NEBULA_STATS__
    this->profRenderGroups.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    Move current group to the beginning of the specified pass.
    This should allow nodes to iterate passes for sorting them.
*/
bool
nSceneGraph::BeginRender(const int passIndex)
{
    // FIXME crashes if there aren't that many passes
    if (passIndex >= this->passGroups.Size())
    {
        return false;
    }
    int numGroups = this->passGroups[passIndex].Size();
    if (numGroups == 0)
    {
        return false;
    }

    //this->profRenderGroups.StartAccum();

    this->PushPass(passIndex);
    this->currentIndex = 0;

    //this->profRenderGroups.StopAccum();

    return true;
}

//------------------------------------------------------------------------------
/**
    Move to next group in current pass.
*/
bool
nSceneGraph::Next()
{
    n_assert(this->stackDepth > 0);
    ++this->currentIndex;
    if(this->currentIndex < this->passGroups[this->currentPassIndex].Size())
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render current group.
*/
bool
nSceneGraph::RenderCurrent()
{
    //this->profRenderGroups.StartAccum();

    n_assert(this->stackDepth > 0);
    nArray<int>& indexArray = this->passGroups[this->currentPassIndex];
    Group& curGroup = this->groupArray[indexArray[this->currentIndex]];

    //this->profRenderGroups.StopAccum();

    return curGroup.sceneNode->Render(this, curGroup.entityObject);
}

//------------------------------------------------------------------------------
/**
    Move current group to the beginning of the specified pass.
    This should allow nodes to iterate passes for sorting them.
*/
void
nSceneGraph::EndRender()
{
    n_assert(this->stackDepth > 0);

    //this->profRenderGroups.StartAccum();

    this->PopPass();

    //this->profRenderGroups.StopAccum();
}

//------------------------------------------------------------------------------
/**
    Move current group to the beginning of the specified pass.
    This should allow nodes to iterate passes for sorting them.
*/
const int
nSceneGraph::GetPassNumGroups(const int index)
{
    if (index < this->passGroups.Size())
    {
        nArray<int>& indexArray = this->passGroups[index];
        return indexArray.Size();
    }
    return 0;
}
