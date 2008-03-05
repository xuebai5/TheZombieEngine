#ifndef N_VIEWPORTDEBUGMODULE_H
#define N_VIEWPORTDEBUGMODULE_H
//------------------------------------------------------------------------------
/**
    @class nViewportDebugModule
    @ingroup NebulaConjurerEditor
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief handles debug options for an editor viewport.

    (C) 2005 Conjurer Services, S.A.
*/
#include "ndebug/ndebugmodule.h"
#include "gfx2/nlineserver.h"
#include "kernel/nref.h"

class nShader2;
class nTexture2;
class nSceneGraph;
class nEditorViewport;
class ncScene;
class nEntityObject;
class nConjurerDebugCompServer;
class ncSpatialQuadtreeCell;
class nLoadArea;

//------------------------------------------------------------------------------
class nViewportDebugModule : public nDebugModule
{
public:
    /// constructor
    nViewportDebugModule();
    /// destructor
    virtual ~nViewportDebugModule();
    /// called when the module is created
    virtual void OnDebugModuleCreate();
    /// called when an option is assigned
    virtual void OnDebugOptionUpdated();
    /// set application viewport
    void SetAppViewport(nEditorViewport *appViewport);
    /// get application viewport
    nEditorViewport *GetAppViewport();

    /// open debug module for render
    void Open();
    /// override render context variables
    void Overrides(ncScene *renderContext);
    /// begin debug scene
    void BeginScene();
    /// render debug scene
    void Render(nSceneGraph *sceneGraph);
    /// end debug scene
    void EndScene();
    /// handle input
    void HandleInput(nTime frameTime);
    /// toggle the debug option (i.e. enable it if disabled and vice versa)
    void ToggleOption(const char *name);
    /// close debug module for render
    void Close();

    /// Set time to end displaying camera velocity
    void SetTimeToDrawCameraVelocity( nTime time );

private:
    /// draw bounding boxes for spatial elements
    void DrawBoundingBoxes();
    /// draw the light regions
    void DrawLightRegions();
    /// draw the lights
    void DrawLights();
    /// draw level bounds
    void DrawLevelBounds();
    /// draw wrapper meshes for indoor cells
    void DrawWrapperMeshes();
    /// draw spatial shadow models
    void DrawSpatialShadowModels();
    /// draw spatial portals
    void DrawPortals(bool drawVertices);
    /// draw occluders
    void DrawOccluders();
    /// draw horizon
    void DrawHorizon();
    /// draw ai info
    void DrawAI();
    /// draw navigation meshes for outdoor and indoors
    void DrawNavigationMeshes();
    /// draw obstacles found while generating the navigation mesh for the outdoor
    void DrawNavigationOutdoorObstacles();
    /// draw AI entities debug info
    void DrawAIEntities();
    /// draw agent FSM stack
    void DrawAgentFSMStack( nEntityObject* entity );
    /// draw agent sight field
    void DrawAgentSight( nEntityObject* entity );
    /// draw agent hearing field
    void DrawAgentHearing( nEntityObject* entity );
    /// draw agent feeling field
    void DrawAgentFeeling( nEntityObject* entity );
    /// draw agent/trigger proximity culling sphere
    void DrawPerceptionCulling( nEntityObject* entity );
    /// draw agent memoty
    void DrawAgentMemory( nEntityObject* entity );
    /// draw agent motion
    void DrawAgentMotion( nEntityObject* entity );
    /// draw an area trigger
    void DrawAreaTrigger( nEntityObject* entity );
    /// draw a spawner or spawn points
    void DrawSpawner( nEntityObject* entity );
    /// draw oriented bounding boxes for spatial portals
    void DrawPortalOrientedBoundingBoxes();
    /// draw coordinate axes
    void DrawAxes();
    /// render wireframe overlay
    void RenderWireframeOverlay(nSceneGraph *sceneGraph);
    /// render character skeleton
    void RenderSkeleton(nSceneGraph *sceneGraph);
    /// render geometry vertex components
    void RenderNormals(nSceneGraph *sceneGraph);
    /// draw oriented bounding boxes for non-loaded entities
    void RenderInvalidBoxes(nSceneGraph *sceneGraph);
    /// draw loading areas
    void RenderLoadAreas();
    /// render frames per second
    void RenderFPS();
    /// render onscreen statistiscs
    void RenderStats();
    /// display a message showing option toggled
    void DisplayOptionToggled(const char* optionName, bool enabled);

    enum OptionFlag
    {
        Wireframe       = 1<<0,
        Lightmaps       = 1<<1,
        Diffmaps        = 1<<2,
        Bumpmaps        = 1<<3,
        Diffuse         = 1<<4,
        Specular        = 1<<5,
        Skeleton        = 1<<6,
        BoundingBoxes   = 1<<7,
        Portals         = 1<<8,
        TerrainLod      = 1<<9,
        Physics         = 1<<10,
        ShowAxes        = 1<<11,
        AI              = 1<<12,
        FPS             = 1<<13,
        WrapperMeshes   = 1<<14,
        DebugGraphics   = 1<<15,
        DebugTexts      = 1<<16,
        ParallaxMap     = 1<<17,
        ShadowVolumes   = 1<<18,
        ShadowSpModels  = 1<<19,
        WayPoints       = 1<<20,
        SoundSources    = 1<<21,
        SoundSourcesRadius    = 1<<22,
        Occluders       = 1<<23,
        OccludersMValue = 1<<24,
        HorizSegments   = 1<<25,
        Horizon         = 1<<26,
        InvalidBoxes    = 1<<27,
        LevelBounds     = 1<<28,
        LoadAreas       = 1<<29,
        Statistics      = 1<<30,
        ShowSelectionInfo     = 1<<31,
    };

    enum BoxesFlag
    {
        BBoxesCells     = 1<<0,
        BBoxesObjects   = 1<<1,
        BBoxesLights    = 1<<2,
        BSpheresCells   = 1<<3,
        PortalOBBoxes   = 1<<4,
        PortalVertices  = 1<<5,
        BLightRegions   = 1<<6,
        BOmniRegions    = 1<<7,
        BSphereRegions  = 1<<8,
        BFrustumRegions = 1<<9,

        OptionsUsingEntityArrayMask = (BBoxesCells|BBoxesObjects|BBoxesLights|BSpheresCells|
                                       PortalOBBoxes|PortalVertices|BLightRegions)
    };

    enum GrassFlag
    {
        GrassDrawDisable     = 1<<0,
        GrassDebugInfo       = 1<<1,
        GrassMeshDisable     = 1<<2,
    };

    enum BlendMode
    {
        BlendZero = 1,
        BlendOne = 2,
        BlendSrcColor = 3,
        BlendInvSrcColor = 4,
        BlendSrcAlpha = 5,
        BlendInvSrcAlpha = 6,
    };

    nEditorViewport *appViewport;
    nRef<nShader2> refWireframeShader;
    nRef<nShader2> refSpatialShapeShader;
    nRef<nShader2> refFillShader;
    nRef<nShader2> refShapeNoCullShader;
    nRef<nShader2> refShapeOverlayShader;
    nRef<nShader2> refNormalsShader;
    nRef<nShader2> refBillboardShader;
    nRef<nTexture2> refWhiteTexture;
    nRef<nTexture2> refFlatTexture;
    nRef<nTexture2> refBlackTexture;
    nRef<nMesh2> refAxisMesh;
    nRef<nMesh2> refBillboardMesh;
    nRef<nTexture2> refLightIconTexture;

    nRef<nConjurerDebugCompServer> refDebugComponentServer;

    nLineHandler skeletonLineHandler;
    nLineHandler spatialLineHandler;
    nLineHandler navOutdoorBoundaryLineHandler;
    nLineHandler navIndoorBoundaryLineHandler;

    int wireframeShaderIndex;

    static nArray<nEntityObject*> skeletonEntities;
    static nArray<matrix44> characterTransforms;
    static nArray<int> lodIndices;
    int terrainPassFlags;

    uint fillMode;
    uint optionFlags;
    uint editorFlags;
    uint physicsFlags;
    uint aiFlags;
    uint grassFlags;
    uint boxesFlags;

    // Time to draw current viewport camera velocity
    nTime timeToDrawCameraVelocity;

    // time when FPS has to be updated
    nTime timeNextUpdateFPS;
    unsigned int frameIdLastUpdateFPS;
    nString textFPS;

    nVariable::Handle shadowSrcBlendVarHandle;
    nVariable::Handle shadowDstBlendVarHandle;

    #ifdef __NEBULA_STATS__
    nWatched watchNumDrawCalls;
    nWatched watchNumPrimitives;
    nWatched watchNumMeshChanges;
    nWatched watchNumTextureChanges;
    nWatched watchNumVertexDeclChanges;
    nWatched watchNumShaderChanges;
    nWatched watchNumSurfaceChanges;
    nWatched watchNumGeometryChanges;
    nWatched watchNumShaderChangesByPass;
    nWatched watchNumSurfaceChangesByPass;
    nWatched watchNumGeometryChangesByPass;
    nWatched watchGeometrySize;
    nWatched watchTextureSize;

    nString passNames[32];
    #endif

    /// traverse down the quadtree starting from a non-leaf cell
    void CollectLeafCellsFrom(ncSpatialQuadtreeCell* quadtreeCell, nArray<ncSpatialQuadtreeCell*>& leafCells);

    /// render a single box in a load area
    void DrawAreaBox(nGfxServer2* gfxServer, nLoadArea* loadArea, const bbox3& box);
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
nViewportDebugModule::SetAppViewport(nEditorViewport* appViewport)
{
    this->appViewport = appViewport;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEditorViewport*
nViewportDebugModule::GetAppViewport()
{
    n_assert(this->appViewport);
    return this->appViewport;
}

//------------------------------------------------------------------------------
#endif // N_VIEWPORTDEBUGMODULE_H
