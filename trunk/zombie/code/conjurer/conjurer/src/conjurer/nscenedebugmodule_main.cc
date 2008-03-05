#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nscenedebugmodule_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nscenedebugmodule.h"
#include "conjurer/nconjurersceneserver.h"
#include "conjurer/nconjurerapp.h"
#include "gfx2/nd3d9server.h"

nNebulaClass(nSceneDebugModule, "ndebugmodule");

//------------------------------------------------------------------------------
/**
*/
nSceneDebugModule::nSceneDebugModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSceneDebugModule::~nSceneDebugModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneDebugModule::OnDebugModuleCreate()
{
    // graphics
    this->RegisterDebugFlag("drawprimitive",  "Disable draw primitive",           "Graphics");
    this->RegisterDebugFlag("drawtext",       "Disable draw text",                "Graphics");
    this->RegisterDebugFlag("drawlines",      "Disable draw lines",               "Graphics");
    this->RegisterDebugFlag("drawstats",      "Enable draw statistics",           "Graphics");

    // scene
    this->RegisterDebugFlag("noattach",       "Disable scene attach",             "Scene");
    this->RegisterDebugFlag("norender",       "Disable scene render",             "Scene");
    this->RegisterDebugFlag("scenedebug",     "Disable scene debug",              "Scene");
    this->RegisterDebugFlag("transforms",     "Disable transform pass",           "Scene");
    this->RegisterDebugFlag("depthshapes",    "Disable depth pass",               "Scene");
    this->RegisterDebugFlag("colorshapes",    "Disable color pass",               "Scene");
    this->RegisterDebugFlag("alphashapes",    "Disable alpha pass",               "Scene");
    this->RegisterDebugFlag("instancing",     "Disable instanced pass",           "Scene");
    this->RegisterDebugFlag("gmm0shapes",     "Disable gmm0 pass",                "Scene");
    this->RegisterDebugFlag("gmm1shapes",     "Disable gmm1 pass",                "Scene");
    this->RegisterDebugFlag("gmmshapes",      "Disable terrain pass",             "Scene");
    this->RegisterDebugFlag("scenelight",     "Disable scene lights",             "Scene");
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneDebugModule::OnDebugOptionUpdated()
{
    nConjurerApp::Instance()->SetAttachEnabled(!this->GetFlagEnabled("noattach"));
    nConjurerApp::Instance()->SetRenderEnabled(!this->GetFlagEnabled("norender"));

    nConjurerSceneServer* sceneServer = static_cast<nConjurerSceneServer*>(nSceneServer::Instance());
    sceneServer->SetDebugEnabled(!this->GetFlagEnabled("scenedebug"));
    sceneServer->SetPassEnabled("trfm", !this->GetFlagEnabled("transforms"));
    sceneServer->SetPassEnabled("dept", !this->GetFlagEnabled("depthshapes"));
    sceneServer->SetPassEnabled("colr", !this->GetFlagEnabled("colorshapes"));
    sceneServer->SetPassEnabled("alph", !this->GetFlagEnabled("alphashapes"));
    sceneServer->SetPassEnabled("inst", !this->GetFlagEnabled("instancing"));
    sceneServer->SetPassEnabled("gmm0", !this->GetFlagEnabled("gmm0shapes")&&!this->GetFlagEnabled("gmmshapes"));
    sceneServer->SetPassEnabled("gmm1", !this->GetFlagEnabled("gmm1shapes")&&!this->GetFlagEnabled("gmmshapes"));
    sceneServer->SetPassEnabled("lght", !this->GetFlagEnabled("scenelight"));

    #ifndef NGAME
    nD3D9Server* d3d9Server = static_cast<nD3D9Server*>(nGfxServer2::Instance());

    #ifdef __NEBULA_STATS__
    d3d9Server->SetStatsEnabled(this->GetFlagEnabled("drawstats"));
    #endif

    d3d9Server->SetDrawEnabled(!this->GetFlagEnabled("drawprimitive"));
    d3d9Server->SetDrawLines(!this->GetFlagEnabled("drawlines"));
    d3d9Server->SetDrawText(!this->GetFlagEnabled("drawtext"));
    #endif //NGAME
}
