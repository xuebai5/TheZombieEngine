//------------------------------------------------------------------------------
//  ncgpgrenadeclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpgrenadeclass.h"

nNebulaComponentClass(ncGPGrenadeClass,ncGameplayClass);

//------------------------------------------------------------------------------
/**
*/
ncGPGrenadeClass::ncGPGrenadeClass():
    launchPower( 1000 ),
    maxLifeTime( 5.0f )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPGrenadeClass::~ncGPGrenadeClass()
{
    // empty
}

//------------------------------------------------------------------------------
