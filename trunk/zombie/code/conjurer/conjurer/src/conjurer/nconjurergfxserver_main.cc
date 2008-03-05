//------------------------------------------------------------------------------
//  nconjurergfxserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchconjurerapp.h"
#include "conjurer/nconjurergfxserver.h"
#include "conjurer/nconjurerwindowhandler.h"

nNebulaClass(nConjurerGfxServer, "nd3d9server");

//------------------------------------------------------------------------------
/**
    Create window handler
*/
nConjurerGfxServer::nConjurerGfxServer()
{
    n_delete( this->windowHandler );
    this->windowHandler = CreateWindowHandler();
}

//------------------------------------------------------------------------------
/**
    Create window handler
*/
nD3D9WindowHandler*
nConjurerGfxServer::CreateWindowHandler()
{
    return n_new(nConjurerWindowHandler)(this);
}
