#include "scene/sceneapp.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "util/nrandomlogic.h"

//------------------------------------------------------------------------------
/**
    run script that loads required resources, etc.
	and sets initial position of everything
*/
void SceneApp::Init()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
bool SceneApp::Open()
{
    //nGfxServer2* gfxServer = nGfxServer2::Instance();
    
    return true;
}

//------------------------------------------------------------------------------

void SceneApp::Close()
{
    //empty
}

//------------------------------------------------------------------------------

void SceneApp::Tick( float /*fTimeElapsed*/ )
{
    //nInputServer* inputServer = nInputServer::Instance();
}

//------------------------------------------------------------------------------

void SceneApp::Render()
{
    //nGfxServer2* gfxServer = nGfxServer2::Instance();
}
