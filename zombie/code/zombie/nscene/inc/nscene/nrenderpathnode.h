#ifndef N_RENDERPATHNODE_H
#define N_RENDERPATHNODE_H
//------------------------------------------------------------------------------
/**
    @class nRenderPathNode
    @ingroup SceneNodes
    
    @brief An auxiliary node responsible for organizing scene rendering.

    (C) 2004 Conjurer Services, S.A.
*/

#include "kernel/ncmdprotonativecpp.h"
#include "nscene/nscenenode.h"
#include "nscene/nshapebucket.h"
#include "nrenderpath/nrenderpath2.h"
#include "kernel/nprofiler.h"

class nSceneGraph;
#ifdef __NEBULA_STATS__
class nGeometryNode;
class nAbstractShaderNode;
#endif

//------------------------------------------------------------------------------
class nRenderPathNode : public nSceneNode
{
public:
    /// constructor
    nRenderPathNode();
    /// destructor
    virtual ~nRenderPathNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by app when render context is going to be released
    virtual void EntityDestroyed(nEntityObject* entityObject);
    /// called by nSceneGraph when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// called by nSceneGraph when node is to be rendered
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// access to embedded render path object
    nRenderPath2& GetRenderPath();
    /// get if lights are on for current pass
    const bool GetLightsEnabled();

    /// @name Script Interface
    //@{

    /// set filename of render path definition XML file
    void SetRenderPathFilename(const nString&);
    /// get the render path filename
    const nString& GetRenderPathFilename() const;
    /// set per-shape lighting
    void SetObeyLightLinks(bool);
    /// get per shape lighting
    bool GetObeyLightLinks() const;
    /// set max material profile
    void SetMaxMaterialLevel(int);
    /// get max material profile
    int GetMaxMaterialLevel() const;

    //@}

protected:
    /// do the render path rendering
    void DoRenderPath(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// sort pass for scene lights
    void SortLights(nSceneGraph* sceneGraph);
    /// sort a phase from the render path
    void SortPhase(nRpPhase& curPhase, nSceneGraph* sceneGraph);
    /// group shapes by shader, return number of buckets
    int BeginPhase(nRpPhase& curPhase, nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// render a phase from the render path
    void RenderPhase(nRpPhase& curPhase, nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// check if a shape is to be rendered based on the phase light mode
    bool CheckPhaseFlags(nRpPhase& curPhase, nSceneGraph* sceneGraph);
    /// get shader index from current geometry node
    int GetShaderIndex(nRpPhase& curPhase, nSceneGraph* sceneGraph);
    /// get sequence index for a shader, and register
    int GetSequenceIndex(nRpPhase& curPhase, int shaderIndex);

    /// a qsort() hook for sorting geometry by priority
    static int __cdecl PrioritySorter(const int *elm0, const int *elm1);
    /// a qsort() hook for sorting geometry by surface
    static int __cdecl SurfaceSorter(const int *elm0, const int *elm1);
    /// a qsort() hook for sorting geometry by distance
    static int __cdecl DistanceSorter(const int *elm0, const int *elm1);
    /// a qsort() hook for sorting geometry by distance using local boxes
    static int __cdecl LocalBoxDistanceSorter(const int *elm0, const int *elm1);

    static nRpPhase::SortingOrder curSortingOrder;
    static nSceneGraph *curSceneGraph;
    static vector3 viewerPos;

    nString renderPathFilename;
    nRenderPath2 renderPath; // TEMP create one instance per entity?

    /// shape bucket for every phase in the render path
    nFixedArray<nShapeBucket> shapeBucket;
    /// sequence bucket for every phase in the render path
    nFixedArray<nShapeBucket> sequenceBucket;
    /// index global sequences into their index within their phase
    nFixedArray<int> sequenceIndices;
    /// index shaders into their phase sequence
    nKeyArray<int> sequenceShaderIndices;

    bool obeyLightLinks;
    bool lightsEnabled;
    int maxMaterialLevel;
    int errorShaderIndex;

    /// map of lights into light indices
    nKeyArray<int> lightArray;
    /// array of lights for each shape index
    nArray<nArray<int> > shapeLightArray;

    /// indices of lights affecting current geometry (RenderOncePerLight)
    nArray<int> lightIndices;
    /// selector types for all lights in the scene (!obeyLightLinks)
    nArray<nFourCC> selectorTypes;

    #ifndef NGAME
    nVariable::Handle obeyLightLinksVarHandle;
    nVariable::Handle maxMaterialLevelVarHandle;
    #endif

    int viewPassIndex;
    int lghtPassIndex;

    #if __NEBULA_STATS__
    nProfiler profSort;
    nProfiler profRender;
    nProfiler profBeginPass;
    nProfiler profBeginPhase;
    nProfiler profShaderIndex;
    nProfiler profRenderLights;
    nProfiler profRenderLightNodes;
    nProfiler profApplySurface;
    nProfiler profApplyGeometry;
    nProfiler profRenderSurface;
    nProfiler profRenderGeometry;

    nWatched dbgNumShaderChanges;
    nWatched dbgNumSurfaceChanges;
    nWatched dbgNumGeometryChanges;

    nWatched dbgNumShaderChangesByPass;
    nWatched dbgNumSurfaceChangesByPass;
    nWatched dbgNumGeometryChangesByPass;

    nWatched dbgGeometrySize;
    nWatched dbgTextureSize;

    enum
    {
        MaxScenePasses = 32,
    };

    static nArg *arrayNumShaderChangesByPass;
    static nArg *arrayNumSurfaceChangesByPass;
    static nArg *arrayNumGeometryChangesByPass;

    int statsNumShaderChanges;
    int statsNumSurfaceChanges;
    int statsNumGeometryChanges;
    int statsGeometrySize;
    int statsTextureSize;

    /// reset resource counterfor this frame
    void BeginResourceStats();
    /// collect resource size from geometry
    void CollectGeometryStats(nGeometryNode* geometry);
    /// collect texture size from geometry and surface
    void CollectTextureStats(nAbstractShaderNode* surface);
    /// end resource counters
    void EndResourceStats();

    static nArray<nTexture2*> visitedTextures;
    static nArray<nMesh2*> visitedMeshes;

    /// stats level variable handle
    nVariable::Handle statsLevelVarHandle;
    /// stats level (0: none, 1: basic, 2: resource)
    int statsLevel;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath2&
nRenderPathNode::GetRenderPath()
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPathNode::SetRenderPathFilename(const nString& n)
{
    this->renderPath.SetFilename(n);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRenderPathNode::GetRenderPathFilename() const
{
    return this->renderPath.GetFilename();
}

//------------------------------------------------------------------------------
/**
    Turn obey light links on/off. When turned off, every shape will
    be lit by every light in the scene. If turned on, the Nebula2 application
    is responsible for establishing bidirectional light links between the
    render context objects.
*/
inline
void
nRenderPathNode::SetObeyLightLinks(bool b)
{
    this->obeyLightLinks = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRenderPathNode::GetObeyLightLinks() const
{
    return this->obeyLightLinks;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bool
nRenderPathNode::GetLightsEnabled()
{
    return this->lightsEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRenderPathNode::SetMaxMaterialLevel(int level)
{
    this->maxMaterialLevel = level;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRenderPathNode::GetMaxMaterialLevel() const
{
    return this->maxMaterialLevel;
}

//------------------------------------------------------------------------------
#endif
