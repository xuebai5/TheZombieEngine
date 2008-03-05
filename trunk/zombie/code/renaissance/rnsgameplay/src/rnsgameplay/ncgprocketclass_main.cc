//------------------------------------------------------------------------------
//  ncgprocketclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgprocketclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncGPRocketClass, ncGameplayClass);

//------------------------------------------------------------------------------
/**
*/
ncGPRocketClass::ncGPRocketClass():
    initialThrustPower( 10000.0f ),
    maxThrustTime( 1.0f ),
    maxLifeTime( 10.0f ),
    maxMeters( 10000.0f )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPRocketClass::~ncGPRocketClass()
{
    // empty
}

//------------------------------------------------------------------------------
