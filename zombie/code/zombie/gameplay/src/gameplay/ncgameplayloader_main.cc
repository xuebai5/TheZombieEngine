#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplayloader_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gameplay/ncgameplayloader.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplayLoader,ncLoader);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayLoader)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    constructor
*/
ncGameplayLoader::ncGameplayLoader() :
    ncLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncGameplayLoader::~ncGameplayLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncGameplayLoader::LoadComponents()
{
    return ncLoader::LoadComponents();
}
