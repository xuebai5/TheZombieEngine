#ifndef N_CONJURERSCENESERVER_H
#define N_CONJURERSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nConjurerSceneServer
    @ingroup NebulaConjurerEditor

    Conjurer-specific scene server. It debug initialization.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/nstdsceneserver.h"
#include "kernel/ncmdprotonativecpp.h"
#include "misc/nwatched.h"

//------------------------------------------------------------------------------
class nConjurerSceneServer : public nStdSceneServer
{
public:
    /// constructor
    nConjurerSceneServer();
    /// destructor
    virtual ~nConjurerSceneServer();
    /// begin building scene graph(s) for rendering
    virtual bool BeginScene();
    /// finish building scene graph(s) for rendering
    virtual void EndScene();
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();

        /// set debug enabled
        void SetDebugEnabled(bool);
        /// get debug enabled
        bool GetDebugEnabled();
        /// set render pass enabled
        void SetPassEnabled(const char *, bool);
        /// get render pass enabled
        bool GetPassEnabled(const char *);

private:
    bool debugEnabled;
    uint enabledPassFlags;

    nWatched watchNumTransforms;
    nWatched watchNumDepthShapes;
    nWatched watchNumColorShapes;
    nWatched watchNumAlphaShapes;
    nWatched watchNumTerrainCells;
    nWatched watchNumTerrainDetailCells;

    nProfiler profRender;
    nProfiler profRenderTransforms;
    nProfiler profValidateResources;
};

//------------------------------------------------------------------------------
/**
*/
inline 
void
nConjurerSceneServer::SetDebugEnabled(bool enabled)
{
    this->debugEnabled = enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool
nConjurerSceneServer::GetDebugEnabled()
{
    return this->debugEnabled;
}

//------------------------------------------------------------------------------
#endif // N_CONJURERSCENESERVER_H
