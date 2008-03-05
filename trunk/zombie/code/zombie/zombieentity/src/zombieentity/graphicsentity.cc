#include "precompiled/pchzombieentity.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "zombieentity/graphicsentity.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all graphics entity objects and classes
*/
#define N_DEFINE_NEBULA_ENTITY 
// disable warnings about inherits via dominance
#include "graphicsentitylist.cc"
#undef N_DEFINE_NEBULA_ENTITY

#pragma warning( default : 4250 )
