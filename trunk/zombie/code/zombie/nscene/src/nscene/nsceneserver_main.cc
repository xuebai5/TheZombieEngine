#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nsceneserver_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nsceneserver.h"
#include "nscene/nscenegraph.h"
#include "kernel/nkernelserver.h"
#include "resource/nresourceserver.h"

#include "kernel/nlogclass.h"

nNebulaScriptClass(nSceneServer, "nroot");
nSceneServer* nSceneServer::Singleton = 0;

NCREATELOGLEVEL(sceneLog, "Scene Events", false, 2)
// this logs scene processes, except rendering (covered in scenegraph, renderpass logs)
// 0- events only
// 1- per frame general info
// 2- per frame detailed info

//------------------------------------------------------------------------------
/**
*/
nSceneServer::nSceneServer() :
    isOpen(false),
    inBeginScene(false),
    loadShadersOnDemand(true),
    priorityByType(16, 16),
    shaderDatabaseFilename("shaders:_shaderindex.xml")
    #if __NEBULA_STATS__
   ,statsNumValidShaders(0),
    profPresentScene("profScenePresent", true),
    watchNumShaders("sceneNumShaders", nArg::Int),
    watchNumValidShaders("sceneNumValidShaders", nArg::Int)
    #endif
{
    n_assert(0 == Singleton);
    Singleton = this;
    this->graphPool = kernelServer->New("nroot", "/sys/share/rsrc/scene");
}

//------------------------------------------------------------------------------
/**
*/
nSceneServer::~nSceneServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;
    this->graphPool->Release();
    this->ClearShaders();
}

//------------------------------------------------------------------------------
/**
    Open the scene server. This initializes the embedded render path
    object.
*/
bool
nSceneServer::Open()
{
    n_assert(!this->isOpen);
    
    // open the display
    n_verify(nGfxServer2::Instance()->OpenDisplay());

    this->isOpen = true;

    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the scene server.
*/
void
nSceneServer::Close()
{
    n_assert(this->isOpen);
    nGfxServer2::Instance()->CloseDisplay();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
int
nSceneServer::GetPassIndexByFourCC(nFourCC fourcc)
{
    for (int i = 0; i < this->passes.Size(); i++)
    {
        if (this->passes[i] == fourcc)
        {
            return i;
        }
    }
    this->passes.Append(fourcc);
    return this->passes.Size() - 1;
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::SetPriorityBySelectorType(nFourCC typeFourCC, int renderPri)
{
    if (this->priorityByType.HasKey(typeFourCC))
    {
        this->priorityByType.GetElement(typeFourCC) = renderPri;
    }
    else
    {
        this->priorityByType.Add(typeFourCC, renderPri);
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nSceneServer::GetPriorityBySelectorType(nFourCC typeFourCC)
{
    if (this->priorityByType.HasKey(typeFourCC))
    {
        return this->priorityByType.GetElement(typeFourCC);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    Create a new possible shared scene graph. Bumps refcount on an 
    existing scene graph object. Pass a zero if a (non-shared) resource 
    should be created.

    @param  rsrcName    the graph name (for resource sharing), can be 0
    @return             pointer to graph object

*/
nSceneGraph *
nSceneServer::NewSceneGraph(const char *graphName)
{
    char graphId[N_MAXNAMELEN];
    nResourceServer::Instance()->GetResourceId(graphName, graphId, sizeof(graphId));
    nSceneGraph *graph = (nSceneGraph *) graphPool->Find(graphId);
    if (graph)
    {
        graph->AddRef();
    }
    else
    {
        kernelServer->PushCwd(this->graphPool);
        graph = (nSceneGraph *) kernelServer->New("nscenegraph", graphId);
        kernelServer->PopCwd();
        n_assert(graph);
    }
    return graph;
}

//------------------------------------------------------------------------------
/**
    Attach a scene graph for rendering.
*/
void
nSceneServer::AddGraph(nSceneGraph *sceneGraph)
{
    n_assert(!this->inBeginScene);

    int index;
    for (index = 0; index < this->refGraphArray.Size(); index++)
    {
        if (this->refGraphArray[index] == sceneGraph)
        {
            return;
        }
    }
    this->refGraphArray.Append(sceneGraph);
}

//------------------------------------------------------------------------------
/**
    Remove a scene graph for rendering.
*/
void
nSceneServer::RemoveGraph(nSceneGraph *sceneGraph)
{
    n_assert(!this->inBeginScene);

    int index;
    for (index = 0; index < this->refGraphArray.Size(); index++)
    {
        if (this->refGraphArray[index] == sceneGraph)
        {
            this->refGraphArray.Erase(index);
            return;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Begin building the scene graph(s). Must be called once before attaching 
    nSceneNode hierarchies using nSceneGraph::Attach().
*/
bool
nSceneServer::BeginScene()
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginScene);

    this->inBeginScene = nGfxServer2::Instance()->BeginFrame();

    if (this->inBeginScene)
    {
        int index;
        for (index = 0; index < this->refGraphArray.Size(); index++)
        {
            this->refGraphArray[index]->BeginAttach();
        }
    }

    return this->inBeginScene;
}

//------------------------------------------------------------------------------
/**
    Finish attaching the scene graph(s).
*/
void
nSceneServer::EndScene()
{
    n_assert(this->inBeginScene);

    int index;
    for (index = 0; index < this->refGraphArray.Size(); index++)
    {
        this->refGraphArray[index]->EndAttach();
    }

    this->inBeginScene = false;
}

//------------------------------------------------------------------------------
/**
    Render the actual scene. This method should be implemented by
    subclasses of nSceneServer. The frame will not be visible until
    PresentScene() is called. Additional render calls to the gfx server
    can be invoked between RenderScene() and PresentScene().
*/
void
nSceneServer::RenderScene()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Finalize rendering and present the current frame. No additional rendering
    calls may be invoked after calling nSceneServer::PresentScene()

    @todo ma.garcias port profilers from nebulasdk
*/
void
nSceneServer::PresentScene()
{
    //if (this->inBeginScene)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->DrawTextBuffer();
        gfxServer->EndScene();

        #if __NEBULA_STATS__
        this->profPresentScene.Start();
        #endif
        gfxServer->PresentScene();
        #if __NEBULA_STATS__
        this->profPresentScene.Stop();
        #endif

        gfxServer->EndFrame();
    }

    #if __NEBULA_STATS__
    this->watchNumShaders->SetI(this->shaderArray.Size());
    this->watchNumValidShaders->SetI(this->statsNumValidShaders);
    #endif
}
