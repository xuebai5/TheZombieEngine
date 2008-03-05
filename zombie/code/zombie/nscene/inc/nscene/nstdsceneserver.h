#ifndef N_STDSCENESERVER_H
#define N_STDSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nStdSceneServer
    @ingroup SceneServers

    @brief The standard scene server is customized to render all of
    the known passes in the right order for the standard scene nodes.

    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/nsceneserver.h"

//------------------------------------------------------------------------------
class nStdSceneServer : public nSceneServer
{
public:
    /// constructor
    nStdSceneServer();
    /// destructor
    ~nStdSceneServer();

    /// render the scene
    virtual void RenderScene();

protected:
    bool inBeginScene;
    nProfiler profRender;
    nProfiler profRenderTransform;
    nProfiler profRenderViewport;
};

//------------------------------------------------------------------------------
#endif
