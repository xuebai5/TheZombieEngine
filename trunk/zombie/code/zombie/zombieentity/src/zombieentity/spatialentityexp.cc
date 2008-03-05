#include "precompiled/pchzombieentityexp.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "zombieentity/spatialentityexp.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all spatial entity objects and classes
*/
// disable warnings about inherits via dominance
#define N_DEFINE_NEBULA_ENTITY 
#include "../../src/zombieentity/spatialentitylistexp.cc"
#undef N_DEFINE_NEBULA_ENTITY 

#pragma warning( default : 4250 )
