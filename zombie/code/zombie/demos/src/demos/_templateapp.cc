#include "demos/_templatapp.h"

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
void _TemplateApp::Init()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
bool _TemplateApp::Open()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    
    return true;
}

//------------------------------------------------------------------------------

void _TemplateApp::Close()
{
    //empty
}

//------------------------------------------------------------------------------

void _TemplateApp::Tick( float /*fTimeElapsed*/ )
{
    nInputServer* inputServer = nInputServer::Instance();
}

//------------------------------------------------------------------------------

void _TemplateApp::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
}
