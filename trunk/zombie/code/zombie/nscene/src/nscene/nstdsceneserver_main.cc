#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nstdsceneserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "nscene/nstdsceneserver.h"
#include "nscene/nscenegraph.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nprofiler.h"

nNebulaClass(nStdSceneServer, "nsceneserver");

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::nStdSceneServer() :
    inBeginScene(false),
    profRender("profStdSceneRender"),
    profRenderTransform("profStdSceneRenderTransform"),
    profRenderViewport("profStdSceneRenderViewport")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStdSceneServer::~nStdSceneServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the actual scene.

    This implementation only takes into account nodes in the standard scene.
    Derived, application-specific classes can reuse the implementation here
    to render any other passes that require some other specific ordering.
*/
void
nStdSceneServer::RenderScene()
{
    static int trfmPassIndex = this->GetPassIndexByFourCC(FOURCC('trfm'));
    static int rtgtPassIndex = this->GetPassIndexByFourCC(FOURCC('rtgt'));
    static int viewPassIndex = this->GetPassIndexByFourCC(FOURCC('view'));

    this->profRender.StartAccum();

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    int index;
    for (index = 0; index < this->GetNumGraphs(); index++)
    {
        nSceneGraph *sceneGraph = this->GetGraphAt(index);

        this->profRenderTransform.Start();
        sceneGraph->RenderPass(trfmPassIndex);  // compute transforms
        this->profRenderTransform.Stop();

        sceneGraph->RenderPass(rtgtPassIndex);  // render offscreen texture
    }

    //<HACK>
    //this->inBeginScene = gfxServer->BeginScene();
    //n_assert(this->inBeginScene);
    //</HACK>

    this->profRenderViewport.Start();
    for (index = 0; index < this->GetNumGraphs(); index++)
    {
        nSceneGraph *sceneGraph = this->GetGraphAt(index);
        sceneGraph->RenderPass(viewPassIndex);  // render viewport(s)
    }

    this->profRenderViewport.Stop();
    this->profRender.Stop();

    // HACK this is to allow additional rendering after viewport(s)
    this->inBeginScene = gfxServer->BeginScene();
}
