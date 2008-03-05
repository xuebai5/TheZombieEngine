#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nconjurersceneserver_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nconjurersceneserver.h"
#include "conjurer/nviewportdebugmodule.h"
#include "conjurer/nconjurerapp.h"
#include "nscene/nscenegraph.h"
#include "kernel/nkernelserver.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nConjurerSceneServer, "nstdsceneserver");

//------------------------------------------------------------------------------
/**
*/
nConjurerSceneServer::nConjurerSceneServer() :
    debugEnabled(true),
    enabledPassFlags(0xffffffff),
    profRender("profSceneRender", true),
    profRenderTransforms("profSceneRenderTransforms", true),
    profValidateResources("profSceneValidateResources", true),
    watchNumTransforms("sceneNumTransforms", nArg::Int),
    watchNumDepthShapes("sceneNumDepthShapes", nArg::Int),
    watchNumColorShapes("sceneNumColorShapes", nArg::Int),
    watchNumAlphaShapes("sceneNumAlphaShapes", nArg::Int),
    watchNumTerrainCells("sceneNumTerrainCells", nArg::Int),
    watchNumTerrainDetailCells("sceneNumTerrainDetailCells", nArg::Int)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nConjurerSceneServer::~nConjurerSceneServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerSceneServer::SetPassEnabled(const char *passFourCC, bool enabled)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(passFourCC);
    uint passIndex = this->GetPassIndexByFourCC(fourcc);
    if (enabled)
    {
        this->enabledPassFlags |= (1<<passIndex);
    }
    else
    {
        this->enabledPassFlags &= ~(1<<passIndex);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nConjurerSceneServer::GetPassEnabled(const char *passFourCC)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(passFourCC);
    uint passIndex = this->GetPassIndexByFourCC(fourcc);
    return (this->enabledPassFlags & (1<<passIndex)) != 0;
}

//------------------------------------------------------------------------------
/**
    Begin building the scene graph(s). Must be called once before attaching 
    nSceneNode hierarchies using nSceneGraph::Attach().
*/
bool
nConjurerSceneServer::BeginScene()
{
    if (nConjurerApp::Instance()->IsAttachEnabled())
    {
        return nSceneServer::BeginScene();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Finish attaching the scene graph(s).
*/
void
nConjurerSceneServer::EndScene()
{
    if (nConjurerApp::Instance()->IsAttachEnabled())
    {
        nSceneServer::EndScene();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerSceneServer::RenderScene()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    if (!nConjurerApp::Instance()->IsRenderEnabled())
    {
        // HACK this is to allow additional rendering after viewport(s)
        this->inBeginScene = gfxServer->BeginScene();
        return;
    }

    this->profRender.StartAccum();

    if (!this->debugEnabled)
    {
        nStdSceneServer::RenderScene();
        this->profRender.StopAccum();
        return;
    }

    static nVariable::Handle debugVarHandle = nVariableServer::Instance()->GetVariableHandleByName("debug");

    static int trfmPassIndex = this->GetPassIndexByFourCC(FOURCC('trfm'));
    static int rtgtPassIndex = this->GetPassIndexByFourCC(FOURCC('rtgt'));
    static int viewPassIndex = this->GetPassIndexByFourCC(FOURCC('view'));

    static int deptPassIndex = this->GetPassIndexByFourCC(FOURCC('dept'));
    static int colrPassIndex = this->GetPassIndexByFourCC(FOURCC('colr'));
    static int alphPassIndex = this->GetPassIndexByFourCC(FOURCC('alph'));
    static int gmm0PassIndex = this->GetPassIndexByFourCC(FOURCC('gmm0'));
    static int gmm1PassIndex = this->GetPassIndexByFourCC(FOURCC('gmm1'));

    int numTransforms = 0;
    int numDepthShapes = 0;
    int numColorShapes = 0;
    int numAlphaShapes = 0;
    int numTerrainCells = 0;
    int numTerrainDetailCells = 0;

    int index;
    for (index = 0; index < this->GetNumGraphs(); index++)
    {
        nSceneGraph *sceneGraph = this->GetGraphAt(index);

        this->profValidateResources.Start();
        sceneGraph->ValidateNodeResources();
        this->profValidateResources.Stop();

        if (this->enabledPassFlags & (1<<trfmPassIndex))
        {
            numTransforms += sceneGraph->GetPassNumGroups(trfmPassIndex);
            this->profRenderTransforms.StartAccum();
            sceneGraph->RenderPass(trfmPassIndex);  // compute transforms
            this->profRenderTransforms.StopAccum();
        }
        if (this->enabledPassFlags & (1<<rtgtPassIndex))
        {
            sceneGraph->RenderPass(rtgtPassIndex);  // render offscreen texture(s)
        }

        // totalize num groups for active passes
        if (this->enabledPassFlags & (1<<deptPassIndex))
        {
            numDepthShapes += sceneGraph->GetPassNumGroups(deptPassIndex);
        }
        if (this->enabledPassFlags & (1<<colrPassIndex))
        {
            numColorShapes += sceneGraph->GetPassNumGroups(colrPassIndex);
        }
        if (this->enabledPassFlags & (1<<alphPassIndex))
        {
            numAlphaShapes += sceneGraph->GetPassNumGroups(alphPassIndex);
        }
        if (this->enabledPassFlags & (1<<gmm0PassIndex))
        {
            numTerrainDetailCells += sceneGraph->GetPassNumGroups(gmm0PassIndex);
        }
        if (this->enabledPassFlags & (1<<gmm1PassIndex))
        {
            numTerrainCells += sceneGraph->GetPassNumGroups(gmm1PassIndex);
        }
    }

    // HACK clear the entire buffer before rendering the viewports
    // for they will clear only their rectangle region
    gfxServer->SetRenderTarget(0, 0);
    if (gfxServer->BeginScene())
    {
        gfxServer->Clear(nGfxServer2::ColorBuffer, this->bgColor.x, this->bgColor.y, this->bgColor.z, this->bgColor.w, 1.0f, 0);
        gfxServer->EndScene();
    }

    // instead of rendering the whole "view" pass, render each viewport separately
    // and for each one, render the debug information.
    for (index = 0; index < this->GetNumGraphs(); index++)
    {
        nSceneGraph *sceneGraph = this->GetGraphAt(index);
        if (this->enabledPassFlags & (1<<viewPassIndex))
        {
            if (sceneGraph->BeginRender(viewPassIndex))
            {
                do {
                    // override pass flags in the view context
                    nEntityObject* entityObject = (nEntityObject*) sceneGraph->GetCurrentEntity();
                    ncScene* sceneComp = entityObject->GetComponent<ncScene>();
                    uint passFlags = sceneComp->GetPassEnabledFlags();
                    sceneComp->SetPassEnabledFlags(passFlags & this->enabledPassFlags);

                    // render debug module
                    nViewportDebugModule *debugModule = 0;
                    nVariable* var = sceneComp->FindLocalVar(debugVarHandle);
                    if (var)
                    {
                        debugModule = (nViewportDebugModule *) var->GetObj();
                        debugModule->BeginScene();
                    }
                    sceneGraph->RenderCurrent();
                    if (debugModule)
                    {
                        debugModule->Render(sceneGraph);
                        debugModule->EndScene();
                    }
                    sceneComp->SetPassEnabledFlags(passFlags);
                }
                while (sceneGraph->Next());
                sceneGraph->EndRender();
            }
        }
    }
    
    // HACK this is to allow additional rendering after viewport(s)
    this->inBeginScene = gfxServer->BeginScene();

    this->profRender.StopAccum();
    this->watchNumTransforms->SetI(numTransforms);
    this->watchNumDepthShapes->SetI(numDepthShapes);
    this->watchNumColorShapes->SetI(numColorShapes);
    this->watchNumAlphaShapes->SetI(numAlphaShapes);
    this->watchNumTerrainCells->SetI(numTerrainCells);
    this->watchNumTerrainDetailCells->SetI(numTerrainDetailCells);
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerSceneServer::PresentScene()
{
    nStdSceneServer::PresentScene();
}
