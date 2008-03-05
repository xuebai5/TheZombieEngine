#include "precompiled/pchzombieentityexp.h"
//------------------------------------------------------------------------------
/**
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#pragma warning( disable : 4250 )

//------------------------------------------------------------------------------
#include "zombieentity/graphicsentityexp.h"

//------------------------------------------------------------------------------
/**
    Declaration of classes for all graphics entity objects and classes
*/
#define N_DEFINE_NEBULA_ENTITY 
// disable warnings about inherits via dominance
#include "graphicsentitylistexp.cc"
#undef N_DEFINE_NEBULA_ENTITY

#pragma warning( default : 4250 )
