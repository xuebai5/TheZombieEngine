//------------------------------------------------------------------------------
//  ncgpexplosionclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpexplosionclass.h"

nNebulaComponentClass(ncGPExplosionClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncGPExplosionClass::ncGPExplosionClass():
    explosionRadius( 4 ),
    explosionPower( 1000 ),
    explosionDamage( 1000 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPExplosionClass::~ncGPExplosionClass()
{
    // empty
}

//------------------------------------------------------------------------------
