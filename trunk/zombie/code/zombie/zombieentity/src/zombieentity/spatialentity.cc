#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "zombieentity/spatialentity.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all spatial entity objects and classes
*/
// disable warnings about inherits via dominance
#define N_DEFINE_NEBULA_ENTITY 
#include "../../src/zombieentity/spatialentitylist.cc"
#undef N_DEFINE_NEBULA_ENTITY 

#pragma warning( default : 4250 )
