#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  neditordebugmodule_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditordebugmodule.h"
#include "conjurer/nconjurerapp.h"
#include "ndebug/ndebugcomponentserver.h"
#include "ndebug/ndebugserver.h"
#include "ndebug/nceditor.h"
#include "nspatial/nspatialserver.h"
#include "zombieentity/nloaderserver.h"
#include "kernel/logclassregistry.h"
#include "kernel/nlogoutputobject.h"
#include "ntrigger/ncagenttrigger.h"
#include "ncaimovengine/ncaimovengine.h"
#include "npathfinder/npathfinder.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "npreloadmanager/npreloadmanager.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/ncterraingmm.h"
#include "nmaterial/nmaterialserver.h"
#include "animcomp/ncharacterserver.h"
#include "nscene/nsceneserver.h"

#ifndef NGAME
#include "gfx2/nd3d9mesh.h"
#endif //NGAME

nNebulaClass(nEditorDebugModule, "ndebugmodule");

//------------------------------------------------------------------------------
/**
*/
nEditorDebugModule::nEditorDebugModule()
{
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nEditorDebugModule::OnLevelLoaded,
                                          0);
}

//------------------------------------------------------------------------------
/**
*/
nEditorDebugModule::~nEditorDebugModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorDebugModule::OnDebugModuleCreate()
{
    this->RegisterDebugFlag("labels",          "Draw labels",                       "General");
    this->RegisterDebugFlag("labelsex",        "Draw labels complete",              "General");
    this->RegisterDebugFlag("screenlog",       "Disable screen log",                "General");
    this->RegisterDebugFlag("waypoints",       "Draw waypoints",                    "General");
    this->RegisterDebugFlag("runphysics",      "Run physics simulation",            "General");
    this->RegisterDebugFlag("dbgmemstats",     "Enable debug memory statistics",    "General");

    this->RegisterDebugFlag("noloadbatches",       "Disable loading batches",       "Graphics");
    this->RegisterDebugFlag("noloadspaces",        "Disable loading spaces",        "Graphics");
    this->RegisterDebugFlag("noloadentities",      "Disable loading resources",     "Graphics");
    this->RegisterDebugFlag("noloadareas",         "Disable loading areas",         "Graphics");
    this->RegisterDebugFlag("unloadclasses",       "Disable unloading classes",     "Graphics");
    this->RegisterDebugFlag("noloadlightmaps",     "Disable loading terrain lightmaps",      "Graphics");
    this->RegisterDebugFlag("terrainpink",         "Render unpainted cells (pink)", "Graphics");
    this->RegisterDebugFlag("debugmaterial",       "Replace all shaders",           "Graphics");
    this->RegisterDebugFlag("debugsurface",        "Replace all materials",         "Graphics");
    this->RegisterDebugFlag("nomeshoptimization",  "Disable mesh optimization",     "Graphics");
    this->RegisterDebugFlag("sceneloadshaders",    "Preload all shaders",           "Graphics");
    this->RegisterDebugFlag("animfixedframerate",  "Animation - fixed frame rate",  "Graphics");
    this->RegisterDebugFlag("animdisablephysics",  "Animation - disable physics animation",  "Graphics");

    this->RegisterDebugFlag("octrees",         "Enable octrees visibility",      "Visibility");
    this->RegisterDebugFlag("noterrain",       "Disable terrain",                "Visibility");
    this->RegisterDebugFlag("disablehorizon",  "Disable horizon culling",        "Visibility");

    this->RegisterDebugFlag("unsight",         "Disable sight sense",            "Perception");
    this->RegisterDebugFlag("unhearing",       "Disable hearing sense",          "Perception");
    this->RegisterDebugFlag("unfeeling",       "Disable feeling sense",          "Perception");

    this->RegisterDebugFlag("dontavoidobstacles",  "Disable obstacle avoidance",         "Motion");
    this->RegisterDebugFlag("enablethrottle",      "Enable throttle to avoid obstacles", "Motion");
    this->RegisterDebugFlag("penetrate",           "Disable non-penetration constraint", "Motion");
    this->RegisterDebugFlag("disablecontainment",  "Disable containment constraint",     "Motion");
    this->RegisterDebugFlag("dontclearpath",       "Disable path cleaning",              "Motion");
    this->RegisterDebugFlag("dontroundpath",       "Disable path rounding",              "Motion");
    this->RegisterDebugFlag("dontsinglesmooth",    "Disable path single smoothing",      "Motion");
    this->RegisterDebugFlag("dontdisplaceanim",    "Disable animation displacement",     "Motion");
    this->RegisterDebugFlag("dontsmoothrotations", "Disable rotation smoothing",         "Motion");
    this->RegisterDebugFlag("constantmotion",      "Simulate constant delta time",       "Motion");

}

//------------------------------------------------------------------------------
/**
*/
void
nEditorDebugModule::OnDebugOptionUpdated()
{
    nDebugComponentServer* dbgCompServer = nDebugComponentServer::Instance();

    int drawFlags = ncEditor::None;

    if (this->GetFlagEnabled("labels"))
    {
        drawFlags |= ncEditor::DrawLabel;
    }
    if (this->GetFlagEnabled("labelsex"))
    {
        drawFlags |= ncEditor::DrawLabelEx;
    }
    if (this->GetFlagEnabled("waypoints")) 
    {
        drawFlags |= ncEditor::DrawWaypoint;
    }

    nLogOutputObject * logOutputObject = LogClassRegistry::Instance()->GetUserLogOutputObject();
    if ( logOutputObject )
    {
        logOutputObject->SetEnabled( ! this->GetFlagEnabled("screenlog") );
    }

    nSpatialServer::Instance()->SetUseOctrees(this->GetFlagEnabled("octrees"));
    nSpatialServer::Instance()->SetTerrainVisible(!this->GetFlagEnabled("noterrain"));

    nConjurerApp::Instance()->SetPhysicsEnabled(this->GetFlagEnabled("runphysics"));
    nConjurerApp::Instance()->SetDebugMemoryStats(this->GetFlagEnabled("dbgmemstats"));

    ncAgentTrigger::SetSightEnabled( !this->GetFlagEnabled("unsight") );
    ncAgentTrigger::SetHearingEnabled( !this->GetFlagEnabled("unhearing") );
    ncAgentTrigger::SetFeelingEnabled( !this->GetFlagEnabled("unfeeling") );

    ncAIMovEngine::EnableObstaclesAvoidance( !this->GetFlagEnabled("dontavoidobstacles") );
    ncAIMovEngine::EnableThrottleSteering( this->GetFlagEnabled("enablethrottle") );
    ncAIMovEngine::EnableNonPenetrationConstraint( !this->GetFlagEnabled("penetrate") );
    ncAIMovEngine::EnableContainmentConstraint( !this->GetFlagEnabled("disablecontainment") );
    ncAIMovEngine::EnableAnimDisplacement( !this->GetFlagEnabled("dontdisplaceanim") );
    ncAIMovEngine::EnableRotationSmoothing( !this->GetFlagEnabled("dontsmoothrotations") );
    ncAIMovEngine::EnableConstantTime( this->GetFlagEnabled("constantmotion") );
    nPathFinder::SetPathCleaning( !this->GetFlagEnabled("dontclearpath") );
    nPathFinder::SetPathRounding( !this->GetFlagEnabled("dontroundpath") );
    nPathFinder::SetSingleSmoothing( !this->GetFlagEnabled("dontsinglesmooth") );

    nSpatialServer::Instance()->SetDoHorizonCulling(!this->GetFlagEnabled("disablehorizon"));

    dbgCompServer->SetDrawingFlags( drawFlags );

    nLoaderServer::Instance()->SetLoadResources(!this->GetFlagEnabled("noloadentities"));
    nPreloadManager::Instance()->SetPreloadEnabled(!this->GetFlagEnabled("noloadentities"));
    nLoaderServer::Instance()->SetLoadBatches(!this->GetFlagEnabled("noloadbatches"));
    nLoaderServer::Instance()->SetLoadSpaces(!this->GetFlagEnabled("noloadspaces"));
    nLoaderServer::Instance()->SetLoadAreasEnabled(!this->GetFlagEnabled("noloadareas"));
    nLoaderServer::Instance()->SetUnloadClassResources(!this->GetFlagEnabled("unloadclasses"));

    nMaterialServer::Instance()->SetDebugMaterialEnabled(this->GetFlagEnabled("debugmaterial"));
    nMaterialServer::Instance()->SetDebugSurfaceEnabled(this->GetFlagEnabled("debugsurface"));

    nCharacterServer::Instance()->SetFixedFPS(this->GetFlagEnabled("animfixedframerate") ? 30 : 100);
    nCharacterServer::Instance()->SetPhysicsEnabled(!this->GetFlagEnabled("animdisablephysics"));

    #ifndef NGAME
    nD3D9Mesh::optimizeMesh = !this->GetFlagEnabled("nomeshoptimization");
    #endif //NGAME

    this->TerrainLightmapsEnabled(!this->GetFlagEnabled("noloadlightmaps"));

    // set on demand loading of shaders
    nSceneServer::Instance()->SetLoadShadersOnDemand(!this->GetFlagEnabled("sceneloadshaders"));

    // set if render unpainted (pink) cells 
    nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    if (outdoor)
    {
        ncTerrainGMM * tgmm = outdoor->GetComponentSafe<ncTerrainGMM>();
        if (tgmm->GetUnpaintedCellsInvisible() == this->GetFlagEnabled("terrainpink"))
        {
            tgmm->SetUnpaintedCellsInvisible( !this->GetFlagEnabled("terrainpink") );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorDebugModule::TerrainLightmapsEnabled(bool enabled)
{
    nLevel* curLevel = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(curLevel);
    if (curLevel)
    {
        nEntityObjectId outdoorId = curLevel->FindEntity("outdoor");
        if (outdoorId)
        {
            nEntityObject* outdoor = nEntityObjectServer::Instance()->GetEntityObject(outdoorId);
            if (outdoor && outdoor->GetClassComponent<ncTerrainMaterialClass>())
            {
                ncTerrainMaterialClass* material = outdoor->GetClassComponent<ncTerrainMaterialClass>();
                nEntityObject* lightmap = material->GetLightMap();
                if (enabled)
                {
                    if (!lightmap && this->refPrevLightmap.isvalid())
                    {
                        material->SetLightMap(this->refPrevLightmap.get());
                    }
                }
                else
                {
                    if (lightmap)
                    {
                        this->refPrevLightmap = lightmap;
                        material->SetLightMap(0);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    refresh load lightmaps option when 
*/
void
nEditorDebugModule::OnLevelLoaded()
{
    this->refPrevLightmap.invalidate();
    this->TerrainLightmapsEnabled(!this->GetFlagEnabled("noloadlightmaps"));
}
