#include "precompiled/pchrenaissanceapp.h"
//------------------------------------------------------------------------------
//  ncommonviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nrnsappviewport/nrnsappviewport.h"

#ifdef NGAME
nNebulaClass(nRnsAppViewport, "ncommonviewport");
#else
nNebulaClass(nRnsAppViewport, "neditorviewport");
#endif

//------------------------------------------------------------------------------
/**
*/
nRnsAppViewport::nRnsAppViewport()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
nRnsAppViewport::~nRnsAppViewport()
{
    //Empty
}
