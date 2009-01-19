#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nviewportdebugmodule_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nviewportdebugmodule.h"
#include "conjurer/neditorviewport.h"
#include "kernel/nkernelserver.h"
#include "ndebug/ndebugserver.h"
#include "ndebug/ndebuggraphicsserver.h"
#include "nscene/nrenderpathnode.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nskingeometrynode.h"
#include "nscene/ncsceneclass.h"
#include "animcomp/nccharacter.h"
#include "nscene/nscenegraph.h"
#include "character2/ncharskeleton.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nshapeserver.h"
#include "input/ninputserver.h"
#include "nphysics/nphysicsserver.h"
#include "entity/nentity.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialoccluder.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/nspatialindoorcell.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/nlightregion.h"
#include "conjurer/nconjurerapp.h"
#include "ndebug/nceditor.h"
#include "nrenderpath/nrenderpath2.h"
#include "nwaypointserver/nwaypointserver.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "nlayermanager/nlayermanager.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "nspatial/nccellhorizon.h"
#include "zombieentity/nloaderserver.h"
#include "zombieentity/nloadarea.h"
#include "zombieentity/ncdictionary.h"
#include "ngeomipmap/ncterraingmmcell.h"

// AI
#include "mathlib/cone.h"
#include "ncagentmemory/ncagentmemory.h"
#include "ncaimovengine/ncaimovengine.h"
#include "ncaistate/ncaistate.h"
#include "ncfsm/ncfsm.h"
#include "ncgpperception/ncgpsight.h"
#include "ncgpperception/ncgphearing.h"
#include "ncgpperception/ncgpfeeling.h"
#include "ncnavmesh/ncnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "ncspawner/ncspawner.h"
#include "ncspawnpoint/ncspawnpoint.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ntriggerserver.h"

#ifndef NGAME
#include "ndebug/ndebugcomponentserver.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/nconjurerdebugcompserver.h"
#include "rnsgameplay/ngameplayutils.h"
#endif

nNebulaClass(nViewportDebugModule, "ndebugmodule");
//------------------------------------------------------------------------------
const vector4 DefaultIndoorBoundaryColour(1.0f, 0.4f, 1.0f, 1.0f);
const vector4 DefaultOutdoorBoundaryColour(0.4f, 1.0f, 0.4f, 1.0f);

//------------------------------------------------------------------------------
static const int numColors = 7;
static vector4 color[numColors] = { vector4(0.0f, 0.0f, 1.0f, 0.2f),
                                    vector4(0.0f, 1.0f, 0.0f, 0.2f),
                                    vector4(0.0f, 1.0f, 1.0f, 0.2f),
                                    vector4(1.0f, 0.0f, 0.0f, 0.2f),
                                    vector4(1.0f, 0.0f, 1.0f, 0.2f),
                                    vector4(1.0f, 1.0f, 0.0f, 0.2f),
                                    vector4(1.0f, 0.5f, 1.0f, 0.2f),};

// declare static members
nArray<nEntityObject*> nViewportDebugModule::skeletonEntities;
nArray<matrix44> nViewportDebugModule::characterTransforms;
nArray<int> nViewportDebugModule::lodIndices;

//------------------------------------------------------------------------------
#ifdef __NEBULA_STATS__
#define AI_DEBUG_PROF( name ) \
    static nProfiler& GetProf##name##() \
    { \
        static nProfiler prof("profAIDebug_" # name, true); \
        return prof; \
    }

AI_DEBUG_PROF( NavMesh )
AI_DEBUG_PROF( NavOutdoorObstacles )
AI_DEBUG_PROF( AIEntities )
AI_DEBUG_PROF( FSMStack )
AI_DEBUG_PROF( Sight )
AI_DEBUG_PROF( Hearing )
AI_DEBUG_PROF( Feeling )
AI_DEBUG_PROF( PerceptionCulling )
AI_DEBUG_PROF( Memory )
AI_DEBUG_PROF( AIMotion )
AI_DEBUG_PROF( Triggers )
AI_DEBUG_PROF( Spawners )
#endif


//------------------------------------------------------------------------------

NCREATELOGLEVEL ( viewportDebugOptions, "Viewport Debug Options", true , 2 )

//------------------------------------------------------------------------------
/**
*/
nViewportDebugModule::nViewportDebugModule() :
    fillMode(nShaderState::Solid),
    optionFlags(0x00000000),
    editorFlags(0x00000000),
    physicsFlags(0x00000000),
    aiFlags(0x00000000),
    boxesFlags(0x00000000),
    grassFlags(0x00000000),
    skeletonLineHandler(nGfxServer2::LineList, nMesh2::Coord),
    spatialLineHandler(nGfxServer2::LineStrip, nMesh2::Coord),
    navOutdoorBoundaryLineHandler(nGfxServer2::LineList, nMesh2::Coord),
    navIndoorBoundaryLineHandler(nGfxServer2::LineList, nMesh2::Coord),
    appViewport(0),
    wireframeShaderIndex(-1),
    terrainPassFlags(0),
    timeToDrawCameraVelocity( 0.0f ),
    timeNextUpdateFPS( 0.0f ),
    shadowSrcBlendVarHandle(nVariable::InvalidHandle),
    shadowDstBlendVarHandle(nVariable::InvalidHandle)
    #ifdef __NEBULA_STATS__
   ,watchNumDrawCalls("gfxNumDrawCalls", nArg::Int),
    watchNumPrimitives("gfxNumPrimitives", nArg::Int),
    watchNumMeshChanges("gfxNumMeshChanges", nArg::Int),
    watchNumTextureChanges("gfxNumTextureChanges", nArg::Int),
    watchNumVertexDeclChanges("gfxNumVertexDeclarationChanges", nArg::Int),
    watchNumShaderChanges("sceneNumShaderChanges", nArg::Int),
    watchNumSurfaceChanges("sceneNumSurfaceChanges", nArg::Int),
    watchNumGeometryChanges("sceneNumGeometryChanges", nArg::Int),
    watchNumShaderChangesByPass("sceneNumShaderChangesByPass", nArg::List),
    watchNumSurfaceChangesByPass("sceneNumSurfaceChangesByPass", nArg::List),
    watchNumGeometryChangesByPass("sceneNumGeometryChangesByPass", nArg::List),
    watchGeometrySize("sceneGeometrySize", nArg::Int),
    watchTextureSize("sceneTextureSize", nArg::Int)
    #endif
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nViewportDebugModule::~nViewportDebugModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create relevant debug options.
*/
void
nViewportDebugModule::OnDebugModuleCreate()
{
    this->RegisterDebugFlag("showaxes",       "Show coordinate axes",               "General");
    this->RegisterDebugFlag("maxdist",        "Enable maximum render distance",     "General");
    this->RegisterDebugFlag("show3daxes",     "Show 3D axes",                       "General");
    this->RegisterDebugFlag("fillmode",       "Wireframe fillmode",                 "General");
    this->RegisterDebugFlag("wireframe",      "Wireframe overlay",                  "General");
    this->RegisterDebugFlag("fps",            "Show frames per second",             "General");
    this->RegisterDebugFlag("stats",          "Show onscreen statistics",           "General");

    this->RegisterDebugFlag("lightmap",       "Disable lightmaps",                  "Graphics");
    this->RegisterDebugFlag("diffmap",        "Disable diffuse maps",               "Graphics");
    this->RegisterDebugFlag("bumpmap",        "Disable bump maps",                  "Graphics");
    this->RegisterDebugFlag("parallaxmap",    "Disable the parallax mapping",       "Graphics");
    this->RegisterDebugFlag("diffuse",        "Disable diffuse lighting",           "Graphics");
    this->RegisterDebugFlag("specular",       "Disable specular lighting",          "Graphics");
    this->RegisterDebugFlag("shadows",        "Render shadows",                     "Graphics");
    this->RegisterDebugFlag("shadowvolume",   "Render shadow volumes",              "Graphics");
    this->RegisterDebugFlag("loadareas",      "Render loading areas",               "Graphics");
    this->RegisterDebugFlag("invalidboxes",   "Render unloaded entities as boxes",  "Graphics");
    this->RegisterDebugFlag("grassnodraw",    "Grass - disable grass draw",         "Graphics");
    this->RegisterDebugFlag("grassInfo",      "Grass - show grass debug info",      "Graphics");
    this->RegisterDebugFlag("grassnomesh",    "Grass - do not call draw primitive", "Graphics");
    this->RegisterDebugFlag("movelight",      "Move light with camera",             "Graphics");
    this->RegisterDebugFlag("showedit",       "Show light icons",                   "Graphics");
    this->RegisterDebugFlag("skeleton",       "Render skeleton overlay",            "Graphics");
    this->RegisterDebugFlag("normals",        "Render vertex normals",              "Graphics");
    this->RegisterDebugFlag("tangents",       "Render vertex tangents",             "Graphics");
    this->RegisterDebugFlag("binormals",      "Render vertex binormals",            "Graphics");
    this->RegisterDebugFlag("mrt",            "Enable MRT rendering - emissive maps", "Graphics");
    this->RegisterDebugFlag("sepia",          "Enable MRT rendering - sepia tone",  "Graphics");
    this->RegisterDebugFlag("hdr",            "Enable HDR rendering",               "Graphics");

    this->RegisterDebugFlag("boxes",          "Render bounding boxes",              "Visibility");
    this->RegisterDebugFlag("boxescells",     "Render bounding boxes - cells",      "Visibility");
    this->RegisterDebugFlag("frustumregions", "Render bounding boxes - light frustum regions", "Visibility");
    this->RegisterDebugFlag("omniregions",    "Render bounding boxes - light omni regions",    "Visibility");
    this->RegisterDebugFlag("lightregions",   "Render bounding boxes - light regions",         "Visibility");
    this->RegisterDebugFlag("sphereregions",  "Render bounding boxes - light sphere regions",  "Visibility");
    this->RegisterDebugFlag("boxeslights",    "Render bounding boxes - lights",                "Visibility");
    this->RegisterDebugFlag("boxesobjects",   "Render bounding boxes - objects",               "Visibility");
    this->RegisterDebugFlag("spherescells",   "Render bounding spheres - cells",               "Visibility");
    this->RegisterDebugFlag("horizon",        "Render horizon",                     "Visibility");
    this->RegisterDebugFlag("horizsegments",  "Render horizon segments",            "Visibility");
    this->RegisterDebugFlag("occluders",      "Render occluders",                   "Visibility");
    this->RegisterDebugFlag("occludersmvalue","Render occluders 'm' value",         "Visibility");
    this->RegisterDebugFlag("boxesportals",   "Render oriented bounding boxes - portals", "Visibility");
    this->RegisterDebugFlag("portals",        "Render portals",                     "Visibility");
    this->RegisterDebugFlag("portalverts",    "Render portal vertices",             "Visibility");
    this->RegisterDebugFlag("lightlinks",     "Enable light visibility",            "Visibility");
    this->RegisterDebugFlag("boxesmeshes",    "Render wrapper meshes",              "Visibility");
    this->RegisterDebugFlag("levelbounds",    "Draw level bounds",                  "Visibility");
    this->RegisterDebugFlag("shadowspmodels", "Render spatial shadow volumes",      "Visibility");

    this->RegisterDebugFlag("ai",             "Draw AI",                                  "AI");
    this->RegisterDebugFlag("ainavoutdoor",   "NavMesh - draw outdoor",                   "AI");
    this->RegisterDebugFlag("ainavindoors",   "NavMesh - draw indoors",                   "AI");
    this->RegisterDebugFlag("ainavwire",      "NavMesh - draw in wireframe",              "AI");
    this->RegisterDebugFlag("ainavoverlay",   "NavMesh - draw overlayed",                 "AI");
    this->RegisterDebugFlag("ainavlinks",     "NavMesh - show links",                     "AI");
    this->RegisterDebugFlag("ainavoutdoorobstacles", "NavMesh - show outdoor obstacles",  "AI");
    this->RegisterDebugFlag("aiculling",      "Perception - show proximity culling",      "AI");
    this->RegisterDebugFlag("aisight",        "Perception - draw sight field",            "AI");
    this->RegisterDebugFlag("aihearing",      "Perception - draw hearing field",          "AI");
    this->RegisterDebugFlag("aifeeling",      "Perception - draw feeling field",          "AI");
    this->RegisterDebugFlag("aisightray",     "Perception - show sight ray",              "AI");
    this->RegisterDebugFlag("aibehavior",     "State - show FSM state",                   "AI");
    this->RegisterDebugFlag("aimemory",       "State - show known entities",              "AI");
    this->RegisterDebugFlag("aitarget",       "State - show target",                      "AI");
    this->RegisterDebugFlag("aitriggers",     "Misc - draw trigger shapes",               "AI");
    this->RegisterDebugFlag("aispawners",     "Misc - draw spawners",                     "AI");
    this->RegisterDebugFlag("aimotion",       "Motion - path following",                  "AI");
    this->RegisterDebugFlag("aiavoid",        "Motion - obstacles avoidance",             "AI");
    this->RegisterDebugFlag("aistaticavoid",  "Motion - static obstacles avoidance",      "AI");
    this->RegisterDebugColourSetting("ainavcolourboundin", "NavMesh - boundary colour (indoor)", "AI", DefaultIndoorBoundaryColour);
    this->RegisterDebugColourSetting("ainavcolourboundout", "NavMesh - boundary colour (outdoor)", "AI", DefaultOutdoorBoundaryColour);

    this->RegisterDebugFlag("physics",        "Draw physics",                       "Physics");
    this->RegisterDebugFlag("phyaabb",        "Draw axis-aligned bounding boxes",   "Physics");
    this->RegisterDebugFlag("physhapes",      "Draw shapes",                        "Physics");
    this->RegisterDebugFlag("physpaces",      "Draw spaces bounding boxes",         "Physics");
    this->RegisterDebugFlag("phyhm",          "Draw height map mesh",               "Physics");
    this->RegisterDebugFlag("phyrays",        "Draw rays",                          "Physics");
    this->RegisterDebugFlag("phytm",          "Draw triangle meshes",               "Physics");
    this->RegisterDebugFlag("phyareas",       "Draw enabled/disabled areas map",    "Physics");
    this->RegisterDebugFlag("phyjoints",      "Draw joints",                        "Physics");
    this->RegisterDebugFlag("phynormals",     "Draw normals",                       "Physics");

    this->RegisterDebugFlag("debuggraphics",       "Graphics",                      "Debug");
    this->RegisterDebugFlag("debugtexts",          "Texts",                         "Debug");
    this->RegisterDebugFlag("wp",                  "Waypoints",                     "Debug");
    this->RegisterDebugFlag("soundsources",        "Sound sources",                 "Debug");
    this->RegisterDebugFlag("soundsourcesradius",  "Sound sources radius",          "Debug");
    this->RegisterDebugFlag("selectinfo",          "Show info on selecting entity", "Debug");

}

//------------------------------------------------------------------------------
/**
    Update debug options.
*/
void
nViewportDebugModule::OnDebugOptionUpdated()
{
    this->fillMode = this->GetFlagEnabled("fillmode") ? nShaderState::Wireframe : nShaderState::Solid;

    this->optionFlags = 0;
    if (this->GetFlagEnabled("wireframe"))    this->optionFlags |= Wireframe;
    if (this->GetFlagEnabled("lightmap"))     this->optionFlags |= Lightmaps;
    if (this->GetFlagEnabled("diffmap"))      this->optionFlags |= Diffmaps;
    if (this->GetFlagEnabled("bumpmap"))      this->optionFlags |= Bumpmaps;
    if (this->GetFlagEnabled("parallaxmap"))  this->optionFlags |= ParallaxMap;
    if (this->GetFlagEnabled("diffuse"))      this->optionFlags |= Diffuse;
    if (this->GetFlagEnabled("specular"))     this->optionFlags |= Specular;
    if (this->GetFlagEnabled("skeleton"))     this->optionFlags |= Skeleton;
    if (this->GetFlagEnabled("boxes"))        this->optionFlags |= BoundingBoxes;
    if (this->GetFlagEnabled("lightregions")) this->optionFlags |= BLightRegions;
    if (this->GetFlagEnabled("levelbounds"))  this->optionFlags |= LevelBounds;
    if (this->GetFlagEnabled("horizon"))      this->optionFlags |= Horizon;
    if (this->GetFlagEnabled("horizsegments")) this->optionFlags |= HorizSegments;
    if (this->GetFlagEnabled("boxesmeshes"))  this->optionFlags |= WrapperMeshes;
    if (this->GetFlagEnabled("portals"))      this->optionFlags |= Portals;
    if (this->GetFlagEnabled("occluders"))    this->optionFlags |= Occluders;
    if (this->GetFlagEnabled("occludersmvalue")) this->optionFlags |= OccludersMValue;
    if (this->GetFlagEnabled("physics"))      this->optionFlags |= Physics;
    if (this->GetFlagEnabled("wp"))           this->optionFlags |= WayPoints;
    if (this->GetFlagEnabled("showaxes"))     this->optionFlags |= ShowAxes;
    if (this->GetFlagEnabled("ai"))           this->optionFlags |= AI;
    if (this->GetFlagEnabled("fps"))          this->optionFlags |= FPS;
    if (this->GetFlagEnabled("shadowvolume")) this->optionFlags |= ShadowVolumes;
    if (this->GetFlagEnabled("shadowspmodels")) this->optionFlags |= ShadowSpModels;
    if (this->GetFlagEnabled("debuggraphics")) this->optionFlags |= DebugGraphics;
    if (this->GetFlagEnabled("debugtexts"))   this->optionFlags |= DebugTexts;
    if (this->GetFlagEnabled("soundsources")) this->optionFlags |= SoundSources;
    if (this->GetFlagEnabled("soundsourcesradius")) this->optionFlags |= SoundSourcesRadius;
    if (this->GetFlagEnabled("invalidboxes")) this->optionFlags |= InvalidBoxes;
    if (this->GetFlagEnabled("loadareas"))    this->optionFlags |= LoadAreas;
    if (this->GetFlagEnabled("stats"))        this->optionFlags |= Statistics;
    if (this->GetFlagEnabled("selectinfo"))   this->optionFlags |= ShowSelectionInfo;

    this->boxesFlags = 0;
    if (this->GetFlagEnabled("boxescells"))     this->boxesFlags  |= BBoxesCells;
    if (this->GetFlagEnabled("boxesobjects"))   this->boxesFlags  |= BBoxesObjects;
    if (this->GetFlagEnabled("boxeslights"))    this->boxesFlags  |= BBoxesLights;
    if (this->GetFlagEnabled("lightregions"))   this->boxesFlags  |= BLightRegions;
    if (this->GetFlagEnabled("omniregions"))    this->boxesFlags  |= BOmniRegions;
    if (this->GetFlagEnabled("sphereregions"))  this->boxesFlags  |= BSphereRegions;
    if (this->GetFlagEnabled("frustumregions")) this->boxesFlags  |= BFrustumRegions;
    if (this->GetFlagEnabled("spherescells"))   this->boxesFlags  |= BSpheresCells;
    if (this->GetFlagEnabled("boxesportals"))   this->boxesFlags  |= PortalOBBoxes;
    if (this->GetFlagEnabled("portalverts"))    this->boxesFlags  |= PortalVertices;

    this->editorFlags = 0;
    if (this->GetFlagEnabled("normals"))      this->editorFlags |= ncEditor::DrawNormals;
    if (this->GetFlagEnabled("tangents"))     this->editorFlags |= ncEditor::DrawTangents;
    if (this->GetFlagEnabled("binormals"))    this->editorFlags |= ncEditor::DrawBinormals;

    this->physicsFlags = 0;
    if (this->GetFlagEnabled("phyaabb"))      this->physicsFlags |= nPhysicsServer::phyAABB;
    if (this->GetFlagEnabled("physhapes"))    this->physicsFlags |= nPhysicsServer::phyShapes;
    if (this->GetFlagEnabled("physpaces"))    this->physicsFlags |= nPhysicsServer::phySpaces;
    if (this->GetFlagEnabled("phyhm"))        this->physicsFlags |= nPhysicsServer::phyHeightMap;
    if (this->GetFlagEnabled("phyrays"))      this->physicsFlags |= nPhysicsServer::phyRays;
    if (this->GetFlagEnabled("phytm"))        this->physicsFlags |= nPhysicsServer::phyTriMesh;
    if (this->GetFlagEnabled("phyareas"))     this->physicsFlags |= nPhysicsServer::phyAreas;
    if (this->GetFlagEnabled("phyjoints"))    this->physicsFlags |= nPhysicsServer::phyJoints;
    if (this->GetFlagEnabled("phynormals"))   this->physicsFlags |= nPhysicsServer::phyNormals;

    this->aiFlags = 0;
    if (this->GetFlagEnabled("ainavoutdoor")) this->aiFlags |= AINavMeshOutdoor;
    if (this->GetFlagEnabled("ainavindoors")) this->aiFlags |= AINavMeshIndoors;
    if (this->GetFlagEnabled("ainavwire"))    this->aiFlags |= AINavMeshWireframe;
    if (this->GetFlagEnabled("ainavoverlay")) this->aiFlags |= AINavMeshOverlay;
    if (this->GetFlagEnabled("ainavlinks"))   this->aiFlags |= AINavMeshLinks;
    if (this->GetFlagEnabled("ainavoutdoorobstacles")) this->aiFlags |= AINavMeshOutdoorObstacles;
    if (this->GetFlagEnabled("aiculling"))    this->aiFlags |= AINearCulling;
    if (this->GetFlagEnabled("aisight"))      this->aiFlags |= AISight;
    if (this->GetFlagEnabled("aihearing"))    this->aiFlags |= AIHearing;
    if (this->GetFlagEnabled("aifeeling"))    this->aiFlags |= AIFeeling;
    if (this->GetFlagEnabled("aisightray"))   this->aiFlags |= AISightRay;
    if (this->GetFlagEnabled("aibehavior"))   this->aiFlags |= AIBehavior;
    if (this->GetFlagEnabled("aimemory"))     this->aiFlags |= AIMemory;
    if (this->GetFlagEnabled("aitarget"))     this->aiFlags |= AITarget;
    if (this->GetFlagEnabled("aitriggers"))   this->aiFlags |= AITriggerShapes;
    if (this->GetFlagEnabled("aispawners"))   this->aiFlags |= AISpawners;
    if (this->GetFlagEnabled("aimotion"))     this->aiFlags |= AIMotion;
    if (this->GetFlagEnabled("aiavoid"))      this->aiFlags |= AIObstaclesAvoid;
    if (this->GetFlagEnabled("aistaticavoid")) this->aiFlags |= AIStaticObstaclesAvoid;

    this->grassFlags = 0;
    if (this->GetFlagEnabled("grassnodraw"))  this->grassFlags |= GrassDrawDisable;
    if (this->GetFlagEnabled("grassInfo"))    this->grassFlags |= GrassDebugInfo;
    if (this->GetFlagEnabled("grassnomesh"))  this->grassFlags |= GrassMeshDisable;
    
    nEditorViewport* viewport = this->GetAppViewport();
    viewport->SetMoveLight(this->GetFlagEnabled("movelight"));
    viewport->SetAxesVisible(this->GetFlagEnabled("show3daxes"));
    viewport->SetObeyLightLinks(this->GetFlagEnabled("lightlinks"));
    viewport->SetObeyMaxDistance(this->GetFlagEnabled("maxdist"));
    viewport->SetPassEnabled(FOURCC('edit'), this->GetFlagEnabled("showedit"));
    viewport->SetPassEnabled(FOURCC('shdv'), this->GetFlagEnabled("shadows"));
    viewport->SetPassEnabled(FOURCC('shaq'), this->GetFlagEnabled("shadows"));
    viewport->SetStatsLevel(this->GetFlagEnabled("stats") ? 1 : 0);
    viewport->UpdateShaderOverrides();

    // enable collector array for specific debug options
    bool collectEntities = ((this->optionFlags & BoundingBoxes) &&
                            (this->boxesFlags & OptionsUsingEntityArrayMask) != 0) ||
                           (this->optionFlags & LoadAreas) ||
                           ((this->optionFlags & AI) &&
                            (this->aiFlags & (AINavMesh|AIEntities)));
    viewport->SetCollectEntities(collectEntities);

    // enable/disable nav mesh nodes from visibility
    if ( (this->optionFlags & AI) && (this->aiFlags & AINavMesh) )
    {
        if ( !nSpatialServer::Instance()->GetVisCategoriesMap()->Find( nSpatialTypes::CAT_NAVMESHNODES ) )
        {
            nSpatialServer::Instance()->AddVisCategory( nSpatialTypes::CAT_NAVMESHNODES );
        }
    }
    else
    {
        if ( nSpatialServer::Instance()->GetVisCategoriesMap()->Find( nSpatialTypes::CAT_NAVMESHNODES ) )
        {
            nSpatialServer::Instance()->RemoveVisCategory( nSpatialTypes::CAT_NAVMESHNODES );
        }
    }

    // enable/disable area triggers and spawners from visibility
    if ( (this->optionFlags & AI) && (this->aiFlags & (AITriggerShapes|AISpawners)) )
    {
        if ( !nSpatialServer::Instance()->GetVisCategoriesMap()->Find( nSpatialTypes::CAT_NON_VISIBLES ) )
        {
            nSpatialServer::Instance()->AddVisCategory( nSpatialTypes::CAT_NON_VISIBLES );
        }
    }
    else
    {
        if ( nSpatialServer::Instance()->GetVisCategoriesMap()->Find( nSpatialTypes::CAT_NON_VISIBLES ) )
        {
            nSpatialServer::Instance()->RemoveVisCategory( nSpatialTypes::CAT_NON_VISIBLES );
        }
    }

    // render mode
    nString viewportResource("wc:libs/system/cameras/stdcam.n2");

    if (this->GetFlagEnabled("mrt"))
    {
        viewportResource = "wc:libs/system/cameras/mrtcam.n2";
    }
    if (this->GetFlagEnabled("sepia"))
    {
        viewportResource = "wc:libs/system/cameras/sepiacam.n2";
    }
    if (this->GetFlagEnabled("hdr"))
    {
        viewportResource = "wc:libs/system/cameras/hdrcam.n2";
    }
    //...other renderers

    if (viewportResource != nString(viewport->GetResourceFile()))
    {
        viewport->SetResourceFile(viewportResource.Get());
        viewport->Refresh();
    }

    ncSpatialCamera* spatialCam = this->GetAppViewport()->GetSpatialCamera();
    spatialCam->SetDebuggingHorizon(this->GetFlagEnabled("horizon"));
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::Open()
{
    nGfxServer2 *gfxServer = nGfxServer2::Instance();

    // load resources for debug visualizations
    this->refWireframeShader = gfxServer->NewShader("wireframe");
    n_assert(this->refWireframeShader.isvalid());
    this->refWireframeShader->SetFilename("shaders:wireframe.fx");
    this->refWireframeShader->Load();
    
    this->wireframeShaderIndex = nSceneServer::Instance()->FindShader(refWireframeShader.get());
    if (this->wireframeShaderIndex == -1)
    {
        nSceneShader newSceneShader;
        newSceneShader.SetShaderName("wireframe");
        newSceneShader.SetShader("shaders:wireframe.fx");
        newSceneShader.SetShaderObject(refWireframeShader.get());
        this->wireframeShaderIndex = nSceneServer::Instance()->AddShader(newSceneShader);
        n_assert(this->wireframeShaderIndex != -1);
    }

    this->terrainPassFlags = 0;
    this->terrainPassFlags |= (1<<nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmm0')));
    this->terrainPassFlags |= (1<<nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('gmm1')));
    
    this->refFillShader = gfxServer->NewShader("pass_fillmode");
    n_assert(this->refFillShader.isvalid());
    this->refFillShader->SetFilename("shaders:pass_fillmode.fx");
    this->refFillShader->Load();

    this->refSpatialShapeShader = gfxServer->NewShader("shape_spatial");
    n_assert(this->refSpatialShapeShader.isvalid());
    this->refSpatialShapeShader->SetFilename("shaders:shape_spatial.fx");
    this->refSpatialShapeShader->Load();
    
    this->refNormalsShader = gfxServer->NewShader("shaders:line_normals.fx");
    n_assert(this->refNormalsShader.isvalid());
    if (!this->refNormalsShader->IsValid())
    {
        this->refNormalsShader->SetFilename("shaders:line_normals.fx");
        this->refNormalsShader->Load();
    }

    this->refBillboardShader = gfxServer->NewShader("shaders:sfx_edit_billboard.fx");
    n_assert(this->refBillboardShader.isvalid());
    this->refBillboardShader->SetFilename("shaders:sfx_edit_billboard.fx");
    this->refBillboardShader->Load();

    this->refShapeNoCullShader = nGfxServer2::Instance()->NewShader( "shape_no_cull" );
    n_assert( this->refShapeNoCullShader.isvalid() );
    if ( !this->refShapeNoCullShader->IsLoaded() )
    {
        this->refShapeNoCullShader->SetFilename( "shaders:shape_no_cull.fx" );
        this->refShapeNoCullShader->Load();
    }

    if ( nSceneServer::Instance()->FindShader( this->refShapeNoCullShader ) == -1 )
    {
        nSceneShader sceneShader;
        sceneShader.SetShader( "shaders:shape_no_cull.fx" );
        sceneShader.SetShaderName( "shape_no_cull" );
        sceneShader.SetShaderObject( this->refShapeNoCullShader );
        nSceneServer::Instance()->AddShader( sceneShader );
    }

    this->refShapeOverlayShader = nGfxServer2::Instance()->NewShader( "shape_overlay" );
    n_assert( this->refShapeOverlayShader.isvalid() );
    if ( !this->refShapeOverlayShader->IsLoaded() )
    {
        this->refShapeOverlayShader->SetFilename( "shaders:shape_overlay.fx" );
        this->refShapeOverlayShader->Load();
    }

    if ( nSceneServer::Instance()->FindShader( this->refShapeOverlayShader ) == -1 )
    {
        nSceneShader sceneShader;
        sceneShader.SetShader( "shaders:shape_overlay.fx" );
        sceneShader.SetShaderName( "shape_overlay" );
        sceneShader.SetShaderObject( this->refShapeOverlayShader );
        nSceneServer::Instance()->AddShader( sceneShader );
    }

    this->navOutdoorBoundaryLineHandler.SetShader( "shaders:line_navmesh.fx" );
    this->navIndoorBoundaryLineHandler.SetShader( "shaders:line_navmesh.fx" );

    this->refWhiteTexture = gfxServer->NewTexture("white");
    n_assert(this->refWhiteTexture.isvalid());
    this->refWhiteTexture->SetFilename("wc:libs/system/textures/white.dds");
    this->refWhiteTexture->Load();

    this->refBlackTexture = gfxServer->NewTexture("black");
    n_assert(this->refBlackTexture.isvalid());
    this->refBlackTexture->SetFilename("wc:libs/system/textures/black.dds");
    this->refBlackTexture->Load();
    
    this->refFlatTexture = gfxServer->NewTexture("nobump");
    n_assert(this->refFlatTexture.isvalid());
    this->refFlatTexture->SetFilename("wc:libs/system/textures/nobump.dds");
    this->refFlatTexture->Load();

    this->refAxisMesh = gfxServer->NewMesh("AxisX");
    n_assert(this->refAxisMesh.isvalid());
    this->refAxisMesh->SetFilename("wc:libs/system/meshes/AxisX.nvx2");
    this->refAxisMesh->Load();

    this->refBillboardMesh = gfxServer->NewMesh("Billboard");
    n_assert(this->refBillboardMesh.isvalid());
    this->refBillboardMesh->SetFilename("wc:libs/system/meshes/billboard.n3d2");
    this->refBillboardMesh->Load();

    this->refLightIconTexture = gfxServer->NewTexture("LightIcon");
    n_assert(this->refLightIconTexture.isvalid());
    this->refLightIconTexture->SetFilename("wc:libs/system/textures/lightbulb.tga");
    this->refLightIconTexture->Load();

    this->refDebugComponentServer = static_cast<nConjurerDebugCompServer*>(nDebugComponentServer::Instance());

    this->shadowSrcBlendVarHandle = nVariableServer::Instance()->GetVariableHandleByName("ShadowSrcBlend");
    this->shadowDstBlendVarHandle = nVariableServer::Instance()->GetVariableHandleByName("ShadowDstBlend");

    this->SetFlagEnabled("boxes", true);
    this->SetFlagEnabled("showedit", true);

    #ifdef __NEBULA_STATS__
    nSceneServer* sceneServer = nSceneServer::Instance();
    int passIndex;

    #define N_PASSNAME(fourcc, name) \
        passIndex = sceneServer->GetPassIndexByFourCC(FOURCC(fourcc)); \
        this->passNames[passIndex] = name;

    N_PASSNAME('dept', "- Depth");
    N_PASSNAME('inst', "- Instanced");
    N_PASSNAME('colr', "- Opaque");
    N_PASSNAME('alph', "- Alpha");
    N_PASSNAME('gmm0', "- Terrain Detail");
    N_PASSNAME('gmm1', "- Terrain Global");
    N_PASSNAME('sky0', "- Sky");
    N_PASSNAME('emis', "- Emissive");
    N_PASSNAME('shdv', "- Shadow volume");
    N_PASSNAME('sdep', "- Shadow depth");
    N_PASSNAME('lase', "- Laser receiver");
    N_PASSNAME('las2', "- Laser beam");
    N_PASSNAME('over', "- Alpha overlay");
    N_PASSNAME('edit', "- Icons");
    #undef N_PASSNAME
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::Close()
{
    this->refAxisMesh->Release();
    this->refFlatTexture->Release();
    this->refWhiteTexture->Release();
    this->refBlackTexture->Release();
    this->refNormalsShader->Release();
    this->refWireframeShader->Release();
    this->refSpatialShapeShader->Release();
    this->refFillShader->Release();
    this->refShapeNoCullShader->Release();
    this->refShapeOverlayShader->Release();
    this->refBillboardMesh->Release();
    this->refLightIconTexture->Release();
}
//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::ToggleOption(const char *name)
{
    n_assert(name);

    bool enable = !this->GetFlagEnabled(name);

    bool updated = this->SetFlagEnabled(name, enable);
    
    if (updated)
    {
        this->DisplayOptionToggled(name, enable);
        this->SignalDebugOptionChanged(this, name);
    }
}
//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::DisplayOptionToggled(const char *optionName, bool enabled)
{
    n_assert(optionName);

    const char* description = this->GetOptionDescription(optionName);

    nString updateString;

    if (enabled)
    {
        updateString = "Switched on debug option: ";
    }
    else
    {
        updateString = "Switched off debug option: ";
    }

    updateString.Append(description);
    NLOG(viewportDebugOptions, (NLOGUSER, updateString.Get()));
}
//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::HandleInput(nTime /*frameTime*/)
{
    nInputServer *inputServer = nInputServer::Instance();

    if (inputServer->GetButton("fillmode"))
    {
        this->ToggleOption("fillmode");
    }
    if (inputServer->GetButton("wireframe"))
    {
        this->ToggleOption("wireframe");
    }
    if (inputServer->GetButton("lightmap"))
    {
        this->ToggleOption("lightmap");
    }
    if (inputServer->GetButton("diffmap"))
    {
        this->ToggleOption("diffmap");
    }
    if (inputServer->GetButton("bumpmap"))
    {
        this->ToggleOption("bumpmap");
    }
    if (inputServer->GetButton("diffuse"))
    {
        this->ToggleOption("diffuse");
    }
    if (inputServer->GetButton("specular"))
    {
        this->ToggleOption("specular");
    }
    if (inputServer->GetButton("skeleton"))
    {
        this->ToggleOption("skeleton");
    }
    if (inputServer->GetButton("showaxes"))
    {
        this->ToggleOption("showaxes");
    }
    if (inputServer->GetButton("show3daxes"))
    {
        this->ToggleOption("show3daxes");
    }
    if (inputServer->GetButton("movelight"))
    {
        this->ToggleOption("movelight");
    }
    if (inputServer->GetButton("showedit"))
    {
        this->ToggleOption("showedit");
    }
    if (inputServer->GetButton("toggle_drawbboxes"))
    {
        this->ToggleOption("boxes");
    }
    if (inputServer->GetButton("toggle_useoctrees") && nConjurerApp::Instance()->GetTemporaryModeEnabled())
    {
        nDebugServer::Instance()->SetFlagEnabled("editor", "octrees", 
            !nDebugServer::Instance()->GetFlagEnabled("editor", "octrees"));
    }
    if (inputServer->GetButton("toggle_drawportals"))
    {
        this->ToggleOption("portals");
    }
    if (inputServer->GetButton("toggle_physics"))
    {
        this->ToggleOption("physics");
    }
    if (inputServer->GetButton("recalculate_campos"))
    {
        this->ToggleOption("reccampos");
    }
    if (inputServer->GetButton("toggle_fps"))
    {
        this->ToggleOption("fps");
    }
    if (inputServer->GetButton("hdr"))
    {
        this->ToggleOption("toggle_hdr");
    }
    if (inputServer->GetButton("toggle_wp"))
    {
        this->ToggleOption("wp");
    }
    if (inputServer->GetButton("toggle_obeylightlinks"))
    {
        this->ToggleOption("lightlinks");
    }
    if (inputServer->GetButton("toggle_stats"))
    {
        this->ToggleOption("stats");
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::BeginScene()
{
    this->refFillShader->SetInt(nShaderState::FillMode, this->fillMode);
    nGfxServer2::Instance()->SetShader(this->refFillShader.get());
    this->refFillShader->Begin(true);
    this->refFillShader->BeginPass(0);
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::EndScene()
{
    this->refFillShader->EndPass();
    this->refFillShader->End();
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::Overrides(ncScene *renderContext)
{
    nShaderParams& shaderOverrides = renderContext->GetShaderOverrides();
    // other shader overrides shouldn't be cleared - see ticket #474
    shaderOverrides.ClearArg(nShaderState::BumpMap0);
    shaderOverrides.ClearArg(nShaderState::bumpMap);
    shaderOverrides.ClearArg(nShaderState::BumpMap3);
    shaderOverrides.ClearArg(nShaderState::DiffMap0);
    shaderOverrides.ClearArg(nShaderState::diffMap);
    shaderOverrides.ClearArg(nShaderState::diffMap2);
    shaderOverrides.ClearArg(nShaderState::lightMap);
    shaderOverrides.ClearArg(nShaderState::matSpecular);
    shaderOverrides.ClearArg(nShaderState::MatSpecular);
    shaderOverrides.ClearArg(nShaderState::matDiffuse);
    shaderOverrides.ClearArg(nShaderState::MatDiffuse);
    shaderOverrides.ClearArg(nShaderState::parallaxFactor);
    
    if (this->optionFlags & Bumpmaps)
    {
        shaderOverrides.SetArg(nShaderState::BumpMap0, this->refFlatTexture.get());
        shaderOverrides.SetArg(nShaderState::bumpMap, this->refFlatTexture.get());
        shaderOverrides.SetArg(nShaderState::BumpMap3, this->refFlatTexture.get());
    }

    if (this->optionFlags & ParallaxMap)
    {
        renderContext->GetShaderOverrides().SetArg(nShaderState::parallaxFactor, 0.0f);
    }
    if (this->optionFlags & Diffmaps)
    {
        shaderOverrides.SetArg(nShaderState::DiffMap0, this->refWhiteTexture.get());
        shaderOverrides.SetArg(nShaderState::diffMap, this->refWhiteTexture.get());
        shaderOverrides.SetArg(nShaderState::diffMap2, this->refWhiteTexture.get());
    }
    if (this->optionFlags & Lightmaps)
    {
        shaderOverrides.SetArg(nShaderState::DiffMap1, this->refBlackTexture.get());
        shaderOverrides.SetArg(nShaderState::lightMap, this->refBlackTexture.get());
    }
 
    nFloat4 zero = {0.0f, 0.0f, 0.0f, 0.0f};
    if (this->optionFlags & Specular)
    {
        shaderOverrides.SetArg(nShaderState::MatSpecular, nShaderArg(zero));
        shaderOverrides.SetArg(nShaderState::matSpecular, nShaderArg(zero));
    }

    if (this->optionFlags & Diffuse)
    {
        shaderOverrides.SetArg(nShaderState::MatDiffuse, nShaderArg(zero));
        shaderOverrides.SetArg(nShaderState::matDiffuse, nShaderArg(zero));
    }

    ncDictionary* varContext = renderContext->GetComponent<ncDictionary>();
    nVariable* blendSrcVar = varContext->GetVariable(this->shadowSrcBlendVarHandle);
    nVariable* blendDstVar = varContext->GetVariable(this->shadowDstBlendVarHandle);
    if (blendSrcVar && blendDstVar)
    {
        blendSrcVar->SetInt(this->optionFlags & ShadowVolumes ? BlendSrcAlpha : BlendZero);
        blendDstVar->SetInt(this->optionFlags & ShadowVolumes ? BlendInvSrcAlpha : BlendOne);
    }

    shaderOverrides.SetArg(nShaderState::CullMode, nShaderState::NoCull);
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::Render(nSceneGraph* sceneGraph)
{
    if (nGfxServer2::Instance()->BeginScene())
    {
        if (this->optionFlags & BoundingBoxes)
        {
            this->DrawBoundingBoxes();

            if (this->boxesFlags & PortalOBBoxes)
            {
                this->DrawPortalOrientedBoundingBoxes();
            }
        }

        if (this->optionFlags & LevelBounds)
        {
            this->DrawLevelBounds();
        }

        if (this->optionFlags & ShadowSpModels)
        {
            this->DrawSpatialShadowModels();
        }

        if (this->optionFlags & WrapperMeshes)
        {
            this->DrawWrapperMeshes();
        }

        if (this->optionFlags & Portals)
        {
            this->DrawPortals((this->boxesFlags & PortalVertices) != 0);
        }

        if (this->optionFlags & Occluders)
        {
            this->DrawOccluders();
        }

        if (this->optionFlags & Horizon)
        {
            this->DrawHorizon();
        }

        if (this->optionFlags & Wireframe)
        {
            this->RenderWireframeOverlay(sceneGraph);
        }

        if (this->optionFlags & Skeleton)
        {
            this->RenderSkeleton(sceneGraph);
        }

        if (this->editorFlags)
        {
            this->RenderNormals(sceneGraph);
        }

        if (this->optionFlags & InvalidBoxes)
        {
            this->RenderInvalidBoxes(sceneGraph);
        }

        if (this->optionFlags & LoadAreas)
        {
            this->RenderLoadAreas();
        }

        if (this->optionFlags & ShowAxes)
        {
            this->DrawAxes();
        }

        if (this->optionFlags & Physics)
        {
            nPhysicsServer::Instance()->SetDraw(this->physicsFlags);
            nPhysicsServer::Instance()->Draw(nGfxServer2::Instance());
        }

        if (this->optionFlags & WayPoints)
        {
            nWayPointServer::Instance()->Draw();
        }

        if (this->optionFlags & AI)
        {
            this->DrawAI();
        }

        if (this->optionFlags & FPS)
        {
            this->RenderFPS();
        }

        if (this->optionFlags & Statistics)
        {
            this->RenderStats();
        }

        if (this->optionFlags & DebugGraphics)
        {
            nDebugComponentServer::Instance()->Draw( nGfxServer2::Instance() );
            nDebugGraphicsServer::Instance()->Draw( nGfxServer2::Instance() );
        }

        //Update the node draw
        ncTerrainVegetationClass::debugDisableDrawGrass = 0 != (this->grassFlags & GrassDrawDisable);
        ncTerrainVegetationClass::debugInfo             = 0 != (this->grassFlags & GrassDebugInfo) ;
        ncTerrainVegetationClass::debugDisableDrawMesh  = 0 != (this->grassFlags & GrassMeshDisable);
        
        if (this->optionFlags & SoundSources)
        {
            this->refDebugComponentServer->DrawSoundSources( (this->optionFlags & SoundSourcesRadius) == 0);
        }

        // Summoner drawing: tools, grid, etc.
        n_assert( this->GetAppViewport() );
        nString stateName = nConjurerApp::Instance()->GetCurrentState();
        if ( stateName == "terrain" )
        {
            nTerrainEditorState* terrState = static_cast<nTerrainEditorState*>( nConjurerApp::Instance()->FindState( stateName ) );

            // Erase z-buffer for drawing overlaid editor elements
            nGfxServer2::Instance()->Clear( nGfxServer2::DepthBuffer, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0);

            terrState->DrawDebug( this->GetAppViewport() );
        }
        else if ( stateName == "object" )
        {
            nObjectEditorState* objState = static_cast<nObjectEditorState*>( nConjurerApp::Instance()->FindState( stateName ) );
        
            objState->DrawGrid( this->GetAppViewport() );

            // Erase z-buffer for drawing overlaid editor elements
            nGfxServer2::Instance()->Clear( nGfxServer2::DepthBuffer, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0);
            
            objState->SetShowInfoOnSelection( (this->optionFlags & ShowSelectionInfo) != 0 );
            
            objState->DrawDebug( this->GetAppViewport() );
        }

        // Draw viewport camera velocity if needed
        if ( timeToDrawCameraVelocity > nApplication::Instance()->GetTime() )
        {
            nString velStr("Camera velocity: ");
            char velChar[24];
            sprintf(velChar,"%20.2f", this->GetAppViewport()->GetViewerVelocity() );
            velStr.Append( velChar );
            velStr.Append(" m/s");
            nGfxServer2::Instance()->Text( velStr.Get(), vector4(0.0f,0.5f,1.0f,1.0f), 0.5,-0.5);
        }


        nGfxServer2::Instance()->EndScene();
    }
}

//------------------------------------------------------------------------------
/**
    draw the bounding boxes of the visible elements.
*/
void 
nViewportDebugModule::DrawBoundingBoxes()
{
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    
    gfxServer->PushTransform(nGfxServer2::Model, matrix44());
    gfxServer->BeginShapes(this->refSpatialShapeShader.get());

    // cells and objects
    ncSpatialCamera* spatialCam = this->GetAppViewport()->GetSpatialCamera();
    const nArray<nRef<nEntityObject> >& visibleEntities = spatialCam->GetVisibleEntities();
    nEntityObject* visEntity = 0;
    for (int i = 0; i < visibleEntities.Size(); ++i)
    {
        if (!visibleEntities[i].isvalid())
        {
            continue;
        }
        visEntity = visibleEntities[i];
        bbox3 box;
        ncSpatial *spatialComponent = visEntity->GetComponent<ncSpatial>();
        if (spatialComponent)
        {
            if (this->boxesFlags & BBoxesObjects)
            {
                box = spatialComponent->GetBBox();
            }
            else
            {
                continue;
            }
        }
        else
        {
            ncSpatialQuadtreeCell *spatialCellComp = visEntity->GetComponent<ncSpatialQuadtreeCell>();
            if (spatialCellComp)
            {
                if (this->optionFlags & HorizSegments)
                {
                    ncCellHorizon *hc = spatialCellComp->GetComponentSafe<ncCellHorizon>();
                    hc->DrawHorizonSegments(gfxServer);
                }

                if (this->boxesFlags & BBoxesCells)
                {
                    box = spatialCellComp->GetBBox();
                }
                else if (this->boxesFlags & BSpheresCells)
                {
                    const sphere &s = spatialCellComp->GetBoundingSphere();
                    matrix44 m;
                    m.scale(vector3(s.r, s.r, s.r));
                    m.set_translation(s.p);
                    int indexColor = int(reinterpret_cast<size_t>(visEntity));
                    indexColor = (indexColor * 3 / 8) % numColors;
                    gfxServer->DrawShape(nGfxServer2::Sphere, m, color[indexColor]);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        const vector3 nodeCenter(box.center());
        matrix44 m;
        m.scale(box.vmax - box.vmin);
        m.set_translation(nodeCenter);

        int indexColor = int(reinterpret_cast<size_t>(visEntity));
        indexColor = (indexColor * 3 / 8) % numColors;
        gfxServer->DrawShape(nGfxServer2::Box, m, color[indexColor]);
    }

    gfxServer->EndShapes();
    gfxServer->PopTransform(nGfxServer2::Model);

    // lights
    if (this->boxesFlags & BBoxesLights)
    {
        gfxServer->PushTransform(nGfxServer2::Model, matrix44());
        this->DrawLights();
        gfxServer->PopTransform(nGfxServer2::Model);
    }

    gfxServer->PushTransform(nGfxServer2::Model, matrix44());
    
    if (this->boxesFlags & BLightRegions)
    {
        this->DrawLightRegions();
    }

    gfxServer->PopTransform(nGfxServer2::Model);
}

//------------------------------------------------------------------------------
/**
    draw the light regions
*/
void
nViewportDebugModule::DrawLightRegions()
{
    ncSpatialCamera* cam = this->GetAppViewport()->GetSpatialCamera();
    const nArray<nSpatialIndoorCell*> &indoorCells = cam->GetVisibleIndoorCells();

    nLightRegion *currentRegion;
    ncSpatialLight *light;
    for ( int i(0); i < indoorCells.Size(); ++i )
    {
        const nArray<nLightRegion*> &regions = indoorCells[i]->GetLightRegions();
        for ( int j(0); j < regions.Size(); ++j )
        {
            currentRegion = regions[j];
            light = currentRegion->GetParentLight();
            n_assert(light);
            if ( (!light->GetTestModel() && (this->boxesFlags & BOmniRegions)) ||
                    (light->GetTestModel() && light->GetTestModel()->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE && (this->boxesFlags & BSphereRegions)) ||
                    (light->GetTestModel() && light->GetTestModel()->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM && (this->boxesFlags & BFrustumRegions))
                )
            {
                currentRegion->Draw();
            }
        }
    }

    const nArray<ncSpatialQuadtreeCell*> &quadCells = cam->GetVisibleQuadtreeCells();
    for ( int i(0); i < quadCells.Size(); ++i )
    {
        const nArray<nLightRegion*> &regions = quadCells[i]->GetLightRegions();
        for ( int j(0); j < regions.Size(); ++j )
        {
            currentRegion = regions[j];
            light = currentRegion->GetParentLight();
            n_assert(light);
            if ( (!light->GetTestModel() && (this->boxesFlags & BOmniRegions)) ||
                (light->GetTestModel() && light->GetTestModel()->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE && (this->boxesFlags & BSphereRegions)) ||
                (light->GetTestModel() && light->GetTestModel()->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM && (this->boxesFlags & BFrustumRegions))
                )
            {
                currentRegion->Draw();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    draw the lights
*/
void
nViewportDebugModule::DrawLights()
{
    ncSpatialCamera* cam = this->GetAppViewport()->GetSpatialCamera();
    const nArray<nSpatialIndoorCell*> &indoorCells = cam->GetVisibleIndoorCells();

    for ( int i(0); i < indoorCells.Size(); ++i )
    {
        const nArray<nEntityObject*> &lights = indoorCells[i]->GetCategory(nSpatialTypes::CAT_LIGHTS);
        for ( int j(0); j < lights.Size(); ++j )
        {
            lights[j]->GetComponentSafe<ncSpatialLight>()->DrawROI( this->refSpatialShapeShader );
        }
    }

    const nArray<ncSpatialQuadtreeCell*> &quadCells = cam->GetVisibleQuadtreeCells();
    for ( int i(0); i < quadCells.Size(); ++i )
    {
        const nArray<nEntityObject*> &lights = quadCells[i]->GetCategory(nSpatialTypes::CAT_LIGHTS);
        for ( int j(0); j < lights.Size(); ++j )
        {
            lights[j]->GetComponentSafe<ncSpatialLight>()->DrawROI(this->refSpatialShapeShader);
        }
    }

    this->refBillboardShader->Begin(true);
    this->refBillboardShader->BeginPass(0);
    this->refBillboardShader->SetTexture(nShaderState::diffMap, this->refLightIconTexture.get());
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    gfxServer->SetShader(this->refBillboardShader);
    gfxServer->SetMesh(this->refBillboardMesh.get(), this->refBillboardMesh.get());
    gfxServer->SetVertexRange(0, this->refBillboardMesh->GetNumVertices());
    gfxServer->SetIndexRange(0, this->refBillboardMesh->GetNumIndices());

    for ( int i(0); i < indoorCells.Size(); ++i )
    {
        const nArray<nEntityObject*> &lights = indoorCells[i]->GetCategory(nSpatialTypes::CAT_LIGHTS);
        for ( int j(0); j < lights.Size(); ++j )
        {
            const matrix44 &model = lights[j]->GetComponentSafe<ncTransform>()->GetTransform();
            gfxServer->SetTransform(nGfxServer2::Model, model);
            gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
        }
    }

    for ( int i(0); i < quadCells.Size(); ++i )
    {
        const nArray<nEntityObject*> &lights = quadCells[i]->GetCategory(nSpatialTypes::CAT_LIGHTS);
        for ( int j(0); j < lights.Size(); ++j )
        {
            const matrix44 &model = lights[j]->GetComponentSafe<ncTransform>()->GetTransform();
            gfxServer->SetTransform(nGfxServer2::Model, model);
            gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
        }
    }

    this->refBillboardShader->EndPass();
    this->refBillboardShader->End();
}

//------------------------------------------------------------------------------
/**
    draw level bounds
*/
void 
nViewportDebugModule::DrawLevelBounds()
{
    ncSpatialQuadtree *quadtree = nSpatialServer::Instance()->GetQuadtreeSpace();
    if (!quadtree)
    {
        return;
    }

    const bbox3 &levelBox = quadtree->GetBBox();
    
    vector3 pointsList[5];
    pointsList[0] = vector3(levelBox.vmin.x, 0.5f, levelBox.vmin.z);
    pointsList[1] = vector3(levelBox.vmin.x, 0.5f, levelBox.vmax.z);
    pointsList[2] = vector3(levelBox.vmax.x, 0.5f, levelBox.vmax.z);
    pointsList[3] = vector3(levelBox.vmax.x, 0.5f, levelBox.vmin.z);
    pointsList[4] = pointsList[0];

    bool zTestEnabled(nGfxServer2::Instance()->GetBoolRenderOption(nGfxServer2::ZTestEnable));
    nGfxServer2::Instance()->SetBoolRenderOption(nGfxServer2::ZTestEnable, false);
    float prevLineWidth(nGfxServer2::Instance()->GetFloatRenderOption(nGfxServer2::LineWidth));
    nGfxServer2::Instance()->SetFloatRenderOption(nGfxServer2::LineWidth, 0.5f);
    matrix44 idMatrix;
    this->spatialLineHandler.BeginLines(idMatrix);
    this->spatialLineHandler.DrawLines3d(pointsList, 0, 5, vector4(0.1f, 1.0f, 0.2f, 1.0f));
    this->spatialLineHandler.EndLines();
    nGfxServer2::Instance()->SetBoolRenderOption(nGfxServer2::ZTestEnable, zTestEnabled);
    nGfxServer2::Instance()->SetFloatRenderOption(nGfxServer2::LineWidth, prevLineWidth);
}

//------------------------------------------------------------------------------
/**
    Draw the wrapper meshes for indoor cells.
*/
void
nViewportDebugModule::DrawWrapperMeshes()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->PushTransform(nGfxServer2::Model, matrix44());

    // use same shader as in shapes
    nShader2 *shader = this->refSpatialShapeShader.get();
    n_verify(shader->Begin(true) == 1);
    gfxServer->SetShader(shader);
    shader->BeginPass(0);

    // render all wrapper meshes from the current camera
    ncSpatialCamera *spatialCamera = this->appViewport->GetSpatialCamera();
    const nArray<nSpatialIndoorCell*>& indoorCells = spatialCamera->GetVisibleIndoorCells();
    for (int i = 0; i < indoorCells.Size(); i++)
    {
        nSpatialIndoorCell* indoorCell = indoorCells[i];
        int indexColor = int(reinterpret_cast<size_t>(indoorCell));
        indexColor = (indexColor * 3 / 8) % numColors;
        shader->SetVector4(nShaderState::MatDiffuse, color[indexColor]);

        nWrapperMesh& wrapperMesh = indoorCells[i]->GetWrapperMesh();
        wrapperMesh.RenderDebug();
    }

    shader->EndPass();
    shader->End();

    gfxServer->PopTransform(nGfxServer2::Model);
}

//------------------------------------------------------------------------------
/**
    draw spatial shadow models
*/
void 
nViewportDebugModule::DrawSpatialShadowModels()
{
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    
    gfxServer->PushTransform(nGfxServer2::Model, matrix44());
    gfxServer->BeginShapes(this->refSpatialShapeShader.get());

    // cells and objects
    ncSpatial* spComp = 0;
    nSpatialShadowModel* shadowModel = 0;
    const nArray<nSpatialShadowModel*> *shadowModels = 0;
    ncSpatialCamera* spatialCam = this->GetAppViewport()->GetSpatialCamera();
    const nArray<nRef<nEntityObject> >& visibleEntities = spatialCam->GetVisibleEntities();
    int i;
    for ( i = 0; i < visibleEntities.Size(); i++ )
    {
        if(!visibleEntities[i].isvalid())
        {
            continue;
        }
        spComp = visibleEntities[i]->GetComponent<ncSpatial>();
        if ( spComp && spComp->GetIsShadowCaster() )
        {
            shadowModels = spComp->GetShadowModels();
            if ( shadowModels )
            {
                for ( int i(0); i < shadowModels->Size(); ++i )
                {
                    shadowModel = (*shadowModels)[i];
                    n_assert(shadowModel);
                    const cylinder& cyl = shadowModel->GetCylinder();
                    //float length(spComp->GetClassComponentSafe<ncSpatialClass>()->GetShadowLength());

                    vector3 dir( cyl.p2 - cyl.p1 );
                    dir.norm();
                    matrix44 m;
                    ///@todo miquelangel.rujula fix rendering of shadow models
                    //m.ident();
                    //m.scale( vector3(cyl.r,
                    //                 cyl.r,
                    //                 length) );
                    //m.translate( vector3(0, 0, length/2.f) );
                    ////m.rotate_x( atan(dir.y / dir.z) );
                    ////m.rotate_y( atan(dir.z / dir.x) );
                    ////m.rotate_z( atan(dir.y / dir.x) );
                    ////m.rotate_y( HALFPI );
                    //quaternion quat(dir.x, dir.y, dir.z, 0.f);
                    //m *= matrix44(quat);

                    ////m.translate( vector3(0, -length/2.f, 0) );
                    //m.translate( cyl.p1 );
                    //nGfxServer2::Instance()->DrawShape( nGfxServer2::Cylinder, m, vector4(0.0f, 0.4f, 0.8f, 0.8f) );
                    
                    // draw p1
                    m.ident();
                    m.scale( vector3( cyl.r, cyl.r, cyl.r ) );
                    m.translate( cyl.p1 );
                    nGfxServer2::Instance()->DrawShape( nGfxServer2::Sphere, m, vector4(1.0f, 0.0f, 0.0f, 0.8f) );

                    // draw p2
                    m.ident();
                    m.scale( vector3( cyl.r, cyl.r, cyl.r ) );
                    m.translate( cyl.p2 );
                    nGfxServer2::Instance()->DrawShape( nGfxServer2::Sphere, m, vector4(0.0f, 0.0f, 1.0f, 0.8f) );
                }
            }
        }
    }

    gfxServer->EndShapes();
    gfxServer->PopTransform(nGfxServer2::Model);
}

//------------------------------------------------------------------------------
/**
    draw portal's bounding rectangle for debug.
*/
void 
nViewportDebugModule::DrawPortals(bool drawVertices)
{
    ncSpatialCamera *spatialCamera = this->appViewport->GetSpatialCamera();
    matrix44 idMatrix;
    const nArray<ncSpatialPortal*> &portals = spatialCamera->GetVisiblePortals();
    for (nArray<ncSpatialPortal*>::iterator pPortal  = portals.Begin();
                                            pPortal != portals.End();
                                            pPortal++)
    {
        // draw portal's clip rectangle
        vector3 clipRectCenter;
        const vector3 *rectPoints = (*pPortal)->GetVertices();
        vector3 pointsList[5];
        for (int i = 0; i < 4; i++)
        {
            pointsList[i] = rectPoints[i];
            clipRectCenter += rectPoints[i];
        }

        pointsList[4] = rectPoints[0];

        // draw portal polygon
        this->spatialLineHandler.BeginLines(idMatrix);
        this->spatialLineHandler.DrawLines3d(pointsList, 0, 5, vector4(0.5f, 1.0f, 0.4f, 1.0f));
        this->spatialLineHandler.EndLines();

        if (drawVertices)
        {
            // compute the normal of the portal plane
            plane portalPlane = (*pPortal)->GetPlane();
            clipRectCenter *= 0.25;
            pointsList[0] = clipRectCenter;
            const vector3 pl = portalPlane.normal();
            pointsList[1] = pointsList[0] + pl;

            // draw portal normal using an arrow shape
            quaternion q;
            q.set_from_axes2(vector3(0.0f, 0.0f, 1.0f), pl);
            matrix44 arrowMatrix(q);
            arrowMatrix.translate(clipRectCenter);

            nShapeServer* shapeServer = nShapeServer::Instance();
            shapeServer->BeginShapes();
            shapeServer->DrawShape(this->refAxisMesh.get(), arrowMatrix, vector4(0.0f, 0.0f, 1.0f, 1.0f));
            shapeServer->EndShapes();

            // draw portal vertices
            vector4 sphereColour;
            for (int i = 0; i < 4; i++)
            {
                matrix44 sphereMatrix;
                sphereMatrix.scale(vector3(0.04f, 0.04f, 0.04f));
                sphereMatrix.translate(rectPoints[i]);

                switch (i)
                {
                    case 0: 
                        sphereColour.set(1.0f, 0.0f, 0.0f, 0.9f);
                        break;
                    case 1: 
                        sphereColour.set(0.0f, 1.0f, 0.0f, 0.9f);
                        break;
                    case 2: 
                        sphereColour.set(0.0f, 0.0f, 1.0f, 0.9f);
                        break;
                    case 3: 
                        sphereColour.set(1.0f, 1.0f, 1.0f, 0.9f);
                        break;
                    default:
                        break;
                }

                shapeServer->BeginShapes();
                shapeServer->DrawShape(nGfxServer2::Sphere, sphereMatrix, sphereColour);
                shapeServer->EndShapes();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draw portals oriented bounding boxes
*/
void
nViewportDebugModule::DrawPortalOrientedBoundingBoxes()
{
    ncSpatialCamera *spatialCamera = this->appViewport->GetSpatialCamera();
    const nArray<ncSpatialPortal*> &portals = spatialCamera->GetVisiblePortals();
    for (nArray<ncSpatialPortal*>::iterator pPortal  = portals.Begin();
                                            pPortal != portals.End();
                                            pPortal++)
    {
        obbox3& box = (*pPortal)->GetOBBox();

        nGfxServer2::Instance()->BeginShapes();
        matrix44 boxMatrix;
        boxMatrix.scale(box.GetSize());
        boxMatrix *= box.GetTransform().getmatrix();
        
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Box,
                                           boxMatrix, 
                                           vector4(0, 0, 1, 0.7f));
        nGfxServer2::Instance()->EndShapes();
    }
}

//------------------------------------------------------------------------------
/**
    Draw occluders
*/
void 
nViewportDebugModule::DrawOccluders()
{
    if (nSpatialServer::Instance()->GetUseOctrees())
    {
        // octree visibility is executing, so don't draw the portals
        return;
    }

    ncSpatialCamera *spatialCamera = this->appViewport->GetSpatialCamera();
    const matrix44 &viewProjection = spatialCamera->GetViewProjection();
    matrix44 idMatrix;

    // draw selected occluders
    const nArray<ncSpatialCamera::SelectedOccluder*> &occluders = spatialCamera->GetSelectedOccluders();
    vector4 selectedColor(0.5f, 0.4f, 1.f, 1.f);
    for (int i = 0; i < occluders.Size(); i++)
    {
        const vector3 *rectPoints = occluders[i]->oc->GetVertices();
        vector3 pointsList[5];
        for (int j = 0; j < 4; j++)
        {
            pointsList[j] = rectPoints[j];
        }

        pointsList[4] = rectPoints[0];

        // draw occluder's polygon
        this->spatialLineHandler.BeginLines(idMatrix);
        this->spatialLineHandler.DrawLines3d(pointsList, 0, 5, selectedColor);
        this->spatialLineHandler.EndLines();

        if (this->optionFlags & OccludersMValue)
        {
            // draw 'm' on the occluder's center
            vector4 v0 = occluders[i]->oc->GetCenter();
            v0.w = 1.f;
            vector4 v = viewProjection * v0;
            if ( (v.x < -v.w) || (v.x > v.w) || (v.y < -v.w) || (v.y > v.w) || v.w <= 0)
            {
                continue;
            }

            nString str("m = ");
            str.AppendFloat(occluders[i]->m);
            vector2 pos;
            pos.x =  v.x / v.w;
            pos.y = -v.y / v.w;
            pos.y -= nGfxServer2::Instance()->GetTextExtent( str.Get() ).y * 2;
            nGfxServer2::Instance()->Text(str.Get(), selectedColor, pos.x, pos.y);
        }
    }

#ifndef NGAME
    // draw discarded occluders
    const nArray<ncSpatialCamera::SelectedOccluder*> &discOccluders = spatialCamera->GetDiscardedOccluders();
    vector4 discColor(1.0f, 0.4f, 0.5f, 1.f);
    for (int i = 0; i < discOccluders.Size(); i++)
    {
        const vector3 *rectPoints = discOccluders[i]->oc->GetVertices();
        vector3 pointsList[5];
        for (int j = 0; j < 4; j++)
        {
            pointsList[j] = rectPoints[j];
        }

        pointsList[4] = rectPoints[0];

        // draw occluder's polygon
        this->spatialLineHandler.BeginLines(idMatrix);
        this->spatialLineHandler.DrawLines3d(pointsList, 0, 5, discColor);
        this->spatialLineHandler.EndLines();

        if (this->optionFlags & OccludersMValue)
        {
            // draw 'm' on the occluder's center
            vector4 v0 = discOccluders[i]->oc->GetCenter();
            v0.w = 1.f;
            vector4 v = viewProjection * v0;
            if ( (v.x < -v.w) || (v.x > v.w) || (v.y < -v.w) || (v.y > v.w) || v.w <= 0)
            {
                continue;
            }

            nString str("m = ");
            str.AppendFloat(discOccluders[i]->m);
            vector2 pos;
            pos.x =  v.x / v.w;
            pos.y = -v.y / v.w;
            pos.y -= nGfxServer2::Instance()->GetTextExtent( str.Get() ).y * 2;
            nGfxServer2::Instance()->Text(str.Get(), discColor, pos.x, pos.y);
        }
    }
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    Draw horizon
*/
void 
nViewportDebugModule::DrawHorizon()
{
#ifndef NGAME
    ncSpatialCamera *spatialCamera = this->appViewport->GetSpatialCamera();
    spatialCamera->GetHorizon().Draw(nGfxServer2::Instance());
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    Draw AI
*/
void 
nViewportDebugModule::DrawAI()
{
    if ( this->aiFlags & AINavMesh )
    {
        this->DrawNavigationMeshes();
    }
    if ( this->aiFlags & AINavMeshOutdoorObstacles )
    {
        this->DrawNavigationOutdoorObstacles();
    }
    if ( this->aiFlags & AIEntities )
    {
        this->DrawAIEntities();
    }
}

//------------------------------------------------------------------------------
/**
    Draw navigation meshes
*/
void 
nViewportDebugModule::DrawNavigationMeshes()
{
#ifdef __NEBULA_STATS__
    GetProfNavMesh().StartAccum();
#endif

    matrix44 m;
    m.ident();
    nShaderArg zTestEnable( this->aiFlags & AINavMeshOverlay ? 0 : 1 );
    nShaderArg zWriteEnable( this->aiFlags & AINavMeshOverlay ? 0 : 1 );

    // Outdoor
    if ( this->aiFlags & AINavMeshOutdoor )
    {
        this->navOutdoorBoundaryLineHandler.GetShaderParams().SetArg( nShaderState::ZEnable, zTestEnable );
        this->navOutdoorBoundaryLineHandler.GetShaderParams().SetArg( nShaderState::ZWriteEnable, zWriteEnable );
        this->navOutdoorBoundaryLineHandler.BeginLines( m );

        ncSpatialCamera* camera = this->GetAppViewport()->GetSpatialCamera();
        const nArray<ncNavMeshNode*>& meshNodes = camera->GetCollector()->GetVisibleNavMeshNodesOutdoor();
        for ( int i(0); i < meshNodes.Size(); ++i )
        {
            ncNavMeshNode* meshNode = meshNodes[i];
            if ( meshNode )
            {
                meshNode->SetOutdoorBoundaryColour( this->GetColourSettingValue("ainavcolourboundout") );
                meshNode->Draw( &this->navOutdoorBoundaryLineHandler,
                                (this->aiFlags & AINavMeshWireframe) != 0,
                                (this->aiFlags & AINavMeshOverlay) != 0,
                                (this->aiFlags & AINavMeshLinks) != 0 );
            }
        }

        this->navOutdoorBoundaryLineHandler.EndLines();
    }

    // Indoors
    if ( this->aiFlags & AINavMeshIndoors )
    {
        this->navIndoorBoundaryLineHandler.GetShaderParams().SetArg( nShaderState::ZEnable, zTestEnable );
        this->navIndoorBoundaryLineHandler.GetShaderParams().SetArg( nShaderState::ZWriteEnable, zWriteEnable );
        this->navIndoorBoundaryLineHandler.BeginLines( m );

        ncSpatialCamera* camera = this->GetAppViewport()->GetSpatialCamera();
        const nArray<ncNavMeshNode*>& meshNodes = camera->GetCollector()->GetVisibleNavMeshNodesIndoor();
        for ( int i(0); i < meshNodes.Size(); ++i )
        {
            ncNavMeshNode* meshNode = meshNodes[i];
            if ( meshNode )
            {
                meshNode->SetIndoorBoundaryColour( this->GetColourSettingValue("ainavcolourboundin") );
                meshNode->Draw( &this->navIndoorBoundaryLineHandler,
                                (this->aiFlags & AINavMeshWireframe) != 0,
                                (this->aiFlags & AINavMeshOverlay) != 0,
                                (this->aiFlags & AINavMeshLinks) != 0 );
            }
        }

        this->navIndoorBoundaryLineHandler.EndLines();
    }

#ifdef __NEBULA_STATS__
    GetProfNavMesh().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw obstacles found while generating the navigation mesh for the outdoor
*/
void 
nViewportDebugModule::DrawNavigationOutdoorObstacles()
{
#ifdef __NEBULA_STATS__
    GetProfNavOutdoorObstacles().StartAccum();
#endif

    nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    if ( outdoor )
    {
        nNavMesh* navMesh = outdoor->GetComponentSafe<ncNavMesh>()->GetNavMesh();
        if ( navMesh && navMesh->IsValid() )
        {
            navMesh->DrawObstacles( nGfxServer2::Instance(), 0 );
        }
    }

#ifdef __NEBULA_STATS__
    GetProfNavOutdoorObstacles().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw AI entities debug info
*/
void 
nViewportDebugModule::DrawAIEntities()
{
#ifdef __NEBULA_STATS__
    GetProfAIEntities().StartAccum();
#endif

    ncSpatialCamera* camera = this->GetAppViewport()->GetSpatialCamera();
    const nArray< nRef<nEntityObject> >& entities = camera->GetCollector()->GetVisibleEntities();
    for ( int i(0); i < entities.Size(); ++i )
    {
        nEntityObject* entity = entities[i];
        if ( this->aiFlags & AIBehavior )
        {
            this->DrawAgentFSMStack( entity );
        }
        if ( this->aiFlags & (AISight|AISightRay) )
        {
            this->DrawAgentSight( entity );
        }
        if ( this->aiFlags & AIHearing )
        {
            this->DrawAgentHearing( entity );
        }
        if ( this->aiFlags & AIFeeling )
        {
            this->DrawAgentFeeling( entity );
        }
        if ( this->aiFlags & AINearCulling )
        {
            this->DrawPerceptionCulling( entity );
        }
        if ( this->aiFlags & (AIMemory|AITarget) )
        {
            this->DrawAgentMemory( entity );
        }
        if ( this->aiFlags & (AIMotion|AIObstaclesAvoid|AIStaticObstaclesAvoid) )
        {
            this->DrawAgentMotion( entity );
        }
        if ( this->aiFlags & AITriggerShapes )
        {
            this->DrawAreaTrigger( entity );
        }
        if ( this->aiFlags & AISpawners )
        {
            this->DrawSpawner( entity );
        }
    }

#ifdef __NEBULA_STATS__
    GetProfAIEntities().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent FSM stack
*/
void 
nViewportDebugModule::DrawAgentFSMStack( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfFSMStack().StartAccum();
#endif

    ncFSM* fsm = entity->GetComponent<ncFSM>();
    if ( fsm )
    {
        // Display state stack above the agent
        ncTransform* transform = entity->GetComponentSafe<ncTransform>();
        ncPhysicsObj* physics = entity->GetComponentSafe<ncPhysicsObj>();

        nPhysicsAABB bb;
        physics->GetAABB(bb);
        vector3 pos = transform->GetPosition();
        pos.y = bb.maxy;

        n_assert(this->appViewport);
        matrix44 matV = this->appViewport->GetViewMatrix();
        matV.invert();
        matV *= nCamera2(this->appViewport->GetCamera()).GetProjection();
        vector4 pos4;
        pos4 = pos;
        pos4 = matV * pos4;

        if(pos4.w > 0)
        {
            pos4.x = pos4.x / pos4.w;
            pos4.y = -pos4.y / pos4.w;

            nString text;
            fsm->Draw(vector4(1.0f,0.5f,0.5f,0.5f), vector2(pos4.x,pos4.y));
        }
    }

#ifdef __NEBULA_STATS__
    GetProfFSMStack().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent sight field
*/
void 
nViewportDebugModule::DrawAgentSight( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfSight().StartAccum();
#endif

    ncGPSight* sight = entity->GetComponent<ncGPSight>();
    if (sight)
    {
        if ( this->aiFlags & AISight )
        {
            // Draw sight field as a conus
            const cone* c = sight->GetSightCone();
            matrix44 m, m2;
            sight->GetSightTransform(m2);
            m.ident();
            m.scale(vector3(c->getRadius()*2, c->getRadius()*2, sight->GetSightRadius()));
            m *= m2;
            nShapeServer::Instance()->BeginShapes();
            nShapeServer::Instance()->DrawShape(nGfxServer2::Cone, m, vector4(0.1f, 0.7f, 0.1f, 0.2f));
            nShapeServer::Instance()->EndShapes();
        }

        if ( this->aiFlags & AISightRay )
        {
            // Draw line used to check visibility of an entity
            const line3 & sightRay( sight->GetSightRay() );

            matrix44 m0;
            m0.ident();
            nGfxServer2::Instance()->SetTransform( nGfxServer2::Model, m0 );

            vector4 color( 0.0f, 0.5f, 0.0f, 1.0f );
            vector3 points[2] = { sightRay.start(), sightRay.end() };
            nGfxServer2::Instance()->BeginLines();
            nGfxServer2::Instance()->DrawLines3d( points, 2, color );
            nGfxServer2::Instance()->EndLines();
        }
    }

#ifdef __NEBULA_STATS__
    GetProfSight().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent hearing field
*/
void 
nViewportDebugModule::DrawAgentHearing( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfHearing().StartAccum();
#endif

    ncGPHearing* hearing = entity->GetComponent<ncGPHearing>();
    if (hearing)
    {
        // Draw hearing field as a sphere
        float r = hearing->GetHearingRadius();
        matrix44 m;
        m.ident();
        m.scale(vector3(r, r, r));
        m *= entity->GetComponentSafe<ncTransform>()->GetTransform();
        nShapeServer::Instance()->BeginShapes();
        nShapeServer::Instance()->DrawShape(nGfxServer2::Sphere, m, vector4(0.7f, 0.7f, 0.1f, 0.2f));
        nShapeServer::Instance()->EndShapes();
    }

#ifdef __NEBULA_STATS__
    GetProfHearing().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent feeling field
*/
void 
nViewportDebugModule::DrawAgentFeeling( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfFeeling().StartAccum();
#endif

    ncGPFeeling* feeling = entity->GetComponent<ncGPFeeling>();
    if (feeling)
    {
        // Draw feeling field as a sphere
        float r = feeling->GetFeelingRadius();
        matrix44 m, m2;
        feeling->GetFeelingTransform(m2);
        m.ident();
        m.scale(vector3(r, r, r));
        m *= m2;
        nShapeServer::Instance()->BeginShapes();
        nShapeServer::Instance()->DrawShape(nGfxServer2::Sphere, m, vector4(0.1f, 0.7f, 0.7f, 0.2f));
        nShapeServer::Instance()->EndShapes();
    }

#ifdef __NEBULA_STATS__
    GetProfFeeling().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent/trigger proximity culling sphere
*/
void 
nViewportDebugModule::DrawPerceptionCulling( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfPerceptionCulling().StartAccum();
#endif

    ncTrigger* trigger = entity->GetComponent<ncTrigger>();
    if (trigger)
    {
        // Draw perception proximity culling as a sphere
        float r = trigger->GetCullingRadius();
        matrix44 m;
        m.ident();
        m.scale(vector3(r, r, r));
        m *= entity->GetComponentSafe<ncTransform>()->GetTransform();
        nShapeServer::Instance()->BeginShapes();
        nShapeServer::Instance()->DrawShape(nGfxServer2::Sphere, m, vector4(0.0f, 0.0f, 0.0f, 0.1f));
        nShapeServer::Instance()->EndShapes();
    }

#ifdef __NEBULA_STATS__
    GetProfPerceptionCulling().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent memory
*/
void 
nViewportDebugModule::DrawAgentMemory( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfMemory().StartAccum();
#endif

    ncAIState* state = entity->GetComponent<ncAIState>();
    ncAgentMemory* memory = entity->GetComponent<ncAgentMemory>();
    if (state && memory)
    {
        matrix44 m0;
        m0.ident();
        nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, m0);

        // Draw lines from this entity towards all the known entities,
        // fading out the lines if entities are not being perceived

        // Set origin point for all lines above the agent (the end will be the target's position)
        vector3 linesOrigin = entity->GetComponentSafe<ncTransform>()->GetPosition();
        ncPhysicsObj* physics = entity->GetComponentSafe<ncPhysicsObj>();
        nPhysicsAABB bb;
        physics->GetAABB(bb);
        linesOrigin.y = bb.maxy;

        const float memoryExpirationTime = float(memory->GetRecallTime());
        nGfxServer2::Instance()->BeginLines();
        for (ncAgentMemory::EventsIterator it(memory->GetEventsIterator()); !it.IsEnd(); it.Next())
        {
            nEntityObject* knownEntity( NULL );
            nGameEvent* event( nTriggerServer::Instance()->GetEvent( it.Get() ) );
            if ( event )
            {
                if ( event->GetSourceEntity() != nEntityObjectServer::IDINVALID )
                {
                    knownEntity = nEntityObjectServer::Instance()->GetEntityObject( event->GetSourceEntity() );
                }
            }
            if (knownEntity)
            {
                // Lines for entities not currently perceived are drawn with alpha,
                // more transparent as more times happend since last perception
                // (1 when currently perceived, 0 when the entity is going to be removed from memory)
                float reminderAge = float( it.GetReminder().GetReminderAge() );
                float fadeOut = 1.0f - reminderAge / memoryExpirationTime;
                if (fadeOut < 0)
                {
                    fadeOut = 0;
                }

                // Draw in another color the line for the entity selected as the target for the basic actions
                vector4 color;
                if (knownEntity == state->GetTarget() && (this->aiFlags & AITarget))
                {
                    color = vector4(1.0f, 0.7f, 0.1f, fadeOut);
                }
                else if (this->aiFlags & AIMemory)
                {
                    color = vector4(0.15f, 0.15f, 0.15f, fadeOut);
                }
                else
                {
                    continue;
                }

                // Lines are drawn from above the agent towards the target's position
                // This makes easier to identify which is the target and which is the source
                vector3 points[2] = {
                    linesOrigin,
                    knownEntity->GetComponentSafe<ncTransform>()->GetPosition()
                };
                nGfxServer2::Instance()->DrawLines3d(points, 2, color);
            }
        }
        nGfxServer2::Instance()->EndLines();
    }

#ifdef __NEBULA_STATS__
    GetProfMemory().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw agent motion
*/
void 
nViewportDebugModule::DrawAgentMotion( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfAIMotion().StartAccum();
#endif

    ncAIMovEngine* motion = entity->GetComponent<ncAIMovEngine>();
    if (motion)
    {
        motion->EnableDrawMotion((this->aiFlags & AIMotion) != 0);
        motion->EnableDrawAvoid((this->aiFlags & AIObstaclesAvoid) != 0);
        motion->EnableDrawStaticAvoid((this->aiFlags & AIStaticObstaclesAvoid) != 0);
        motion->Draw(nGfxServer2::Instance());
    }

#ifdef __NEBULA_STATS__
    GetProfAIMotion().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw an area trigger
*/
void 
nViewportDebugModule::DrawAreaTrigger( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfTriggers().StartAccum();
#endif

    this->refDebugComponentServer->DrawTriggerShape( entity );

#ifdef __NEBULA_STATS__
    GetProfTriggers().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw a spawner or spawn point
*/
void 
nViewportDebugModule::DrawSpawner( nEntityObject* entity )
{
#ifdef __NEBULA_STATS__
    GetProfSpawners().StartAccum();
#endif

    ncSpawner* spawner = entity->GetComponent<ncSpawner>();
    if (spawner)
    {
        // Draw spawner as a box
        vector3 v[2];
        v[0] = entity->GetComponentSafe<ncTransform>()->GetPosition();
        matrix44 m;
        m.ident();
        m.translate(v[0]);
        nGfxServer2::Instance()->BeginShapes();
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Box, m, vector4(0.9f, 0.6f, 0.2f, 0.5f));
        nGfxServer2::Instance()->EndShapes();

        // Show which are the children spawners with connecting lines
        m.ident();
        nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, m);
        vector4 lineColor(0.9f, 0.6f, 0.2f, 1);
        for (int i(0); i < spawner->GetSpawnersNumber(); ++i)
        {
            v[1] = spawner->GetSpawnerByIndex(i)->GetComponentSafe<ncTransform>()->GetPosition();
            nGfxServer2::Instance()->BeginLines();
            nGfxServer2::Instance()->DrawLines3d(v, 2, lineColor);
            nGfxServer2::Instance()->EndLines();
        }
        nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);
    }

    ncSpawnPoint* spawnPoint = entity->GetComponent<ncSpawnPoint>();
    if (spawnPoint)
    {
        // Draw spawn point as a box
        vector3 pos = entity->GetComponentSafe<ncTransform>()->GetPosition();
        matrix44 m;
        m.ident();
        m.translate(pos);
        nGfxServer2::Instance()->BeginShapes();
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Box, m, vector4(0.5f, 0.2f, 0.0f, 0.5f));
        nGfxServer2::Instance()->EndShapes();
    }

#ifdef __NEBULA_STATS__
    GetProfSpawners().StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Draw coordinate axes
*/
void 
nViewportDebugModule::DrawAxes()
{
    rectangle rect;

    const nDisplayMode2& screen= nGfxServer2::Instance()->GetDisplayMode();
    const matrix44& view = nGfxServer2::Instance()->GetTransform(nGfxServer2::View);
    const matrix44& proj = nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection);
    const nViewport& viewport = nGfxServer2::Instance()->GetViewport();

    matrix44 tra;
    vector3 offset(0.0f, 0.0f, -2.01f);
    tra.scale( vector3( 20, -30, 0) );
    tra.translate( vector3( 20, viewport.height - 30, 0) );

    vector3 tmp = offset;
    
    vector2 pointsList[2];
    tmp = tra * tmp;
    pointsList[0].x = tmp.x;
    pointsList[0].y = tmp.y;

    static vector4 color[3] = { vector4(1.0f, 0.0f , 0.0f, 1.0f),
                                vector4(0.0f, 1.0f , 0.0f, 1.0f),
                                vector4(0.0f, 0.0f , 1.0f, 1.0f) };
    static char* buf[3] = { "x" , "y" ,"z" };

    nGfxServer2::Instance()->BeginLines();

    for (int idx = 0; idx < 3; ++idx)
    {
        tmp = vector3(view.m[idx][0], view.m[idx][1], view.m[idx][2]);
        tmp.norm();
        tmp += offset;
        tmp = tra * proj.transform_coord(tmp);
        pointsList[1].x = tmp.x;
        pointsList[1].y = tmp.y;
        nGfxServer2::Instance()->DrawLines2d(pointsList, 2, color[idx]);
        rect.set(vector2((viewport.x + pointsList[1].x)  / screen.GetWidth(), (viewport.y + pointsList[1].y) / screen.GetHeight()), vector2(1.0f, 1.0f));
        nGfxServer2::Instance()->DrawText(buf[idx], color[idx], rect, 0);
    }

    nGfxServer2::Instance()->EndLines();
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::RenderWireframeOverlay(nSceneGraph* sceneGraph)
{
    // get the passes to debug: dept, colr, alph, etc. from each viewport(s)
    static nArray<uint> passIndices;
    passIndices.Reset();

    uint passEnabledFlags = this->appViewport->GetPassEnabledFlags();
    nRenderPathNode *viewport = (nRenderPathNode *) sceneGraph->GetCurrentNode();
    const nRenderPath2& renderPath = viewport->GetRenderPath();
    int pass;
    const nArray<nRpPass>& passes = renderPath.GetPasses();
    for (pass = 0; pass < passes.Size(); ++pass)
    {
        nRpPass& rpPass = renderPath.GetPass(pass);
        const nArray<nRpPhase>& phases = rpPass.GetPhases();
        int phase;
        for (phase = 0; phase < phases.Size(); ++phase)
        {
            passIndices.Append(rpPass.GetPhase(phase).GetPassIndex());
        }
    }

    static const vector4 lodColor[4] = {vector4(1.0f, 0.0f, 0.0f, 1.0f),//red
                                        vector4(1.0f, 0.5f, 0.0f, 1.0f),//orange
                                        vector4(0.0f, 1.0f, 0.0f, 1.0f),//green
                                        vector4(1.0f, 1.0f, 1.0f, 1.0f)};//white

    // now render all the passes in this viewport in wireframe
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    gfxServer->SetShader(this->refWireframeShader.get());
    this->refWireframeShader->SetInt(nShaderState::FillMode, 2); // wireframe
    this->refWireframeShader->Begin(true);
    this->refWireframeShader->BeginPass(0);
    for (int i = 0; i < passIndices.Size(); i++)
    {
        if (passEnabledFlags&(1<<passIndices[i]))
        {
            int numGroups = sceneGraph->GetPassNumGroups(passIndices[i]);
            if (sceneGraph->BeginRender(passIndices[i]))
            {
                int index = 0;
                nGeometryNode *prevNode = 0;
                do {
                    sceneGraph->SetShaderIndex(this->wireframeShaderIndex);
                    refWireframeShader->SetVector4(nShaderState::MatDiffuse, lodColor[sceneGraph->GetCurrentLod()]);
                    refWireframeShader->SetInt(nShaderState::isSkinned, 0);
                    refWireframeShader->SetInt(nShaderState::CullMode, nShaderState::CW);
                    //apply at first new node
                    if (prevNode != sceneGraph->GetCurrentNode())
                    {
                        if (prevNode)
                        {
                            //flush when node changes
                            prevNode->Flush(sceneGraph);
                            prevNode = 0;
                        }

                        prevNode = (nGeometryNode *) sceneGraph->GetCurrentNode();
                        prevNode->Apply(sceneGraph);
                    }
                    if ((this->terrainPassFlags & (1<<passIndices[i])) == 0)
                    {
                        gfxServer->SetTransform(nGfxServer2::Model, sceneGraph->GetModelTransform());
                    }
                    sceneGraph->RenderCurrent();
                    ++index;
                    if (index == numGroups && prevNode)
                    {
                        //flush at last group
                        prevNode->Flush(sceneGraph);
                        prevNode = 0;
                    }
                } while (sceneGraph->Next());
                sceneGraph->EndRender();
            }
        }
    }

    this->refWireframeShader->EndPass();
    this->refWireframeShader->End();
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::RenderSkeleton(nSceneGraph* sceneGraph)
{
    static int colrIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('colr'));
    if (sceneGraph->BeginRender(colrIndex))
    {
        this->skeletonEntities.Reset();
        this->characterTransforms.Reset();
        this->lodIndices.Reset();

        // draw joints
        do {
            nEntityObject *entityObject = (nEntityObject*) sceneGraph->GetCurrentEntity();
            ncCharacter* charComp = entityObject->GetComponent<ncCharacter>();
            nCharSkeleton* charSkeleton = 0;
            if (charComp)
            {
                if( ! charComp->GetRagdollActive() )
                {
                    charSkeleton = &charComp->GetCharacter(sceneGraph->GetCurrentLod())->GetSkeleton();
                }
                else
                {
                    charSkeleton = &charComp->GetRagdollCharacter()->GetSkeleton();
                }
            }
            if (charSkeleton && !this->skeletonEntities.Find(entityObject))
            {
                matrix44 scaleTransform;
                scaleTransform.scale(vector3(0.02f, 0.02f, 0.02f));
                matrix44 modelTransform;
                modelTransform = sceneGraph->GetModelTransform();

                this->skeletonEntities.Append(entityObject);
                this->characterTransforms.Append(modelTransform);
                this->lodIndices.Append(sceneGraph->GetCurrentLod());

                // draw joints
                // FIXME: shapes need to be drawn separately from lines, refs #586
                nGfxServer2::Instance()->BeginShapes();
                for (int jointIndex = 0; jointIndex < charSkeleton->GetNumJoints(); jointIndex++)
                {
                    const matrix44& jointMatrix = charSkeleton->GetJointAt(jointIndex).GetMatrix();
                    
                    static vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
                    nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, scaleTransform * jointMatrix * modelTransform , color);
                }
                nGfxServer2::Instance()->EndShapes();
            }
        } while (sceneGraph->Next());
        sceneGraph->EndRender();

        // draw bones
        int i;
        for (i = 0; i < this->skeletonEntities.Size(); i++)
        {
            matrix44& modelTransform = this->characterTransforms[i];
            nEntityObject *entityObject = this->skeletonEntities[i];
            ncCharacter* charComp = entityObject->GetComponent<ncCharacter>();

            nCharSkeleton* charSkeleton;

            if( ! charComp->GetRagdollActive() )
            {
                charSkeleton = &charComp->GetCharacter(this->lodIndices[i])->GetSkeleton();
            }
            else
            {
                charSkeleton = &charComp->GetRagdollCharacter()->GetSkeleton();
            }

            int parentjointindex;
            vector3 jointlist[2];
            
            this->skeletonLineHandler.BeginLines(modelTransform);
            int numJoints = charSkeleton->GetNumJoints();
            for (int jointIndex = 0; jointIndex < numJoints; jointIndex++)
            { 
                const matrix44& jointMatrix = charSkeleton->GetJointAt(jointIndex).GetMatrix();

                parentjointindex = charSkeleton->GetJointAt(jointIndex).GetParentJointIndex();
                if (parentjointindex != -1)
                {
                    jointlist[0] = jointMatrix.pos_component();
                    jointlist[1] = charSkeleton->GetJointAt(parentjointindex).GetMatrix().pos_component();
                    
                    static vector4 colorlines(0.0f, 1.0f, 0.0f, 1.0f);
                    this->skeletonLineHandler.DrawLines3d(jointlist, 0, 2, colorlines);
                }
            }
            this->skeletonLineHandler.EndLines();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::RenderNormals(nSceneGraph* sceneGraph)
{
#ifndef NGAME
    static int colrIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('colr'));

    if (sceneGraph->BeginRender(colrIndex))
    {
        nShapeServer* shapeServer = nShapeServer::Instance();
        shapeServer->SetPrimitiveType(nGfxServer2::LineList);
        shapeServer->SetIndexed(false);
        shapeServer->SetShader(this->refNormalsShader.get());
        shapeServer->BeginShapes();
        do
        {
            nEntityObject* entityObject = (nEntityObject*) sceneGraph->GetCurrentEntity();
            ncEditor* editorComp = entityObject->GetComponent<ncEditor>();
            if (editorComp)
            {
                editorComp->SetDrawingFlags(this->editorFlags);
                editorComp->Draw(sceneGraph);
            }
        }
        while (sceneGraph->Next());
        shapeServer->EndShapes();
        sceneGraph->EndRender();
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Render oriented bounding boxes for invalid graphic components.
*/
void
nViewportDebugModule::RenderInvalidBoxes(nSceneGraph *sceneGraph)
{
#ifndef NGAME
    static int bboxPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('bbox'));
    if (sceneGraph->BeginRender(bboxPassIndex))
    {
        nGfxServer2::Instance()->BeginShapes(this->refSpatialShapeShader);
        do {
            ncSpatial *spatialComp = (ncSpatial *) sceneGraph->GetCurrentEntity()->GetComponent<ncSpatial>();
            if (spatialComp && sceneGraph->GetCurrentEntity()->GetComponent<ncTransform>())
            {
                ncTransform *transform = (ncTransform *) sceneGraph->GetCurrentEntity()->GetComponent<ncTransform>();
                const bbox3 localBox = spatialComp->GetOriginalBBox();
                matrix44 boxMatrix;
                boxMatrix.scale(localBox.size());
                boxMatrix.translate(localBox.center());
                boxMatrix *= transform->GetTransform();
                nGfxServer2::Instance()->DrawShape(nGfxServer2::Box, boxMatrix, vector4(0.0f, 0.0f, 0.5f, 0.3f));
            }
        }
        while (sceneGraph->Next());
        nGfxServer2::Instance()->EndShapes();
        sceneGraph->EndRender();
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Render oriented bounding boxes for invalid graphic components.
*/
void
nViewportDebugModule::RenderLoadAreas()
{
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    nLoaderServer* loaderServer = nLoaderServer::Instance();
    
    if (!loaderServer->AreasAreLoaded())
    {
        return;
    }

    gfxServer->PushTransform(nGfxServer2::Model, matrix44());
    gfxServer->BeginShapes(this->refSpatialShapeShader.get());

    // cells and objects
    ncSpatialCamera* spatialCam = this->GetAppViewport()->GetSpatialCamera();
    const nArray<nRef<nEntityObject> >& visibleEntities = spatialCam->GetVisibleEntities();
    nEntityObject* visEntity = 0;
    for (int i = 0; i < visibleEntities.Size(); ++i)
    {
        if (!visibleEntities[i].isvalid())
        {
            continue;
        }

        static nArray<bbox3> boxes;
        boxes.Reset();

        nEntityObjectId spaceId = 0;
        nLoadArea* loadArea = 0;

        visEntity = visibleEntities[i];
        ncSpatial *spatialComponent = visEntity->GetComponent<ncSpatial>();
        if (spatialComponent)
        {
            if (spatialComponent->GetIndoorSpace())
            {
                spaceId = spatialComponent->GetIndoorSpace()->GetEntityObject()->GetId();
                loadArea = loaderServer->FindAreaContainingCell(spaceId, -1);
                if (loadArea)
                {
                    this->DrawAreaBox(gfxServer, loadArea, spatialComponent->GetBBox());
                }
            }
        }
        else
        {
            ncSpatialQuadtreeCell* quadtreeCell = visEntity->GetComponent<ncSpatialQuadtreeCell>();
            if (quadtreeCell)
            {
                static nArray<ncSpatialQuadtreeCell*> terrainCells;
                terrainCells.Reset();

                //for non-leaf cells, traverse down the quadtree
                if (quadtreeCell->IsLeaf())
                {
                    terrainCells.Append(quadtreeCell);
                }
                else
                {
                    this->CollectLeafCellsFrom(quadtreeCell, terrainCells);
                }

                for (int index = 0; index < terrainCells.Size(); ++index)
                {
                    ncSpatialQuadtreeCell* curCell = terrainCells[index];
                    spaceId = quadtreeCell->GetParentSpace()->GetEntityObject()->GetId();
                    loadArea = loaderServer->FindAreaContainingCell(spaceId, curCell->GetId());
                    if (loadArea)
                    {
                        this->DrawAreaBox(gfxServer, loadArea, curCell->GetComponent<ncTerrainGMMCell>()->GetBBox());
                    }
                }
            }
        }
    }

    gfxServer->EndShapes();
    gfxServer->PopTransform(nGfxServer2::Model);
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::CollectLeafCellsFrom(ncSpatialQuadtreeCell* quadtreeCell, nArray<ncSpatialQuadtreeCell*>& leafCells)
{
    for (int i = 0; i < quadtreeCell->GetNumSubcells(); ++i)
    {
        if (quadtreeCell->GetSubcells()[i]->IsLeaf())
        {
            leafCells.Append(quadtreeCell->GetSubcells()[i]);
        }
        else
        {
            this->CollectLeafCellsFrom(quadtreeCell->GetSubcells()[i], leafCells);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::DrawAreaBox(nGfxServer2* gfxServer, nLoadArea* loadArea, const bbox3& box)
{
    n_assert(loadArea);

    const vector3 nodeCenter(box.center());
    matrix44 m;
    m.scale(box.vmax - box.vmin);
    m.set_translation(nodeCenter);

    int indexColor = loadArea->GetAreaIndex() % numColors;
    gfxServer->DrawShape(nGfxServer2::Box, m, color[indexColor]);
}

//------------------------------------------------------------------------------
/**
*/
void 
nViewportDebugModule::RenderFPS()
{
    // Update every 0.2 seconds or (after 100 frames without update, safe fallback)
    if (this->appViewport->GetTime() > this->timeNextUpdateFPS ||
        this->appViewport->GetFrameId() > (this->frameIdLastUpdateFPS + 100) )
    {
        this->textFPS.Format("%03.1f FPS  %03.1f ms frameId=%05d time=%05.3f s",
                             nApplication::Instance()->GetFPS(),
                             this->appViewport->GetFrameTime() * 1000,
                             this->appViewport->GetFrameId(),
                             nTimeServer::Instance()->GetFrameTime());
        this->timeNextUpdateFPS = this->appViewport->GetTime() + 0.2f;
        this->frameIdLastUpdateFPS = this->appViewport->GetFrameId();
    }

    nGfxServer2::Instance()->Text(textFPS.Get(), vector4(1,0,0,1), -0.9f, -0.9f);
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportDebugModule::RenderStats()
{
    #ifdef __NEBULA_STATS__
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    const vector2 extents( gfxServer->GetTextExtent("XXXXXXXXXXXXXXXXXXXXXXXX") * 2.0f );
    const vector4 COLOR_WHITE( 1.0f, 1.0f, 1.0f, 1.0f );
    const vector4 COLOR_YELLOW( 1.0f, 1.0f, 0.0f, 1.0f );
    nString titleStr;
    nString valueStr;
    float left = 1.0f - 2.0f * extents.x;
    float top = -1.0f;
    int index = 0;
    int size;
    nArg* pArg;

    // render statistics onscreen
    #define STATS_DRAW( title, format, value ) \
        gfxServer->Text(title, COLOR_WHITE, left, top); \
        valueStr.Format(format, value); \
        gfxServer->Text(valueStr.Get(), COLOR_YELLOW, left + extents.x, top); \
        ++index; \
        top += extents.y;

    #define STATS_DRAWPASS( title, format, value ) \
        gfxServer->Text(title, COLOR_WHITE, left, top); \
        valueStr.Format(format, value); \
        gfxServer->Text(valueStr.Get(), COLOR_YELLOW, left + extents.x, top); \
        ++index; \
        top += extents.y;

    #define STATS_DRAWARRAY( array ) \
        size = array->GetL(pArg); \
        for (int pass = 0; pass < size; ++pass) \
        { \
            if (pArg[pass].GetI() > 0) \
            { \
                STATS_DRAWPASS( this->passNames[pass].Get(), "%u", pArg[pass].GetI() ); \
            } \
        }

    STATS_DRAW( "Draw calls",           "%u", this->watchNumDrawCalls->GetI() );
    STATS_DRAW( "Primitives",           "%u", this->watchNumPrimitives->GetI() );
    STATS_DRAW( "Mesh changes",         "%u", this->watchNumMeshChanges->GetI() );
    STATS_DRAW( "Mesh format changes",  "%u", this->watchNumVertexDeclChanges->GetI() );
    STATS_DRAW( "Mesh size (Kb)",       "%u", this->watchGeometrySize->GetI() / 1024 );
    STATS_DRAW( "Texture changes",      "%u", this->watchNumTextureChanges->GetI() );
    STATS_DRAW( "Texture size (Kb)",    "%u", this->watchTextureSize->GetI() / 1024 );
    STATS_DRAW( "Shader changes",       "%u", this->watchNumShaderChanges->GetI() );
    STATS_DRAWARRAY( this->watchNumShaderChangesByPass );
    STATS_DRAW( "Material changes",     "%u", this->watchNumSurfaceChanges->GetI() );
    STATS_DRAWARRAY( this->watchNumSurfaceChangesByPass );
    STATS_DRAW( "Geometry changes",     "%u", this->watchNumGeometryChanges->GetI() );
    STATS_DRAWARRAY( this->watchNumGeometryChangesByPass );

    #undef STATS_DRAW
    #undef STATS_DRAWPASS
    #undef STATS_DRAWARRAY

    /** @todo
        give the option through some automated process to dump detailed process
        information (passes, shaders, textures, meshes, etc) into a log or file. */

    #endif
}

//------------------------------------------------------------------------------
/**
    @brief Set time to end displaying camera velocity
    @param time The time that the displaying of camera velocity will last
*/
void 
nViewportDebugModule::SetTimeToDrawCameraVelocity( nTime time )
{
    this->timeToDrawCameraVelocity = max( this->timeToDrawCameraVelocity, nTimeServer::Instance()->GetTime() + time );
}
